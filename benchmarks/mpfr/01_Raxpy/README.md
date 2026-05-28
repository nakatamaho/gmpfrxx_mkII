<!--
Copyright (c) 2026
     Nakata, Maho
     All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
-->

# 01_Raxpy

This directory benchmarks the MPFR real AXPY operation

```text
y_i = y_i + alpha * x_i
```

for fixed-precision `mpfr_t` and `mpfrxx::mpfr_class` vectors. The benchmark compares raw C MPFR kernels, expression-template wrapper kernels, explicit rounding source variants, FMA-capable builds, fixed-precision builds, and OpenMP worker loops at 512-bit and 1024-bit precision.

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
| `ROUNDING` | none | Uses an explicit `mpfr_rnd_t` source file with `with_rounding` and avoids default-rounding lookup in the timed loop. |
| `ROUNDING_PRECISION` | `GMPFRXX_MKII_FAST_FIXED_PREC` | Builds the explicit-context source with fixed-precision assumptions. |
| final `FMA` | `GMPFRXX_MKII_ENABLE_FMA` | Builds an FMA-capturable source shape with FMA enabled. |

FMA targets are generated only for direct-expression variant `01`, where the source can lower to one `mpfr_fma` call.

## Source Transitions

`01` is the FMA-capturable wrapper source in this benchmark because the product remains in the update expression. `01 -> 02` introduces an explicit reusable product object and copy-then-multiply source. `02 -> 03` keeps reusable storage but assigns it from `alpha * x[i]`, matching the raw split multiply/add reusable-product class. `03 -> 04` moves product construction into the loop as a lifetime stress case. `ROUNDING` variants are separate source files that capture explicit `mpfr_rnd_t` before the loop; `PRECISION` and final `FMA` are build modifiers, not new variant numbers.

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
| Run ID | `run_all_p512_repeat10_20260527_094954` |
| Date | 2026-05-27 |
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
| Raw result directory | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260527_094954/` |
| Raw log | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260527_094954/benchmark_raxpy_mpfr_n10000000_p512_repeat10.log` |
| Raw CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260527_094954/raw_raxpy_mpfr_n10000000_p512_repeat10.csv` |
| Summary CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260527_094954/summary_raxpy_mpfr_n10000000_p512_repeat10.csv` |
| Correctness | 470 / 470 runs reported OK. |

![MPFR Raxpy serial 512-bit repeat-10](results_raw/run_all_p512_repeat10_20260527_094954/raxpy_mpfr_n10000000_p512_repeat10_serial.png)

![MPFR Raxpy OpenMP 512-bit repeat-10](results_raw/run_all_p512_repeat10_20260527_094954/raxpy_mpfr_n10000000_p512_repeat10_openmp.png)

Plot regeneration command:

```bash
python3 benchmarks/mpfr/01_Raxpy/plot_repeat_summary.py \
    benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260527_094954/benchmark_raxpy_mpfr_n10000000_p512_repeat10.log \
    --output-dir benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260527_094954 \
    --output-prefix raxpy_mpfr_n10000000_p512_repeat10 \
    --title-prefix "MPFR Raxpy N=10000000, precision=512, repeat=10"
```

### 1024-bit run

| Field | Value |
|-------|-------|
| Run ID | `run_all_p1024_repeat10_20260527_094954` |
| Date | 2026-05-27 |
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
| Raw result directory | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260527_094954/` |
| Raw log | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260527_094954/benchmark_raxpy_mpfr_n10000000_p1024_repeat10.log` |
| Raw CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260527_094954/raw_raxpy_mpfr_n10000000_p1024_repeat10.csv` |
| Summary CSV | `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260527_094954/summary_raxpy_mpfr_n10000000_p1024_repeat10.csv` |
| Correctness | 470 / 470 runs reported OK. |

![MPFR Raxpy serial 1024-bit repeat-10](results_raw/run_all_p1024_repeat10_20260527_094954/raxpy_mpfr_n10000000_p1024_repeat10_serial.png)

![MPFR Raxpy OpenMP 1024-bit repeat-10](results_raw/run_all_p1024_repeat10_20260527_094954/raxpy_mpfr_n10000000_p1024_repeat10_openmp.png)

Plot regeneration command:

```bash
python3 benchmarks/mpfr/01_Raxpy/plot_repeat_summary.py \
    benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260527_094954/benchmark_raxpy_mpfr_n10000000_p1024_repeat10.log \
    --output-dir benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260527_094954 \
    --output-prefix raxpy_mpfr_n10000000_p1024_repeat10 \
    --title-prefix "MPFR Raxpy N=10000000, precision=1024, repeat=10"
