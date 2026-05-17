<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 02_Rgemv

This directory benchmarks MPFR dense real matrix-vector multiplication:

```text
y = alpha * A * x + beta * y
```

The timed kernels compare raw `mpfr_t` C code with `mpfrxx_mkII`
expression-template wrappers.  The main questions are source shape, row-owned
OpenMP versus column-major streaming, explicit evaluation context, reusable
temporaries, FMA/FMMA lowering, and the fixed-precision fast path.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

Executables are written under:

```text
build_bench_release/benchmarks/mpfr/02_Rgemv/
```

Each executable takes:

```text
<rows m> <cols n> <precision>
```

Example:

```bash
build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07_mkII 4000 4000 512
```

For OpenMP runs, use explicit affinity:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread \
build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07_mkII \
    4000 4000 512
```

## Benchmark Parameters

| Parameter | Meaning |
|-----------|---------|
| `m` | Number of rows in `A` and length of `y`. |
| `n` | Number of columns in `A` and length of `x`. |
| `precision` | MPFR precision in bits. |

Each executable reports `Elapsed time`, `MFLOPS`, `L1 Norm of difference`, and
`Result OK` / `Result NG`.  MFLOPS uses the timed loop only:

```text
2 * m * n / elapsed_seconds / 1e6
```

The correctness reference is `Rgemv()` in `Rgemv.hpp`; the timed kernel is
`_Rgemv()` in each benchmark source.

Wrapper suffixes:

| Suffix | Meaning |
|--------|---------|
| `mkII` | Default `mpfrxx_mkII` wrapper path. |
| `mkII_FIXED_PRECISION_FASTPATH` | Build with `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`. |
| `mkII_FIXED_PRECISION_FASTPATH_FMA` | Fixed precision plus `MPFRXX_ENABLE_FMA`. |
| `mkII_FMA` | `MPFRXX_ENABLE_FMA` without the fixed-precision build option. |

MPFR-specific note: every arithmetic operation takes a rounding mode.  The
explicit-context wrapper kernels capture precision and rounding outside the hot
loop and route assignments through `mpfrxx::with_context`.

## Variant Shapes

The serial kernels include row-dot and column-AXPY shapes.  The OpenMP kernels
use race-free variants: row-owned loops, row blocks, or column partitioning
with thread-local partial vectors.

| Variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|--------------------|---------------------------|---------|
| `01` | Row-dot: `temp += A[i + j*lda] * x[j]`, then `y[i] = alpha * temp + beta * y[i]`. | Wrapper constructs one row accumulator per row; raw C reuses `mpfr_t` scratch objects. | Baseline expression/source-shape stress case. |
| `02` | Column AXPY: scale `y`, compute `temp = alpha * x[j]`, then update `y[i] += temp * A[i + j*lda]`. | Reusable `temp` and `templ` objects outside the loop nest. | Main non-context reusable-temporary path. |
| `03` | Raw C and explicit-context wrapper row-dot: `temp += A[i + j*lda] * x[j]`, then `y[i] = alpha * temp + beta * y[i]`. | Raw C uses one reusable `mpfr_t temp`; wrapper uses one reusable `mpfr_class temp` through `with_context`. | Compare wrapper FMA path with raw C `mpfr_fma` accumulation and final `mpfr_fmma`. |
| `04` | Explicit-context column AXPY wrapper: scale `y`, compute `temp = alpha * x[j]`, then `templ = temp * A[...]`, `y += templ`. | Reusable `temp` and `templ` through `with_context`. | Best serial wrapper source shape. |
| `openmp_01` | Row-owned direct row-dot expression. | Each row owns its accumulator; separate parallel regions scale and update rows. | Race-free OpenMP baseline. |
| `openmp_02` | Currently the same row-owned row-dot source shape as `openmp_01`. | Same policy as `openmp_01`. | Numbered placeholder; treat current source as duplicate of `openmp_01`. |
| `openmp_03` | Explicit-context row-owned row-dot. | One reusable `temp` per thread through `with_context`. | Explicit-context row-dot and FMA comparison path. |
| `openmp_04` | Explicit-context row-owned copy-then-multiply. | Per-thread reusable `temp` and `templ`; recomputes `alpha * x[j]` per row. | Race-free counterpart to serial column-AXPY logic, but with less reuse. |
| `openmp_05` | Precompute `scaled_x[j] = alpha * x[j]`, then row-owned update. | Shared read-only `scaled_x`, per-thread reusable `templ`, explicit context. | Remove repeated `alpha * x[j]` from row-owned OpenMP. |
| `openmp_06` | 256-row blocks, then column loop and contiguous row loop inside the block. | Per-thread reusable `temp` and `templ`, explicit context. | Restore contiguous `A` access inside each row block. |
| `openmp_07` | Column partitioning with thread-local partial `y` vectors and final reduction. | `num_threads * m` partial accumulators plus reduction, explicit context. | Keep serial-like column-major `A` streaming without racing on `y`. |

## C Native Equivalent Kernels

The mapping is based on the timed `_Rgemv()` source shape.

| C native kernel | Equivalent wrapper kernel(s) | Notes |
|-----------------|------------------------------|-------|
| `C_native_01` | `kernel_01_*` | Row-dot baseline with raw reusable `mpfr_t temp` and `prod`. |
| `C_native_01_FMA` | FMA-enabled `kernel_01_*` | Row-dot baseline using `mpfr_fma` accumulation and `mpfr_fmma` final update. |
| `C_native_02` | `kernel_02_*` | Column AXPY with `alpha * x[j]` hoisted outside the row loop. |
| `C_native_02_FMA` | Raw C FMA counterpart of the column-AXPY source class | Best serial raw C path. |
| `C_native_03` | `kernel_03_mkII_FMA` | Raw C row-dot with `mpfr_fma` accumulation and final `mpfr_fmma`. |
| `C_native_04` | `kernel_04_mkII` | Raw C counterpart of the explicit-context wrapper column-AXPY source shape. |
| `C_native_openmp_01` | `kernel_openmp_01_*` | Row-owned dot-product source shape. |
| `C_native_openmp_01_FMA` | FMA-enabled `kernel_openmp_01_*` | Raw C row-owned path with explicit `mpfr_fma`. |
| `C_native_openmp_02` | `kernel_openmp_02_*` | Row-owned dot-product duplicate matching the current wrapper `openmp_02` placeholder. |
| `C_native_openmp_02_FMA` | FMA-enabled `kernel_openmp_02_*` | FMA row-dot duplicate matching the current wrapper `openmp_02` placeholder. |
| `C_native_openmp_03` | `kernel_openmp_03_mkII_FMA` | Row-owned raw C path with one private `temp`, `mpfr_fma`, and final `mpfr_fmma`. |
| `C_native_openmp_04` | `kernel_openmp_04_mkII` | Row-owned copy-then-multiply; avoids races but recomputes `alpha * x[j]` per row. |
| `C_native_openmp_04_FMA` | FMA counterpart of `kernel_openmp_04_mkII` | Row-owned copy-then-FMA source shape. |
| `C_native_openmp_05` | `kernel_openmp_05_mkII` | Precomputed `scaled_x` counterpart; uses raw `mpfr_t` storage and one reusable `templ` per thread. |
| `C_native_openmp_06` | `kernel_openmp_06_mkII` | 256-row-block counterpart; uses raw `mpfr_t temp` and `templ` per thread. |
| `C_native_openmp_07` | `kernel_openmp_07_mkII` | Column-partition partial-vector counterpart; uses `num_threads * m` raw `mpfr_t` partial accumulators and final reduction. |

## Recorded Run

The current checked-in data uses:

```text
M = 4000
N = 4000
precision = 512
repeat = 10
OMP_NUM_THREADS = 32
OMP_PLACES = cores
OMP_PROC_BIND = spread
CPU = AMD Ryzen Threadripper 3970X 32-Core Processor
```

Results are stored in:

```text
results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/
```

Files:

- [Raw log](results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/benchmark_rgemv_mpfr_m4000_n4000_p512_repeat10.log)
- [Raw CSV](results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/raw_rgemv_mpfr_m4000_n4000_p512_repeat10.csv)
- [Summary CSV](results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv)

All 37 variants reported `Result OK` in all 10 runs, for 370 successful timed
runs.

Plot regeneration command:

```bash
python3 benchmarks/mpfr/02_Rgemv/plot_repeat_summary.py \
    benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/benchmark_rgemv_mpfr_m4000_n4000_p512_repeat10.log \
    --output-dir benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858 \
    --output-prefix rgemv_mpfr_m4000_n4000_p512_repeat10 \
    --title-prefix "MPFR Rgemv m=4000 n=4000 precision=512 repeat=10"
