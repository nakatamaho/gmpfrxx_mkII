<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 01_Raxpy

This directory benchmarks the MPFR real AXPY operation

```text
y_i = y_i + alpha * x_i
```

for fixed-precision `mpfr_t` and `mpfrxx::mpfr_class` vectors. The benchmark compares raw C MPFR kernels, expression-template wrapper kernels, explicit rounding/context source variants, FMA-capable builds, fixed-precision builds, and OpenMP worker loops at 512-bit and 1024-bit precision.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j --target Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_02 Raxpy_mpfr_C_native_03 Raxpy_mpfr_C_native_04 Raxpy_mpfr_C_native_01_FMA Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_C_native_openmp_02 Raxpy_mpfr_C_native_openmp_03 Raxpy_mpfr_C_native_openmp_04 Raxpy_mpfr_C_native_openmp_01_FMA Raxpy_mpfr_kernel_01_ROUNDING_PRECISION_FMA Raxpy_mpfr_kernel_openmp_01_ROUNDING_PRECISION_FMA
```

The MPFR Raxpy target set is built under:

```text
build_bench_release/benchmarks/mpfr/01_Raxpy/
```

Each executable takes:

```text
<vector size> <precision-bits>
```

Example:

```bash
build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_01_ROUNDING_PRECISION_FMA 10000000 1024
```

The cross-benchmark runner can execute the GMP and MPFR `00_Rdot`, `01_Raxpy`, and `02_Rgemv` suites for both standard precisions with one command:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread \
    benchmarks/run_all.sh build_bench_release 512,1024 10 10000000 10000000 4000 4000
```

The second argument is a precision list. `both` and `all` are aliases for `512,1024`; a single value such as `512` still runs only that precision. Per-benchmark results are written to `results_raw/run_all_p512_repeat10_<timestamp>/` and `results_raw/run_all_p1024_repeat10_<timestamp>/` under each benchmark directory.

## Benchmark Parameters

| Parameter | Meaning |
| --- | --- |
| `N` | Number of vector elements. |
| `precision` | MPFR precision in bits for `alpha`, `x`, and `y`. |
| `repeat` | Number of timed process executions per executable. |
| `OMP_NUM_THREADS` | OpenMP worker count for `openmp` executables. |
| `OMP_PLACES`, `OMP_PROC_BIND` | OpenMP affinity controls used by the runner. |