```

## Resource or Bandwidth Estimates

The values below are model estimates derived from MFLOPS, not hardware-counter measurements. They count active limb bytes plus a header-inclusive object model. They exclude allocator metadata, cache-line overfetch, instruction fetch, and final OpenMP reduction traffic.

| Case | Representative best-avg variant | Avg MFLOPS | Active bytes/iteration | Header-inclusive bytes/iteration | Active GB/s | Header-inclusive GB/s |
| --- | --- | --- | --- | --- | --- | --- |
| 512-bit serial | `kernel_01_ROUNDING_PRECISION_FMA` | 22.963 | 192 | 288 | 2.204 | 3.307 |
| 512-bit OpenMP | `C_native_openmp_01_FMA` | 415.811 | 192 | 288 | 39.918 | 59.877 |
| 1024-bit serial | `C_native_01_FMA` | 9.351 | 384 | 480 | 1.795 | 2.244 |
| 1024-bit OpenMP | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 252.310 | 384 | 480 | 48.443 | 60.554 |

For `Raxpy`, the per-iteration byte model is a compact arithmetic-stream estimate. It is not a full cache-footprint or hardware-bandwidth measurement.

## Headline Results

The headline rows below are regenerated from the committed 512-bit and 1024-bit `run_all` summary CSV files.

| Precision | Class | Variant | Max MFLOPS | Avg MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| 512 | Best max serial | `C_native_packed_custom_layout_FMA` | 23.787 | 22.937 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| 512 | Best average serial | `kernel_01_ROUNDING_PRECISION_FMA` | 23.209 | 22.963 | Wrapper fixed-precision FMA build for an FMA-capturable expression shape. |
| 512 | Best max OpenMP | `C_native_openmp_01_FMA` | 422.804 | 415.811 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| 512 | Best average OpenMP | `C_native_openmp_01_FMA` | 422.804 | 415.811 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| 1024 | Best max serial | `kernel_01_ROUNDING_PRECISION_FMA` | 9.403 | 9.309 | Wrapper fixed-precision FMA build for an FMA-capturable expression shape. |
| 1024 | Best average serial | `C_native_01_FMA` | 9.393 | 9.351 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| 1024 | Best max OpenMP | `C_native_openmp_01_FMA` | 257.429 | 251.583 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| 1024 | Best average OpenMP | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 256.399 | 252.310 | Wrapper fixed-precision FMA build for an FMA-capturable expression shape. |

## Serial Results

### 512-bit serial interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260527_094954/summary_raxpy_mpfr_n10000000_p512_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C average | `C_native_packed_custom_layout_FMA` | 23.787 | 22.937 | 22.218 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| Best wrapper baseline average | `kernel_03` | 19.809 | 19.531 | 19.343 | Wrapper baseline for the numbered source shape. |
| Best wrapper rounding average | `kernel_03_ROUNDING` | 22.972 | 22.817 | 22.455 | Wrapper source captures rounding outside the loop to avoid default-rounding lookup in the timed body. |
| Best wrapper precision average | `kernel_01_ROUNDING_PRECISION` | 21.978 | 21.588 | 21.398 | Wrapper source captures rounding outside the loop and uses the fixed-precision build. |
| Best wrapper FMA average | `kernel_01_ROUNDING_PRECISION_FMA` | 23.209 | 22.963 | 22.701 | Wrapper fixed-precision FMA build for an FMA-capturable expression shape. |
| Best max | `C_native_packed_custom_layout_FMA` | 23.787 | 22.937 | 22.218 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |

<details>
<summary>512-bit serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_packed_custom_layout_FMA` | 23.787 | 22.937 | 22.218 |
| 2 | `kernel_01_ROUNDING_PRECISION_FMA` | 23.209 | 22.963 | 22.701 |
| 3 | `C_native_01_FMA` | 23.137 | 22.742 | 22.270 |
| 4 | `C_native_01` | 23.136 | 22.635 | 22.148 |
| 5 | `C_native_03` | 23.024 | 22.732 | 22.480 |
| 6 | `kernel_03_ROUNDING` | 22.972 | 22.817 | 22.455 |
| 7 | `kernel_01_PRECISION_FMA` | 22.069 | 21.737 | 21.023 |
| 8 | `kernel_01_ROUNDING_PRECISION` | 21.978 | 21.588 | 21.398 |
| 9 | `kernel_03_ROUNDING_PRECISION` | 21.690 | 21.520 | 21.325 |
| 10 | `kernel_01_PRECISION` | 21.412 | 20.770 | 20.460 |
| 11 | `kernel_03_PRECISION` | 20.544 | 20.066 | 19.732 |
| 12 | `kernel_02_ROUNDING` | 20.113 | 19.979 | 19.858 |
| 13 | `C_native_02` | 20.013 | 19.854 | 19.666 |
| 14 | `kernel_02_ROUNDING_PRECISION` | 19.903 | 19.765 | 19.639 |
| 15 | `kernel_03` | 19.809 | 19.531 | 19.343 |
| 16 | `C_native_04` | 18.308 | 18.041 | 17.727 |
| 17 | `kernel_02_PRECISION` | 17.989 | 17.880 | 17.758 |
| 18 | `kernel_02` | 17.979 | 17.787 | 17.520 |
| 19 | `kernel_01_ROUNDING` | 17.447 | 17.083 | 16.903 |
| 20 | `kernel_04_ROUNDING` | 17.306 | 16.894 | 16.630 |
| 21 | `kernel_04` | 17.029 | 16.829 | 16.693 |
| 22 | `kernel_01` | 16.616 | 16.433 | 16.290 |
| 23 | `kernel_04_PRECISION` | 16.408 | 16.246 | 16.061 |
| 24 | `kernel_04_ROUNDING_PRECISION` | 15.977 | 15.878 | 15.816 |

</details>

