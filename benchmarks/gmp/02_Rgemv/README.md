<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 02_Rgemv

This directory benchmarks the GMP real dense matrix-vector product

```text
y <- alpha * A * x + beta * y
```

with fixed-precision `mpf_t`, upstream `gmpxx.h`, and `gmpxx_mkII` data. The
performance question is which source-level temporary policy and OpenMP work
partitioning shape determine the emitted GMP hot loop.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

Executables are created under:

```text
build_bench_release/benchmarks/gmp/02_Rgemv/
```

Each executable takes `<rows m> <cols n> <precision>`. Example:

```bash
build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_03_mkII 4000 4000 512
```

The repeat-10 run used:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread \
    benchmarks/gmp/02_Rgemv/run_repeat.sh build_bench_release 4000 4000 512 10
```

The mkII fixed-precision variants use `GMPFRXX_MKII_FAST_FIXED_PREC`;
executable suffixes keep the historical `FIXED_PRECISION_FASTPATH` label for
benchmark continuity.

The cross-benchmark runner can execute the GMP and MPFR `00_Rdot`, `01_Raxpy`, and `02_Rgemv` suites for both standard precisions with one command:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread \
    benchmarks/run_all.sh build_bench_release 512,1024 10 10000000 10000000 4000 4000
```

The second argument is a precision list. `both` and `all` are aliases for `512,1024`; a single value such as `512` still runs only that precision. Per-benchmark results are written to `results_raw/run_all_p512_repeat10_<timestamp>/` and `results_raw/run_all_p1024_repeat10_<timestamp>/` under each benchmark directory.

## Benchmark Parameters

| Parameter | Meaning |
| --- | --- |
| `m` | Number of matrix rows and length of `y`. |
| `n` | Number of matrix columns and length of `x`. |
| `precision` | Requested GMP `mpf` precision in bits for matrix/vector/scalar inputs and temporaries. |
| `repeat` | Number of timed process executions per executable. |
| `OMP_NUM_THREADS` | OpenMP worker count for `openmp` executables. |
| `OMP_PLACES`, `OMP_PROC_BIND` | OpenMP affinity controls used by the runner. |

