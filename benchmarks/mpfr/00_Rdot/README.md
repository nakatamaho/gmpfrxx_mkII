<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 00_Rdot

This directory benchmarks the MPFR real dot product

```text
sum_i x_i * y_i
```

with fixed-precision `mpfr_t` and `mpfrxx::mpfr_class` data.  The benchmark is
kept parallel to `benchmarks/gmp/00_Rdot/`: every kernel shape is a standalone
translation unit and the timed function is `_Rdot()`, so source shape,
optimization flags, and hotpath disassembly can be compared directly.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

Executables are created under:

```text
build_bench_release/benchmarks/mpfr/00_Rdot/
```

Each executable takes:

```text
<vector size> <precision>
```

Example:

```bash
build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03_mkII 10000000 512
```

## Kernel Shapes

Kernel numbers `01..06` intentionally match GMP Rdot.  MPFR-specific explicit
context kernels are added as `07` and `08`.  The same number means the same
source-level shape for serial and OpenMP wrapper kernels.

| Variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|--------------------|---------------------------|---------|
| `01` | `acc += dx[i] * dy[i]` | Expression product is formed in the compound assignment. | Test the ET spelling.  FMA builds can lower this source to one `mpfr_fma` call per element. |
| `02` | `mpfr_class templ = dx[i] * dy[i]; acc += templ;` | Loop-local product object is constructed and destroyed inside every iteration. | Intentionally expensive control for temporary lifetime. |
| `03` | `templ = dx[i] * dy[i]; acc += templ;` | One product object is initialized before the loop and reused. | Main non-FMA reusable-product wrapper shape. |
| `04` | `templ = dx[i]; templ *= dy[i]; acc += templ;` | One product object is reused, but each iteration copies `dx[i]` before multiplication. | Separate product-object reuse from copy-then-multiply spelling. |
| `05` | Four accumulators with one reused product object. | Four accumulators share one product temporary. | Test accumulator unrolling while keeping one product temporary. |
| `06` | Four accumulators with four reused product objects. | Four accumulators and four independent product temporaries are reused. | Test accumulator unrolling plus independent product temporaries. |
| `07` | `with_context(acc, precision, rnd) += dx[i] * dy[i]` | Rounding is captured before the loop, and fixed-precision targets compile out context precision checks. | Context-bound `01`; FMA builds can lower this to `mpfr_fma`. |
| `08` | `with_context(templ, precision, rnd) = dx[i] * dy[i]; with_context(acc, precision, rnd) += templ;` | One product object is reused, rounding is cached before the loop, and fixed-precision targets compile out context precision checks. | Context-bound `03`; raw-C-like non-FMA reusable-product control. |

Raw C baselines use:

```text
Rdot_mpfr_C_native_NN
Rdot_mpfr_C_native_openmp_NN
```

The raw FMA baseline is intentionally a separate source file:

```text
Rdot_mpfr_C_native_01_FMA
Rdot_mpfr_C_native_openmp_01_FMA
```

There is no exact raw C non-FMA equivalent for the C++ expression spelling
`acc += dx[i] * dy[i]`.  Raw C has to choose either a split `mpfr_mul` plus
`mpfr_add` sequence, which is equivalent to a product-temporary source shape,
or a fused `mpfr_fma` call, which is equivalent to FMA-enabled `01` and `07`.

Wrapper suffixes are cumulative:

| Suffix | Build option | Meaning |
|--------|--------------|---------|
| `mkII` | none | Generic wrapper expression path. |
| `STABLE_ROUNDING` | `GMPFRXX_MKII_FAST_STABLE_RND` | Avoids the generic default-rounding lookup path and uses the wrapper stable rounding path. |
| `STABLE_ROUNDING_FMA` | stable rounding + `GMPFRXX_MKII_ENABLE_FMA` | Allows expression shapes such as `acc += dx[i] * dy[i]` to lower to `mpfr_fma`. |
| `STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | stable rounding + FMA + `GMPFRXX_MKII_FAST_FIXED_PREC` | Enables fixed-precision wrapper specialization where applicable. |

Context-bound kernels are explicit targets.  The primary context comparison
points are the fixed-precision targets because the source intentionally assumes
that the context precision matches the destination precision:

```text
Rdot_mpfr_kernel_07_mkII
Rdot_mpfr_kernel_07_mkII_FIXED_PRECISION_FASTPATH
Rdot_mpfr_kernel_07_mkII_FMA
Rdot_mpfr_kernel_07_mkII_FIXED_PRECISION_FASTPATH_FMA
Rdot_mpfr_kernel_08_mkII
Rdot_mpfr_kernel_08_mkII_FIXED_PRECISION_FASTPATH
Rdot_mpfr_kernel_openmp_07_mkII
Rdot_mpfr_kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH
Rdot_mpfr_kernel_openmp_07_mkII_FMA
Rdot_mpfr_kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH_FMA
Rdot_mpfr_kernel_openmp_08_mkII
Rdot_mpfr_kernel_openmp_08_mkII_FIXED_PRECISION_FASTPATH
```

## C Native Equivalent Kernels

The C native executables are the reference hot-loop shapes for the C++ wrapper
kernels.  The mapping is based on the timed `_Rdot()` body, not on the
post-run correctness reference.

| C native kernel | Equivalent C++ wrapper kernel(s) | Equivalence basis |
|-----------------|----------------------------------|-------------------|
| No exact non-FMA raw C kernel | `kernel_01_mkII`, `kernel_01_mkII_STABLE_ROUNDING` | `01` is an expression-template source spelling.  Non-FMA generated code must be classified by disassembly, not by raw C source spelling. |
| `C_native_01_FMA` | `kernel_01_mkII_STABLE_ROUNDING_FMA`, `kernel_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH`, `kernel_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | One `mpfr_fma` call per element.  Raw C passes a cached local `rnd`; explicit-context `07` also passes loop-local cached rounding. |
| `C_native_02` | `kernel_02_*` | Loop-local product object: create product inside every iteration, add it, then destroy it. |
| `C_native_03` | `kernel_03_*`, `kernel_08_mkII_FIXED_PRECISION_FASTPATH` | One product object is initialized before the loop and reused with direct product assignment.  `08` is the explicit-context version of this split multiply/add shape. |
| `C_native_04` | `kernel_04_*` | One product object is reused, but each iteration copies `dx[i]` before multiplying by `dy[i]`. |
| `C_native_05` | `kernel_05_*` | Four accumulators with one reused product object.  This is an unrolling/control variant, not a required wrapper API shape. |
| `C_native_06` | `kernel_06_*` | Four accumulators with four reused product objects.  This is an unrolling/control variant, not a required wrapper API shape. |
| `C_native_openmp_01_FMA` | `kernel_openmp_01_*_FMA`, `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | OpenMP one-call FMA equivalent. |
| `C_native_openmp_02` | `kernel_openmp_02_*` | OpenMP loop-local product object. |
| `C_native_openmp_03` | `kernel_openmp_03_*`, `kernel_openmp_08_mkII_FIXED_PRECISION_FASTPATH` | OpenMP reusable-product split multiply/add shape. |
| `C_native_openmp_04` | `kernel_openmp_04_*` | OpenMP copy-then-multiply shape. |
| `C_native_openmp_05` | `kernel_openmp_05_*` | OpenMP four-accumulator, one-product control. |
| `C_native_openmp_06` | `kernel_openmp_06_*` | OpenMP four-accumulator, four-product control. |

`C_native_01` and `C_native_openmp_01` are legacy raw C loop-local-product
controls retained for old result comparability.  They should not be used as the
exact source-level equivalent of wrapper `01`; use `C_native_01_FMA` for the
FMA-lowered `01`/`07` comparison and `C_native_02` for loop-local product
materialization.

## Recorded Run

The current checked-in MPFR Rdot data was regenerated from scratch after
removing older result directories.

```text
N = 10000000
precision = 512
repeat = 10
OMP_NUM_THREADS = 32
OMP_PLACES = cores
OMP_PROC_BIND = spread
CPU = AMD Ryzen Threadripper 3970X 32-Core Processor
```

Results are stored in:

```text
results_raw/rdot_mpfr_n10000000_p512_repeat10_20260522_171957/
```

Files:

- [Raw log](results_raw/rdot_mpfr_n10000000_p512_repeat10_20260522_171957/benchmark_rdot_mpfr_n10000000_p512_repeat10.log)
- [Raw CSV](results_raw/rdot_mpfr_n10000000_p512_repeat10_20260522_171957/raw_rdot_mpfr_n10000000_p512_repeat10.csv)
- [Summary CSV](results_raw/rdot_mpfr_n10000000_p512_repeat10_20260522_171957/summary_rdot_mpfr_n10000000_p512_repeat10.csv)

The sweep covers 74 variants and 740 timed runs.  Every timed run reported
`OK`.

The plots show average MFLOPS as vertical bars.  The black range line on each
bar is the observed min-to-max interval across the 10 repeats; labels show the
average and the range endpoints.

![MPFR Rdot serial repeat-10](results_raw/rdot_mpfr_n10000000_p512_repeat10_20260522_171957/rdot_mpfr_n10000000_p512_repeat10_serial.png)

![MPFR Rdot OpenMP repeat-10](results_raw/rdot_mpfr_n10000000_p512_repeat10_20260522_171957/rdot_mpfr_n10000000_p512_repeat10_openmp.png)

The images can be regenerated with:

```bash
python3 benchmarks/mpfr/00_Rdot/plot_repeat_summary.py \
    benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260522_171957/benchmark_rdot_mpfr_n10000000_p512_repeat10.log \
    --output-dir benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260522_171957 \
    --output-prefix rdot_mpfr_n10000000_p512_repeat10 \
    --title-prefix "MPFR Rdot N=10000000 precision=512 repeat=10"