<details>
<summary>512-bit serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_01_ROUNDING_PRECISION_FMA` | 23.209 | 22.963 | 22.701 |
| 2 | `C_native_packed_custom_layout_FMA` | 23.787 | 22.937 | 22.218 |
| 3 | `kernel_03_ROUNDING` | 22.972 | 22.817 | 22.455 |
| 4 | `C_native_01_FMA` | 23.137 | 22.742 | 22.270 |
| 5 | `C_native_03` | 23.024 | 22.732 | 22.480 |
| 6 | `C_native_01` | 23.136 | 22.635 | 22.148 |
| 7 | `kernel_01_PRECISION_FMA` | 22.069 | 21.737 | 21.023 |
| 8 | `kernel_01_ROUNDING_PRECISION` | 21.978 | 21.588 | 21.398 |
| 9 | `kernel_03_ROUNDING_PRECISION` | 21.690 | 21.520 | 21.325 |
| 10 | `kernel_01_PRECISION` | 21.412 | 20.770 | 20.460 |
| 11 | `kernel_03_PRECISION` | 20.544 | 20.066 | 19.732 |
| 12 | `kernel_02_ROUNDING` | 20.113 | 19.979 | 19.858 |
| 13 | `C_native_02` | 20.013 | 19.854 | 19.666 |
| 14 | `kernel_02_ROUNDING_PRECISION` | 19.903 | 19.765 | 19.639 |
| 15 | `kernel_03` | 19.809 | 19.531 | 19.343 |
| 16 | `C_native_04` | 18.308 | 18.041 | 17.727 |
| 17 | `kernel_02_PRECISION` | 17.989 | 17.880 | 17.758 |
| 18 | `kernel_02` | 17.979 | 17.787 | 17.520 |
| 19 | `kernel_01_ROUNDING` | 17.447 | 17.083 | 16.903 |
| 20 | `kernel_04_ROUNDING` | 17.306 | 16.894 | 16.630 |
| 21 | `kernel_04` | 17.029 | 16.829 | 16.693 |
| 22 | `kernel_01` | 16.616 | 16.433 | 16.290 |
| 23 | `kernel_04_PRECISION` | 16.408 | 16.246 | 16.061 |
| 24 | `kernel_04_ROUNDING_PRECISION` | 15.977 | 15.878 | 15.816 |

</details>

### 1024-bit serial interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260527_094954/summary_raxpy_mpfr_n10000000_p1024_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C average | `C_native_01_FMA` | 9.393 | 9.351 | 9.275 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| Best wrapper baseline average | `kernel_03` | 9.008 | 8.820 | 8.701 | Wrapper baseline for the numbered source shape. |
| Best wrapper rounding average | `kernel_03_ROUNDING_PRECISION` | 9.072 | 9.025 | 8.949 | Wrapper source captures rounding outside the loop and uses the fixed-precision build. |
| Best wrapper precision average | `kernel_03_ROUNDING_PRECISION` | 9.072 | 9.025 | 8.949 | Wrapper source captures rounding outside the loop and uses the fixed-precision build. |
| Best wrapper FMA average | `kernel_01_ROUNDING_PRECISION_FMA` | 9.403 | 9.309 | 9.238 | Wrapper fixed-precision FMA build for an FMA-capturable expression shape. |
| Best max | `kernel_01_ROUNDING_PRECISION_FMA` | 9.403 | 9.309 | 9.238 | Wrapper fixed-precision FMA build for an FMA-capturable expression shape. |

<details>
<summary>1024-bit serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_01_ROUNDING_PRECISION_FMA` | 9.403 | 9.309 | 9.238 |
| 2 | `C_native_01_FMA` | 9.393 | 9.351 | 9.275 |
| 3 | `C_native_packed_custom_layout_FMA` | 9.339 | 9.244 | 9.126 |
| 4 | `kernel_01_PRECISION_FMA` | 9.318 | 9.072 | 8.991 |
| 5 | `kernel_03_ROUNDING` | 9.214 | 8.990 | 8.864 |
| 6 | `kernel_02_ROUNDING_PRECISION` | 9.200 | 8.965 | 8.876 |
| 7 | `kernel_02_ROUNDING` | 9.189 | 8.992 | 8.888 |
| 8 | `kernel_03_PRECISION` | 9.113 | 8.839 | 8.710 |
| 9 | `C_native_01` | 9.111 | 9.059 | 8.999 |
| 10 | `C_native_03` | 9.101 | 9.058 | 8.983 |
| 11 | `kernel_03_ROUNDING_PRECISION` | 9.072 | 9.025 | 8.949 |
| 12 | `C_native_02` | 9.044 | 8.983 | 8.900 |
| 13 | `kernel_03` | 9.008 | 8.820 | 8.701 |
| 14 | `kernel_01_ROUNDING_PRECISION` | 8.954 | 8.770 | 8.696 |
| 15 | `kernel_01_PRECISION` | 8.732 | 8.704 | 8.676 |
| 16 | `kernel_02` | 8.693 | 8.577 | 8.489 |
| 17 | `kernel_02_PRECISION` | 8.649 | 8.453 | 8.339 |
| 18 | `kernel_01_ROUNDING` | 8.495 | 8.272 | 8.159 |
| 19 | `kernel_01` | 8.297 | 8.131 | 8.012 |
| 20 | `kernel_04_ROUNDING_PRECISION` | 8.193 | 8.025 | 7.928 |
| 21 | `C_native_04` | 8.186 | 8.164 | 8.115 |
| 22 | `kernel_04_PRECISION` | 8.184 | 8.018 | 7.946 |
| 23 | `kernel_04` | 8.142 | 8.065 | 8.011 |
| 24 | `kernel_04_ROUNDING` | 8.035 | 7.992 | 7.932 |

</details>