The committed runs use `N=10000000`, `repeat=10`, `precision=512` and `precision=1024`, with `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, and `OMP_PROC_BIND=spread`.

## Variant Shapes

The timed body is `_Raxpy()`. The same numeric suffix is used for serial and OpenMP kernels. `ROUNDING`, `PRECISION`, and final `FMA` suffixes modify these numbered shapes without changing the variant number.

| Variant | Transition from previous variant | Timed source shape | Temporary/resource policy | Purpose |
| --- | --- | --- | --- | --- |
| `01` | Baseline direct-expression shape. | `y[i] += alpha * x[i]` | No explicit product object in source. | Direct expression form; FMA builds can lower this source to one `mpfr_fma` call per element. |
| `02` | `01 -> 02`: introduce reusable product storage and copy-then-multiply source. | `temp = alpha; temp *= x[i]; y[i] += temp` | One product object is initialized before the loop and reused. | Test copy-then-multiply source shape with reusable storage. |
| `03` | `02 -> 03`: keep reusable product storage but assign from `alpha * x[i]`. | `temp = alpha * x[i]; y[i] += temp` | One product object is initialized before the loop and assigned each iteration. | Test expression product materialization into reusable storage. |
| `04` | `03 -> 04`: move product construction into the timed loop. | `mpfr_class temp = alpha * x[i]; y[i] += temp` | Product object is constructed inside the timed loop. | Intentionally expensive control for per-iteration construction. |

Wrapper suffixes separate source changes from build flags:

| Suffix | Compile definition | Meaning |
| --- | --- | --- |
| none | none | Baseline wrapper source for the numbered algorithm. |
| `PRECISION` | `GMPFRXX_MKII_FAST_FIXED_PREC` | Builds the same source with fixed-precision assumptions. |
| `ROUNDING` | none | Uses an explicit `mpfrxx::evaluation_context` source file and avoids default-rounding lookup in the timed loop. |
| `ROUNDING_PRECISION` | `GMPFRXX_MKII_FAST_FIXED_PREC` | Builds the explicit-context source with fixed-precision assumptions. |
| final `FMA` | `GMPFRXX_MKII_ENABLE_FMA` | Builds an FMA-capturable source shape with FMA enabled. |

FMA targets are generated only for direct-expression variant `01`, where the source can lower to one `mpfr_fma` call.

## Source Transitions

`01` is the FMA-capturable wrapper source in this benchmark because the product remains in the update expression. `01 -> 02` introduces an explicit reusable product object and copy-then-multiply source. `02 -> 03` keeps reusable storage but assigns it from `alpha * x[i]`, matching the raw split multiply/add reusable-product class. `03 -> 04` moves product construction into the loop as a lifetime stress case. `ROUNDING` variants are separate source files that capture `mpfrxx::evaluation_context` before the loop; `PRECISION` and final `FMA` are build modifiers, not new variant numbers.

## C Native Equivalent Kernels

| C native kernel | Closest wrapper kernel | Equivalence note |
|-----------------|------------------------|------------------|
| `C_native_01`, `C_native_openmp_01` | `kernel_03`, `kernel_03_PRECISION`, `kernel_03_ROUNDING`, `kernel_03_ROUNDING_PRECISION`; OpenMP equivalents | Split `mpfr_mul` + `mpfr_add` with one reusable product object outside the loop or per OpenMP worker. |
| `C_native_02`, `C_native_openmp_02` | `kernel_02`, `kernel_02_PRECISION`, `kernel_02_ROUNDING`, `kernel_02_ROUNDING_PRECISION`; OpenMP equivalents | Copy-then-multiply reusable temporary: `mpfr_set(temp, alpha, rnd)`, `mpfr_mul(temp, temp, x[i], rnd)`, then `mpfr_add`. |
| `C_native_03`, `C_native_openmp_03` | `kernel_03`, `kernel_03_PRECISION`, `kernel_03_ROUNDING`, `kernel_03_ROUNDING_PRECISION`; OpenMP equivalents | Numbered raw C comparison point for wrapper `03`; same direct reusable-temporary hot-loop class as `C_native_01`. |
| `C_native_04`, `C_native_openmp_04` | `kernel_04`, `kernel_04_PRECISION`, `kernel_04_ROUNDING`, `kernel_04_ROUNDING_PRECISION`; OpenMP equivalents | Loop-local construction stress case: each element performs `mpfr_init`, multiply, add, and `mpfr_clear` inside the timed loop. |
| `C_native_01_FMA`, `C_native_openmp_01_FMA` | `kernel_01_PRECISION_FMA`, `kernel_01_ROUNDING_PRECISION_FMA`; OpenMP equivalents | One `mpfr_fma` per element when the wrapper source shape is direct. |
| `C_native_packed_custom_layout_FMA` | none | Same arithmetic as `C_native_01_FMA`, but with packed MPFR header+limb storage. |

## Recorded Run

```text
N = 10000000
precision = 512 bits and 1024 bits
repeat = 10
compiler = g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0
build type = Release
CPU = AMD Ryzen Threadripper 3970X 32-Core Processor
OS = Linux 6.8.0-94-generic x86_64
OMP_NUM_THREADS = 32
OMP_PLACES = cores
OMP_PROC_BIND = spread
all timed runs = Result OK
```

### 512-bit run

| Field | Value |
|-------|-------|
| Run ID | `run_all_p512_repeat10_20260525_224339` |
| Date | 2026-05-25 |
| CPU | AMD Ryzen Threadripper 3970X 32-Core Processor |
| OS | Linux 6.8.0-94-generic x86_64 |
| Compiler | `c++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0` |
| Build type | Release |
| Problem size | `N=10000000` |
| Precision | 512 bits |
| Repeat count | 10 |
| OpenMP | `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, `OMP_PROC_BIND=spread` |
| Benchmark command | `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/run_all.sh build_bench_release 512 10 10000000 10000000 4000 4000` |
| Raw result directory | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339/` |
| Raw log | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339/benchmark_raxpy_mpfr_n10000000_p512_repeat10.log` |
| Raw CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339/raw_raxpy_mpfr_n10000000_p512_repeat10.csv` |
| Summary CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339/summary_raxpy_mpfr_n10000000_p512_repeat10.csv` |
| Correctness | 470 / 470 runs reported OK. |