```

## Headline Results

| Class | Best average variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Notes |
|-------|----------------------|-----------:|-----------:|-----------:|-------|
| Serial wrapper | `kernel_06_mkII_STABLE_ROUNDING` | 23.724 | 23.515 | 23.307 | Best serial wrapper average; stable-rounding four-accumulator/four-product shape. |
| Serial raw C | `C_native_06` | 23.688 | 23.542 | 23.287 | Best serial raw C average in this run. |
| OpenMP wrapper | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 579.031 | 554.490 | 543.438 | Best OpenMP wrapper average; expression FMA with stable rounding and fixed precision. |
| OpenMP raw C | `C_native_openmp_01_FMA` | 579.952 | 553.249 | 517.083 | Best OpenMP raw C average; FMA with cached rounding. |

## Serial Results

Main interpretation table:

| Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
|---------|-----------:|-----------:|-----------:|----------------|
| `C_native_06` | 23.688 | 23.542 | 23.287 | Best serial raw C average in this run; four accumulators and four reused products. |
| `kernel_06_mkII_STABLE_ROUNDING` | 23.724 | 23.515 | 23.307 | Best serial wrapper average; stable rounding plus four accumulators/four products reaches the raw C unrolled class. |
| `kernel_03_mkII_STABLE_ROUNDING` | 24.004 | 23.307 | 22.862 | Reusable-product non-FMA wrapper shape; stable rounding puts it in the top serial class. |
| `kernel_01_mkII_STABLE_ROUNDING_FMA` | 23.564 | 23.151 | 22.884 | Expression spelling lowers to one MPFR FMA call per element. |
| `kernel_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | 23.459 | 23.150 | 22.846 | Explicit-context FMA path with cached rounding and fixed-precision context checks compiled out. |
| `kernel_08_mkII_FIXED_PRECISION_FASTPATH` | 23.438 | 23.046 | 22.783 | Explicit-context reusable-product non-FMA path; matches the raw `C_native_03` call shape. |
| `C_native_01_FMA` | 23.605 | 23.203 | 22.951 | Raw C FMA baseline with rounding loaded before the loop. |
| `kernel_01_mkII` | 16.692 | 16.490 | 16.261 | Generic wrapper expression path; product materialization and generic rounding delivery are both visible. |

<details>
<summary>Serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|-----------:|-----------:|-----------:|
| 1 | `kernel_03_mkII_STABLE_ROUNDING` | 24.004 | 23.307 | 22.862 |
| 2 | `kernel_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.771 | 23.055 | 22.629 |
| 3 | `kernel_05_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.754 | 23.254 | 22.684 |
| 4 | `kernel_06_mkII_STABLE_ROUNDING` | 23.724 | 23.515 | 23.307 |
| 5 | `kernel_03_mkII_STABLE_ROUNDING_FMA` | 23.692 | 23.108 | 22.663 |
| 6 | `C_native_06` | 23.688 | 23.542 | 23.287 |
| 7 | `kernel_05_mkII_STABLE_ROUNDING` | 23.638 | 23.121 | 22.806 |
| 8 | `kernel_08_mkII` | 23.634 | 23.120 | 22.839 |
| 9 | `C_native_01_FMA` | 23.605 | 23.203 | 22.951 |
| 10 | `kernel_06_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.572 | 23.377 | 23.116 |
| 11 | `kernel_01_mkII_STABLE_ROUNDING_FMA` | 23.564 | 23.151 | 22.884 |
| 12 | `kernel_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.535 | 23.214 | 22.838 |
| 13 | `kernel_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | 23.459 | 23.150 | 22.846 |
| 14 | `kernel_08_mkII_FIXED_PRECISION_FASTPATH` | 23.438 | 23.046 | 22.783 |
| 15 | `C_native_03` | 23.419 | 23.070 | 22.710 |
| 16 | `kernel_07_mkII_FMA` | 23.408 | 23.105 | 22.865 |
| 17 | `C_native_05` | 23.315 | 23.140 | 22.912 |
| 18 | `kernel_05_mkII_STABLE_ROUNDING_FMA` | 23.294 | 23.110 | 22.906 |
| 19 | `kernel_06_mkII_STABLE_ROUNDING_FMA` | 22.559 | 22.418 | 22.156 |
| 20 | `kernel_07_mkII_FIXED_PRECISION_FASTPATH` | 22.075 | 21.867 | 21.357 |
| 21 | `kernel_06_mkII` | 21.712 | 21.341 | 20.323 |
| 22 | `kernel_05_mkII` | 21.179 | 20.686 | 19.951 |
| 23 | `kernel_03_mkII` | 21.042 | 20.812 | 20.360 |
| 24 | `kernel_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 20.709 | 20.082 | 19.827 |
| 25 | `kernel_04_mkII_STABLE_ROUNDING` | 20.510 | 20.084 | 19.846 |
| 26 | `C_native_04` | 20.472 | 20.061 | 19.763 |
| 27 | `kernel_04_mkII_STABLE_ROUNDING_FMA` | 20.363 | 20.113 | 19.831 |
| 28 | `kernel_07_mkII` | 19.389 | 18.984 | 18.627 |
| 29 | `kernel_01_mkII_STABLE_ROUNDING` | 19.329 | 18.934 | 18.650 |
| 30 | `kernel_02_mkII_STABLE_ROUNDING_FMA` | 19.181 | 18.573 | 18.265 |
| 31 | `kernel_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 18.896 | 18.299 | 17.105 |
| 32 | `kernel_02_mkII_STABLE_ROUNDING` | 18.686 | 18.486 | 18.115 |
| 33 | `C_native_01` | 18.677 | 18.474 | 18.240 |
| 34 | `C_native_02` | 18.522 | 18.402 | 18.263 |
| 35 | `kernel_04_mkII` | 18.349 | 18.226 | 17.993 |
| 36 | `kernel_01_mkII` | 16.692 | 16.490 | 16.261 |
| 37 | `kernel_02_mkII` | 15.959 | 15.733 | 15.300 |

</details>

<details>
<summary>Serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|-----------:|-----------:|-----------:|
| 1 | `C_native_06` | 23.688 | 23.542 | 23.287 |
| 2 | `kernel_06_mkII_STABLE_ROUNDING` | 23.724 | 23.515 | 23.307 |
| 3 | `kernel_06_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.572 | 23.377 | 23.116 |
| 4 | `kernel_03_mkII_STABLE_ROUNDING` | 24.004 | 23.307 | 22.862 |
| 5 | `kernel_05_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.754 | 23.254 | 22.684 |
| 6 | `kernel_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.535 | 23.214 | 22.838 |
| 7 | `C_native_01_FMA` | 23.605 | 23.203 | 22.951 |
| 8 | `kernel_01_mkII_STABLE_ROUNDING_FMA` | 23.564 | 23.151 | 22.884 |
| 9 | `kernel_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | 23.459 | 23.150 | 22.846 |
| 10 | `C_native_05` | 23.315 | 23.140 | 22.912 |
| 11 | `kernel_05_mkII_STABLE_ROUNDING` | 23.638 | 23.121 | 22.806 |
| 12 | `kernel_08_mkII` | 23.634 | 23.120 | 22.839 |
| 13 | `kernel_05_mkII_STABLE_ROUNDING_FMA` | 23.294 | 23.110 | 22.906 |
| 14 | `kernel_03_mkII_STABLE_ROUNDING_FMA` | 23.692 | 23.108 | 22.663 |
| 15 | `kernel_07_mkII_FMA` | 23.408 | 23.105 | 22.865 |
| 16 | `C_native_03` | 23.419 | 23.070 | 22.710 |
| 17 | `kernel_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.771 | 23.055 | 22.629 |
| 18 | `kernel_08_mkII_FIXED_PRECISION_FASTPATH` | 23.438 | 23.046 | 22.783 |
| 19 | `kernel_06_mkII_STABLE_ROUNDING_FMA` | 22.559 | 22.418 | 22.156 |
| 20 | `kernel_07_mkII_FIXED_PRECISION_FASTPATH` | 22.075 | 21.867 | 21.357 |
| 21 | `kernel_06_mkII` | 21.712 | 21.341 | 20.323 |
| 22 | `kernel_03_mkII` | 21.042 | 20.812 | 20.360 |
| 23 | `kernel_05_mkII` | 21.179 | 20.686 | 19.951 |
| 24 | `kernel_04_mkII_STABLE_ROUNDING_FMA` | 20.363 | 20.113 | 19.831 |
| 25 | `kernel_04_mkII_STABLE_ROUNDING` | 20.510 | 20.084 | 19.846 |
| 26 | `kernel_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 20.709 | 20.082 | 19.827 |
| 27 | `C_native_04` | 20.472 | 20.061 | 19.763 |
| 28 | `kernel_07_mkII` | 19.389 | 18.984 | 18.627 |
| 29 | `kernel_01_mkII_STABLE_ROUNDING` | 19.329 | 18.934 | 18.650 |
| 30 | `kernel_02_mkII_STABLE_ROUNDING_FMA` | 19.181 | 18.573 | 18.265 |
| 31 | `kernel_02_mkII_STABLE_ROUNDING` | 18.686 | 18.486 | 18.115 |
| 32 | `C_native_01` | 18.677 | 18.474 | 18.240 |
| 33 | `C_native_02` | 18.522 | 18.402 | 18.263 |
| 34 | `kernel_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 18.896 | 18.299 | 17.105 |
| 35 | `kernel_04_mkII` | 18.349 | 18.226 | 17.993 |
| 36 | `kernel_01_mkII` | 16.692 | 16.490 | 16.261 |
| 37 | `kernel_02_mkII` | 15.959 | 15.733 | 15.300 |