<details>
<summary>1024-bit serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_01_FMA` | 9.393 | 9.351 | 9.275 |
| 2 | `kernel_01_ROUNDING_PRECISION_FMA` | 9.403 | 9.309 | 9.238 |
| 3 | `C_native_packed_custom_layout_FMA` | 9.339 | 9.244 | 9.126 |
| 4 | `kernel_01_PRECISION_FMA` | 9.318 | 9.072 | 8.991 |
| 5 | `C_native_01` | 9.111 | 9.059 | 8.999 |
| 6 | `C_native_03` | 9.101 | 9.058 | 8.983 |
| 7 | `kernel_03_ROUNDING_PRECISION` | 9.072 | 9.025 | 8.949 |
| 8 | `kernel_02_ROUNDING` | 9.189 | 8.992 | 8.888 |
| 9 | `kernel_03_ROUNDING` | 9.214 | 8.990 | 8.864 |
| 10 | `C_native_02` | 9.044 | 8.983 | 8.900 |
| 11 | `kernel_02_ROUNDING_PRECISION` | 9.200 | 8.965 | 8.876 |
| 12 | `kernel_03_PRECISION` | 9.113 | 8.839 | 8.710 |
| 13 | `kernel_03` | 9.008 | 8.820 | 8.701 |
| 14 | `kernel_01_ROUNDING_PRECISION` | 8.954 | 8.770 | 8.696 |
| 15 | `kernel_01_PRECISION` | 8.732 | 8.704 | 8.676 |
| 16 | `kernel_02` | 8.693 | 8.577 | 8.489 |
| 17 | `kernel_02_PRECISION` | 8.649 | 8.453 | 8.339 |
| 18 | `kernel_01_ROUNDING` | 8.495 | 8.272 | 8.159 |
| 19 | `C_native_04` | 8.186 | 8.164 | 8.115 |
| 20 | `kernel_01` | 8.297 | 8.131 | 8.012 |
| 21 | `kernel_04` | 8.142 | 8.065 | 8.011 |
| 22 | `kernel_04_ROUNDING_PRECISION` | 8.193 | 8.025 | 7.928 |
| 23 | `kernel_04_PRECISION` | 8.184 | 8.018 | 7.946 |
| 24 | `kernel_04_ROUNDING` | 8.035 | 7.992 | 7.932 |

</details>

## OpenMP Results

### 512-bit OpenMP interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p512_repeat10_20260527_094954/summary_raxpy_mpfr_n10000000_p512_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C average | `C_native_openmp_01_FMA` | 422.804 | 415.811 | 402.988 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| Best wrapper baseline average | `kernel_openmp_03` | 407.349 | 394.560 | 365.484 | Wrapper baseline for the numbered source shape. |
| Best wrapper rounding average | `kernel_openmp_03_ROUNDING_PRECISION` | 415.411 | 406.901 | 388.240 | Wrapper source captures rounding outside the loop and uses the fixed-precision build. |
| Best wrapper precision average | `kernel_openmp_03_ROUNDING_PRECISION` | 415.411 | 406.901 | 388.240 | Wrapper source captures rounding outside the loop and uses the fixed-precision build. |
| Best wrapper FMA average | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 421.713 | 412.858 | 388.020 | Wrapper fixed-precision FMA build for an FMA-capturable expression shape. |
| Best max | `C_native_openmp_01_FMA` | 422.804 | 415.811 | 402.988 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |

<details>
<summary>512-bit OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_openmp_01_FMA` | 422.804 | 415.811 | 402.988 |
| 2 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 421.713 | 412.858 | 388.020 |
| 3 | `kernel_openmp_01_PRECISION_FMA` | 419.567 | 411.238 | 396.643 |
| 4 | `kernel_openmp_03_ROUNDING_PRECISION` | 415.411 | 406.901 | 388.240 |
| 5 | `kernel_openmp_03_ROUNDING` | 412.993 | 405.515 | 385.239 |
| 6 | `C_native_openmp_02` | 412.961 | 405.970 | 397.086 |
| 7 | `kernel_openmp_02_ROUNDING_PRECISION` | 412.584 | 406.144 | 395.440 |
| 8 | `kernel_openmp_01_ROUNDING_PRECISION` | 411.684 | 405.750 | 398.061 |
| 9 | `kernel_openmp_03_PRECISION` | 411.285 | 405.454 | 391.142 |
| 10 | `kernel_openmp_02_ROUNDING` | 408.975 | 399.966 | 367.931 |
| 11 | `C_native_openmp_03` | 407.836 | 400.523 | 390.683 |
| 12 | `kernel_openmp_03` | 407.349 | 394.560 | 365.484 |
| 13 | `kernel_openmp_02_PRECISION` | 406.708 | 389.503 | 358.768 |
| 14 | `C_native_openmp_01` | 406.245 | 394.689 | 377.531 |
| 15 | `kernel_openmp_01_PRECISION` | 406.205 | 398.642 | 384.118 |
| 16 | `kernel_openmp_02` | 402.730 | 388.891 | 351.250 |
| 17 | `kernel_openmp_01_ROUNDING` | 399.074 | 381.256 | 349.494 |
| 18 | `kernel_openmp_01` | 395.841 | 384.446 | 368.842 |
| 19 | `kernel_openmp_04` | 395.654 | 386.941 | 363.947 |
| 20 | `kernel_openmp_04_ROUNDING_PRECISION` | 394.433 | 386.387 | 370.387 |
| 21 | `kernel_openmp_04_PRECISION` | 393.936 | 381.727 | 363.455 |
| 22 | `kernel_openmp_04_ROUNDING` | 393.144 | 383.982 | 373.606 |
| 23 | `C_native_openmp_04` | 389.568 | 379.496 | 359.819 |