```

![MPFR Rgemv serial repeat-10](results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/rgemv_mpfr_m4000_n4000_p512_repeat10_serial.png)

![MPFR Rgemv OpenMP repeat-10](results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/rgemv_mpfr_m4000_n4000_p512_repeat10_openmp.png)

## Resource and Bandwidth Estimates

These are model estimates derived from MFLOPS, not hardware-counter
measurements.  On this LP64 system:

```text
sizeof(__mpfr_struct) = 32 bytes
sizeof(mp_limb_t)     = 8 bytes
precision             = 512 bits
active limbs          = 8
active mpfr value     = 32-byte header + 8 limbs * 8 = 96 bytes
```

For one matrix element at 512-bit precision:

```text
A+x active logical GB/s   = MFLOPS * 0.096
A+x+y active logical GB/s = MFLOPS * 0.192
```

`A+x` counts one matrix value and one vector value per two floating-point
operations.  `A+x+y` additionally counts one read and one write of `y`.
Actual traffic can differ because `mpfr_t` headers are contiguous but each
`_mpfr_d` points to limb storage.

| Variant | Avg MFLOPS | Max MFLOPS | A+x avg GB/s | A+x+y avg GB/s |
|---------|-----------:|-----------:|-------------:|---------------:|
| `C_native_openmp_07` | 442.510 | 456.226 | 42.48 | 84.96 |
| `kernel_openmp_07_mkII` | 431.660 | 447.584 | 41.44 | 82.88 |
| `C_native_openmp_06` | 311.476 | 318.191 | 29.90 | 59.80 |
| `kernel_openmp_06_mkII` | 301.489 | 309.504 | 28.94 | 57.89 |
| `kernel_openmp_03_mkII_FMA` | 280.321 | 286.304 | 26.91 | 53.82 |
| `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA` | 276.625 | 281.978 | 26.56 | 53.11 |
| `C_native_openmp_02` | 274.149 | 282.505 | 26.32 | 52.64 |
| `C_native_02_FMA` | 23.586 | 23.776 | 2.26 | 4.53 |
| `kernel_04_mkII` | 20.416 | 20.674 | 1.96 | 3.92 |

## Serial Results

Main interpretation table:

| Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
|---------|-----------:|-----------:|-----------:|----------------|
| `C_native_02_FMA` | 23.776 | 23.586 | 23.459 | Best serial result; column AXPY hoists `alpha * x[j]` and uses one `mpfr_fma` per matrix element. |
| `C_native_04` | 20.866 | 20.435 | 20.041 | Raw C counterpart of the explicit-context column-AXPY wrapper shape. |
| `kernel_04_mkII` | 20.674 | 20.416 | 20.297 | Best serial wrapper; explicit context plus reusable `temp`/`templ`. |
| `C_native_02` | 20.679 | 20.512 | 20.348 | Raw non-FMA column-AXPY baseline. |
| `kernel_02_mkII` | 18.747 | 18.310 | 18.030 | Non-context wrapper column-AXPY path; same source class as raw `02`. |
| `kernel_02_mkII_FIXED_PRECISION_FASTPATH_FMA` | 18.628 | 18.206 | 18.040 | Fixed precision plus FMA changes call shape but not the dominant serial memory walk. |
| `C_native_01` | 11.403 | 10.682 | 10.040 | Raw row-dot baseline; strided column-major matrix walk is slow. |
| `C_native_03` | 10.825 | 10.605 | 10.402 | Raw C row-dot with `mpfr_fma` and `mpfr_fmma`; row-dot traversal still dominates. |
| `kernel_03_mkII_FMA` | 10.770 | 10.257 | 10.072 | Wrapper row-dot FMA path; close to `C_native_03`, still traversal-limited. |

<details>
<summary>Serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|-----------:|-----------:|-----------:|
| 1 | `C_native_02_FMA` | 23.776 | 23.586 | 23.459 |
| 2 | `C_native_04` | 20.866 | 20.435 | 20.041 |
| 3 | `C_native_02` | 20.679 | 20.512 | 20.348 |
| 4 | `kernel_04_mkII` | 20.674 | 20.416 | 20.297 |
| 5 | `kernel_02_mkII` | 18.747 | 18.310 | 18.030 |
| 6 | `kernel_02_mkII_FIXED_PRECISION_FASTPATH_FMA` | 18.628 | 18.206 | 18.040 |
| 7 | `kernel_02_mkII_FIXED_PRECISION_FASTPATH` | 18.150 | 17.967 | 17.781 |
| 8 | `C_native_01` | 11.403 | 10.682 | 10.040 |
| 9 | `C_native_01_FMA` | 11.149 | 10.719 | 10.406 |
| 10 | `C_native_03` | 10.825 | 10.605 | 10.402 |
| 11 | `kernel_03_mkII_FMA` | 10.770 | 10.257 | 10.072 |
| 12 | `kernel_01_mkII_FIXED_PRECISION_FASTPATH` | 10.522 | 9.941 | 9.275 |
| 13 | `kernel_03_mkII` | 10.424 | 9.612 | 9.112 |
| 14 | `kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA` | 10.321 | 10.101 | 9.880 |
| 15 | `kernel_01_mkII` | 9.308 | 8.983 | 8.723 |

</details>

<details>
<summary>Serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|-----------:|-----------:|-----------:|
| 1 | `C_native_02_FMA` | 23.776 | 23.586 | 23.459 |
| 2 | `C_native_02` | 20.679 | 20.512 | 20.348 |
| 3 | `C_native_04` | 20.866 | 20.435 | 20.041 |
| 4 | `kernel_04_mkII` | 20.674 | 20.416 | 20.297 |
| 5 | `kernel_02_mkII` | 18.747 | 18.310 | 18.030 |
| 6 | `kernel_02_mkII_FIXED_PRECISION_FASTPATH_FMA` | 18.628 | 18.206 | 18.040 |
| 7 | `kernel_02_mkII_FIXED_PRECISION_FASTPATH` | 18.150 | 17.967 | 17.781 |
| 8 | `C_native_01_FMA` | 11.149 | 10.719 | 10.406 |
| 9 | `C_native_01` | 11.403 | 10.682 | 10.040 |
| 10 | `C_native_03` | 10.825 | 10.605 | 10.402 |
| 11 | `kernel_03_mkII_FMA` | 10.770 | 10.257 | 10.072 |
| 12 | `kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA` | 10.321 | 10.101 | 9.880 |
| 13 | `kernel_01_mkII_FIXED_PRECISION_FASTPATH` | 10.522 | 9.941 | 9.275 |
| 14 | `kernel_03_mkII` | 10.424 | 9.612 | 9.112 |
| 15 | `kernel_01_mkII` | 9.308 | 8.983 | 8.723 |

</details>

## OpenMP Results

Main interpretation table:

| Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
|---------|-----------:|-----------:|-----------:|----------------|
| `C_native_openmp_07` | 456.226 | 442.510 | 416.431 | Best overall result; column partitioning keeps column-major `A` streaming and reduces raw thread-local partial `y` vectors. |
| `kernel_openmp_07_mkII` | 447.584 | 431.660 | 408.902 | Best wrapper result; same source shape as `C_native_openmp_07` with wrapper-managed partial vectors. |
| `C_native_openmp_06` | 318.191 | 311.476 | 299.564 | Best row-blocked raw C shape; 256-row blocking restores contiguous `A` access inside each block. |
| `kernel_openmp_06_mkII` | 309.504 | 301.489 | 277.692 | Best row-blocked wrapper shape; close to the raw C counterpart. |
| `kernel_openmp_03_mkII_FMA` | 286.304 | 280.321 | 252.779 | Best row-dot wrapper path; FMA/FMMA helps arithmetic shape but does not fix row traversal. |
| `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA` | 281.978 | 276.625 | 270.645 | Row-owned source class plus fixed precision and FMA; same performance class as `openmp_01` FMA. |
| `C_native_openmp_02` | 282.505 | 274.149 | 262.166 | Raw row-owned duplicate of the current `openmp_02` placeholder. |
| `kernel_openmp_05_mkII` | 265.694 | 260.170 | 247.676 | Precomputed `scaled_x`; removes repeated `alpha*x[j]` but remains row-traversal limited. |
| `kernel_openmp_04_mkII` | 202.201 | 195.507 | 185.160 | Row-owned copy-then-multiply path; recomputes `alpha*x[j]` per row. |

<details>
<summary>OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|-----------:|-----------:|-----------:|
| 1 | `C_native_openmp_07` | 456.226 | 442.510 | 416.431 |
| 2 | `kernel_openmp_07_mkII` | 447.584 | 431.660 | 408.902 |
| 3 | `C_native_openmp_06` | 318.191 | 311.476 | 299.564 |
| 4 | `kernel_openmp_06_mkII` | 309.504 | 301.489 | 277.692 |
| 5 | `kernel_openmp_03_mkII_FMA` | 286.304 | 280.321 | 252.779 |
| 6 | `C_native_openmp_02` | 282.505 | 274.149 | 262.166 |
| 7 | `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA` | 281.978 | 276.625 | 270.645 |
| 8 | `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH_FMA` | 281.594 | 276.370 | 266.658 |
| 9 | `C_native_openmp_01` | 275.700 | 268.765 | 256.168 |
| 10 | `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH` | 268.094 | 253.886 | 230.330 |
| 11 | `C_native_openmp_02_FMA` | 267.292 | 262.298 | 246.478 |
| 12 | `kernel_openmp_05_mkII` | 265.694 | 260.170 | 247.676 |
| 13 | `C_native_openmp_03` | 265.537 | 261.454 | 258.557 |
| 14 | `C_native_openmp_05` | 264.821 | 259.256 | 250.082 |
| 15 | `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH` | 264.264 | 259.202 | 254.078 |
| 16 | `kernel_openmp_03_mkII` | 256.240 | 249.694 | 240.369 |
| 17 | `kernel_openmp_01_mkII` | 252.671 | 246.742 | 241.724 |
| 18 | `kernel_openmp_02_mkII` | 250.406 | 244.205 | 236.385 |
| 19 | `C_native_openmp_01_FMA` | 213.581 | 210.304 | 204.646 |
| 20 | `C_native_openmp_04_FMA` | 212.349 | 208.340 | 199.104 |
| 21 | `C_native_openmp_04` | 202.858 | 198.898 | 191.888 |
| 22 | `kernel_openmp_04_mkII` | 202.201 | 195.507 | 185.160 |

</details>

<details>
<summary>OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|-----------:|-----------:|-----------:|
| 1 | `C_native_openmp_07` | 456.226 | 442.510 | 416.431 |
| 2 | `kernel_openmp_07_mkII` | 447.584 | 431.660 | 408.902 |
| 3 | `C_native_openmp_06` | 318.191 | 311.476 | 299.564 |
| 4 | `kernel_openmp_06_mkII` | 309.504 | 301.489 | 277.692 |
| 5 | `kernel_openmp_03_mkII_FMA` | 286.304 | 280.321 | 252.779 |
| 6 | `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA` | 281.978 | 276.625 | 270.645 |
| 7 | `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH_FMA` | 281.594 | 276.370 | 266.658 |
| 8 | `C_native_openmp_02` | 282.505 | 274.149 | 262.166 |
| 9 | `C_native_openmp_01` | 275.700 | 268.765 | 256.168 |
| 10 | `C_native_openmp_02_FMA` | 267.292 | 262.298 | 246.478 |
| 11 | `C_native_openmp_03` | 265.537 | 261.454 | 258.557 |
| 12 | `kernel_openmp_05_mkII` | 265.694 | 260.170 | 247.676 |
| 13 | `C_native_openmp_05` | 264.821 | 259.256 | 250.082 |
| 14 | `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH` | 264.264 | 259.202 | 254.078 |
| 15 | `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH` | 268.094 | 253.886 | 230.330 |
| 16 | `kernel_openmp_03_mkII` | 256.240 | 249.694 | 240.369 |
| 17 | `kernel_openmp_01_mkII` | 252.671 | 246.742 | 241.724 |
| 18 | `kernel_openmp_02_mkII` | 250.406 | 244.205 | 236.385 |
| 19 | `C_native_openmp_01_FMA` | 213.581 | 210.304 | 204.646 |
| 20 | `C_native_openmp_04_FMA` | 212.349 | 208.340 | 199.104 |
| 21 | `C_native_openmp_04` | 202.858 | 198.898 | 191.888 |
| 22 | `kernel_openmp_04_mkII` | 202.201 | 195.507 | 185.160 |

</details>

## Hotpath Disassembly

The snippets below show representative call structure in the Release build.
Addresses are intentionally omitted because they are build-local.

### `C_native_02_FMA`

Best serial raw C shape: default rounding is loaded once, `alpha*x[j]` is
hoisted, and the inner row loop is one `mpfr_fma`.

```asm
call   mpfr_get_default_rounding_mode@plt
call   mpfr_init2@plt
...
call   mpfr_mul@plt        # y[i] *= beta
...
call   mpfr_set@plt        # temp = alpha
call   mpfr_mul@plt        # temp *= x[j]
...
call   mpfr_fma@plt        # y[i] = temp * A[i+j*lda] + y[i]
...
call   mpfr_clear@plt
```

### `C_native_03` and `C_native_openmp_03`

Raw C row-dot comparison target for wrapper `03` FMA:

```asm
call   mpfr_init2@plt
call   mpfr_get_default_rounding_mode@plt
call   mpfr_set_d@plt      # temp = 0
...
call   mpfr_fma@plt        # temp += A[i+j*lda] * x[j]
...
call   mpfr_fmma@plt       # y = alpha * temp + beta * y
call   mpfr_clear@plt
```

### `kernel_openmp_03_mkII_FMA`

Wrapper row-dot FMA path:

```asm
call   mpfr_init2@plt
call   mpfr_get_default_rounding_mode@plt
call   mpfr_set_d@plt      # temp = 0
...
call   mpfr_fma@plt        # temp += A[i+j*lda] * x[j]
...
call   mpfr_fmma@plt       # y = alpha * temp + beta * y
call   GOMP_barrier@plt
call   mpfr_clear@plt
```

### `kernel_openmp_07_mkII`

`openmp_07` does not win by a new arithmetic primitive.  It wins by source
shape: column partitioning streams contiguous column-major `A`, accumulates
into thread-local partial `y`, then performs a reduction.  The arithmetic hot
loop is still multiply plus add through reusable wrapper temporaries.

## Lessons Learned

Serial MPFR Rgemv is dominated by source shape.  The column-AXPY shape wins
because it hoists `alpha*x[j]` outside the inner row loop and streams
column-major `A` contiguously.  `C_native_02_FMA` is the best serial result
because the inner row loop is reduced to one `mpfr_fma` call.

Explicit context helps the wrapper, but it cannot compensate for a poor memory
walk.  `kernel_04_mkII` is the best serial wrapper because it combines
column-AXPY traversal, reusable temporaries, and loop-invariant rounding.  The
row-dot `kernel_03_mkII_FMA` reaches the intended `mpfr_fma`/`mpfr_fmma` shape
but remains slow in serial because it walks a column-major matrix with a large
stride.

OpenMP changes the best strategy.  Row-owned kernels avoid races on `y`, but
they make the matrix access pattern poor.  `openmp_06` improves that with
256-row blocks, and `openmp_07` goes further by partitioning columns and using
thread-local partial `y` vectors.  That is why `C_native_openmp_07` reaches
442.510 average MFLOPS and `kernel_openmp_07_mkII` reaches 431.660 average
MFLOPS while the best row-owned FMA wrapper remains around 280 MFLOPS.

`openmp_05` confirms that precomputing `scaled_x` alone is not enough.  It
removes repeated `alpha*x[j]`, but without fixing the matrix traversal it stays
below the blocked and column-partitioned variants.

Compared with GMP `mpf`, MPFR adds a rounding-mode dimension to every
operation.  Explicit contexts remain important, but the largest gains in this
benchmark come from restoring column-major streaming and avoiding write races
without forcing row-strided matrix access.