</details>

## OpenMP Results

Main interpretation table:

| Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
|---------|-----------:|-----------:|-----------:|----------------|
| `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 579.031 | 554.490 | 543.438 | Best OpenMP wrapper average in this run; expression FMA with stable rounding and fixed precision. |
| `C_native_openmp_01_FMA` | 579.952 | 553.249 | 517.083 | Best OpenMP raw C average; one `mpfr_fma` call per element with cached rounding. |
| `C_native_openmp_03` | 567.639 | 550.842 | 531.772 | Raw OpenMP reusable-product non-FMA baseline; close to the best wrapper average in this run. |
| `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 576.793 | 550.225 | 500.010 | Reusable-product wrapper path in the upper OpenMP class; range still shows OpenMP variance. |
| `kernel_openmp_08_mkII_FIXED_PRECISION_FASTPATH` | 573.447 | 548.290 | 513.005 | Explicit-context OpenMP reusable-product path; cached rounding and fixed-precision context checks compiled out. |
| `kernel_openmp_06_mkII_STABLE_ROUNDING_FMA` | 582.990 | 547.109 | 510.640 | Four-accumulator/four-product OpenMP control; in the upper OpenMP class but not a distinct tier. |
| `kernel_openmp_03_mkII_STABLE_ROUNDING` | 517.870 | 501.946 | 471.086 | Reusable-product stable-rounding non-FMA path; lower than the FMA/fixed top class in this run. |
| `kernel_openmp_01_mkII` | 420.727 | 405.447 | 365.891 | Generic OpenMP wrapper expression path remains below stable/FMA/context paths. |