The committed runs use `m=4000`, `n=4000`, `repeat=10`, `precision=512` and `precision=1024`, with `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, and `OMP_PROC_BIND=spread`.

## Variant Shapes

The timed body is `_Rgemv()`. `A` is stored in column-major order. The same numeric suffix has the same source-level meaning for raw C, upstream C++, mkII, serial, and OpenMP targets when that execution mode implements it. Serial targets cover `01`-`04`; OpenMP targets cover `01`-`07`.

| Variant | Transition from previous variant | Timed source shape | Temporary/resource policy | Purpose |
| --- | --- | --- | --- | --- |
| `01` | Baseline nested-expression shape for serial and OpenMP. | `y[i] += (alpha * x[j]) * A[i + j*lda]` | Product materializes inside the inner loop. Raw C initializes and clears a product `mpf_t` per matrix element. | Direct nested-expression stress case. |
| `02` | `01 -> 02`: introduce reusable copy-then-multiply temporaries. | `temp = alpha; temp *= x[j]; templ = temp; templ *= A[i + j*lda]; y[i] += templ` | `temp` and `templ` are initialized before the loops and reused. | Copy-then-multiply reusable-temporary path. |
| `03` | `02 -> 03`: keep reusable storage but assign temporaries from product expressions. | `temp = alpha * x[j]; templ = temp * A[i + j*lda]; y[i] += templ` | `temp` and `templ` are initialized before the loops and assigned from product expressions. | Main optimized serial wrapper baseline. |
| `04` | `03 -> 04`: move product object lifetime into the loop nest. | Loop-local `temp = alpha * x[j]`; loop-local `templ = temp * A[i + j*lda]`; `y[i] += templ` | Product objects are constructed inside the loop nest. | Lifetime/allocation stress case. |
| `05` | OpenMP branch from row-partitioned `03`: precompute `alpha * x[j]`. | Precompute `scaled_x[j] = alpha * x[j]`, then row-partitioned update. | Shared read-only `scaled_x`, per-thread reusable product object. | Remove repeated `alpha * x[j]` from row-partitioned OpenMP. |
| `06` | `05 -> 06`: add fixed 256-row blocking. | 256-row blocks, then column loop and contiguous row loop inside each block. | Per-thread reusable `temp` and `prod`. | Restore contiguous `A` access inside each row block. |
| `07` | `06 -> 07`: switch ownership to column partitioning with final reduction. | Column partitioning with thread-local partial `y` vectors and final reduction. | `num_threads * m` partial accumulators plus final reduction. | Keep serial-like column-major `A` streaming without racing on `y`. |

## Source Transitions

`01 -> 02` replaces direct nested products with reusable copy-then-multiply temporaries while keeping the column-major update structure. `02 -> 03` keeps reusable storage but assigns temporaries from product expressions, which is the main serial optimized comparison point. `03 -> 04` moves product object lifetime into the loop nest as a construction stress case. OpenMP `05` branches from the row-partitioned reusable class by precomputing `alpha * x`; `05 -> 06` adds fixed 256-row blocking; `06 -> 07` changes ownership from rows to columns and uses thread-local partial `y` vectors plus final reduction.

## C Native Equivalent Kernels

The mapping is based on the timed `_Rgemv()` hot-loop source shape, not just on
matching numeric suffixes.

| C native kernel | Equivalent C++ wrapper kernel(s) | Equivalence notes |
|-----------------|----------------------------------|-------------------|
| `C_native_01` | Closest to `kernel_01_*` | Raw C performs direct nested multiplication with a product `mpf_t` initialized and cleared for every matrix element. |
| `C_native_02` | `kernel_02_*` | Reusable `temp` and `templ` with explicit copy-then-multiply operations. |
| `C_native_03` | `kernel_03_*` | Reusable `temp_b` and `prod` assigned directly from product operations. This is the primary optimized serial C equivalent. |
| `C_native_04` | `kernel_04_*` | Loop-local `temp` and `templ` objects inside the loop nest. |
| `C_native_openmp_01` | `kernel_openmp_01_*` | Row-partitioned direct-expression stress shape. |
| `C_native_openmp_02` | `kernel_openmp_02_*` | Row-partitioned copy-then-multiply with per-thread reusable temporaries. |
| `C_native_openmp_03` | `kernel_openmp_03_*` | Row-partitioned expression-assignment path with per-thread reusable temporaries. |
| `C_native_openmp_04` | `kernel_openmp_04_*` | Row-partitioned loop-local product-object stress case. |
| `C_native_openmp_05` | `kernel_openmp_05_*` | Precomputed `scaled_x` row-partitioned path. |
| `C_native_openmp_06` | `kernel_openmp_06_*` | 256-row blocked row-partitioned path. |
| `C_native_openmp_07` | `kernel_openmp_07_*` | Column-partitioned thread-local partial-vector reduction path. |

`kernel_01_*` is an expression-template spelling, so its exact C native class is
confirmed by disassembly rather than by the suffix alone.

## Recorded Run

### 512-bit run

| Field | Value |
|-------|-------|
| Run ID | `run_all_p512_repeat10_20260526_062542` |
| Date | 2026-05-26 |
| CPU | AMD Ryzen Threadripper 3970X 32-Core Processor |
| OS | Linux 6.8.0-94-generic x86_64 |
| Compiler | `c++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0` |
| Build type | Release |
| Problem size | `m=4000`, `n=4000` |
| Precision | 512 bits |
| Repeat count | 10 |
| OpenMP | `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, `OMP_PROC_BIND=spread` |
| Default precision env | `GMPXX_DEFAULT_MPF_PRECISION_BITS=512` |
| Benchmark command | `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/run_all.sh build_bench_release 512,1024 10` |
| Raw result directory | `benchmarks/gmp/02_Rgemv/results_raw/run_all_p512_repeat10_20260526_062542/` |
| Raw log | `benchmarks/gmp/02_Rgemv/results_raw/run_all_p512_repeat10_20260526_062542/benchmark_rgemv_gmp_m4000_n4000_p512_repeat10.log` |
| Raw CSV | `benchmarks/gmp/02_Rgemv/results_raw/run_all_p512_repeat10_20260526_062542/raw_rgemv_gmp_m4000_n4000_p512_repeat10.csv` |
| Summary CSV | `benchmarks/gmp/02_Rgemv/results_raw/run_all_p512_repeat10_20260526_062542/summary_rgemv_gmp_m4000_n4000_p512_repeat10.csv` |
| Correctness | 440 / 440 runs reported OK. |

![GMP Rgemv serial 512-bit repeat-10](results_raw/run_all_p512_repeat10_20260526_062542/rgemv_gmp_m4000_n4000_p512_repeat10_serial.png)

![GMP Rgemv OpenMP 512-bit repeat-10](results_raw/run_all_p512_repeat10_20260526_062542/rgemv_gmp_m4000_n4000_p512_repeat10_openmp.png)

Plot regeneration command:

```bash
python3 benchmarks/gmp/02_Rgemv/plot_repeat_summary.py \
    benchmarks/gmp/02_Rgemv/results_raw/run_all_p512_repeat10_20260526_062542/benchmark_rgemv_gmp_m4000_n4000_p512_repeat10.log \
    --output-dir benchmarks/gmp/02_Rgemv/results_raw/run_all_p512_repeat10_20260526_062542 \
    --output-prefix rgemv_gmp_m4000_n4000_p512_repeat10 \
    --title-prefix "GMP Rgemv m=4000, n=4000, precision=512, repeat=10"
```

