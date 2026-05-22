<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 01_Raxpy

This directory benchmarks the MPFR real AXPY operation

```text
y_i = y_i + alpha * x_i
```

for fixed-precision `mpfr_t` and `mpfrxx::mpfr_class` vectors.  The benchmark
compares raw C MPFR kernels, mkII expression-template kernels, explicit-context
mkII kernels, FMA builds, stable-rounding builds, fixed-precision builds, and
OpenMP worker loops.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j --target Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_01_FMA Raxpy_mpfr_C_native_openmp_01_FMA
```

The full run used all MPFR Raxpy targets under:

```text
build_bench_release/benchmarks/mpfr/01_Raxpy/
```

Each executable takes:

```text
<vector size> <precision-bits>
```

Example:

```bash
build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_01_mkII_STABLE_ROUNDING_FMA 10000000 512
```

## Kernel Shapes

| Variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|--------------------|---------------------------|---------|
| `01` | `y[i] += alpha * x[i]` | No explicit product object in source. | Direct expression form; FMA builds can lower this source to one `mpfr_fma` call per element. |
| `02` | `temp = alpha; temp *= x[i]; y[i] += temp;` | One product object initialized outside the loop and reused. | Tests copy-then-multiply source shape with reusable storage. |
| `03` | `temp = alpha * x[i]; y[i] += temp;` | One product object initialized outside the loop and assigned each iteration. | Tests expression product materialization into reusable storage. |
| `04` | `mpfr_class temp = alpha * x[i]; y[i] += temp;` | Product object is constructed inside the timed loop. | Intentionally expensive control for per-iteration construction. |
| `05` | `with_context(y[i], ctx) += alpha * x[i]` | Context is created before the loop; no explicit product object. | Direct expression with loop-invariant precision/rounding context. |
| `06` | `with_context(temp, ctx) = alpha * x[i]; with_context(y[i], ctx) += temp;` | Context and product object are created before the loop. | Context-bound reusable-product path. |

Wrapper optimization suffixes are cumulative for `01..04`:

| Suffix | Compile definition | Meaning |
|--------|--------------------|---------|
| `mkII` | none | Generic expression-template path. |
| `STABLE_ROUNDING` | `GMPFRXX_MKII_FAST_STABLE_RND` | Uses the stable-rounding cache path. |
| `STABLE_ROUNDING_FMA` | stable rounding + `GMPFRXX_MKII_ENABLE_FMA` | Enables FMA lowering for eligible expression shapes. |
| `STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | stable rounding + FMA + `GMPFRXX_MKII_FAST_FIXED_PREC` | Adds fixed-precision assumptions for eligible wrapper paths. |

Context-bound `05` and `06` are explicit targets: `05_mkII`, `05_mkII_FMA`, and `06_mkII`.

## C Native Equivalent Kernels

| C native kernel | Closest wrapper kernel | Equivalence note |
|-----------------|------------------------|------------------|
| `C_native_01` | `kernel_03_*`, `kernel_06_mkII` | Split `mpfr_mul` + `mpfr_add` with a reusable product object. |
| `C_native_01_FMA` | `kernel_01_*_FMA`, `kernel_05_mkII_FMA` | One `mpfr_fma` per element when the wrapper source shape is direct. |
| `C_native_openmp_01` | `kernel_openmp_03_*`, `kernel_openmp_06_mkII` | OpenMP worker loop with split multiply/add and reusable per-worker state. |
| `C_native_openmp_01_FMA` | `kernel_openmp_01_*_FMA`, `kernel_openmp_05_mkII_FMA` | OpenMP worker loop with one `mpfr_fma` per element. |
| `C_native_packed_custom_layout_FMA` | none | Same arithmetic as `C_native_01_FMA`, but with packed MPFR header+limb storage. |

## Recorded Run