</details>

<details>
<summary>512-bit OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_openmp_01_FMA` | 422.804 | 415.811 | 402.988 |
| 2 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 421.713 | 412.858 | 388.020 |
| 3 | `kernel_openmp_01_PRECISION_FMA` | 419.567 | 411.238 | 396.643 |
| 4 | `kernel_openmp_03_ROUNDING_PRECISION` | 415.411 | 406.901 | 388.240 |
| 5 | `kernel_openmp_02_ROUNDING_PRECISION` | 412.584 | 406.144 | 395.440 |
| 6 | `C_native_openmp_02` | 412.961 | 405.970 | 397.086 |
| 7 | `kernel_openmp_01_ROUNDING_PRECISION` | 411.684 | 405.750 | 398.061 |
| 8 | `kernel_openmp_03_ROUNDING` | 412.993 | 405.515 | 385.239 |
| 9 | `kernel_openmp_03_PRECISION` | 411.285 | 405.454 | 391.142 |
| 10 | `C_native_openmp_03` | 407.836 | 400.523 | 390.683 |
| 11 | `kernel_openmp_02_ROUNDING` | 408.975 | 399.966 | 367.931 |
| 12 | `kernel_openmp_01_PRECISION` | 406.205 | 398.642 | 384.118 |
| 13 | `C_native_openmp_01` | 406.245 | 394.689 | 377.531 |
| 14 | `kernel_openmp_03` | 407.349 | 394.560 | 365.484 |
| 15 | `kernel_openmp_02_PRECISION` | 406.708 | 389.503 | 358.768 |
| 16 | `kernel_openmp_02` | 402.730 | 388.891 | 351.250 |
| 17 | `kernel_openmp_04` | 395.654 | 386.941 | 363.947 |
| 18 | `kernel_openmp_04_ROUNDING_PRECISION` | 394.433 | 386.387 | 370.387 |
| 19 | `kernel_openmp_01` | 395.841 | 384.446 | 368.842 |
| 20 | `kernel_openmp_04_ROUNDING` | 393.144 | 383.982 | 373.606 |
| 21 | `kernel_openmp_04_PRECISION` | 393.936 | 381.727 | 363.455 |
| 22 | `kernel_openmp_01_ROUNDING` | 399.074 | 381.256 | 349.494 |
| 23 | `C_native_openmp_04` | 389.568 | 379.496 | 359.819 |

</details>

### 1024-bit OpenMP interpretation

These rows are derived from `benchmarks/mpfr/01_Raxpy/results_raw/run_all_p1024_repeat10_20260527_094954/summary_raxpy_mpfr_n10000000_p1024_repeat10.csv`.

| Observation | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Interpretation |
| --- | --- | --- | --- | --- | --- |
| Best raw C average | `C_native_openmp_01_FMA` | 257.429 | 251.583 | 237.733 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |
| Best wrapper baseline average | `kernel_openmp_03` | 236.654 | 231.296 | 215.319 | Wrapper baseline for the numbered source shape. |
| Best wrapper rounding average | `kernel_openmp_03_ROUNDING_PRECISION` | 245.472 | 242.070 | 234.736 | Wrapper source captures rounding outside the loop and uses the fixed-precision build. |
| Best wrapper precision average | `kernel_openmp_03_ROUNDING_PRECISION` | 245.472 | 242.070 | 234.736 | Wrapper source captures rounding outside the loop and uses the fixed-precision build. |
| Best wrapper FMA average | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 256.399 | 252.310 | 240.106 | Wrapper fixed-precision FMA build for an FMA-capturable expression shape. |
| Best max | `C_native_openmp_01_FMA` | 257.429 | 251.583 | 237.733 | Raw C FMA reference; the hot loop uses the fused backend operation where the source shape permits it. |

<details>
<summary>1024-bit OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `C_native_openmp_01_FMA` | 257.429 | 251.583 | 237.733 |
| 2 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 256.399 | 252.310 | 240.106 |
| 3 | `kernel_openmp_01_PRECISION_FMA` | 254.164 | 248.454 | 234.304 |
| 4 | `C_native_openmp_03` | 250.625 | 246.166 | 242.079 |
| 5 | `C_native_openmp_01` | 248.832 | 245.418 | 232.639 |
| 6 | `C_native_openmp_02` | 248.070 | 242.766 | 237.850 |
| 7 | `kernel_openmp_03_ROUNDING` | 246.066 | 240.421 | 227.768 |
| 8 | `kernel_openmp_03_ROUNDING_PRECISION` | 245.472 | 242.070 | 234.736 |
| 9 | `kernel_openmp_02_ROUNDING` | 243.846 | 239.063 | 231.341 |
| 10 | `kernel_openmp_03_PRECISION` | 242.633 | 236.696 | 218.512 |
| 11 | `kernel_openmp_01_ROUNDING_PRECISION` | 242.279 | 230.637 | 181.809 |
| 12 | `kernel_openmp_02_ROUNDING_PRECISION` | 241.046 | 237.954 | 227.319 |
| 13 | `kernel_openmp_01_PRECISION` | 240.419 | 231.717 | 214.204 |
| 14 | `kernel_openmp_03` | 236.654 | 231.296 | 215.319 |
| 15 | `kernel_openmp_02_PRECISION` | 235.366 | 228.454 | 215.390 |
| 16 | `kernel_openmp_02` | 235.016 | 230.169 | 226.502 |
| 17 | `C_native_openmp_04` | 231.961 | 228.912 | 225.692 |
| 18 | `kernel_openmp_01_ROUNDING` | 226.782 | 212.286 | 166.084 |
| 19 | `kernel_openmp_01` | 224.327 | 216.308 | 193.811 |
| 20 | `kernel_openmp_04_PRECISION` | 222.806 | 219.627 | 209.891 |
| 21 | `kernel_openmp_04_ROUNDING_PRECISION` | 220.486 | 213.909 | 188.132 |
| 22 | `kernel_openmp_04` | 218.752 | 211.991 | 200.857 |
| 23 | `kernel_openmp_04_ROUNDING` | 216.985 | 209.320 | 171.266 |

</details>

<details>
<summary>1024-bit OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
| --- | --- | --- | --- | --- |
| 1 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` | 256.399 | 252.310 | 240.106 |
| 2 | `C_native_openmp_01_FMA` | 257.429 | 251.583 | 237.733 |
| 3 | `kernel_openmp_01_PRECISION_FMA` | 254.164 | 248.454 | 234.304 |
| 4 | `C_native_openmp_03` | 250.625 | 246.166 | 242.079 |
| 5 | `C_native_openmp_01` | 248.832 | 245.418 | 232.639 |
| 6 | `C_native_openmp_02` | 248.070 | 242.766 | 237.850 |
| 7 | `kernel_openmp_03_ROUNDING_PRECISION` | 245.472 | 242.070 | 234.736 |
| 8 | `kernel_openmp_03_ROUNDING` | 246.066 | 240.421 | 227.768 |
| 9 | `kernel_openmp_02_ROUNDING` | 243.846 | 239.063 | 231.341 |
| 10 | `kernel_openmp_02_ROUNDING_PRECISION` | 241.046 | 237.954 | 227.319 |
| 11 | `kernel_openmp_03_PRECISION` | 242.633 | 236.696 | 218.512 |
| 12 | `kernel_openmp_01_PRECISION` | 240.419 | 231.717 | 214.204 |
| 13 | `kernel_openmp_03` | 236.654 | 231.296 | 215.319 |
| 14 | `kernel_openmp_01_ROUNDING_PRECISION` | 242.279 | 230.637 | 181.809 |
| 15 | `kernel_openmp_02` | 235.016 | 230.169 | 226.502 |
| 16 | `C_native_openmp_04` | 231.961 | 228.912 | 225.692 |
| 17 | `kernel_openmp_02_PRECISION` | 235.366 | 228.454 | 215.390 |
| 18 | `kernel_openmp_04_PRECISION` | 222.806 | 219.627 | 209.891 |
| 19 | `kernel_openmp_01` | 224.327 | 216.308 | 193.811 |
| 20 | `kernel_openmp_04_ROUNDING_PRECISION` | 220.486 | 213.909 | 188.132 |
| 21 | `kernel_openmp_01_ROUNDING` | 226.782 | 212.286 | 166.084 |
| 22 | `kernel_openmp_04` | 218.752 | 211.991 | 200.857 |
| 23 | `kernel_openmp_04_ROUNDING` | 216.985 | 209.320 | 171.266 |