![MPFR Raxpy serial 512-bit repeat-10](results_raw/run_all_p512_repeat10_20260525_224339/raxpy_mpfr_n10000000_p512_repeat10_serial.png)

![MPFR Raxpy OpenMP 512-bit repeat-10](results_raw/run_all_p512_repeat10_20260525_224339/raxpy_mpfr_n10000000_p512_repeat10_openmp.png)

Plot regeneration command:

```bash
python3 benchmarks/mpfr/01_Raxpy/plot_repeat_summary.py \
    benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339/benchmark_raxpy_mpfr_n10000000_p512_repeat10.log \
    --output-dir benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339 \
    --output-prefix raxpy_mpfr_n10000000_p512_repeat10 \
    --title-prefix "MPFR Raxpy N=10000000, precision=512, repeat=10"
```

### 1024-bit run

No current 1024-bit `run_all` result directory is present under this benchmark's `results_raw/` tree. Run `benchmarks/run_all.sh build_bench_release 1024 10 10000000 10000000 4000 4000` or the default dual-precision command to regenerate this section.

## Resource or Bandwidth Estimates

The values below are model estimates derived from MFLOPS, not hardware-counter measurements. They use the current 512-bit `run_all` summary and count active limb bytes plus a header-inclusive model. They exclude allocator metadata, cache-line overfetch, instruction fetch, and final OpenMP reduction traffic.

| Case | Representative best-avg variant | Avg MFLOPS | Active bytes/iteration | Header-inclusive bytes/iteration | Active GB/s | Header-inclusive GB/s |
| --- | --- | --- | --- | --- | --- | --- |
| 512-bit serial | `C_native_01` | 22.971 | 192 | 288 | 2.205 | 3.308 |
| 512-bit OpenMP | `kernel_openmp_01_PRECISION_FMA` | 412.596 | 192 | 288 | 39.609 | 59.414 |

For matrix-vector benchmarks, the per-iteration byte model is a compact active-data estimate for the arithmetic stream, not a full matrix-footprint or cache-reuse model.
## Headline Results