<details>
<summary>OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|-----------:|-----------:|-----------:|
| 1 | `kernel_openmp_06_mkII_STABLE_ROUNDING_FMA` | 582.990 | 547.109 | 510.640 |
| 2 | `C_native_openmp_01_FMA` | 579.952 | 553.249 | 517.083 |
| 3 | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 579.031 | 554.490 | 543.438 |
| 4 | `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 576.793 | 550.225 | 500.010 |
| 5 | `kernel_openmp_08_mkII_FIXED_PRECISION_FASTPATH` | 573.447 | 548.290 | 513.005 |
| 6 | `C_native_openmp_03` | 567.639 | 550.842 | 531.772 |
| 7 | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | 567.036 | 535.198 | 469.995 |
| 8 | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA` | 566.537 | 540.485 | 500.851 |
| 9 | `C_native_openmp_05` | 566.034 | 535.066 | 410.528 |
| 10 | `kernel_openmp_08_mkII` | 565.176 | 548.486 | 502.840 |
| 11 | `C_native_openmp_06` | 563.745 | 546.895 | 528.213 |
| 12 | `kernel_openmp_05_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 562.023 | 537.923 | 482.141 |
| 13 | `kernel_openmp_07_mkII_FMA` | 561.245 | 536.228 | 469.541 |
| 14 | `kernel_openmp_06_mkII_STABLE_ROUNDING` | 559.024 | 504.773 | 406.643 |
| 15 | `kernel_openmp_05_mkII_STABLE_ROUNDING_FMA` | 558.017 | 533.493 | 476.686 |
| 16 | `kernel_openmp_06_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 557.679 | 538.978 | 497.587 |
| 17 | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH` | 550.599 | 531.897 | 482.749 |
| 18 | `kernel_openmp_05_mkII_STABLE_ROUNDING` | 544.900 | 511.378 | 406.558 |
| 19 | `kernel_openmp_03_mkII` | 540.471 | 511.211 | 478.628 |
| 20 | `kernel_openmp_06_mkII` | 530.436 | 494.665 | 381.276 |
| 21 | `kernel_openmp_05_mkII` | 519.707 | 501.881 | 444.652 |
| 22 | `kernel_openmp_03_mkII_STABLE_ROUNDING` | 517.870 | 501.946 | 471.086 |
| 23 | `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA` | 517.831 | 509.728 | 500.769 |
| 24 | `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 512.224 | 478.600 | 436.642 |
| 25 | `kernel_openmp_04_mkII_STABLE_ROUNDING` | 501.692 | 476.330 | 440.866 |
| 26 | `C_native_openmp_04` | 501.256 | 486.650 | 463.768 |
| 27 | `C_native_openmp_01` | 487.749 | 468.523 | 450.908 |
| 28 | `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA` | 486.743 | 467.920 | 435.020 |
| 29 | `kernel_openmp_01_mkII_STABLE_ROUNDING` | 486.024 | 468.275 | 421.041 |
| 30 | `C_native_openmp_02` | 482.220 | 457.189 | 407.201 |
| 31 | `kernel_openmp_07_mkII` | 480.054 | 464.878 | 421.341 |
| 32 | `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA` | 474.669 | 457.848 | 437.516 |
| 33 | `kernel_openmp_04_mkII` | 469.190 | 453.294 | 410.895 |
| 34 | `kernel_openmp_02_mkII_STABLE_ROUNDING` | 468.320 | 447.013 | 426.376 |
| 35 | `kernel_openmp_01_mkII` | 420.727 | 405.447 | 365.891 |
| 36 | `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 418.415 | 406.422 | 389.264 |
| 37 | `kernel_openmp_02_mkII` | 407.451 | 389.757 | 350.555 |

</details>

<details>
<summary>OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|-----------:|-----------:|-----------:|
| 1 | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 579.031 | 554.490 | 543.438 |
| 2 | `C_native_openmp_01_FMA` | 579.952 | 553.249 | 517.083 |
| 3 | `C_native_openmp_03` | 567.639 | 550.842 | 531.772 |
| 4 | `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 576.793 | 550.225 | 500.010 |
| 5 | `kernel_openmp_08_mkII` | 565.176 | 548.486 | 502.840 |
| 6 | `kernel_openmp_08_mkII_FIXED_PRECISION_FASTPATH` | 573.447 | 548.290 | 513.005 |
| 7 | `kernel_openmp_06_mkII_STABLE_ROUNDING_FMA` | 582.990 | 547.109 | 510.640 |
| 8 | `C_native_openmp_06` | 563.745 | 546.895 | 528.213 |
| 9 | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA` | 566.537 | 540.485 | 500.851 |
| 10 | `kernel_openmp_06_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 557.679 | 538.978 | 497.587 |
| 11 | `kernel_openmp_05_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 562.023 | 537.923 | 482.141 |
| 12 | `kernel_openmp_07_mkII_FMA` | 561.245 | 536.228 | 469.541 |
| 13 | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | 567.036 | 535.198 | 469.995 |
| 14 | `C_native_openmp_05` | 566.034 | 535.066 | 410.528 |
| 15 | `kernel_openmp_05_mkII_STABLE_ROUNDING_FMA` | 558.017 | 533.493 | 476.686 |
| 16 | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH` | 550.599 | 531.897 | 482.749 |
| 17 | `kernel_openmp_05_mkII_STABLE_ROUNDING` | 544.900 | 511.378 | 406.558 |
| 18 | `kernel_openmp_03_mkII` | 540.471 | 511.211 | 478.628 |
| 19 | `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA` | 517.831 | 509.728 | 500.769 |
| 20 | `kernel_openmp_06_mkII_STABLE_ROUNDING` | 559.024 | 504.773 | 406.643 |
| 21 | `kernel_openmp_03_mkII_STABLE_ROUNDING` | 517.870 | 501.946 | 471.086 |
| 22 | `kernel_openmp_05_mkII` | 519.707 | 501.881 | 444.652 |
| 23 | `kernel_openmp_06_mkII` | 530.436 | 494.665 | 381.276 |
| 24 | `C_native_openmp_04` | 501.256 | 486.650 | 463.768 |
| 25 | `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 512.224 | 478.600 | 436.642 |
| 26 | `kernel_openmp_04_mkII_STABLE_ROUNDING` | 501.692 | 476.330 | 440.866 |
| 27 | `C_native_openmp_01` | 487.749 | 468.523 | 450.908 |
| 28 | `kernel_openmp_01_mkII_STABLE_ROUNDING` | 486.024 | 468.275 | 421.041 |
| 29 | `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA` | 486.743 | 467.920 | 435.020 |
| 30 | `kernel_openmp_07_mkII` | 480.054 | 464.878 | 421.341 |
| 31 | `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA` | 474.669 | 457.848 | 437.516 |
| 32 | `C_native_openmp_02` | 482.220 | 457.189 | 407.201 |
| 33 | `kernel_openmp_04_mkII` | 469.190 | 453.294 | 410.895 |
| 34 | `kernel_openmp_02_mkII_STABLE_ROUNDING` | 468.320 | 447.013 | 426.376 |
| 35 | `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 418.415 | 406.422 | 389.264 |
| 36 | `kernel_openmp_01_mkII` | 420.727 | 405.447 | 365.891 |
| 37 | `kernel_openmp_02_mkII` | 407.451 | 389.757 | 350.555 |