</details>

## Comparison with GMP version

MPFR and GMP `mpf` do not implement the same numerical contract. MPFR carries explicit rounding and range behavior, while GMP `mpf` follows GMP's floating-point model. The comparison below uses the same problem size, precision, repeat count, and OpenMP settings, and reports best average MFLOPS from the committed run.

| Precision | Mode | GMP best avg | MPFR best avg | MPFR / GMP | Interpretation |
| --- | --- | --- | --- | --- | --- |
| 512 | serial | `C_native_03` 33.727 | `kernel_01_ROUNDING_PRECISION_FMA` 22.963 | 0.681 | MPFR pays explicit rounding/range semantics, but FMA and rounding-capture paths narrow the gap when the hot loop matches the raw C arithmetic class. |
| 512 | OpenMP | `kernel_openmp_02_orig` 393.715 | `C_native_openmp_01_FMA` 415.811 | 1.056 | MPFR pays explicit rounding/range semantics, but FMA and rounding-capture paths narrow the gap when the hot loop matches the raw C arithmetic class. |
| 1024 | serial | `C_native_01` 11.970 | `C_native_01_FMA` 9.351 | 0.781 | MPFR pays explicit rounding/range semantics, but FMA and rounding-capture paths narrow the gap when the hot loop matches the raw C arithmetic class. |
| 1024 | OpenMP | `kernel_openmp_02_orig` 252.424 | `kernel_openmp_01_ROUNDING_PRECISION_FMA` 252.310 | 1.000 | MPFR pays explicit rounding/range semantics, but FMA and rounding-capture paths narrow the gap when the hot loop matches the raw C arithmetic class. |

## Hotpath Disassembly

Representative commands:

```bash
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_01_ROUNDING_PRECISION_FMA
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_03_ROUNDING_PRECISION
```

The representative disassembly now uses the current target names. The wrapper
FMA variants match the raw C arithmetic call class, but still carry wrapper
control paths outside or around the fast path.