### 1024-bit run

| Field | Value |
|-------|-------|
| Run ID | `run_all_p1024_repeat10_20260526_062542` |
| Date | 2026-05-26 |
| CPU | AMD Ryzen Threadripper 3970X 32-Core Processor |
| OS | Linux 6.8.0-94-generic x86_64 |
| Compiler | `c++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0` |
| Build type | Release |
| Problem size | `m=4000`, `n=4000` |
| Precision | 1024 bits |
| Repeat count | 10 |
| OpenMP | `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, `OMP_PROC_BIND=spread` |
| Default precision env | `GMPXX_DEFAULT_MPF_PRECISION_BITS=1024` |
| Benchmark command | `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/run_all.sh build_bench_release 512,1024 10` |
| Raw result directory | `benchmarks/gmp/02_Rgemv/results_raw/run_all_p1024_repeat10_20260526_062542/` |
| Raw log | `benchmarks/gmp/02_Rgemv/results_raw/run_all_p1024_repeat10_20260526_062542/benchmark_rgemv_gmp_m4000_n4000_p1024_repeat10.log` |
| Raw CSV | `benchmarks/gmp/02_Rgemv/results_raw/run_all_p1024_repeat10_20260526_062542/raw_rgemv_gmp_m4000_n4000_p1024_repeat10.csv` |
| Summary CSV | `benchmarks/gmp/02_Rgemv/results_raw/run_all_p1024_repeat10_20260526_062542/summary_rgemv_gmp_m4000_n4000_p1024_repeat10.csv` |
| Correctness | 440 / 440 runs reported OK. |

![GMP Rgemv serial 1024-bit repeat-10](results_raw/run_all_p1024_repeat10_20260526_062542/rgemv_gmp_m4000_n4000_p1024_repeat10_serial.png)

![GMP Rgemv OpenMP 1024-bit repeat-10](results_raw/run_all_p1024_repeat10_20260526_062542/rgemv_gmp_m4000_n4000_p1024_repeat10_openmp.png)

Plot regeneration command:

```bash
python3 benchmarks/gmp/02_Rgemv/plot_repeat_summary.py \
    benchmarks/gmp/02_Rgemv/results_raw/run_all_p1024_repeat10_20260526_062542/benchmark_rgemv_gmp_m4000_n4000_p1024_repeat10.log \
    --output-dir benchmarks/gmp/02_Rgemv/results_raw/run_all_p1024_repeat10_20260526_062542 \
    --output-prefix rgemv_gmp_m4000_n4000_p1024_repeat10 \
    --title-prefix "GMP Rgemv m=4000, n=4000, precision=1024, repeat=10"
```

## Resource or Bandwidth Estimates

The values below are model estimates derived from MFLOPS, not hardware-counter measurements. They count active limb bytes plus a header-inclusive object model. They exclude allocator metadata, cache-line overfetch, instruction fetch, and final OpenMP reduction traffic.

| Case | Representative best-avg variant | Avg MFLOPS | Active bytes/iteration | Header-inclusive bytes/iteration | Active GB/s | Header-inclusive GB/s |
| --- | --- | --- | --- | --- | --- | --- |
| 512-bit serial | `kernel_03_mkII` | 31.521 | 192 | 264 | 3.026 | 4.161 |
| 512-bit OpenMP | `kernel_openmp_07_mkII` | 537.954 | 192 | 264 | 51.644 | 71.010 |
| 1024-bit serial | `kernel_03_mkII` | 11.247 | 384 | 456 | 2.160 | 2.564 |
| 1024-bit OpenMP | `C_native_openmp_07` | 260.114 | 384 | 456 | 49.942 | 59.306 |

For `Rgemv`, the per-iteration byte model is a compact arithmetic-stream estimate. It is not a full cache-footprint or hardware-bandwidth measurement.

## Headline Results

The headline rows below are regenerated from the committed 512-bit and 1024-bit `run_all` summary CSV files.

| Precision | Class | Variant | Max MFLOPS | Avg MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| 512 | Best serial max | `kernel_03_mkII` | 32.374 | 31.521 | mkII wrapper baseline for the numbered source shape. |
| 512 | Best serial average | `kernel_03_mkII` | 32.374 | 31.521 | mkII wrapper baseline for the numbered source shape. |
| 512 | Best OpenMP max | `kernel_openmp_07_orig` | 556.804 | 517.286 | Upstream gmpxx.h wrapper for the same numbered source shape. |
| 512 | Best OpenMP average | `kernel_openmp_07_mkII` | 551.376 | 537.954 | mkII wrapper baseline for the numbered source shape. |
| 1024 | Best serial max | `kernel_03_mkII_FIXED_PRECISION_FASTPATH` | 11.450 | 11.240 | Wrapper fixed-precision build; intended to remove repeated precision checks or scratch setup when the source shape allows it. |
| 1024 | Best serial average | `kernel_03_mkII` | 11.325 | 11.247 | mkII wrapper baseline for the numbered source shape. |
| 1024 | Best OpenMP max | `C_native_openmp_07` | 264.972 | 260.114 | Raw C OpenMP column-partitioned class with per-thread partial vectors and final reduction outside the hot loop. |
| 1024 | Best OpenMP average | `C_native_openmp_07` | 264.972 | 260.114 | Raw C OpenMP column-partitioned class with per-thread partial vectors and final reduction outside the hot loop. |

## Serial Results

### 512-bit serial interpretation

These rows are derived from `benchmarks/gmp/02_Rgemv/results_raw/run_all_p512_repeat10_20260526_062542/summary_rgemv_gmp_m4000_n4000_p512_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C serial avg | `C_native_03` | 31.724 | 31.486 | 31.350 | Raw C reference for the numbered source shape. |
| Best upstream serial avg | `kernel_03_orig` | 31.738 | 31.474 | 31.193 | Upstream gmpxx.h wrapper for the same numbered source shape. |
| Best mkII serial avg | `kernel_03_mkII` | 32.374 | 31.521 | 31.039 | mkII wrapper baseline for the numbered source shape. |
| Best serial max | `kernel_03_mkII` | 32.374 | 31.521 | 31.039 | mkII wrapper baseline for the numbered source shape. |

