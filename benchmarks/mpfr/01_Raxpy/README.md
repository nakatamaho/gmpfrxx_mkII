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

### 512-bit run

| Field | Value |
|-------|-------|
| Run ID | `run_all_p512_repeat10_20260526_062542` |
| Date | 2026-05-26 |
| CPU | AMD Ryzen Threadripper 3970X 32-Core Processor |
| OS | Linux 6.8.0-94-generic x86_64 |
| Compiler | `c++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0` |
| Build type | Release |
| Problem size | `N=10000000` |
| Precision | 512 bits |
| Repeat count | 10 |
| OpenMP | `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, `OMP_PROC_BIND=spread` |
| Default precision env | `MPFRXX_DEFAULT_PRECISION_BITS=512` |
| Benchmark command | `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/run_all.sh build_bench_release 512,1024 10` |
| Raw result directory | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260526_062542/` |
| Raw log | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260526_062542/benchmark_raxpy_mpfr_n10000000_p512_repeat10.log` |
| Raw CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260526_062542/raw_raxpy_mpfr_n10000000_p512_repeat10.csv` |
| Summary CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260526_062542/summary_raxpy_mpfr_n10000000_p512_repeat10.csv` |
| Correctness | 470 / 470 runs reported OK. |

![MPFR Raxpy serial 512-bit repeat-10](results_raw/run_all_p512_repeat10_20260526_062542/raxpy_mpfr_n10000000_p512_repeat10_serial.png)

![MPFR Raxpy OpenMP 512-bit repeat-10](results_raw/run_all_p512_repeat10_20260526_062542/raxpy_mpfr_n10000000_p512_repeat10_openmp.png)

Plot regeneration command:

```bash
python3 benchmarks/mpfr/01_Raxpy/plot_repeat_summary.py \
    benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260526_062542/benchmark_raxpy_mpfr_n10000000_p512_repeat10.log \
    --output-dir benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260526_062542 \
    --output-prefix raxpy_mpfr_n10000000_p512_repeat10 \
    --title-prefix "MPFR Raxpy N=10000000, precision=512, repeat=10"
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
| Problem size | `N=10000000` |
| Precision | 1024 bits |
| Repeat count | 10 |
| OpenMP | `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, `OMP_PROC_BIND=spread` |
| Default precision env | `MPFRXX_DEFAULT_PRECISION_BITS=1024` |
| Benchmark command | `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/run_all.sh build_bench_release 512,1024 10` |
| Raw result directory | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260526_062542/` |
| Raw log | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260526_062542/benchmark_raxpy_mpfr_n10000000_p1024_repeat10.log` |
| Raw CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260526_062542/raw_raxpy_mpfr_n10000000_p1024_repeat10.csv` |
| Summary CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260526_062542/summary_raxpy_mpfr_n10000000_p1024_repeat10.csv` |
| Correctness | 470 / 470 runs reported OK. |

![MPFR Raxpy serial 1024-bit repeat-10](results_raw/run_all_p1024_repeat10_20260526_062542/raxpy_mpfr_n10000000_p1024_repeat10_serial.png)

![MPFR Raxpy OpenMP 1024-bit repeat-10](results_raw/run_all_p1024_repeat10_20260526_062542/raxpy_mpfr_n10000000_p1024_repeat10_openmp.png)

Plot regeneration command:

```bash
python3 benchmarks/mpfr/01_Raxpy/plot_repeat_summary.py \
    benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260526_062542/benchmark_raxpy_mpfr_n10000000_p1024_repeat10.log \
    --output-dir benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260526_062542 \
    --output-prefix raxpy_mpfr_n10000000_p1024_repeat10 \
    --title-prefix "MPFR Raxpy N=10000000, precision=1024, repeat=10"