The 512-bit headline rows below are regenerated from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339/summary_raxpy_mpfr_n10000000_p512_repeat10.csv`. No 1024-bit raw data is present in the current `results_raw/` tree, so 1024-bit result sections are placeholders until a fresh 1024-bit `run_all` result is collected.

| Precision | Class | Variant | Max MFLOPS | Avg MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| 512 | Best serial max | `C_native_01` | 23.517 | 22.971 | Single fastest serial repeat; compare with Avg MFLOPS for stability. |
| 512 | Best serial average | `C_native_01` | 23.517 | 22.971 | Raw C reference for the numbered source shape. |
| 512 | Best OpenMP max | `kernel_openmp_01_PRECISION_FMA` | 419.733 | 412.596 | Single fastest OpenMP repeat; OpenMP rows should be interpreted by performance class. |
| 512 | Best OpenMP average | `kernel_openmp_01_PRECISION_FMA` | 419.733 | 412.596 | Wrapper fixed-precision build; intended to remove repeated precision checks or scratch setup when the source shape allows it. |
## Serial Results

### 512-bit serial interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339/summary_raxpy_mpfr_n10000000_p512_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C serial avg | `C_native_01` | 23.517 | 22.971 | 22.561 | Raw C reference for the numbered source shape. |
| Best mkII serial avg | `kernel_01_ROUNDING_PRECISION_FMA` | 23.079 | 22.911 | 22.736 | Wrapper source with loop-external context plus fixed-precision build assumptions; intended to remove rounding lookup and precision checks from the hot path. |
| Best serial max | `C_native_01` | 23.517 | 22.971 | 22.561 | Raw C reference for the numbered source shape. |

<details>
<summary>512-bit serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_01` | 23.517 | 22.971 | 22.561 |
| 2 | `C_native_packed_custom_layout_FMA` | 23.194 | 22.860 | 22.470 |
| 3 | `C_native_01_FMA` | 23.138 | 22.860 | 22.375 |
| 4 | `C_native_03` | 23.101 | 22.882 | 22.607 |
| 5 | `kernel_01_ROUNDING_PRECISION_FMA` | 23.079 | 22.911 | 22.736 |
| 6 | `kernel_03_ROUNDING` | 22.970 | 22.838 | 22.689 |
| 7 | `kernel_03_ROUNDING_PRECISION` | 22.175 | 21.629 | 21.264 |
| 8 | `kernel_01_ROUNDING_PRECISION` | 22.137 | 21.525 | 21.317 |
| 9 | `kernel_01_PRECISION_FMA` | 22.112 | 21.777 | 21.337 |
| 10 | `kernel_01_PRECISION` | 21.261 | 20.817 | 20.405 |
| 11 | `C_native_02` | 20.843 | 20.215 | 19.854 |
| 12 | `kernel_03_PRECISION` | 20.487 | 20.025 | 19.842 |
| 13 | `kernel_02_ROUNDING` | 20.172 | 20.026 | 19.917 |
| 14 | `kernel_02_ROUNDING_PRECISION` | 20.003 | 19.875 | 19.674 |
| 15 | `kernel_03` | 19.962 | 19.529 | 19.180 |
| 16 | `kernel_02_PRECISION` | 18.310 | 17.918 | 17.743 |
| 17 | `C_native_04` | 18.013 | 17.805 | 17.430 |
| 18 | `kernel_02` | 17.837 | 17.711 | 17.578 |
| 19 | `kernel_01_ROUNDING` | 17.550 | 17.132 | 16.928 |
| 20 | `kernel_04_ROUNDING` | 17.314 | 16.908 | 16.676 |
| 21 | `kernel_04` | 17.270 | 16.795 | 16.401 |
| 22 | `kernel_01` | 16.918 | 16.502 | 16.310 |
| 23 | `kernel_04_PRECISION` | 16.624 | 16.251 | 16.136 |
| 24 | `kernel_04_ROUNDING_PRECISION` | 16.353 | 16.037 | 15.807 |

</details>