</details>

## Memory Bandwidth Estimates

The benchmark reports one dot-product operation as two FLOPs per element.  At
512-bit precision each MPFR mantissa has 8 limbs, or 64 bytes.  The minimum
payload stream for an Rdot element is therefore two input mantissas:

```text
payload bytes per element = 2 * 64 = 128 bytes
payload GB/s = MFLOPS * 128 / 2 / 1000 = MFLOPS * 0.064
```

That is a lower bound.  MPFR array traversal also reads each `mpfr_t` header to
get the `_mpfr_d` pointer before reaching the limb payload.  Counting only that
pointer adds 8 bytes per input object, or 16 bytes per dot-product element:

```text
payload + data-pointer bytes per element = 128 + 2 * 8 = 144 bytes
payload + data-pointer GB/s = MFLOPS * 144 / 2 / 1000 = MFLOPS * 0.072
```

For reference, counting the full 32-byte `mpfr_t` headers for both inputs gives
192 bytes per element:

```text
payload + full-header bytes per element = 128 + 2 * 32 = 192 bytes
payload + full-header GB/s = MFLOPS * 192 / 2 / 1000 = MFLOPS * 0.096
```

These estimates still ignore allocator locality, cache-line overfetch,
write-allocate effects for temporaries, and per-thread partial reductions.

| Variant | Avg MFLOPS | Limb payload GB/s | Limb + `_mpfr_d` pointer GB/s | Limb + full `mpfr_t` header GB/s |
|---------|-----------:|------------------:|------------------------------:|---------------------------------:|
| `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 554.490 | 35.487 | 39.923 | 53.231 |
| `C_native_openmp_01_FMA` | 553.249 | 35.408 | 39.834 | 53.112 |
| `C_native_openmp_03` | 550.842 | 35.254 | 39.661 | 52.881 |
| `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 550.225 | 35.214 | 39.616 | 52.822 |
| `kernel_openmp_08_mkII` | 548.486 | 35.103 | 39.491 | 52.655 |
| `kernel_openmp_08_mkII_FIXED_PRECISION_FASTPATH` | 548.290 | 35.091 | 39.477 | 52.636 |
| `kernel_openmp_06_mkII_STABLE_ROUNDING_FMA` | 547.109 | 35.015 | 39.392 | 52.522 |

The top MPFR OpenMP Rdot kernels use roughly 35 GB/s by the limb-only model,
40 GB/s when `_mpfr_d` pointer reads are included, and 53 GB/s if the full
`mpfr_t` input headers are counted.  These are model estimates derived from
MFLOPS, not hardware-counter measurements.  The achieved MFLOPS are still
controlled by MPFR call cost, rounding delivery, limb arithmetic, and reduction
structure; bandwidth estimates alone are not enough to explain the ranking.

## Hotpath Disassembly

The disassembly separates three effects that are easy to confuse in source
form: FMA fusion, product-temporary reuse, and rounding-mode delivery.