<details>
<summary>512-bit serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_03_mkII` | 32.374 | 31.521 | 31.039 |
| 2 | `kernel_03_mkII_FIXED_PRECISION_FASTPATH` | 31.806 | 31.367 | 31.106 |
| 3 | `kernel_03_orig` | 31.738 | 31.474 | 31.193 |
| 4 | `C_native_03` | 31.724 | 31.486 | 31.350 |
| 5 | `C_native_02` | 30.021 | 29.710 | 29.369 |
| 6 | `kernel_02_orig` | 29.949 | 29.692 | 29.307 |
| 7 | `kernel_02_mkII` | 29.922 | 29.717 | 29.385 |
| 8 | `kernel_02_mkII_FIXED_PRECISION_FASTPATH` | 29.896 | 29.745 | 29.467 |
| 9 | `C_native_04` | 28.032 | 27.754 | 27.456 |
| 10 | `kernel_04_mkII_FIXED_PRECISION_FASTPATH` | 27.203 | 26.567 | 25.761 |
| 11 | `kernel_04_orig` | 27.169 | 26.602 | 26.248 |
| 12 | `kernel_04_mkII` | 26.948 | 26.448 | 25.532 |
| 13 | `C_native_01` | 17.224 | 17.097 | 16.946 |
| 14 | `kernel_01_mkII` | 16.943 | 16.645 | 16.395 |
| 15 | `kernel_01_mkII_FIXED_PRECISION_FASTPATH` | 16.777 | 16.583 | 16.390 |
| 16 | `kernel_01_orig` | 16.164 | 15.805 | 15.646 |

</details>

<details>
<summary>512-bit serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_03_mkII` | 32.374 | 31.521 | 31.039 |
| 2 | `C_native_03` | 31.724 | 31.486 | 31.350 |
| 3 | `kernel_03_orig` | 31.738 | 31.474 | 31.193 |
| 4 | `kernel_03_mkII_FIXED_PRECISION_FASTPATH` | 31.806 | 31.367 | 31.106 |
| 5 | `kernel_02_mkII_FIXED_PRECISION_FASTPATH` | 29.896 | 29.745 | 29.467 |
| 6 | `kernel_02_mkII` | 29.922 | 29.717 | 29.385 |
| 7 | `C_native_02` | 30.021 | 29.710 | 29.369 |
| 8 | `kernel_02_orig` | 29.949 | 29.692 | 29.307 |
| 9 | `C_native_04` | 28.032 | 27.754 | 27.456 |
| 10 | `kernel_04_orig` | 27.169 | 26.602 | 26.248 |
| 11 | `kernel_04_mkII_FIXED_PRECISION_FASTPATH` | 27.203 | 26.567 | 25.761 |
| 12 | `kernel_04_mkII` | 26.948 | 26.448 | 25.532 |
| 13 | `C_native_01` | 17.224 | 17.097 | 16.946 |
| 14 | `kernel_01_mkII` | 16.943 | 16.645 | 16.395 |
| 15 | `kernel_01_mkII_FIXED_PRECISION_FASTPATH` | 16.777 | 16.583 | 16.390 |
| 16 | `kernel_01_orig` | 16.164 | 15.805 | 15.646 |

</details>

### 1024-bit serial interpretation