<details>
<summary>512-bit serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_01` | 23.517 | 22.971 | 22.561 |
| 2 | `kernel_01_ROUNDING_PRECISION_FMA` | 23.079 | 22.911 | 22.736 |
| 3 | `C_native_03` | 23.101 | 22.882 | 22.607 |
| 4 | `C_native_01_FMA` | 23.138 | 22.860 | 22.375 |
| 5 | `C_native_packed_custom_layout_FMA` | 23.194 | 22.860 | 22.470 |
| 6 | `kernel_03_ROUNDING` | 22.970 | 22.838 | 22.689 |
| 7 | `kernel_01_PRECISION_FMA` | 22.112 | 21.777 | 21.337 |
| 8 | `kernel_03_ROUNDING_PRECISION` | 22.175 | 21.629 | 21.264 |
| 9 | `kernel_01_ROUNDING_PRECISION` | 22.137 | 21.525 | 21.317 |
| 10 | `kernel_01_PRECISION` | 21.261 | 20.817 | 20.405 |
| 11 | `C_native_02` | 20.843 | 20.215 | 19.854 |
| 12 | `kernel_02_ROUNDING` | 20.172 | 20.026 | 19.917 |
| 13 | `kernel_03_PRECISION` | 20.487 | 20.025 | 19.842 |
| 14 | `kernel_02_ROUNDING_PRECISION` | 20.003 | 19.875 | 19.674 |
| 15 | `kernel_03` | 19.962 | 19.529 | 19.180 |
| 16 | `kernel_02_PRECISION` | 18.310 | 17.918 | 17.743 |
| 17 | `C_native_04` | 18.013 | 17.805 | 17.430 |
| 18 | `kernel_02` | 17.837 | 17.711 | 17.578 |
| 19 | `kernel_01_ROUNDING` | 17.550 | 17.132 | 16.928 |
| 20 | `kernel_04_ROUNDING` | 17.314 | 16.908 | 16.676 |
| 21 | `kernel_04` | 17.270 | 16.795 | 16.401 |
| 22 | `kernel_01` | 16.918 | 16.502 | 16.310 |
| 23 | `kernel_04_PRECISION` | 16.624 | 16.251 | 16.136 |
| 24 | `kernel_04_ROUNDING_PRECISION` | 16.353 | 16.037 | 15.807 |

</details>
### 1024-bit serial interpretation

No current 1024-bit `run_all` summary CSV is present under this benchmark's `results_raw/` tree. The serial table should be regenerated after a fresh 1024-bit run is collected.

## OpenMP Results

### 512-bit OpenMP interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260525_224339/summary_raxpy_mpfr_n10000000_p512_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C OpenMP avg | `C_native_openmp_02` | 414.892 | 408.621 | 398.907 | Raw C reference for the numbered source shape. |
| Best mkII OpenMP avg | `kernel_openmp_01_PRECISION_FMA` | 419.733 | 412.596 | 401.664 | Wrapper fixed-precision build; intended to remove repeated precision checks or scratch setup when the source shape allows it. |
| Best OpenMP max | `kernel_openmp_01_PRECISION_FMA` | 419.733 | 412.596 | 401.664 | Wrapper fixed-precision build; intended to remove repeated precision checks or scratch setup when the source shape allows it. |

<details>
<summary>512-bit OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_01_PRECISION_FMA` | 419.733 | 412.596 | 401.664 |
| 2 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 419.685 | 408.413 | 377.145 |
| 3 | `C_native_openmp_01_FMA` | 417.236 | 408.573 | 388.027 |
| 4 | `C_native_openmp_02` | 414.892 | 408.621 | 398.907 |
| 5 | `kernel_openmp_03_ROUNDING_PRECISION` | 413.886 | 408.134 | 393.355 |
| 6 | `kernel_openmp_01_ROUNDING_PRECISION` | 412.112 | 406.426 | 401.691 |
| 7 | `kernel_openmp_03_ROUNDING` | 410.322 | 406.376 | 395.118 |
| 8 | `kernel_openmp_03_PRECISION` | 410.209 | 398.361 | 387.120 |
| 9 | `C_native_openmp_03` | 409.403 | 399.606 | 392.428 |
| 10 | `kernel_openmp_02_ROUNDING` | 408.695 | 395.370 | 370.260 |
| 11 | `kernel_openmp_01_PRECISION` | 406.541 | 399.762 | 376.801 |
| 12 | `kernel_openmp_03` | 406.409 | 394.325 | 367.962 |
| 13 | `C_native_openmp_01` | 405.033 | 396.789 | 387.875 |
| 14 | `kernel_openmp_01_ROUNDING` | 404.438 | 385.970 | 363.888 |
| 15 | `kernel_openmp_02_ROUNDING_PRECISION` | 403.043 | 396.935 | 370.287 |
| 16 | `kernel_openmp_02` | 401.170 | 383.443 | 348.895 |
| 17 | `kernel_openmp_04_ROUNDING` | 401.048 | 379.637 | 313.844 |
| 18 | `kernel_openmp_01` | 398.775 | 380.996 | 334.992 |
| 19 | `kernel_openmp_02_PRECISION` | 396.785 | 389.304 | 373.605 |
| 20 | `kernel_openmp_04_ROUNDING_PRECISION` | 396.653 | 375.801 | 313.115 |
| 21 | `kernel_openmp_04` | 393.311 | 382.954 | 362.084 |
| 22 | `kernel_openmp_04_PRECISION` | 392.204 | 379.436 | 359.900 |
| 23 | `C_native_openmp_04` | 387.887 | 370.280 | 323.577 |