| Variant | Timed call shape | Rounding delivery | Result in this run |
|---------|------------------|-------------------|--------------------|
| `C_native_01_FMA` | one `mpfr_fma` per element | cached register | Serial raw FMA baseline, `23.203` Avg MFLOPS. |
| `C_native_03` | `mpfr_mul` + `mpfr_add` per element | cached register | Raw reusable-product non-FMA baseline, `23.070` Avg MFLOPS. |
| `kernel_01_mkII_STABLE_ROUNDING_FMA` | one `mpfr_fma` per element | TLS load in loop | Serial expression FMA wrapper, `23.151` Avg MFLOPS. |
| `kernel_openmp_03_mkII_STABLE_ROUNDING` | `mpfr_mul` + `mpfr_add` per element | TLS load before each call | Reusable-product OpenMP non-FMA path, `517.870` Max and `501.946` Avg MFLOPS. |
| `kernel_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | one `mpfr_fma` per element | cached context register | Explicit-context fixed-precision FMA path, `23.150` Avg MFLOPS. |
| `kernel_08_mkII_FIXED_PRECISION_FASTPATH` | `mpfr_mul` + `mpfr_add` per element | cached context register | Explicit-context counterpart to `C_native_03`, `23.046` Avg MFLOPS. |

The raw C FMA baseline loads the default rounding mode once before the loop and
passes the cached register to `mpfr_fma`:

```asm
# Rdot_mpfr_C_native_01_FMA::_Rdot
3a19: call   mpfr_get_default_rounding_mode@plt
3a29: mov    %eax,%r12d       # cached rounding
...
3a50: mov    %rbx,%rdx        # y[i]
3a53: mov    %r15,%rsi        # x[i]
3a56: mov    %r12d,%r8d       # cached rounding
3a59: mov    %rbp,%rcx        # accumulator addend
3a5c: mov    %rbp,%rdi        # accumulator destination
3a6b: call   mpfr_fma@plt
3a73: jne    3a50
```

The raw C reusable-product non-FMA baseline is `C_native_03`.  It hoists
`mpfr_get_default_rounding_mode()` out of the loop and then runs one
`mpfr_mul` into `templ` plus one `mpfr_add` into the accumulator per element.
This is the raw C kernel that corresponds to `kernel_08_mkII_FIXED_PRECISION_FASTPATH`.

```asm
# Rdot_mpfr_C_native_03::_Rdot
398b: call   mpfr_get_default_rounding_mode@plt
399e: mov    %eax,%r12d       # cached rounding
...
39f0: mov    %rbp,%rdx        # y[i]
39f3: mov    %rbx,%rsi        # x[i]
39f6: mov    %r12d,%ecx       # cached rounding
39f9: mov    %r13,%rdi        # product destination
39fc: call   mpfr_mul@plt
3a01: mov    %r12d,%ecx       # cached rounding
3a04: mov    %r13,%rdx        # product
3a07: mov    %r14,%rsi        # accumulator addend
3a0a: mov    %r14,%rdi        # accumulator destination
3a19: call   mpfr_add@plt
3a22: jne    39f0
```

The stable wrapper FMA path has the same arithmetic call shape, but the generic
stable-rounding route still loads the rounding value from TLS in the loop:

```asm
# Rdot_mpfr_kernel_01_mkII_STABLE_ROUNDING_FMA::_Rdot
38b0: mov    %r13,%rcx        # accumulator addend
38b3: mov    %rbp,%rdx        # y[i]
38b6: mov    %r12,%rsi        # x[i]
38b9: mov    %r13,%rdi        # accumulator destination
38bc: mov    %fs:0xfffffffffffffffc,%r8d
38c5: call   mpfr_fma@plt
38d9: jne    38b0
```

The OpenMP `kernel_openmp_03_mkII_STABLE_ROUNDING` hot loop is not an FMA loop.
It is the reusable-product non-FMA source shape: one `mpfr_mul` into the
thread-local product object, then one `mpfr_add` into the thread-local
accumulator.  In this build the stable-rounding path still appears as a TLS
load before both MPFR calls.  Its `501.946` Avg MFLOPS is below the top
OpenMP FMA/fixed class in this run.

```asm
# Rdot_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING::_Rdot._omp_fn.0
3970: mov    %rbx,%rdx        # y[i]
3973: mov    %r14,%rsi        # x[i]
3976: mov    %r12,%rdi        # product destination
3979: add    $0x1,%r15
397d: mov    %fs:0xfffffffffffffffc,%ecx  # TLS rounding load
3985: add    $0x20,%r14       # x++
3989: add    $0x20,%rbx       # y++
398d: call   mpfr_mul@plt
3992: mov    %r12,%rdx        # product
3995: mov    %rbp,%rsi        # accumulator addend
3998: mov    %rbp,%rdi        # accumulator destination
399b: mov    %fs:0xfffffffffffffffc,%ecx  # TLS rounding load
39a3: call   mpfr_add@plt
39a8: cmp    %r15,%r13
39ab: jne    3970
39ad: call   GOMP_barrier@plt
39b2: call   GOMP_critical_start@plt
39bc: mov    %fs:0xfffffffffffffffc,%ecx  # final reduction rounding
39ce: call   mpfr_add@plt
39d3: call   GOMP_critical_end@plt
```

The explicit-context fixed-precision FMA kernel captures the rounding mode
before the loop and keeps it in a register.  The fixed-precision target also
removes the context precision check from this benchmark source path.

```asm
# Rdot_mpfr_kernel_07_mkII_FIXED_PRECISION_FASTPATH_FMA::_Rdot
375c: call   mpfr_get_default_rounding_mode@plt
3767: mov    %eax,%r15d       # context rounding
...
3790: mov    %r15d,%r8d       # cached context rounding
3793: mov    %r12,%rcx        # accumulator addend
3796: mov    %rbp,%rdx        # y[i]
3799: mov    %rbx,%rsi        # x[i]
379c: mov    %r12,%rdi        # accumulator destination
379f: call   mpfr_fma@plt
37b3: jne    3790
```

The explicit-context fixed-precision reusable-product kernel is the wrapper
counterpart to `C_native_03`: one `mpfr_mul`, one `mpfr_add`, and cached
rounding in a register for both calls.  It does not call `mpfr_fma`; the point
of `08` is to test whether the wrapper can match the raw C non-FMA
reusable-product hotpath while using an explicit context.

```asm
# Rdot_mpfr_kernel_08_mkII_FIXED_PRECISION_FASTPATH::_Rdot
378f: call   mpfr_get_default_rounding_mode@plt
379a: mov    %eax,%r15d       # context rounding
...
37f0: mov    %r15d,%ecx       # cached context rounding
37f3: mov    %r12,%rdx        # y[i]
37f6: mov    %rbx,%rsi        # x[i]
37f9: mov    %r14,%rdi        # product destination
37fc: call   mpfr_mul@plt
3801: mov    %r15d,%ecx       # cached context rounding
3804: mov    %r14,%rdx        # product
3807: mov    %r13,%rsi        # accumulator addend
380a: mov    %r13,%rdi        # accumulator destination
380d: call   mpfr_add@plt
3822: jne    37f0
```

The key difference from GMP is that MPFR always takes an explicit rounding mode
argument.  Raw C and explicit-context wrapper kernels can keep that mode in a
register.  Generic wrapper kernels must either use the generic default lookup
path or rely on a stable-rounding build path that may still show a TLS load in
the disassembly.

## Comparison with GMP Rdot

GMP `mpf` arithmetic does not pass a rounding mode to every hot operation.
After product temporary construction is removed, the GMP wrapper hotpath can get
very close to the raw C loop.

MPFR has an extra policy axis.  Avoiding product temporary materialization is
necessary, but not sufficient: the hot loop also needs a stable or explicit
rounding source.  The current data show three useful MPFR tiers:

| Tier | Representative | Avg MFLOPS | Meaning |
|------|----------------|-----------:|---------|
| Generic wrapper expression | `kernel_01_mkII` | 16.490 | Product materialization and generic rounding delivery are both visible. |
| Stable reusable product / unrolled product | `kernel_03_mkII_STABLE_ROUNDING`, `kernel_06_mkII_STABLE_ROUNDING` | 23.307, 23.515 | Reused product objects plus stable rounding reach the raw C range. |
| Stable or explicit FMA | `kernel_01_mkII_STABLE_ROUNDING_FMA`, `kernel_07_mkII_FIXED_PRECISION_FASTPATH_FMA` | 23.151, 23.150 | One `mpfr_fma` per element; explicit context removes the TLS load. |

## Lessons Learned

`kernel_01` is the source shape that tests MPFR FMA fusion.  The expression
`acc += dx[i] * dy[i]` preserves the multiply-add pattern; in an FMA-enabled
build the hotpath can become one `mpfr_fma` call per element.  In this run,
`kernel_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` is the best
OpenMP wrapper average, while the serial `kernel_01` FMA variants are in the
same 23 MFLOPS class as raw C FMA.

`kernel_02` is the intentionally bad wrapper shape.  It constructs a product
object inside every iteration, so neither OpenMP nor FMA-oriented build flags
can turn it into the raw C hotpath.

`kernel_03` is the main reusable-product wrapper shape.  It reuses one product
object and can reach the raw C non-FMA range once rounding delivery is
stabilized.  In this run, its OpenMP FMA+fixed variant is in the top OpenMP
class, while the stable non-FMA variant is lower than the previous checked-in
run.

`kernel_04` shows that reusing an object is not enough.  The copy-then-multiply
source shape adds MPFR state movement and stays behind the cleaner reusable
product path.

`kernel_05` and `kernel_06` are controls for unrolling and product temporary
policy.  They can be useful as performance probes, but the `FMA` suffix on
these builds does not mean the loop actually calls `mpfr_fma`; the source shape
and disassembly decide that.  In this run, serial `kernel_06_mkII_STABLE_ROUNDING`
slightly leads the wrapper average table, but it is not a new semantic class.

`kernel_07` is the explicit-context version of `kernel_01`.  It captures
rounding outside the loop and, with FMA enabled, produces the closest wrapper
hotpath to raw C FMA.  The fixed-precision target removes context precision
checks, but the measured difference remains in the same serial performance
class.

`kernel_08` is the explicit-context version of `kernel_03`.  It is the clean
non-FMA control for the same API idea: one reused product object, one multiply,
one add, and no per-iteration default-rounding lookup.

OpenMP results should be read by average and min/max range, not by max alone.
The best wrapper OpenMP average in this run is
`kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` at
`554.490 MFLOPS`, essentially tied with raw FMA
`C_native_openmp_01_FMA` at `553.249 MFLOPS` and raw reusable-product
`C_native_openmp_03` at `550.842 MFLOPS`.  The range remains wide enough that single-run maxima are not
stable evidence.