These rows are derived from `benchmarks/gmp/02_Rgemv/results_raw/run_all_p1024_repeat10_20260526_062542/summary_rgemv_gmp_m4000_n4000_p1024_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C serial avg | `C_native_03` | 11.315 | 11.244 | 11.140 | Raw C reference for the numbered source shape. |
| Best upstream serial avg | `kernel_03_orig` | 11.288 | 11.243 | 11.162 | Upstream gmpxx.h wrapper for the same numbered source shape. |
| Best mkII serial avg | `kernel_03_mkII` | 11.325 | 11.247 | 11.165 | mkII wrapper baseline for the numbered source shape. |
| Best serial max | `kernel_03_mkII_FIXED_PRECISION_FASTPATH` | 11.450 | 11.240 | 11.034 | Wrapper fixed-precision build; intended to remove repeated precision checks or scratch setup when the source shape allows it. |

<details>
<summary>1024-bit serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_03_mkII_FIXED_PRECISION_FASTPATH` | 11.450 | 11.240 | 11.034 |
| 2 | `kernel_03_mkII` | 11.325 | 11.247 | 11.165 |
| 3 | `C_native_03` | 11.315 | 11.244 | 11.140 |
| 4 | `kernel_03_orig` | 11.288 | 11.243 | 11.162 |
| 5 | `kernel_02_orig` | 11.270 | 10.973 | 10.836 |
| 6 | `kernel_02_mkII` | 11.035 | 10.941 | 10.879 |
| 7 | `C_native_02` | 11.013 | 10.931 | 10.835 |
| 8 | `C_native_04` | 11.008 | 10.772 | 10.557 |
| 9 | `kernel_02_mkII_FIXED_PRECISION_FASTPATH` | 11.008 | 10.928 | 10.848 |
| 10 | `kernel_04_mkII` | 10.837 | 10.560 | 10.320 |
| 11 | `kernel_04_mkII_FIXED_PRECISION_FASTPATH` | 10.788 | 10.618 | 10.497 |
| 12 | `kernel_04_orig` | 10.663 | 10.556 | 10.486 |
| 13 | `kernel_01_mkII_FIXED_PRECISION_FASTPATH` | 6.058 | 5.943 | 5.865 |
| 14 | `kernel_01_mkII` | 6.036 | 5.915 | 5.847 |
| 15 | `C_native_01` | 5.992 | 5.942 | 5.899 |
| 16 | `kernel_01_orig` | 5.988 | 5.855 | 5.746 |

</details>

<details>
<summary>1024-bit serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_03_mkII` | 11.325 | 11.247 | 11.165 |
| 2 | `C_native_03` | 11.315 | 11.244 | 11.140 |
| 3 | `kernel_03_orig` | 11.288 | 11.243 | 11.162 |
| 4 | `kernel_03_mkII_FIXED_PRECISION_FASTPATH` | 11.450 | 11.240 | 11.034 |
| 5 | `kernel_02_orig` | 11.270 | 10.973 | 10.836 |
| 6 | `kernel_02_mkII` | 11.035 | 10.941 | 10.879 |
| 7 | `C_native_02` | 11.013 | 10.931 | 10.835 |
| 8 | `kernel_02_mkII_FIXED_PRECISION_FASTPATH` | 11.008 | 10.928 | 10.848 |
| 9 | `C_native_04` | 11.008 | 10.772 | 10.557 |
| 10 | `kernel_04_mkII_FIXED_PRECISION_FASTPATH` | 10.788 | 10.618 | 10.497 |
| 11 | `kernel_04_mkII` | 10.837 | 10.560 | 10.320 |
| 12 | `kernel_04_orig` | 10.663 | 10.556 | 10.486 |
| 13 | `kernel_01_mkII_FIXED_PRECISION_FASTPATH` | 6.058 | 5.943 | 5.865 |
| 14 | `C_native_01` | 5.992 | 5.942 | 5.899 |
| 15 | `kernel_01_mkII` | 6.036 | 5.915 | 5.847 |
| 16 | `kernel_01_orig` | 5.988 | 5.855 | 5.746 |

</details>

## OpenMP Results

### 512-bit OpenMP interpretation

These rows are derived from `benchmarks/gmp/02_Rgemv/results_raw/run_all_p512_repeat10_20260526_062542/summary_rgemv_gmp_m4000_n4000_p512_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C OpenMP avg | `C_native_openmp_07` | 555.445 | 529.617 | 454.980 | Raw C OpenMP column-partitioned class with per-thread partial vectors and final reduction outside the hot loop. |
| Best upstream OpenMP avg | `kernel_openmp_07_orig` | 556.804 | 517.286 | 457.541 | Upstream gmpxx.h wrapper for the same numbered source shape. |
| Best mkII OpenMP avg | `kernel_openmp_07_mkII` | 551.376 | 537.954 | 523.087 | mkII wrapper baseline for the numbered source shape. |
| Best OpenMP max | `kernel_openmp_07_orig` | 556.804 | 517.286 | 457.541 | Upstream gmpxx.h wrapper for the same numbered source shape. |