```

## Resource or Bandwidth Estimates

The values below are model estimates derived from MFLOPS, not hardware-counter measurements. They count active limb bytes plus a header-inclusive object model. They exclude allocator metadata, cache-line overfetch, instruction fetch, and final OpenMP reduction traffic.

| Case | Representative best-avg variant | Avg MFLOPS | Active bytes/iteration | Header-inclusive bytes/iteration | Active GB/s | Header-inclusive GB/s |
| --- | --- | --- | --- | --- | --- | --- |
| 512-bit serial | `kernel_01_ROUNDING_PRECISION_FMA` | 22.981 | 192 | 288 | 2.206 | 3.309 |
| 512-bit OpenMP | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 414.911 | 192 | 288 | 39.831 | 59.747 |
| 1024-bit serial | `C_native_01_FMA` | 9.341 | 384 | 480 | 1.794 | 2.242 |
| 1024-bit OpenMP | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 254.585 | 384 | 480 | 48.880 | 61.100 |

For `Raxpy`, the per-iteration byte model is a compact arithmetic-stream estimate. It is not a full cache-footprint or hardware-bandwidth measurement.

## Headline Results

The headline rows below are regenerated from the committed 512-bit and 1024-bit `run_all` summary CSV files.

| Precision | Class | Variant | Max MFLOPS | Avg MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| 512 | Best serial max | `kernel_03_ROUNDING` | 23.402 | 22.875 | Wrapper source captures rounding/context outside the loop to avoid default-rounding lookup in the timed body. |
| 512 | Best serial average | `kernel_01_ROUNDING_PRECISION_FMA` | 23.147 | 22.981 | Wrapper source with loop-external rounding and fixed-precision FMA build. |
| 512 | Best OpenMP max | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 422.132 | 414.911 | Wrapper source with loop-external rounding and fixed-precision FMA build. |
| 512 | Best OpenMP average | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 422.132 | 414.911 | Wrapper source with loop-external rounding and fixed-precision FMA build. |
| 1024 | Best serial max | `C_native_01_FMA` | 9.572 | 9.341 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| 1024 | Best serial average | `C_native_01_FMA` | 9.572 | 9.341 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| 1024 | Best OpenMP max | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 258.848 | 254.585 | Wrapper source with loop-external rounding and fixed-precision FMA build. |
| 1024 | Best OpenMP average | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 258.848 | 254.585 | Wrapper source with loop-external rounding and fixed-precision FMA build. |

## Serial Results

### 512-bit serial interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260526_062542/summary_raxpy_mpfr_n10000000_p512_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C serial avg | `C_native_01_FMA` | 23.184 | 22.930 | 22.661 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| Best wrapper serial avg | `kernel_01_ROUNDING_PRECISION_FMA` | 23.147 | 22.981 | 22.812 | Wrapper source with loop-external rounding and fixed-precision FMA build. |
| Best serial max | `kernel_03_ROUNDING` | 23.402 | 22.875 | 22.396 | Wrapper source captures rounding/context outside the loop to avoid default-rounding lookup in the timed body. |

<details>
<summary>512-bit serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_03_ROUNDING` | 23.402 | 22.875 | 22.396 |
| 2 | `C_native_01_FMA` | 23.184 | 22.930 | 22.661 |
| 3 | `kernel_01_ROUNDING_PRECISION_FMA` | 23.147 | 22.981 | 22.812 |
| 4 | `C_native_packed_custom_layout_FMA` | 23.089 | 22.819 | 22.549 |
| 5 | `C_native_01` | 23.078 | 22.846 | 22.687 |
| 6 | `C_native_03` | 22.984 | 22.768 | 22.265 |
| 7 | `kernel_01_PRECISION_FMA` | 22.050 | 21.709 | 21.168 |
| 8 | `kernel_03_ROUNDING_PRECISION` | 21.757 | 21.543 | 21.340 |
| 9 | `kernel_01_ROUNDING_PRECISION` | 21.707 | 21.585 | 21.437 |
| 10 | `kernel_01_PRECISION` | 20.971 | 20.714 | 20.445 |
| 11 | `C_native_02` | 20.302 | 20.107 | 19.899 |
| 12 | `kernel_02_ROUNDING` | 20.164 | 19.977 | 19.725 |
| 13 | `kernel_03_PRECISION` | 20.073 | 19.989 | 19.817 |
| 14 | `kernel_02_ROUNDING_PRECISION` | 19.994 | 19.816 | 19.574 |
| 15 | `kernel_03` | 19.585 | 19.404 | 19.179 |
| 16 | `C_native_04` | 18.137 | 17.925 | 17.483 |
| 17 | `kernel_02` | 17.994 | 17.756 | 17.352 |
| 18 | `kernel_02_PRECISION` | 17.950 | 17.773 | 17.602 |
| 19 | `kernel_04_ROUNDING` | 17.322 | 16.721 | 16.378 |
| 20 | `kernel_01_ROUNDING` | 17.165 | 17.056 | 16.913 |
| 21 | `kernel_04` | 17.078 | 16.743 | 16.583 |
| 22 | `kernel_01` | 16.580 | 16.515 | 16.370 |
| 23 | `kernel_04_PRECISION` | 16.332 | 16.171 | 15.996 |
| 24 | `kernel_04_ROUNDING_PRECISION` | 16.239 | 15.950 | 15.784 |