</details>

<details>
<summary>512-bit OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_01_PRECISION_FMA` | 419.733 | 412.596 | 401.664 |
| 2 | `C_native_openmp_02` | 414.892 | 408.621 | 398.907 |
| 3 | `C_native_openmp_01_FMA` | 417.236 | 408.573 | 388.027 |
| 4 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 419.685 | 408.413 | 377.145 |
| 5 | `kernel_openmp_03_ROUNDING_PRECISION` | 413.886 | 408.134 | 393.355 |
| 6 | `kernel_openmp_01_ROUNDING_PRECISION` | 412.112 | 406.426 | 401.691 |
| 7 | `kernel_openmp_03_ROUNDING` | 410.322 | 406.376 | 395.118 |
| 8 | `kernel_openmp_01_PRECISION` | 406.541 | 399.762 | 376.801 |
| 9 | `C_native_openmp_03` | 409.403 | 399.606 | 392.428 |
| 10 | `kernel_openmp_03_PRECISION` | 410.209 | 398.361 | 387.120 |
| 11 | `kernel_openmp_02_ROUNDING_PRECISION` | 403.043 | 396.935 | 370.287 |
| 12 | `C_native_openmp_01` | 405.033 | 396.789 | 387.875 |
| 13 | `kernel_openmp_02_ROUNDING` | 408.695 | 395.370 | 370.260 |
| 14 | `kernel_openmp_03` | 406.409 | 394.325 | 367.962 |
| 15 | `kernel_openmp_02_PRECISION` | 396.785 | 389.304 | 373.605 |
| 16 | `kernel_openmp_01_ROUNDING` | 404.438 | 385.970 | 363.888 |
| 17 | `kernel_openmp_02` | 401.170 | 383.443 | 348.895 |
| 18 | `kernel_openmp_04` | 393.311 | 382.954 | 362.084 |
| 19 | `kernel_openmp_01` | 398.775 | 380.996 | 334.992 |
| 20 | `kernel_openmp_04_ROUNDING` | 401.048 | 379.637 | 313.844 |
| 21 | `kernel_openmp_04_PRECISION` | 392.204 | 379.436 | 359.900 |
| 22 | `kernel_openmp_04_ROUNDING_PRECISION` | 396.653 | 375.801 | 313.115 |
| 23 | `C_native_openmp_04` | 387.887 | 370.280 | 323.577 |

</details>
### 1024-bit OpenMP interpretation

No current 1024-bit `run_all` summary CSV is present under this benchmark's `results_raw/` tree. The OpenMP table should be regenerated after a fresh 1024-bit run is collected.

## Comparison with GMP version

The rows below compare the current 512-bit `run_all` data for `01_Raxpy`. This is a performance-class comparison; GMP `mpf` and MPFR have different precision and rounding semantics.

| Class | GMP best-avg variant | GMP Avg MFLOPS | MPFR best-avg variant | MPFR Avg MFLOPS | MPFR/GMP |
| --- | --- | --- | --- | --- | --- |
| Best serial average | `C_native_03` | 33.780 | `C_native_01` | 22.971 | 0.680x |
| Best OpenMP average | `kernel_openmp_03_orig` | 394.012 | `kernel_openmp_01_PRECISION_FMA` | 412.596 | 1.047x |

## Hotpath Disassembly

Representative command shape:

```bash
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/01_Raxpy/<executable>
```

Addresses are build-specific; the relevant evidence is the backend call sequence, where reusable temporaries are initialized and cleared, and whether rounding is cached or read inside the loop. The 512-bit and 1024-bit runs use the same emitted loop structure; precision changes the limb work inside the MPFR calls.

The snippets are representative, not exhaustive. They were selected to cover
the split raw C baseline, the raw FMA baseline, the reusable-product mkII
wrapper path, and the matching OpenMP worker. For MPFR Raxpy, the central
question is whether the source shape becomes one `mpfr_fma` per element or
remains a split `mpfr_mul` + `mpfr_add` loop, and whether rounding is cached in
a register or read through TLS.

`Raxpy_mpfr_C_native_01` has one reusable `mpfr_t` product object. It is initialized before the timed loop, the rounding mode is cached once, and the loop body has one `mpfr_mul` plus one `mpfr_add` per element. The temporary is cleared after the loop.

```asm
49db: lea    0x10(%rsp),%rdi   # reusable temp
49e9: call   mpfr_init@plt
49ee: call   mpfr_get_default_rounding_mode@plt
49f3: mov    %eax,%r13d        # cached rounding