<details>
<summary>512-bit OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_07_orig` | 556.804 | 517.286 | 457.541 |
| 2 | `C_native_openmp_07` | 555.445 | 529.617 | 454.980 |
| 3 | `kernel_openmp_07_mkII` | 551.376 | 537.954 | 523.087 |
| 4 | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH` | 542.088 | 505.722 | 362.642 |
| 5 | `C_native_openmp_06` | 406.909 | 396.411 | 387.930 |
| 6 | `kernel_openmp_06_orig` | 406.561 | 391.603 | 377.618 |
| 7 | `kernel_openmp_06_mkII` | 404.801 | 397.345 | 381.106 |
| 8 | `kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH` | 400.413 | 390.351 | 367.536 |
| 9 | `kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH` | 296.484 | 287.192 | 256.246 |
| 10 | `kernel_openmp_05_mkII` | 295.000 | 289.400 | 282.518 |
| 11 | `C_native_openmp_05` | 293.632 | 290.723 | 283.988 |
| 12 | `kernel_openmp_05_orig` | 292.134 | 286.045 | 268.277 |
| 13 | `kernel_openmp_03_orig` | 244.923 | 241.073 | 236.339 |
| 14 | `kernel_openmp_03_mkII` | 244.296 | 239.304 | 231.197 |
| 15 | `kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH` | 243.842 | 241.450 | 233.852 |
| 16 | `C_native_openmp_03` | 242.924 | 240.229 | 234.754 |
| 17 | `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH` | 238.852 | 235.130 | 230.450 |
| 18 | `C_native_openmp_02` | 238.391 | 236.388 | 232.640 |
| 19 | `C_native_openmp_01` | 237.496 | 230.629 | 196.692 |
| 20 | `kernel_openmp_02_mkII` | 234.857 | 228.228 | 214.304 |
| 21 | `kernel_openmp_01_mkII` | 233.601 | 227.338 | 197.155 |
| 22 | `kernel_openmp_02_orig` | 232.454 | 227.139 | 217.021 |
| 23 | `C_native_openmp_04` | 231.837 | 226.502 | 212.972 |
| 24 | `kernel_openmp_01_orig` | 229.711 | 226.108 | 221.688 |
| 25 | `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH` | 228.521 | 223.881 | 206.229 |
| 26 | `kernel_openmp_04_mkII` | 226.603 | 221.184 | 197.978 |
| 27 | `kernel_openmp_04_orig` | 226.330 | 223.785 | 218.140 |
| 28 | `kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH` | 221.370 | 218.796 | 216.473 |

</details>

<details>
<summary>512-bit OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_07_mkII` | 551.376 | 537.954 | 523.087 |
| 2 | `C_native_openmp_07` | 555.445 | 529.617 | 454.980 |
| 3 | `kernel_openmp_07_orig` | 556.804 | 517.286 | 457.541 |
| 4 | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH` | 542.088 | 505.722 | 362.642 |
| 5 | `kernel_openmp_06_mkII` | 404.801 | 397.345 | 381.106 |
| 6 | `C_native_openmp_06` | 406.909 | 396.411 | 387.930 |
| 7 | `kernel_openmp_06_orig` | 406.561 | 391.603 | 377.618 |
| 8 | `kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH` | 400.413 | 390.351 | 367.536 |
| 9 | `C_native_openmp_05` | 293.632 | 290.723 | 283.988 |
| 10 | `kernel_openmp_05_mkII` | 295.000 | 289.400 | 282.518 |
| 11 | `kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH` | 296.484 | 287.192 | 256.246 |
| 12 | `kernel_openmp_05_orig` | 292.134 | 286.045 | 268.277 |
| 13 | `kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH` | 243.842 | 241.450 | 233.852 |
| 14 | `kernel_openmp_03_orig` | 244.923 | 241.073 | 236.339 |
| 15 | `C_native_openmp_03` | 242.924 | 240.229 | 234.754 |
| 16 | `kernel_openmp_03_mkII` | 244.296 | 239.304 | 231.197 |
| 17 | `C_native_openmp_02` | 238.391 | 236.388 | 232.640 |
| 18 | `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH` | 238.852 | 235.130 | 230.450 |
| 19 | `C_native_openmp_01` | 237.496 | 230.629 | 196.692 |
| 20 | `kernel_openmp_02_mkII` | 234.857 | 228.228 | 214.304 |
| 21 | `kernel_openmp_01_mkII` | 233.601 | 227.338 | 197.155 |
| 22 | `kernel_openmp_02_orig` | 232.454 | 227.139 | 217.021 |
| 23 | `C_native_openmp_04` | 231.837 | 226.502 | 212.972 |
| 24 | `kernel_openmp_01_orig` | 229.711 | 226.108 | 221.688 |
| 25 | `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH` | 228.521 | 223.881 | 206.229 |
| 26 | `kernel_openmp_04_orig` | 226.330 | 223.785 | 218.140 |
| 27 | `kernel_openmp_04_mkII` | 226.603 | 221.184 | 197.978 |
| 28 | `kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH` | 221.370 | 218.796 | 216.473 |