</details>

<details>
<summary>512-bit serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_01_ROUNDING_PRECISION_FMA` | 23.147 | 22.981 | 22.812 |
| 2 | `C_native_01_FMA` | 23.184 | 22.930 | 22.661 |
| 3 | `kernel_03_ROUNDING` | 23.402 | 22.875 | 22.396 |
| 4 | `C_native_01` | 23.078 | 22.846 | 22.687 |
| 5 | `C_native_packed_custom_layout_FMA` | 23.089 | 22.819 | 22.549 |
| 6 | `C_native_03` | 22.984 | 22.768 | 22.265 |
| 7 | `kernel_01_PRECISION_FMA` | 22.050 | 21.709 | 21.168 |
| 8 | `kernel_01_ROUNDING_PRECISION` | 21.707 | 21.585 | 21.437 |
| 9 | `kernel_03_ROUNDING_PRECISION` | 21.757 | 21.543 | 21.340 |
| 10 | `kernel_01_PRECISION` | 20.971 | 20.714 | 20.445 |
| 11 | `C_native_02` | 20.302 | 20.107 | 19.899 |
| 12 | `kernel_03_PRECISION` | 20.073 | 19.989 | 19.817 |
| 13 | `kernel_02_ROUNDING` | 20.164 | 19.977 | 19.725 |
| 14 | `kernel_02_ROUNDING_PRECISION` | 19.994 | 19.816 | 19.574 |
| 15 | `kernel_03` | 19.585 | 19.404 | 19.179 |
| 16 | `C_native_04` | 18.137 | 17.925 | 17.483 |
| 17 | `kernel_02_PRECISION` | 17.950 | 17.773 | 17.602 |
| 18 | `kernel_02` | 17.994 | 17.756 | 17.352 |
| 19 | `kernel_01_ROUNDING` | 17.165 | 17.056 | 16.913 |
| 20 | `kernel_04` | 17.078 | 16.743 | 16.583 |
| 21 | `kernel_04_ROUNDING` | 17.322 | 16.721 | 16.378 |
| 22 | `kernel_01` | 16.580 | 16.515 | 16.370 |
| 23 | `kernel_04_PRECISION` | 16.332 | 16.171 | 15.996 |
| 24 | `kernel_04_ROUNDING_PRECISION` | 16.239 | 15.950 | 15.784 |

</details>

### 1024-bit serial interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260526_062542/summary_raxpy_mpfr_n10000000_p1024_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C serial avg | `C_native_01_FMA` | 9.572 | 9.341 | 9.251 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| Best wrapper serial avg | `kernel_01_ROUNDING_PRECISION_FMA` | 9.404 | 9.332 | 9.276 | Wrapper source with loop-external rounding and fixed-precision FMA build. |
| Best serial max | `C_native_01_FMA` | 9.572 | 9.341 | 9.251 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |

<details>
<summary>1024-bit serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_01_FMA` | 9.572 | 9.341 | 9.251 |
| 2 | `kernel_01_ROUNDING_PRECISION_FMA` | 9.404 | 9.332 | 9.276 |
| 3 | `C_native_packed_custom_layout_FMA` | 9.369 | 9.244 | 9.186 |
| 4 | `C_native_02` | 9.249 | 8.993 | 8.880 |
| 5 | `kernel_02_ROUNDING_PRECISION` | 9.216 | 8.974 | 8.865 |
| 6 | `kernel_03_ROUNDING` | 9.177 | 8.960 | 8.879 |
| 7 | `C_native_01` | 9.119 | 9.055 | 9.003 |
| 8 | `kernel_03_PRECISION` | 9.105 | 8.891 | 8.818 |
| 9 | `C_native_03` | 9.092 | 9.037 | 8.975 |
| 10 | `kernel_01_PRECISION_FMA` | 9.066 | 9.019 | 8.954 |
| 11 | `kernel_03_ROUNDING_PRECISION` | 9.066 | 9.021 | 8.973 |
| 12 | `kernel_02_ROUNDING` | 9.060 | 8.946 | 8.851 |
| 13 | `kernel_03` | 8.845 | 8.761 | 8.668 |
| 14 | `kernel_01_ROUNDING_PRECISION` | 8.802 | 8.644 | 7.967 |
| 15 | `kernel_01_PRECISION` | 8.793 | 8.737 | 8.643 |
| 16 | `kernel_02` | 8.671 | 8.590 | 8.464 |
| 17 | `kernel_02_PRECISION` | 8.629 | 8.453 | 8.346 |
| 18 | `C_native_04` | 8.402 | 8.215 | 8.125 |
| 19 | `kernel_01_ROUNDING` | 8.384 | 8.245 | 8.116 |
| 20 | `kernel_01` | 8.291 | 8.108 | 7.987 |
| 21 | `kernel_04` | 8.271 | 8.067 | 7.949 |
| 22 | `kernel_04_ROUNDING_PRECISION` | 8.212 | 8.023 | 7.946 |
| 23 | `kernel_04_ROUNDING` | 8.175 | 8.029 | 7.922 |
| 24 | `kernel_04_PRECISION` | 8.058 | 7.996 | 7.956 |

