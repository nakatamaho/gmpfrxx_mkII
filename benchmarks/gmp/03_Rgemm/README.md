<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 03_Rgemm

This directory benchmarks the GMP real dense matrix-matrix product

```text
C = alpha * A * B + beta * C
```

with random `mpf` data at a fixed precision.  It compares raw `mpf_t`,
upstream `gmpxx.h`, `gmpxx_mkII`, and `gmpxx_mkII` built with
`GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

The executables are created under:

```text
build_bench_release/benchmarks/gmp/03_Rgemm/
```

## Run

Run the whole benchmark set through the top-level runner:

```bash
benchmarks/common/run_benchmarks.sh build_bench_release 512
```

For a quick Rgemm-sized smoke run, pass smaller dimensions:

```bash
benchmarks/common/run_benchmarks.sh build_bench_release 128 1000 1000 32 32 16 16 16 \
    benchmarks/gmp/results-smoke
```

The `RGEMM_M RGEMM_K RGEMM_N` runner arguments are used for Rgemm.  Individual
executables take:

```text
<rows m> <cols k> <cols n> <precision>
```

Example:

```bash
build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_01_mkII 500 500 500 512
```

## Reading Results

Each executable prints `Elapsed time`, `MFLOPS`, `L1 Norm of difference`, and a
`Result OK` or `Result NG` check against the reference result.  Higher MFLOPS is
better when the correctness check is `Result OK`.

Variant names:

- `C_native`: raw `mpf_t` implementation.
- `C_native_openmp`: raw `mpf_t` implementation with OpenMP.
- `*_orig`: upstream `gmpxx.h`.
- `*_mkII`: this header with the default precision policy.
- `*_mkII_FIXED_PRECISION_FASTPATH`: this header with `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`.
- `*_openmp_*`: OpenMP variant where the eager benchmark provided one.

## Recorded go.sh Sample

![Rgemm serial benchmark](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemm.png)

![Rgemm OpenMP benchmark](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemm.png)

The committed sample run uses the original `go.sh` dimensions:

```text
M = 500, K = 500, N = 500, precision = 512
```

Results are stored in [../results_raw/Linux_Ryzen_3970X_32-Core/](../results_raw/Linux_Ryzen_3970X_32-Core/):

- [Raw log](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331.log)
- [Serial plot](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemm.png)
- [Serial PDF](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemm.pdf)
- [OpenMP plot](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemm.png)
- [OpenMP PDF](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemm.pdf)

All Rgemm variants in that run report `Result OK`.

OpenMP has the strongest effect here: the timed matrix-matrix body improves by
about 24-31x in the recorded run.  Rgemm has much higher arithmetic intensity
than the vector kernels, so the OpenMP speedup is closer to what one expects
from the available cores.  In serial mode, `kernel_03` is the fastest wrapper
family in this run, while OpenMP makes the differences between wrapper
variants smaller than the difference between serial and parallel execution.

## Recorded OpenMP 02/04/05/06 Sweep

Commit `f996de7` records a focused OpenMP sweep for Rgemm kernels `02`, `04`,
`05`, and `06` at 512-bit precision. The run used 32 OpenMP threads and square
matrices with sizes:

```text
N = M = K = 13 + 31*k, up to 1024
extra sizes: 128, 256, 512, 768, 1024
```

The result artifacts are:

- [CSV](../../results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_512.csv)
- [Raw log](../../results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_512.log)

![GMP Rgemm OpenMP 02/04/05/06 sweep, 512-bit precision](../../results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_512.png)

All 456 runs report `Result OK`.

Kernel shapes:

| Kernel | Shape | Intent | Observed behavior |
|---|---|---|---|
| `02` | Rank-1 update, `j -> p -> i` | Reuse `alpha * B[p,j]` across all rows. | Simple and stable; strong at small and mid sizes, especially for wrapper variants. |
| `04` | 4x4 `C` tile accumulator | Accumulate a 4x4 output tile in scratch objects before writing back. | Usually weak for GMP `mpf_t`/`mpf_class`; the scratch accumulators are expensive objects, not CPU registers. |
| `05` | Four-column `B` panel | Compute `alpha * B[p,j:j+3]` once and stream it over all rows. | Strong large-size C native path and often competitive for wrappers. |
| `06` | Kernel `05` plus fixed 256-row blocking | Keep the four-column `B` panel and process rows in `min(M, 256)` chunks. | Best overall in the recorded sweep; also the best `gmpxx_mkII` wrapper result. |

Representative winners:

| Category | Variant | Size | MFLOPS |
|---|---:|---:|---:|
| Best overall | `Rgemm_gmp_C_native_openmp_06` | `N=1024` | `883.060` |
| Best `gmpxx_mkII` wrapper | `Rgemm_gmp_kernel_openmp_06_mkII` | `N=768` | `842.238` |
| Best upstream `gmpxx.h` wrapper | `Rgemm_gmp_kernel_openmp_05_orig` | `N=768` | `836.414` |

Selected-size winners:

| Size | Winner | MFLOPS |
|---:|---|---:|
| `128` | `Rgemm_gmp_kernel_openmp_02_mkII` | `399.283` |
| `256` | `Rgemm_gmp_C_native_openmp_06` | `772.462` |
| `512` | `Rgemm_gmp_C_native_openmp_06` | `874.884` |
| `768` | `Rgemm_gmp_C_native_openmp_06` | `881.138` |
| `1024` | `Rgemm_gmp_C_native_openmp_06` | `883.060` |

The main conclusion from this sweep is that the 4x4 accumulator form in
`kernel_04` does not translate well to GMP floating objects. In ordinary
floating-point GEMM this would map naturally to registers, but each GMP
accumulator is a managed multiple-precision object. The four-column panel in
`kernel_05` is a better match because it reuses a small number of scaled `B`
values without creating a 4x4 object accumulator. `kernel_06` keeps that
structure and adds row blocking, which improves OpenMP work granularity and
large-size behavior in this measurement.

## Recorded OpenMP 02/04/05/06 Sweep, 1024-bit Precision

The same focused OpenMP sweep was repeated at 1024-bit precision with the same
32-thread setup and the same square sizes:

```text
N = M = K = 13 + 31*k, up to 1024
extra sizes: 128, 256, 512, 768, 1024
```

The result artifacts are:

- [CSV](../../results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.csv)
- [Raw log](../../results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.log)

![GMP Rgemm OpenMP 02/04/05/06 sweep, 1024-bit precision](../../results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.png)

All 456 runs report `Result OK`. The summed timed-loop elapsed time in the CSV
is 750.337 seconds.

Representative winners:

| Category | Variant | Size | MFLOPS |
|---|---:|---:|---:|
| Best overall | `Rgemm_gmp_kernel_openmp_02_mkII` | `N=512` | `732.351` |
| Best `gmpxx_mkII` wrapper | `Rgemm_gmp_kernel_openmp_02_mkII` | `N=512` | `732.351` |
| Best upstream `gmpxx.h` wrapper | `Rgemm_gmp_kernel_openmp_05_orig` | `N=768` | `322.737` |
| Best C native path | `Rgemm_gmp_C_native_openmp_05` | `N=768` | `330.286` |

Selected-size winners:

| Size | Winner | MFLOPS |
|---:|---|---:|
| `128` | `Rgemm_gmp_kernel_openmp_02_mkII` | `341.896` |
| `256` | `Rgemm_gmp_kernel_openmp_02_mkII` | `643.923` |
| `512` | `Rgemm_gmp_kernel_openmp_02_mkII` | `732.351` |
| `768` | `Rgemm_gmp_kernel_openmp_02_mkII` | `719.399` |
| `1024` | `Rgemm_gmp_kernel_openmp_02_mkII` | `719.306` |

The 1024-bit result changes the ranking from the 512-bit sweep. At 512-bit,
large sizes favored `C_native_openmp_06`, while at 1024-bit
`kernel_openmp_02_mkII` dominates from the mid-size range through `N=1024`.
The rank-1 update shape in `kernel_02` reuses `alpha * B[p,j]` across all rows
without the 4x4 managed-object accumulator used by `kernel_04` and without the
extra panel scratch traffic used by `kernel_05` and `kernel_06`. With the
larger `mpf` precision, that lower scratch-object pressure matters more than
the row blocking that helped at 512-bit.

`kernel_04` remains the weakest design family in this sweep. Its 4x4
accumulator form is still a poor fit for GMP floating values because each
accumulator is a multiple-precision object with managed storage, not a CPU
register. The `05` and `06` panel forms are better than `04`, but at 1024-bit
they mostly settle in the 400-460 MFLOPS range for `gmpxx_mkII`, well below the
roughly 700-730 MFLOPS plateau reached by `kernel_openmp_02_mkII`.
