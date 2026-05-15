<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 01_Raxpy

This directory benchmarks the MPFR real AXPY operation

```text
y_i = y_i + alpha * x_i
```

with random `mpfr_t` and `mpfrxx::mpfr_class` data at a fixed precision.  The
kernel numbering follows the GMP `01_Raxpy` benchmark where the same source
shape exists.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

The executables are created under:

```text
build_bench_release/benchmarks/mpfr/01_Raxpy/
```

## Run

Run the MPFR benchmark set through the common MPFR runner:

```bash
benchmarks/common/run_mpfr_benchmarks.sh build_bench_release 512
```

Individual executables take:

```text
<vector size> <precision>
```

Example:

```bash
build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_03_mkII 10000000 512
```

For OpenMP repeat runs, keep affinity explicit:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread \
build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_03_mkII \
    10000000 512
```

## Kernel Shapes

The timed body is `_Raxpy()` in each benchmark executable.  The `Raxpy()`
helper in `Raxpy.hpp` is the post-run correctness reference.

| Variant | Timed source shape | Temporary policy | GMP parallel |
|---------|--------------------|------------------|--------------|
| `C_native_01` | `mpfr_mul(temp, alpha, x[i], rnd); mpfr_add(y[i], y[i], temp, rnd);` | Raw `mpfr_t` product object initialized once. | Same role as GMP raw C native. |
| `C_native_01_FMA` | `mpfr_fma(y[i], alpha, x[i], y[i], rnd);` | No product temporary. | MPFR-specific baseline; GMP `mpf_t` has no matching FMA API. |
| `kernel_01` | `y[i] += alpha * x[i];` | Expression-first source shape. | Matches GMP `kernel_01`. |
| `kernel_02` | `temp = alpha; temp *= x[i]; y[i] += temp;` | One reusable product object, assigned from `alpha` then multiplied in place. | Matches GMP `kernel_02`. |
| `kernel_03` | `temp = alpha * x[i]; y[i] += temp;` | One reusable product object assigned from the product expression. | Matches GMP `kernel_03`. |
| `kernel_04` | `mpfr_class temp = alpha * x[i]; y[i] += temp;` | Loop-local product object. | Matches GMP `kernel_04`; intentionally allocation-heavy. |
| `kernel_openmp_01` | `y[i] += alpha * x[i];` inside `#pragma omp parallel for schedule(static)`. | One independent `y[i]` update per iteration. | Matches GMP OpenMP `kernel_01`. |
| `kernel_openmp_02` | `temp = alpha; temp *= x[i]; y[i] += temp;` inside `#pragma omp parallel for private(temp) schedule(static)`. | One private reusable product object per thread. | Matches GMP OpenMP `kernel_02`. |
| `kernel_openmp_03` | `temp = alpha * x[i]; y[i] += temp;` inside `#pragma omp parallel` plus `omp for schedule(static)`. | One reusable product object per thread, assigned from the expression. | Matches GMP OpenMP `kernel_03`. |

## Variant Suffixes

Each wrapper kernel is built as:

| Suffix | Build option |
|--------|--------------|
| `_mkII` | Default wrapper behavior. |
| `_mkII_STABLE_ROUNDING` | Assumes MPFR rounding mode is loop-invariant. |
| `_mkII_FMA` | Enables the wrapper FMA expression path where the source expression exposes a fused multiply-add. |
| `_mkII_STABLE_ROUNDING_FMA` | Combines stable rounding and FMA expression support. |

The FMA suffix is most relevant to expression-first shapes such as `kernel_01`.
Reusable-product kernels split multiply and add in source, so the suffix is
kept for build-matrix symmetry but should be checked by disassembly before it
is interpreted as an actual `mpfr_fma` hot path.

## Comparison With GMP Raxpy

The source-shape ladder is intentionally parallel to GMP:

```text
01: expression-first AXPY
02: reusable product object via copy-then-multiply
03: reusable product object via expression assignment
04: loop-local product object
OpenMP 01-03: matching parallel forms
```

The MPFR kernels additionally expose rounding-mode behavior.  `STABLE_ROUNDING`
variants are expected to remove repeated default-rounding lookups when the
wrapper can treat rounding as loop-invariant.  FMA-capable source shapes may
collapse multiply and add into one `mpfr_fma` call; this has no direct GMP
`mpf_t` equivalent.