</details>

<details>
<summary>1024-bit serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_01_FMA` | 9.572 | 9.341 | 9.251 |
| 2 | `kernel_01_ROUNDING_PRECISION_FMA` | 9.404 | 9.332 | 9.276 |
| 3 | `C_native_packed_custom_layout_FMA` | 9.369 | 9.244 | 9.186 |
| 4 | `C_native_01` | 9.119 | 9.055 | 9.003 |
| 5 | `C_native_03` | 9.092 | 9.037 | 8.975 |
| 6 | `kernel_03_ROUNDING_PRECISION` | 9.066 | 9.021 | 8.973 |
| 7 | `kernel_01_PRECISION_FMA` | 9.066 | 9.019 | 8.954 |
| 8 | `C_native_02` | 9.249 | 8.993 | 8.880 |
| 9 | `kernel_02_ROUNDING_PRECISION` | 9.216 | 8.974 | 8.865 |
| 10 | `kernel_03_ROUNDING` | 9.177 | 8.960 | 8.879 |
| 11 | `kernel_02_ROUNDING` | 9.060 | 8.946 | 8.851 |
| 12 | `kernel_03_PRECISION` | 9.105 | 8.891 | 8.818 |
| 13 | `kernel_03` | 8.845 | 8.761 | 8.668 |
| 14 | `kernel_01_PRECISION` | 8.793 | 8.737 | 8.643 |
| 15 | `kernel_01_ROUNDING_PRECISION` | 8.802 | 8.644 | 7.967 |
| 16 | `kernel_02` | 8.671 | 8.590 | 8.464 |
| 17 | `kernel_02_PRECISION` | 8.629 | 8.453 | 8.346 |
| 18 | `kernel_01_ROUNDING` | 8.384 | 8.245 | 8.116 |
| 19 | `C_native_04` | 8.402 | 8.215 | 8.125 |
| 20 | `kernel_01` | 8.291 | 8.108 | 7.987 |
| 21 | `kernel_04` | 8.271 | 8.067 | 7.949 |
| 22 | `kernel_04_ROUNDING` | 8.175 | 8.029 | 7.922 |
| 23 | `kernel_04_ROUNDING_PRECISION` | 8.212 | 8.023 | 7.946 |
| 24 | `kernel_04_PRECISION` | 8.058 | 7.996 | 7.956 |

</details>

## OpenMP Results

### 512-bit OpenMP interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260526_062542/summary_raxpy_mpfr_n10000000_p512_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C OpenMP avg | `C_native_openmp_01_FMA` | 417.176 | 412.853 | 405.694 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| Best wrapper OpenMP avg | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 422.132 | 414.911 | 407.995 | Wrapper source with loop-external rounding and fixed-precision FMA build. |
| Best OpenMP max | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 422.132 | 414.911 | 407.995 | Wrapper source with loop-external rounding and fixed-precision FMA build. |

<details>
<summary>512-bit OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 422.132 | 414.911 | 407.995 |
| 2 | `kernel_openmp_01_PRECISION_FMA` | 419.332 | 409.979 | 378.593 |
| 3 | `C_native_openmp_01_FMA` | 417.176 | 412.853 | 405.694 |
| 4 | `kernel_openmp_03_ROUNDING` | 414.323 | 405.983 | 392.753 |
| 5 | `kernel_openmp_03_ROUNDING_PRECISION` | 413.459 | 407.387 | 396.400 |
| 6 | `kernel_openmp_01_ROUNDING_PRECISION` | 413.039 | 404.752 | 390.571 |
| 7 | `kernel_openmp_01_PRECISION` | 410.271 | 402.514 | 389.655 |
| 8 | `C_native_openmp_02` | 409.302 | 404.520 | 392.442 |
| 9 | `kernel_openmp_03` | 408.402 | 398.086 | 385.342 |
| 10 | `kernel_openmp_03_PRECISION` | 408.082 | 396.351 | 379.142 |
| 11 | `kernel_openmp_02_ROUNDING` | 405.982 | 395.169 | 383.243 |
| 12 | `kernel_openmp_02_ROUNDING_PRECISION` | 405.561 | 400.317 | 387.902 |
| 13 | `C_native_openmp_01` | 405.061 | 397.022 | 387.804 |
| 14 | `C_native_openmp_03` | 402.492 | 392.680 | 373.895 |
| 15 | `kernel_openmp_02_PRECISION` | 399.675 | 390.308 | 372.866 |
| 16 | `kernel_openmp_04_ROUNDING_PRECISION` | 398.287 | 383.151 | 342.628 |
| 17 | `kernel_openmp_01_ROUNDING` | 395.612 | 377.816 | 335.276 |
| 18 | `kernel_openmp_04_PRECISION` | 395.430 | 384.678 | 365.743 |
| 19 | `kernel_openmp_02` | 395.162 | 376.781 | 334.097 |
| 20 | `kernel_openmp_04` | 394.209 | 377.495 | 327.217 |
| 21 | `kernel_openmp_01` | 393.293 | 376.624 | 356.355 |
| 22 | `kernel_openmp_04_ROUNDING` | 391.991 | 384.622 | 360.931 |
| 23 | `C_native_openmp_04` | 390.891 | 378.376 | 359.518 |

</details>

<details>
<summary>512-bit OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 422.132 | 414.911 | 407.995 |
| 2 | `C_native_openmp_01_FMA` | 417.176 | 412.853 | 405.694 |
| 3 | `kernel_openmp_01_PRECISION_FMA` | 419.332 | 409.979 | 378.593 |
| 4 | `kernel_openmp_03_ROUNDING_PRECISION` | 413.459 | 407.387 | 396.400 |
| 5 | `kernel_openmp_03_ROUNDING` | 414.323 | 405.983 | 392.753 |
| 6 | `kernel_openmp_01_ROUNDING_PRECISION` | 413.039 | 404.752 | 390.571 |
| 7 | `C_native_openmp_02` | 409.302 | 404.520 | 392.442 |
| 8 | `kernel_openmp_01_PRECISION` | 410.271 | 402.514 | 389.655 |
| 9 | `kernel_openmp_02_ROUNDING_PRECISION` | 405.561 | 400.317 | 387.902 |
| 10 | `kernel_openmp_03` | 408.402 | 398.086 | 385.342 |
| 11 | `C_native_openmp_01` | 405.061 | 397.022 | 387.804 |
| 12 | `kernel_openmp_03_PRECISION` | 408.082 | 396.351 | 379.142 |
| 13 | `kernel_openmp_02_ROUNDING` | 405.982 | 395.169 | 383.243 |
| 14 | `C_native_openmp_03` | 402.492 | 392.680 | 373.895 |
| 15 | `kernel_openmp_02_PRECISION` | 399.675 | 390.308 | 372.866 |
| 16 | `kernel_openmp_04_PRECISION` | 395.430 | 384.678 | 365.743 |
| 17 | `kernel_openmp_04_ROUNDING` | 391.991 | 384.622 | 360.931 |
| 18 | `kernel_openmp_04_ROUNDING_PRECISION` | 398.287 | 383.151 | 342.628 |
| 19 | `C_native_openmp_04` | 390.891 | 378.376 | 359.518 |
| 20 | `kernel_openmp_01_ROUNDING` | 395.612 | 377.816 | 335.276 |
| 21 | `kernel_openmp_04` | 394.209 | 377.495 | 327.217 |
| 22 | `kernel_openmp_02` | 395.162 | 376.781 | 334.097 |
| 23 | `kernel_openmp_01` | 393.293 | 376.624 | 356.355 |

</details>

### 1024-bit OpenMP interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260526_062542/summary_raxpy_mpfr_n10000000_p1024_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C OpenMP avg | `C_native_openmp_01_FMA` | 258.528 | 253.639 | 247.970 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| Best wrapper OpenMP avg | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 258.848 | 254.585 | 245.304 | Wrapper source with loop-external rounding and fixed-precision FMA build. |
| Best OpenMP max | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 258.848 | 254.585 | 245.304 | Wrapper source with loop-external rounding and fixed-precision FMA build. |

<details>
<summary>1024-bit OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 258.848 | 254.585 | 245.304 |
| 2 | `C_native_openmp_01_FMA` | 258.528 | 253.639 | 247.970 |
| 3 | `kernel_openmp_01_PRECISION_FMA` | 252.993 | 250.635 | 247.914 |
| 4 | `C_native_openmp_03` | 250.634 | 246.588 | 239.824 |
| 5 | `C_native_openmp_01` | 249.872 | 244.560 | 230.456 |
| 6 | `kernel_openmp_03_ROUNDING_PRECISION` | 248.196 | 240.198 | 214.621 |
| 7 | `C_native_openmp_02` | 246.814 | 243.520 | 236.102 |
| 8 | `kernel_openmp_03_ROUNDING` | 246.192 | 240.235 | 224.320 |
| 9 | `kernel_openmp_03_PRECISION` | 244.992 | 236.717 | 225.656 |
| 10 | `kernel_openmp_02_ROUNDING` | 244.375 | 237.533 | 221.527 |
| 11 | `kernel_openmp_02_ROUNDING_PRECISION` | 242.701 | 236.966 | 227.536 |
| 12 | `kernel_openmp_01_ROUNDING_PRECISION` | 241.703 | 232.456 | 222.667 |
| 13 | `kernel_openmp_01_PRECISION` | 240.672 | 236.217 | 233.220 |
| 14 | `kernel_openmp_03` | 238.842 | 231.650 | 221.284 |
| 15 | `kernel_openmp_02_PRECISION` | 233.291 | 226.868 | 211.985 |
| 16 | `kernel_openmp_02` | 232.014 | 227.905 | 210.307 |
| 17 | `C_native_openmp_04` | 231.386 | 229.187 | 225.969 |
| 18 | `kernel_openmp_01_ROUNDING` | 227.158 | 223.173 | 218.910 |
| 19 | `kernel_openmp_01` | 222.999 | 217.598 | 207.617 |
| 20 | `kernel_openmp_04_PRECISION` | 222.056 | 216.895 | 210.063 |
| 21 | `kernel_openmp_04_ROUNDING_PRECISION` | 219.786 | 213.026 | 193.694 |
| 22 | `kernel_openmp_04_ROUNDING` | 219.187 | 215.376 | 207.823 |
| 23 | `kernel_openmp_04` | 218.175 | 214.432 | 209.890 |

</details>

<details>
<summary>1024-bit OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 258.848 | 254.585 | 245.304 |
| 2 | `C_native_openmp_01_FMA` | 258.528 | 253.639 | 247.970 |
| 3 | `kernel_openmp_01_PRECISION_FMA` | 252.993 | 250.635 | 247.914 |
| 4 | `C_native_openmp_03` | 250.634 | 246.588 | 239.824 |
| 5 | `C_native_openmp_01` | 249.872 | 244.560 | 230.456 |
| 6 | `C_native_openmp_02` | 246.814 | 243.520 | 236.102 |
| 7 | `kernel_openmp_03_ROUNDING` | 246.192 | 240.235 | 224.320 |
| 8 | `kernel_openmp_03_ROUNDING_PRECISION` | 248.196 | 240.198 | 214.621 |
| 9 | `kernel_openmp_02_ROUNDING` | 244.375 | 237.533 | 221.527 |
| 10 | `kernel_openmp_02_ROUNDING_PRECISION` | 242.701 | 236.966 | 227.536 |
| 11 | `kernel_openmp_03_PRECISION` | 244.992 | 236.717 | 225.656 |
| 12 | `kernel_openmp_01_PRECISION` | 240.672 | 236.217 | 233.220 |
| 13 | `kernel_openmp_01_ROUNDING_PRECISION` | 241.703 | 232.456 | 222.667 |
| 14 | `kernel_openmp_03` | 238.842 | 231.650 | 221.284 |
| 15 | `C_native_openmp_04` | 231.386 | 229.187 | 225.969 |
| 16 | `kernel_openmp_02` | 232.014 | 227.905 | 210.307 |
| 17 | `kernel_openmp_02_PRECISION` | 233.291 | 226.868 | 211.985 |
| 18 | `kernel_openmp_01_ROUNDING` | 227.158 | 223.173 | 218.910 |
| 19 | `kernel_openmp_01` | 222.999 | 217.598 | 207.617 |
| 20 | `kernel_openmp_04_PRECISION` | 222.056 | 216.895 | 210.063 |
| 21 | `kernel_openmp_04_ROUNDING` | 219.187 | 215.376 | 207.823 |
| 22 | `kernel_openmp_04` | 218.175 | 214.432 | 209.890 |
| 23 | `kernel_openmp_04_ROUNDING_PRECISION` | 219.786 | 213.026 | 193.694 |

</details>

## Comparison with GMP version

The rows below compare the current 512-bit and 1024-bit `run_all` data for the same benchmark. This is a performance-class comparison; GMP `mpf` and MPFR have different precision, rounding, and range semantics.

| Precision | Class | GMP best-avg variant | GMP Avg MFLOPS | MPFR best-avg variant | MPFR Avg MFLOPS | MPFR/GMP |
| --- | --- | --- | --- | --- | --- | --- |
| 512 | Best serial average | `C_native_03` | 33.650 | `kernel_01_ROUNDING_PRECISION_FMA` | 22.981 | 0.683x |
| 512 | Best OpenMP average | `kernel_openmp_03_orig` | 393.434 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 414.911 | 1.055x |
| 1024 | Best serial average | `C_native_03` | 11.967 | `C_native_01_FMA` | 9.341 | 0.781x |
| 1024 | Best OpenMP average | `kernel_openmp_03_orig` | 252.368 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 254.585 | 1.009x |

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

- At 512 bits, the best serial average is `kernel_01_ROUNDING_PRECISION_FMA` at 22.981 MFLOPS; the best OpenMP average is `kernel_openmp_01_ROUNDING_PRECISION_FMA` at 414.911 MFLOPS.
- At 1024 bits, the best serial average is `C_native_01_FMA` at 9.341 MFLOPS; the best OpenMP average is `kernel_openmp_01_ROUNDING_PRECISION_FMA` at 254.585 MFLOPS.
- For MPFR, loop-external rounding/context and FMA-capturable source forms are the changes that most often alter the hot-loop class.
- Fixed-precision builds help only when the source shape allows precision checks or scratch setup to be removed from the timed path; they do not automatically dominate every variant.
- OpenMP FMA effects are source-shape dependent: some fused forms improve the class, while others expose a different parallel bottleneck and should be checked against disassembly.