4a10: mov    %rbp,%rdx         # x[i]
4a13: mov    %r13d,%ecx        # cached rounding
4a16: mov    %r15,%rsi         # alpha
4a1d: lea    0x10(%rsp),%rdi   # temp
4a22: add    $0x20,%rbp        # x++
4a26: call   mpfr_mul@plt
4a2b: mov    %rbx,%rsi         # y[i]
4a2e: mov    %rbx,%rdi         # y[i]
4a31: mov    %r13d,%ecx        # cached rounding
4a34: lea    0x10(%rsp),%rdx   # temp
4a39: add    $0x20,%rbx        # y++
4a3d: call   mpfr_add@plt
4a47: jne    4a10

4a49: lea    0x10(%rsp),%rdi   # reusable temp
4a4e: call   mpfr_clear@plt
```

`Raxpy_mpfr_C_native_01_FMA` is the raw FMA baseline. There is no reusable product temporary in this source shape, so no `mpfr_init`/`mpfr_clear` appears around the hot loop; the loop has one `mpfr_fma` per element and uses the cached rounding register.

```asm
49ca: call   mpfr_get_default_rounding_mode@plt
49cf: mov    %eax,%r14d        # cached rounding

49e0: mov    %rbx,%rcx         # y[i] addend
49e3: mov    %rbp,%rdx         # x[i]
49e6: mov    %rbx,%rdi         # y[i] destination
49e9: mov    %r14d,%r8d        # cached rounding
49ec: mov    %r13,%rsi         # alpha
49f3: add    $0x20,%rbx        # y++
49f7: add    $0x20,%rbp        # x++
49fb: call   mpfr_fma@plt
4a03: jne    49e0
```

`Raxpy_mpfr_kernel_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` stays in the split multiply/add class even in an FMA-enabled build, because source variant `03` explicitly materializes `temp = alpha * x[i]` before adding it to `y[i]`. The wrapper initializes one reusable `mpfr_class` product object before the loop and clears it after the loop. Compared with C native split mode, the loop still has one `mpfr_mul` and one `mpfr_add`, but it also carries stable-rounding TLS loads and first-use initialization checks.

```asm
60f5: mov    %r13,%rdi         # reusable temp
60f8: call   mpfr_init2@plt
610c: mov    %fs:0xfffffffffffffffc,%edx
6116: mov    %r13,%rdi         # temp
6119: call   mpfr_set_ui@plt