</details>

### 1024-bit OpenMP interpretation

These rows are derived from `benchmarks/gmp/02_Rgemv/results_raw/run_all_p1024_repeat10_20260526_062542/summary_rgemv_gmp_m4000_n4000_p1024_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C OpenMP avg | `C_native_openmp_07` | 264.972 | 260.114 | 248.080 | Raw C OpenMP column-partitioned class with per-thread partial vectors and final reduction outside the hot loop. |
| Best upstream OpenMP avg | `kernel_openmp_07_orig` | 263.929 | 255.328 | 232.990 | Upstream gmpxx.h wrapper for the same numbered source shape. |
| Best mkII OpenMP avg | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH` | 263.026 | 255.771 | 237.278 | Wrapper fixed-precision build; intended to remove repeated precision checks or scratch setup when the source shape allows it. |
| Best OpenMP max | `C_native_openmp_07` | 264.972 | 260.114 | 248.080 | Raw C OpenMP column-partitioned class with per-thread partial vectors and final reduction outside the hot loop. |

<details>
<summary>1024-bit OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_openmp_07` | 264.972 | 260.114 | 248.080 |
| 2 | `kernel_openmp_07_orig` | 263.929 | 255.328 | 232.990 |
| 3 | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH` | 263.026 | 255.771 | 237.278 |
| 4 | `kernel_openmp_07_mkII` | 262.329 | 255.508 | 246.161 |
| 5 | `kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH` | 192.233 | 187.965 | 181.872 |
| 6 | `C_native_openmp_05` | 191.769 | 189.473 | 185.391 |
| 7 | `kernel_openmp_05_orig` | 191.370 | 187.821 | 176.665 |
| 8 | `kernel_openmp_05_mkII` | 190.581 | 185.441 | 167.709 |
| 9 | `C_native_openmp_06` | 161.091 | 158.241 | 156.033 |
| 10 | `kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH` | 161.015 | 159.709 | 157.777 |
| 11 | `kernel_openmp_06_mkII` | 160.941 | 159.366 | 157.994 |
| 12 | `kernel_openmp_06_orig` | 160.475 | 159.039 | 157.505 |
| 13 | `C_native_openmp_03` | 131.690 | 129.469 | 124.314 |
| 14 | `kernel_openmp_03_mkII` | 131.556 | 129.920 | 126.764 |
| 15 | `kernel_openmp_03_orig` | 131.287 | 128.984 | 120.362 |
| 16 | `kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH` | 130.617 | 128.978 | 124.448 |
| 17 | `kernel_openmp_01_mkII` | 128.682 | 125.271 | 110.005 |
| 18 | `C_native_openmp_02` | 128.422 | 120.700 | 116.953 |
| 19 | `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH` | 128.341 | 127.280 | 125.692 |
| 20 | `kernel_openmp_02_mkII` | 127.985 | 126.959 | 125.638 |
| 21 | `C_native_openmp_04` | 127.708 | 126.180 | 124.239 |
| 22 | `kernel_openmp_01_orig` | 127.615 | 126.243 | 124.627 |
| 23 | `kernel_openmp_02_orig` | 126.986 | 121.107 | 104.717 |
| 24 | `kernel_openmp_04_orig` | 126.096 | 122.772 | 105.057 |
| 25 | `kernel_openmp_04_mkII` | 125.992 | 124.817 | 123.190 |
| 26 | `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH` | 125.968 | 124.728 | 121.386 |
| 27 | `kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH` | 123.543 | 121.718 | 118.622 |
| 28 | `C_native_openmp_01` | 123.301 | 120.224 | 117.766 |

</details>