```text
N = 10000000
precision = 512 bits
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

Raw data:

- [Raw log](results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260522_221824/benchmark_raxpy_mpfr_n10000000_p512_repeat10.log)
- [Raw CSV](results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260522_221824/raw_raxpy_mpfr_n10000000_p512_repeat10.csv)
- [Summary CSV](results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260522_221824/summary_raxpy_mpfr_n10000000_p512_repeat10.csv)

Plots use average MFLOPS bars with min/max error bars over 10 repeats:

![MPFR Raxpy serial repeat-10](results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260522_221824/raxpy_mpfr_n10000000_p512_repeat10_serial.png)

![MPFR Raxpy OpenMP repeat-10](results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260522_221824/raxpy_mpfr_n10000000_p512_repeat10_openmp.png)

Regenerate plots with:

```bash
python3 benchmarks/mpfr/01_Raxpy/plot_repeat_summary.py \
    benchmarks/mpfr/01_Raxpy/results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260522_221824/summary_raxpy_mpfr_n10000000_p512_repeat10.csv \
    --output-prefix benchmarks/mpfr/01_Raxpy/results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260522_221824/raxpy_mpfr_n10000000_p512_repeat10 \
    --title-prefix "MPFR Raxpy N=10000000 p=512 repeat=10"
```

## Headline Results

| Class | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Var MFLOPS | Stddev MFLOPS | Interpretation |
|-------|---------|-----------:|-----------:|-----------:|-----------:|--------------:|----------------|
| Best serial average | `C_native_01_FMA` | 22.975 | 22.823 | 22.631 | 0.017 | 0.131 | Raw C FMA baseline; one `mpfr_fma` call per element and rounding cached before the loop. |
| Best serial maximum | `kernel_03_mkII_STABLE_ROUNDING` | 23.398 | 22.766 | 22.353 | 0.105 | 0.323 | Reusable product object assigned from the product expression; split multiply/add hot loop. Stable rounding is enabled. |
| Best OpenMP average | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA` | 422.613 | 414.601 | 404.807 | 30.961 | 5.564 | OpenMP worker loop; Direct expression form; FMA builds can lower to one `mpfr_fma` per element. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| Best OpenMP maximum | `C_native_openmp_01_FMA` | 422.647 | 412.005 | 397.239 | 45.560 | 6.750 | Raw OpenMP FMA baseline; one `mpfr_fma` call per element inside each worker loop. |

## Serial Results

| Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Var MFLOPS | Stddev MFLOPS | Interpretation |
|---------|-----------:|-----------:|-----------:|-----------:|--------------:|----------------|
| `C_native_01` | 23.139 | 22.533 | 22.091 | 0.071 | 0.267 | Raw C split multiply/add baseline; one reusable `mpfr_t` product and rounding cached before the loop. |
| `C_native_01_FMA` | 22.975 | 22.823 | 22.631 | 0.017 | 0.131 | Raw C FMA baseline; one `mpfr_fma` call per element and rounding cached before the loop. |
| `C_native_packed_custom_layout_FMA` | 22.975 | 22.674 | 22.224 | 0.041 | 0.202 | Same raw FMA arithmetic with packed MPFR header+limb storage; tests data layout rather than wrapper overhead. |
| `kernel_01_mkII` | 17.630 | 17.439 | 17.269 | 0.009 | 0.096 | Direct expression form; FMA builds can lower to one `mpfr_fma` per element. |
| `kernel_01_mkII_STABLE_ROUNDING` | 18.429 | 18.136 | 17.773 | 0.048 | 0.220 | Direct expression form; FMA builds can lower to one `mpfr_fma` per element. Stable rounding is enabled. |
| `kernel_01_mkII_STABLE_ROUNDING_FMA` | 21.862 | 21.301 | 21.037 | 0.050 | 0.223 | Direct expression form; FMA builds can lower to one `mpfr_fma` per element. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.392 | 22.735 | 22.338 | 0.119 | 0.345 | Direct expression form; FMA builds can lower to one `mpfr_fma` per element. Stable rounding, FMA, and fixed-precision fastpath are enabled. |
| `kernel_02_mkII` | 18.595 | 18.256 | 18.043 | 0.026 | 0.161 | Reusable product object using copy-then-multiply source shape. |
| `kernel_02_mkII_STABLE_ROUNDING` | 20.041 | 19.826 | 19.604 | 0.015 | 0.122 | Reusable product object using copy-then-multiply source shape. Stable rounding is enabled. |
| `kernel_02_mkII_STABLE_ROUNDING_FMA` | 19.805 | 19.607 | 19.453 | 0.011 | 0.104 | Reusable product object using copy-then-multiply source shape. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 20.449 | 19.965 | 19.524 | 0.064 | 0.254 | Reusable product object using copy-then-multiply source shape. Stable rounding, FMA, and fixed-precision fastpath are enabled. |
| `kernel_03_mkII` | 21.143 | 20.753 | 20.617 | 0.022 | 0.148 | Reusable product object assigned from the product expression; split multiply/add hot loop. |
| `kernel_03_mkII_STABLE_ROUNDING` | 23.398 | 22.766 | 22.353 | 0.105 | 0.323 | Reusable product object assigned from the product expression; split multiply/add hot loop. Stable rounding is enabled. |
| `kernel_03_mkII_STABLE_ROUNDING_FMA` | 23.151 | 22.555 | 22.197 | 0.059 | 0.242 | Reusable product object assigned from the product expression; split multiply/add hot loop. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.122 | 22.684 | 22.310 | 0.054 | 0.232 | Reusable product object assigned from the product expression; split multiply/add hot loop. Stable rounding, FMA, and fixed-precision fastpath are enabled. |
| `kernel_04_mkII` | 16.591 | 16.127 | 15.850 | 0.046 | 0.214 | Loop-local product object; intentionally expensive construction/control path. |
| `kernel_04_mkII_STABLE_ROUNDING` | 18.537 | 17.991 | 17.677 | 0.045 | 0.211 | Loop-local product object; intentionally expensive construction/control path. Stable rounding is enabled. |
| `kernel_04_mkII_STABLE_ROUNDING_FMA` | 17.208 | 16.835 | 16.620 | 0.039 | 0.198 | Loop-local product object; intentionally expensive construction/control path. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 18.532 | 18.178 | 17.953 | 0.024 | 0.154 | Loop-local product object; intentionally expensive construction/control path. Stable rounding, FMA, and fixed-precision fastpath are enabled. |
| `kernel_05_mkII` | 18.422 | 18.227 | 18.033 | 0.016 | 0.127 | Explicit-context direct expression; rounding is captured outside the loop. |
| `kernel_05_mkII_FMA` | 22.967 | 22.727 | 22.472 | 0.027 | 0.164 | Explicit-context direct expression; rounding is captured outside the loop. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_06_mkII` | 23.149 | 22.756 | 22.493 | 0.031 | 0.175 | Explicit-context reusable product object; cached context rounding with split multiply/add. |

<details>
<summary>Serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Var MFLOPS | Stddev MFLOPS |
|------|---------|-----------:|-----------:|-----------:|-----------:|--------------:|
| 1 | `kernel_03_mkII_STABLE_ROUNDING` | 23.398 | 22.766 | 22.353 | 0.105 | 0.323 |
| 2 | `kernel_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.392 | 22.735 | 22.338 | 0.119 | 0.345 |
| 3 | `kernel_03_mkII_STABLE_ROUNDING_FMA` | 23.151 | 22.555 | 22.197 | 0.059 | 0.242 |
| 4 | `kernel_06_mkII` | 23.149 | 22.756 | 22.493 | 0.031 | 0.175 |
| 5 | `C_native_01` | 23.139 | 22.533 | 22.091 | 0.071 | 0.267 |
| 6 | `kernel_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.122 | 22.684 | 22.310 | 0.054 | 0.232 |
| 7 | `C_native_01_FMA` | 22.975 | 22.823 | 22.631 | 0.017 | 0.131 |
| 8 | `C_native_packed_custom_layout_FMA` | 22.975 | 22.674 | 22.224 | 0.041 | 0.202 |
| 9 | `kernel_05_mkII_FMA` | 22.967 | 22.727 | 22.472 | 0.027 | 0.164 |
| 10 | `kernel_01_mkII_STABLE_ROUNDING_FMA` | 21.862 | 21.301 | 21.037 | 0.050 | 0.223 |
| 11 | `kernel_03_mkII` | 21.143 | 20.753 | 20.617 | 0.022 | 0.148 |
| 12 | `kernel_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 20.449 | 19.965 | 19.524 | 0.064 | 0.254 |
| 13 | `kernel_02_mkII_STABLE_ROUNDING` | 20.041 | 19.826 | 19.604 | 0.015 | 0.122 |
| 14 | `kernel_02_mkII_STABLE_ROUNDING_FMA` | 19.805 | 19.607 | 19.453 | 0.011 | 0.104 |
| 15 | `kernel_02_mkII` | 18.595 | 18.256 | 18.043 | 0.026 | 0.161 |
| 16 | `kernel_04_mkII_STABLE_ROUNDING` | 18.537 | 17.991 | 17.677 | 0.045 | 0.211 |
| 17 | `kernel_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 18.532 | 18.178 | 17.953 | 0.024 | 0.154 |
| 18 | `kernel_01_mkII_STABLE_ROUNDING` | 18.429 | 18.136 | 17.773 | 0.048 | 0.220 |
| 19 | `kernel_05_mkII` | 18.422 | 18.227 | 18.033 | 0.016 | 0.127 |
| 20 | `kernel_01_mkII` | 17.630 | 17.439 | 17.269 | 0.009 | 0.096 |
| 21 | `kernel_04_mkII_STABLE_ROUNDING_FMA` | 17.208 | 16.835 | 16.620 | 0.039 | 0.198 |
| 22 | `kernel_04_mkII` | 16.591 | 16.127 | 15.850 | 0.046 | 0.214 |

</details>

<details>
<summary>Serial results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Var MFLOPS | Stddev MFLOPS |
|------|---------|-----------:|-----------:|-----------:|-----------:|--------------:|
| 1 | `C_native_01_FMA` | 22.975 | 22.823 | 22.631 | 0.017 | 0.131 |
| 2 | `kernel_03_mkII_STABLE_ROUNDING` | 23.398 | 22.766 | 22.353 | 0.105 | 0.323 |
| 3 | `kernel_06_mkII` | 23.149 | 22.756 | 22.493 | 0.031 | 0.175 |
| 4 | `kernel_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.392 | 22.735 | 22.338 | 0.119 | 0.345 |
| 5 | `kernel_05_mkII_FMA` | 22.967 | 22.727 | 22.472 | 0.027 | 0.164 |
| 6 | `kernel_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 23.122 | 22.684 | 22.310 | 0.054 | 0.232 |
| 7 | `C_native_packed_custom_layout_FMA` | 22.975 | 22.674 | 22.224 | 0.041 | 0.202 |
| 8 | `kernel_03_mkII_STABLE_ROUNDING_FMA` | 23.151 | 22.555 | 22.197 | 0.059 | 0.242 |
| 9 | `C_native_01` | 23.139 | 22.533 | 22.091 | 0.071 | 0.267 |
| 10 | `kernel_01_mkII_STABLE_ROUNDING_FMA` | 21.862 | 21.301 | 21.037 | 0.050 | 0.223 |
| 11 | `kernel_03_mkII` | 21.143 | 20.753 | 20.617 | 0.022 | 0.148 |
| 12 | `kernel_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 20.449 | 19.965 | 19.524 | 0.064 | 0.254 |
| 13 | `kernel_02_mkII_STABLE_ROUNDING` | 20.041 | 19.826 | 19.604 | 0.015 | 0.122 |
| 14 | `kernel_02_mkII_STABLE_ROUNDING_FMA` | 19.805 | 19.607 | 19.453 | 0.011 | 0.104 |
| 15 | `kernel_02_mkII` | 18.595 | 18.256 | 18.043 | 0.026 | 0.161 |
| 16 | `kernel_05_mkII` | 18.422 | 18.227 | 18.033 | 0.016 | 0.127 |
| 17 | `kernel_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 18.532 | 18.178 | 17.953 | 0.024 | 0.154 |
| 18 | `kernel_01_mkII_STABLE_ROUNDING` | 18.429 | 18.136 | 17.773 | 0.048 | 0.220 |
| 19 | `kernel_04_mkII_STABLE_ROUNDING` | 18.537 | 17.991 | 17.677 | 0.045 | 0.211 |
| 20 | `kernel_01_mkII` | 17.630 | 17.439 | 17.269 | 0.009 | 0.096 |
| 21 | `kernel_04_mkII_STABLE_ROUNDING_FMA` | 17.208 | 16.835 | 16.620 | 0.039 | 0.198 |
| 22 | `kernel_04_mkII` | 16.591 | 16.127 | 15.850 | 0.046 | 0.214 |

</details>

## OpenMP Results

| Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Var MFLOPS | Stddev MFLOPS | Interpretation |
|---------|-----------:|-----------:|-----------:|-----------:|--------------:|----------------|
| `C_native_openmp_01` | 404.358 | 397.921 | 382.053 | 36.185 | 6.015 | Raw OpenMP split multiply/add baseline; one worker-local reusable product object. |
| `C_native_openmp_01_FMA` | 422.647 | 412.005 | 397.239 | 45.560 | 6.750 | Raw OpenMP FMA baseline; one `mpfr_fma` call per element inside each worker loop. |
| `kernel_openmp_01_mkII` | 397.552 | 388.010 | 372.971 | 59.407 | 7.708 | OpenMP worker loop; Direct expression form; FMA builds can lower to one `mpfr_fma` per element. |
| `kernel_openmp_01_mkII_STABLE_ROUNDING` | 388.881 | 382.625 | 374.177 | 24.528 | 4.953 | OpenMP worker loop; Direct expression form; FMA builds can lower to one `mpfr_fma` per element. Stable rounding is enabled. |
| `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA` | 422.613 | 414.601 | 404.807 | 30.961 | 5.564 | OpenMP worker loop; Direct expression form; FMA builds can lower to one `mpfr_fma` per element. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 420.011 | 412.811 | 396.439 | 58.378 | 7.641 | OpenMP worker loop; Direct expression form; FMA builds can lower to one `mpfr_fma` per element. Stable rounding, FMA, and fixed-precision fastpath are enabled. |
| `kernel_openmp_02_mkII` | 402.761 | 397.517 | 393.693 | 10.690 | 3.270 | OpenMP worker loop; Reusable product object using copy-then-multiply source shape. |
| `kernel_openmp_02_mkII_STABLE_ROUNDING` | 404.415 | 387.554 | 331.484 | 461.276 | 21.477 | OpenMP worker loop; Reusable product object using copy-then-multiply source shape. Stable rounding is enabled. |
| `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA` | 408.120 | 401.023 | 389.165 | 35.026 | 5.918 | OpenMP worker loop; Reusable product object using copy-then-multiply source shape. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 404.445 | 391.459 | 380.405 | 87.876 | 9.374 | OpenMP worker loop; Reusable product object using copy-then-multiply source shape. Stable rounding, FMA, and fixed-precision fastpath are enabled. |
| `kernel_openmp_03_mkII` | 405.126 | 395.355 | 372.712 | 99.632 | 9.982 | OpenMP worker loop; Reusable product object assigned from the product expression; split multiply/add hot loop. |
| `kernel_openmp_03_mkII_STABLE_ROUNDING` | 412.930 | 404.865 | 391.492 | 43.049 | 6.561 | OpenMP worker loop; Reusable product object assigned from the product expression; split multiply/add hot loop. Stable rounding is enabled. |
| `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA` | 413.187 | 395.771 | 372.361 | 131.640 | 11.473 | OpenMP worker loop; Reusable product object assigned from the product expression; split multiply/add hot loop. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 410.457 | 404.020 | 395.976 | 24.756 | 4.976 | OpenMP worker loop; Reusable product object assigned from the product expression; split multiply/add hot loop. Stable rounding, FMA, and fixed-precision fastpath are enabled. |
| `kernel_openmp_04_mkII` | 394.886 | 382.610 | 370.230 | 59.005 | 7.681 | OpenMP worker loop; Loop-local product object; intentionally expensive construction/control path. |
| `kernel_openmp_04_mkII_STABLE_ROUNDING` | 401.977 | 387.049 | 369.021 | 106.410 | 10.316 | OpenMP worker loop; Loop-local product object; intentionally expensive construction/control path. Stable rounding is enabled. |
| `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA` | 395.495 | 380.476 | 341.166 | 289.604 | 17.018 | OpenMP worker loop; Loop-local product object; intentionally expensive construction/control path. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 399.565 | 392.081 | 373.498 | 67.905 | 8.240 | OpenMP worker loop; Loop-local product object; intentionally expensive construction/control path. Stable rounding, FMA, and fixed-precision fastpath are enabled. |
| `kernel_openmp_05_mkII` | 404.269 | 391.510 | 368.697 | 105.857 | 10.289 | OpenMP worker loop; Explicit-context direct expression; rounding is captured outside the loop. |
| `kernel_openmp_05_mkII_FMA` | 421.818 | 413.181 | 391.186 | 84.907 | 9.215 | OpenMP worker loop; Explicit-context direct expression; rounding is captured outside the loop. FMA is enabled; whether it changes the hot loop depends on the source shape. |
| `kernel_openmp_06_mkII` | 412.276 | 406.960 | 402.329 | 8.767 | 2.961 | OpenMP worker loop; Explicit-context reusable product object; cached context rounding with split multiply/add. |

<details>
<summary>OpenMP results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Var MFLOPS | Stddev MFLOPS |
|------|---------|-----------:|-----------:|-----------:|-----------:|--------------:|
| 1 | `C_native_openmp_01_FMA` | 422.647 | 412.005 | 397.239 | 45.560 | 6.750 |
| 2 | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA` | 422.613 | 414.601 | 404.807 | 30.961 | 5.564 |
| 3 | `kernel_openmp_05_mkII_FMA` | 421.818 | 413.181 | 391.186 | 84.907 | 9.215 |
| 4 | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 420.011 | 412.811 | 396.439 | 58.378 | 7.641 |
| 5 | `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA` | 413.187 | 395.771 | 372.361 | 131.640 | 11.473 |
| 6 | `kernel_openmp_03_mkII_STABLE_ROUNDING` | 412.930 | 404.865 | 391.492 | 43.049 | 6.561 |
| 7 | `kernel_openmp_06_mkII` | 412.276 | 406.960 | 402.329 | 8.767 | 2.961 |
| 8 | `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 410.457 | 404.020 | 395.976 | 24.756 | 4.976 |
| 9 | `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA` | 408.120 | 401.023 | 389.165 | 35.026 | 5.918 |
| 10 | `kernel_openmp_03_mkII` | 405.126 | 395.355 | 372.712 | 99.632 | 9.982 |
| 11 | `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 404.445 | 391.459 | 380.405 | 87.876 | 9.374 |
| 12 | `kernel_openmp_02_mkII_STABLE_ROUNDING` | 404.415 | 387.554 | 331.484 | 461.276 | 21.477 |
| 13 | `C_native_openmp_01` | 404.358 | 397.921 | 382.053 | 36.185 | 6.015 |
| 14 | `kernel_openmp_05_mkII` | 404.269 | 391.510 | 368.697 | 105.857 | 10.289 |
| 15 | `kernel_openmp_02_mkII` | 402.761 | 397.517 | 393.693 | 10.690 | 3.270 |
| 16 | `kernel_openmp_04_mkII_STABLE_ROUNDING` | 401.977 | 387.049 | 369.021 | 106.410 | 10.316 |
| 17 | `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 399.565 | 392.081 | 373.498 | 67.905 | 8.240 |
| 18 | `kernel_openmp_01_mkII` | 397.552 | 388.010 | 372.971 | 59.407 | 7.708 |
| 19 | `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA` | 395.495 | 380.476 | 341.166 | 289.604 | 17.018 |
| 20 | `kernel_openmp_04_mkII` | 394.886 | 382.610 | 370.230 | 59.005 | 7.681 |
| 21 | `kernel_openmp_01_mkII_STABLE_ROUNDING` | 388.881 | 382.625 | 374.177 | 24.528 | 4.953 |

</details>

<details>
<summary>OpenMP results sorted by Avg MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS | Var MFLOPS | Stddev MFLOPS |
|------|---------|-----------:|-----------:|-----------:|-----------:|--------------:|
| 1 | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA` | 422.613 | 414.601 | 404.807 | 30.961 | 5.564 |
| 2 | `kernel_openmp_05_mkII_FMA` | 421.818 | 413.181 | 391.186 | 84.907 | 9.215 |
| 3 | `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 420.011 | 412.811 | 396.439 | 58.378 | 7.641 |
| 4 | `C_native_openmp_01_FMA` | 422.647 | 412.005 | 397.239 | 45.560 | 6.750 |
| 5 | `kernel_openmp_06_mkII` | 412.276 | 406.960 | 402.329 | 8.767 | 2.961 |
| 6 | `kernel_openmp_03_mkII_STABLE_ROUNDING` | 412.930 | 404.865 | 391.492 | 43.049 | 6.561 |
| 7 | `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 410.457 | 404.020 | 395.976 | 24.756 | 4.976 |
| 8 | `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA` | 408.120 | 401.023 | 389.165 | 35.026 | 5.918 |
| 9 | `C_native_openmp_01` | 404.358 | 397.921 | 382.053 | 36.185 | 6.015 |
| 10 | `kernel_openmp_02_mkII` | 402.761 | 397.517 | 393.693 | 10.690 | 3.270 |
| 11 | `kernel_openmp_03_mkII_STABLE_ROUNDING_FMA` | 413.187 | 395.771 | 372.361 | 131.640 | 11.473 |
| 12 | `kernel_openmp_03_mkII` | 405.126 | 395.355 | 372.712 | 99.632 | 9.982 |
| 13 | `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 399.565 | 392.081 | 373.498 | 67.905 | 8.240 |
| 14 | `kernel_openmp_05_mkII` | 404.269 | 391.510 | 368.697 | 105.857 | 10.289 |
| 15 | `kernel_openmp_02_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` | 404.445 | 391.459 | 380.405 | 87.876 | 9.374 |
| 16 | `kernel_openmp_01_mkII` | 397.552 | 388.010 | 372.971 | 59.407 | 7.708 |
| 17 | `kernel_openmp_02_mkII_STABLE_ROUNDING` | 404.415 | 387.554 | 331.484 | 461.276 | 21.477 |
| 18 | `kernel_openmp_04_mkII_STABLE_ROUNDING` | 401.977 | 387.049 | 369.021 | 106.410 | 10.316 |
| 19 | `kernel_openmp_01_mkII_STABLE_ROUNDING` | 388.881 | 382.625 | 374.177 | 24.528 | 4.953 |
| 20 | `kernel_openmp_04_mkII` | 394.886 | 382.610 | 370.230 | 59.005 | 7.681 |
| 21 | `kernel_openmp_04_mkII_STABLE_ROUNDING_FMA` | 395.495 | 380.476 | 341.166 | 289.604 | 17.018 |

</details>

## Memory Bandwidth Estimates

These are model estimates, not hardware-counter measurements.  For 512-bit MPFR,
this report assumes an `mpfr_t` header stride of 32 bytes and 8 limbs = 64 limb
bytes per value.  The timed operation reads `x[i]`, reads `y[i]`, and writes
`y[i]`; `alpha` is loop-invariant and excluded.

```text
limb-only bytes/element      = 64 x + 64 y read + 64 y write = 192 B
header-inclusive bytes/elem  = 96 x + 96 y read + 96 y write = 288 B
GB/s estimate                = Avg MFLOPS * bytes_per_element / 2000
```

| Variant | Mode | Avg MFLOPS | Limb-only GB/s | Header-inclusive GB/s |
|---------|------|-----------:|---------------:|----------------------:|
| `C_native_01_FMA` | Serial | 22.823 | 2.19 | 3.29 |
| `C_native_openmp_01_FMA` | OpenMP | 412.005 | 39.55 | 59.33 |
| `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA` | OpenMP | 414.601 | 39.80 | 59.70 |
| `kernel_openmp_05_mkII_FMA` | OpenMP | 413.181 | 39.67 | 59.50 |

The OpenMP best paths imply roughly 40 GB/s limb-only traffic and 60 GB/s
header-inclusive active traffic under this simple model.  This excludes MPFR
internal metadata, allocator effects, cache-line overfetch, and repeated
initialization outside the timed loop.

## Comparison with GMP version

The MPFR and GMP Raxpy runs should be compared by source-level class, not only
by the fastest headline row.  In the serial split multiply/add class, GMP is
still faster: the GMP `C_native_01` average is 33.634 MFLOPS, while the MPFR
`C_native_01` average is 22.533 MFLOPS.  That comparison has the closest timed
loop shape on both sides: one reusable product object, one multiply, and one add
per element.

The apparent MPFR advantage appears in the OpenMP and FMA classes.  The OpenMP
split baselines are close (`C_native_openmp_01`: GMP 390.629 MFLOPS average,
MPFR 397.921 MFLOPS average), which is small enough to treat as the same broad
memory-traffic class.  The MPFR FMA paths are a different class: `mpfr_fma`
removes the explicit product temporary and changes the backend arithmetic call
sequence.  GMP `mpf_t` has no equivalent fused Raxpy call in this benchmark, so
the MPFR FMA rows should not be interpreted as direct evidence that the MPFR
split multiply/add implementation is faster than GMP.

The object-layout model also needs care.  The GMP report's 88-byte
header-inclusive estimate is an active-limb model: 24-byte `mpf_t` header plus
8 active 64-bit limbs.  On this build, however, `mpf_init2(..., 512)` gives
`_mp_prec = 9`, so the allocated-capacity footprint is closer to 96 bytes per
value.  MPFR at 512 bits uses a 32-byte `mpfr_t` header and 8 limbs, also about
96 bytes per value.  The useful distinction is therefore less "88 bytes versus
96 bytes" and more the header stride and backend policy: GMP arrays advance by
24 bytes per header, while MPFR arrays advance by 32 bytes per header, and MPFR
adds an explicit rounding operand to each arithmetic call.

This suggests the current performance picture:

| Comparison point | GMP average | MPFR average | Interpretation |
|------------------|------------:|-------------:|----------------|
| Serial split C native | 33.634 MFLOPS | 22.533 MFLOPS | GMP is faster for the closest split multiply/add serial baseline. |
| OpenMP split C native | 390.629 MFLOPS | 397.921 MFLOPS | Same broad streaming class; MPFR is slightly ahead in this run. |
| OpenMP fused C native | none | 412.005 MFLOPS | MPFR-only FMA class; not directly comparable to GMP split Raxpy. |
| Best OpenMP wrapper | 393.019 MFLOPS | 414.601 MFLOPS | MPFR best row uses an FMA-capable direct-expression path. |

The practical conclusion is that MPFR is not generally faster in the matching
serial split path.  Its best OpenMP rows benefit from a fused backend operation
and a 32-byte header stride that may be friendlier to cache-line streaming than
GMP's 24-byte header stride, while GMP's effective 512-bit `mpf_t` allocation is
not actually smaller once the extra precision limb is included.

## Hotpath Disassembly

Representative command shape:

```bash
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/01_Raxpy/<executable>
```

Addresses are build-specific; the relevant evidence is the backend call
sequence, where reusable temporaries are initialized and cleared, and whether
rounding is cached or read inside the loop.

`Raxpy_mpfr_C_native_01` has one reusable `mpfr_t` product object.  It is
initialized before the timed loop, the rounding mode is cached once, and the
loop body has one `mpfr_mul` plus one `mpfr_add` per element.  The temporary is
cleared after the loop.

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

`Raxpy_mpfr_C_native_01_FMA` is the raw FMA baseline.  There is no reusable
product temporary in this source shape, so no `mpfr_init`/`mpfr_clear` appears
around the hot loop; the loop has one `mpfr_fma` per element and uses the cached
rounding register.

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

`Raxpy_mpfr_kernel_03_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH` stays in
the split multiply/add class even in an FMA-enabled build, because source variant
`03` explicitly materializes `temp = alpha * x[i]` before adding it to `y[i]`.
The wrapper initializes one reusable `mpfr_class` product object before the loop
and clears it after the loop.  Compared with C native split mode, the loop still
has one `mpfr_mul` and one `mpfr_add`, but it also carries stable-rounding TLS
loads and first-use initialization checks.

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

`Raxpy_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING` uses an OpenMP outlined worker.
Each worker initializes one reusable product object before its slice, runs the
same split multiply/add loop, then reaches `GOMP_barrier` and clears the worker
local temporary after the hot loop.

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

The important comparison with the GMP Raxpy disassembly is structural: the
split MPFR variants mirror the GMP reusable-temporary pattern, but MPFR carries
rounding-mode operands and, in wrapper paths, default-state TLS checks.  Direct
FMA variants are a different source-level class: they remove the product
temporary entirely and therefore do not show init/clear around the hot loop.

## Lessons Learned

The primary source-level boundary is whether the hot loop reaches one direct
`mpfr_fma` per element or remains a split `mpfr_mul` + `mpfr_add` path.  Direct
expression kernels (`01`) and explicit-context direct kernels (`05`) can enter
the FMA class; reusable-product kernels (`03`/`06`) stay in the split class by
construction.

Stable rounding and fixed-precision assumptions matter only when they change the
emitted hot loop or remove repeated wrapper work.  They do not make a
loop-local temporary (`04`) competitive with reusable-resource or direct-FMA
shapes.

The best serial and OpenMP variants are close enough that single-run maxima are
less important than performance classes.  The repeat-10 variance shows that
OpenMP ranking should be interpreted by average and hotpath shape, not by one
maximum number.

The packed custom-layout experiment did not create a new 512-bit serial
performance class in this run.  At this vector size and precision, the arithmetic
call shape dominates the visible difference more than the tested layout change.