| Representative | Hotpath observation | Comparison point |
|----------------|---------------------|------------------|
| `C_native_01_FMA` | Caches rounding before the loop and calls one `mpfr_fma` per element for `y[i] = alpha * x[i] + y[i]`. | Raw FMA baseline. |
| `kernel_01_ROUNDING_PRECISION_FMA` | The timed arithmetic path calls `mpfr_fma` with cached rounding; default-state setup and fallback code remain outside or around the quoted fast path. | Closest mkII FMA equivalent, same arithmetic class as C native. |
| `C_native_03` | Reusable product object with one `mpfr_mul` and one `mpfr_add` per element. | Raw split multiply/add baseline. |
| `kernel_03_ROUNDING_PRECISION` | Same split `mpfr_mul` plus `mpfr_add` arithmetic class as `C_native_03`; the reusable temporary is outside the element loop. | Closest mkII reusable-temp equivalent. |
| `kernel_openmp_01_ROUNDING_PRECISION_FMA` | Worker loop keeps the one-`mpfr_fma` arithmetic class; OpenMP scheduling and final synchronization are outside the backend call itself. | Best OpenMP wrapper class in both precision runs. |

Representative excerpts from the current binaries:

```asm
# Raxpy_mpfr_C_native_01_FMA::_Raxpy
29ca: call   mpfr_get_default_rounding_mode@plt
29cf: mov    %eax,%r14d         # cached rounding mode
29d2: test   %r12,%r12
29d5: jle    2a05 <_Raxpy+0x65>
29e0: mov    %rbx,%rcx          # y[i] addend
29e3: mov    %rbp,%rdx          # x[i]
29e6: mov    %rbx,%rdi          # y[i] destination
29e9: mov    %r14d,%r8d         # cached rounding
29ec: mov    %r13,%rsi          # alpha
29ef: add    $0x1,%r15
29f3: add    $0x20,%rbx
29f7: add    $0x20,%rbp
29fb: call   mpfr_fma@plt
2a00: cmp    %r15,%r12
2a03: jne    29e0 <_Raxpy+0x40>
```

```asm
# Raxpy_mpfr_kernel_01_ROUNDING_PRECISION_FMA::_Raxpy
2b51: cmp    %r14,%rax
2b5a: xor    %r14d,%r14d
2b60: mov    %rbx,%rcx          # y[i] addend
2b63: mov    %rbp,%rdx          # x[i]
2b66: mov    %rbx,%rdi          # y[i] destination
2b69: mov    %r15d,%r8d         # cached rounding
2b6c: mov    %r13,%rsi          # alpha
2b6f: add    $0x1,%r14
2b73: add    $0x20,%rbx
2b77: add    $0x20,%rbp
2b7b: call   mpfr_fma@plt
2b80: cmp    %r14,%r12
2b83: jg     2b60 <_Raxpy+0x80>
```

```asm
# Raxpy_mpfr_kernel_03_ROUNDING_PRECISION::_Raxpy
2c49: test   %r15,%r15
2c4c: jle    2c84 <_Raxpy+0xf4>
2c50: mov    (%rsp),%rsi        # alpha
2c54: mov    %r14d,%ecx         # cached rounding
2c57: mov    %rbp,%rdx          # x[i]
2c5a: mov    %r13,%rdi          # reusable product temp
2c5d: call   mpfr_mul@plt
2c62: mov    %r14d,%ecx         # cached rounding
2c65: mov    %r13,%rdx          # product temp
2c68: mov    %rbx,%rsi          # y[i]
2c6b: mov    %rbx,%rdi          # y[i] destination
2c6e: call   mpfr_add@plt
2c73: add    $0x1,%r12
2c77: add    $0x20,%rbp
2c7b: add    $0x20,%rbx
2c82: jne    2c50 <_Raxpy+0xc0>
2c87: call   mpfr_clear@plt
```

Suffix-removal check for the same `03` source shape: the quoted
`Raxpy_mpfr_kernel_03_ROUNDING_PRECISION::_Raxpy` loop above is the cached
reference. Removing `ROUNDING` or `PRECISION` changes different parts of the
hot path.

| Target | Removed cache assumption | Disassembly evidence | Meaning |
|--------|--------------------------|----------------------|---------|
| `kernel_03_PRECISION` | `ROUNDING` | The element loop calls `mpfr_get_default_rounding_mode` before `mpfr_mul` and again before `mpfr_add`. | The rounding mode is not cached by fixed precision. |
| `kernel_03_ROUNDING` | `PRECISION` | The loop uses cached rounding, but it still guards temporary and destination precision before the fast arithmetic path. | The compiler cannot assume the reusable temp and `y[i]` precision are invariant. |
| `kernel_03` | `ROUNDING` and `PRECISION` | The element loop calls `mpfr_get_default_rounding_mode` before both arithmetic calls, and the setup/fallback path is the non-fastpath precision path. | This is the fully uncached baseline for the same reusable-product source shape. |

The `cmpb $0x0,%fs:...` instruction is the generated check for the
DSO-local `static thread_local bool initialized` used by
`initialize_mpfr_defaults_for_current_thread()`. On Linux x86-64, `%fs` is the
TLS base and the displayed displacement is a build/link-specific TLS offset,
not a meaningful absolute address. This guard is not MPFR arithmetic and is not
the rounding value passed to an MPFR operation.