<details>
<summary>1024-bit OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_openmp_07` | 264.972 | 260.114 | 248.080 |
| 2 | `kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH` | 263.026 | 255.771 | 237.278 |
| 3 | `kernel_openmp_07_mkII` | 262.329 | 255.508 | 246.161 |
| 4 | `kernel_openmp_07_orig` | 263.929 | 255.328 | 232.990 |
| 5 | `C_native_openmp_05` | 191.769 | 189.473 | 185.391 |
| 6 | `kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH` | 192.233 | 187.965 | 181.872 |
| 7 | `kernel_openmp_05_orig` | 191.370 | 187.821 | 176.665 |
| 8 | `kernel_openmp_05_mkII` | 190.581 | 185.441 | 167.709 |
| 9 | `kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH` | 161.015 | 159.709 | 157.777 |
| 10 | `kernel_openmp_06_mkII` | 160.941 | 159.366 | 157.994 |
| 11 | `kernel_openmp_06_orig` | 160.475 | 159.039 | 157.505 |
| 12 | `C_native_openmp_06` | 161.091 | 158.241 | 156.033 |
| 13 | `kernel_openmp_03_mkII` | 131.556 | 129.920 | 126.764 |
| 14 | `C_native_openmp_03` | 131.690 | 129.469 | 124.314 |
| 15 | `kernel_openmp_03_orig` | 131.287 | 128.984 | 120.362 |
| 16 | `kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH` | 130.617 | 128.978 | 124.448 |
| 17 | `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH` | 128.341 | 127.280 | 125.692 |
| 18 | `kernel_openmp_02_mkII` | 127.985 | 126.959 | 125.638 |
| 19 | `kernel_openmp_01_orig` | 127.615 | 126.243 | 124.627 |
| 20 | `C_native_openmp_04` | 127.708 | 126.180 | 124.239 |
| 21 | `kernel_openmp_01_mkII` | 128.682 | 125.271 | 110.005 |
| 22 | `kernel_openmp_04_mkII` | 125.992 | 124.817 | 123.190 |
| 23 | `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH` | 125.968 | 124.728 | 121.386 |
| 24 | `kernel_openmp_04_orig` | 126.096 | 122.772 | 105.057 |
| 25 | `kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH` | 123.543 | 121.718 | 118.622 |
| 26 | `kernel_openmp_02_orig` | 126.986 | 121.107 | 104.717 |
| 27 | `C_native_openmp_02` | 128.422 | 120.700 | 116.953 |
| 28 | `C_native_openmp_01` | 123.301 | 120.224 | 117.766 |

</details>

## Hotpath Disassembly

Representative commands:

```bash
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_03_mkII
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_07_mkII
```

The current representative hotpaths were compared against the C native,
upstream wrapper, and mkII variants.

| Representative | Hotpath observation | Comparison point |
|----------------|---------------------|------------------|
| `C_native_03` | Column-major reusable-product loop. Each column forms `temp = alpha * x[j]`; the inner row loop has one `__gmpf_mul` and one `__gmpf_add` per matrix element. Temporary init/clear is outside the inner loop. | Raw serial comparison point for wrapper `03`. |
| `kernel_03_orig` | Same column-major reusable-product backend call sequence as `C_native_03`. | Equivalent arithmetic hot loop to C native. |
| `kernel_03_mkII` | Same inner-loop multiply/add sequence as `C_native_03`; mkII setup and precision handling are outside the matrix-element loop. | Equivalent arithmetic hot loop to C native, with wrapper setup outside the hot path. |
| `C_native_openmp_07` | Column partitioning with thread-local partial `y` vectors. The worker loop keeps the serial-like column-major stream and reduces partial vectors after the parallel work. | Raw OpenMP baseline for variant `07`. |
| `kernel_openmp_07_orig` / `kernel_openmp_07_mkII` | Same algorithmic structure as `C_native_openmp_07`: per-thread partial outputs, reusable temporaries, and final reduction outside the worker hot loop. | Equivalent arithmetic class to C native OpenMP `07`. |

Representative loop classes:

```asm
# Serial 03 inner row loop
call   __gmpf_mul@plt     # product = A[i,j] * temp
call   __gmpf_add@plt     # y[i] += product
jne    <row loop>

# OpenMP 07 worker loop has the same arithmetic class; reduction is outside it.
call   __gmpf_mul@plt
call   __gmpf_add@plt
jne    <worker row loop>
```

## Lessons Learned

Serial Rgemv is still dominated by the reusable column-major product shape.
For 512-bit serial runs, `kernel_03_mkII` is the winner, but the C native,
orig, and mkII `03` variants all disassemble to the same backend arithmetic
class.

For 1024-bit serial runs, the max winner
(`kernel_03_mkII_FIXED_PRECISION_FASTPATH`) and average winner (`kernel_03_mkII`)
are separated by less than 0.1% in average MFLOPS. That is not enough to claim
a durable fixed-precision win for the already reusable-temporary source.

The OpenMP `07` algorithm is a real source-level change: column partitioning
with thread-local partial `y` vectors avoids racing on `y` while preserving the
column-major stream. At 512-bit, `kernel_openmp_07_orig` has the highest max
but `kernel_openmp_07_mkII` has the highest average; the lower minimum on the
orig run indicates OpenMP variance rather than a stable orig-only advantage.

At 1024-bit OpenMP, `C_native_openmp_07` leads both max and average. The wrapper
variants remain in the same algorithmic class, but the raw C path has the
least surrounding control overhead.

The useful implementation lesson is algorithmic rather than syntactic: variant
`07` changes the parallel dataflow, while serial wrapper spelling mostly
collapses to the same GMP multiply/add hot loop.