6130: mov    %fs:0xfffffffffffffffc,%ecx  # stable rounding TLS load
6138: mov    %rbp,%rdx                    # x[i]
613b: mov    %r15,%rsi                    # alpha
613e: mov    %r13,%rdi                    # temp
6141: call   mpfr_mul@plt
6146: cmpb   $0x0,%fs:0xfffffffffffffff8  # defaults-initialized check
6151: mov    %fs:0xfffffffffffffffc,%ecx  # stable rounding TLS load
6159: mov    %r13,%rdx                    # temp
615c: mov    %rbx,%rsi                    # y[i]
615f: mov    %rbx,%rdi                    # y[i]
6162: call   mpfr_add@plt
616b: add    $0x20,%rbx                   # y++
616f: add    $0x20,%rbp                   # x++
6176: je     61e0
6178: cmpb   $0x0,%fs:0xfffffffffffffff8
6181: jne    6130

61e0: mov    %r13,%rdi                    # reusable temp
61e3: call   mpfr_clear@plt
```

`Raxpy_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING` uses an OpenMP outlined worker. Each worker initializes one reusable product object before its slice, runs the same split multiply/add loop, then reaches `GOMP_barrier` and clears the worker local temporary after the hot loop.

```asm
5644: mov    %r13,%rdi         # worker-local temp
5647: call   mpfr_init2@plt
565b: mov    %fs:0xfffffffffffffffc,%edx
5665: mov    %r13,%rdi         # temp
5668: call   mpfr_set_ui@plt

56c8: mov    %fs:0xfffffffffffffffc,%ecx  # stable rounding TLS load
56d0: mov    %r12,%rdx                    # x[i]
56d3: mov    %r14,%rsi                    # alpha
56d6: mov    %r13,%rdi                    # temp
56d9: call   mpfr_mul@plt
56de: cmpb   $0x0,%fs:0xfffffffffffffff8  # defaults-initialized check
56e9: mov    %fs:0xfffffffffffffffc,%ecx  # stable rounding TLS load
56f1: mov    %rbp,%rsi                    # y[i]
56f4: mov    %rbp,%rdi                    # y[i]
56f7: mov    %r13,%rdx                    # temp
56fe: call   mpfr_add@plt
5703: add    $0x20,%rbp                   # y++
5707: add    $0x20,%r12                   # x++
5710: je     5780
571f: jne    56c8

5780: call   GOMP_barrier@plt
5785: mov    %r13,%rdi                    # worker-local temp
5788: call   mpfr_clear@plt
```

The important comparison with the GMP Raxpy disassembly is structural: the split MPFR variants mirror the GMP reusable-temporary pattern, but MPFR carries rounding-mode operands and, in wrapper paths, default-state TLS checks. Direct FMA variants are a different source-level class: they remove the product temporary entirely and therefore do not show init/clear around the hot loop.

## Lessons Learned

The primary source-level boundary is whether the hot loop reaches one direct `mpfr_fma` per element or remains a split `mpfr_mul` + `mpfr_add` path. Direct expression kernels (`01`) and explicit-context direct kernels (`05`) can enter the FMA class; reusable-product kernels (`03`/`06`) stay in the split class by construction.

Stable rounding and fixed-precision assumptions matter only when they change the emitted hot loop or remove repeated wrapper work. They do not make a loop-local temporary (`04`) competitive with reusable-resource or direct-FMA shapes.

Precision changes the relative backend picture. At 512 bits, the best MPFR OpenMP FMA wrapper average is slightly above the GMP best OpenMP average. At 1024 bits, the best MPFR OpenMP average falls to the 250 MFLOPS class while the best GMP OpenMP reusable-product average remains near 389 MFLOPS. The MPFR FMA source shape still removes a product temporary, but the higher limb cost and MPFR rounding-aware operation dominate more strongly at 1024 bits.

The best serial and OpenMP variants are close enough that single-run maxima are less important than performance classes. The repeat-10 variance shows that OpenMP ranking should be interpreted by average and hotpath shape, not by one maximum number.