In the `PRECISION` and vanilla excerpts, the first `mpfr_get_default_rounding_mode`
call following the TLS initialization-flag check belongs to the first-use
MPFR-default initialization/refresh path. Once that per-thread flag is set, the
branch skips that guarded refresh call. The later `mpfr_get_default_rounding_mode`
call is the actual rounding value passed to the MPFR operation. Because this
source shape is split `mpfr_mul` plus `mpfr_add`, removing `ROUNDING` leaves one
steady-state rounding lookup for each arithmetic operation in the element loop.

```asm
# Raxpy_mpfr_kernel_03_PRECISION::_Raxpy
2c20: cmpb   $0x0,%fs:0xfffffffffffffff8
2c43: call   mpfr_get_default_rounding_mode@plt  # first-use default refresh path
2c50: call   mpfr_get_default_rounding_mode@plt  # rounding for mpfr_mul
2c55: mov    %rbp,%rdx          # x[i]
2c58: mov    %r15,%rsi          # alpha
2c5b: mov    %r13,%rdi          # reusable product temp
2c5e: mov    %eax,%ecx          # uncached rounding
2c60: call   mpfr_mul@plt
2c84: call   mpfr_get_default_rounding_mode@plt  # first-use default refresh path
2c91: call   mpfr_get_default_rounding_mode@plt  # rounding for mpfr_add
2c96: mov    %r13,%rdx          # product temp
2c99: mov    %rbx,%rsi          # y[i]
2c9c: mov    %rbx,%rdi          # y[i] destination
2c9f: mov    %eax,%ecx          # uncached rounding
2ca1: call   mpfr_add@plt
2cb5: jne    2c20 <_Raxpy+0xb0>
```

```asm
# Raxpy_mpfr_kernel_03_ROUNDING::_Raxpy
2e08: cmp    0x50(%rsp),%r14    # reusable temp precision guard
2e0d: jne    2850 <_Raxpy.cold+0x46>
2e20: mov    0x10(%rsp),%rsi    # alpha
2e25: mov    %r13d,%ecx         # cached rounding
2e28: mov    %rbx,%rdx          # x[i]
2e2b: mov    %r12,%rdi          # reusable product temp
2e2e: call   mpfr_mul@plt
2e33: cmp    (%r15),%r14        # y[i] precision guard
2e36: jne    2896 <_Raxpy.cold+0x8c>
2e3c: mov    %r13d,%ecx         # cached rounding
2e3f: mov    %r12,%rdx          # product temp
2e42: mov    %r15,%rsi          # y[i]
2e45: mov    %r15,%rdi          # y[i] destination
2e48: call   mpfr_add@plt
2e5e: jne    2e20 <_Raxpy+0xd0>
```

```asm
# Raxpy_mpfr_kernel_03::_Raxpy
2c50: cmpb   $0x0,%fs:0xfffffffffffffff8
2c73: call   mpfr_get_default_rounding_mode@plt  # first-use default refresh path
2c80: call   mpfr_get_default_rounding_mode@plt  # rounding for mpfr_mul
2c85: mov    %rbp,%rdx          # x[i]
2c88: mov    %r15,%rsi          # alpha
2c8b: mov    %r13,%rdi          # reusable product temp
2c8e: mov    %eax,%ecx          # uncached rounding
2c90: call   mpfr_mul@plt
2cb4: call   mpfr_get_default_rounding_mode@plt  # first-use default refresh path
2cc1: call   mpfr_get_default_rounding_mode@plt  # rounding for mpfr_add
2cc6: mov    %r13,%rdx          # product temp
2cc9: mov    %rbx,%rsi          # y[i]
2ccc: mov    %rbx,%rdi          # y[i] destination
2ccf: mov    %eax,%ecx          # uncached rounding
2cd1: call   mpfr_add@plt
2ce5: jne    2c50 <_Raxpy+0xb0>
```

The C native and wrapper FMA excerpts both have one `mpfr_fma` per element.
The reusable-temp wrapper excerpt has the expected split `mpfr_mul` plus
`mpfr_add` sequence with the temporary cleared after the loop.

## Lessons Learned

FMA capture is the meaningful MPFR Raxpy boundary. It changes the arithmetic
loop from split `mpfr_mul` plus `mpfr_add` to one `mpfr_fma` call per element.

For 512-bit serial runs, the diagnostic `C_native_packed_custom_layout_FMA`
case has the highest max, while `kernel_01_ROUNDING_PRECISION_FMA` has the best
average. The packed-layout result is a data-layout experiment, not a normal
wrapper-equivalent kernel; the wrapper result says the FMA-captured source is
still the main reusable Raxpy class.

For 512-bit OpenMP runs, `C_native_openmp_01_FMA` leads both max and average.
The raw C FMA loop is the cleanest control-flow point for this precision.

For 1024-bit serial runs, the max winner is
`kernel_01_ROUNDING_PRECISION_FMA`, while the average winner is
`C_native_01_FMA`. They are the same one-`mpfr_fma` arithmetic class, so this is
run-to-run ordering inside the FMA class rather than a durable wrapper win.

For 1024-bit OpenMP runs, `C_native_openmp_01_FMA` has the highest max and
`kernel_openmp_01_ROUNDING_PRECISION_FMA` has the highest average. The emitted
arithmetic class matches C native FMA, but wrapper setup and fallback paths are
not byte-for-byte identical.

Compared with GMP, MPFR serial performance is lower because rounding and range
semantics are explicit in every backend call. Under OpenMP, the best MPFR FMA
class can be close to GMP for this memory-streaming Raxpy shape, so treat the
small ordering changes by performance class rather than by a single run.
