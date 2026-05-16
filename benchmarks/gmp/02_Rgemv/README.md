<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 02_Rgemv

This directory benchmarks the GMP real dense matrix-vector product

```text
y = alpha * A * x + beta * y
```

with random `mpf` data at a fixed precision.  It compares raw `mpf_t`,
upstream `gmpxx.h`, `gmpxx_mkII`, and `gmpxx_mkII` built with
`GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`.

This README follows the same structure as `../01_Raxpy/README.md`: build and
run instructions, result interpretation, recorded samples, kernel shapes,
hotpath disassembly, and lessons learned.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

The executables are created under:

```text
build_bench_release/benchmarks/gmp/02_Rgemv/
```

## Run

Run the whole benchmark set through the top-level runner:

```bash
benchmarks/common/run_benchmarks.sh build_bench_release 512
```

For a quick Rgemv-sized smoke run, pass smaller dimensions:

```bash
benchmarks/common/run_benchmarks.sh build_bench_release 128 1000 1000 32 32 16 16 16 \
    benchmarks/gmp/results-smoke
```

The `RGEMV_M RGEMV_N` runner arguments are used for Rgemv.  Individual
executables take:

```text
<rows m> <cols n> <precision>
```

Example:

```bash
build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_01_mkII 4000 4000 512
```

For OpenMP runs, keep affinity explicit:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread \
build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_03_mkII \
    4000 4000 512
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

Current C++ kernel source shapes:

| Kernel | Shape |
|---|---|
| `kernel_01` | Column-major AXPY form with direct expression use: `y[i] += (alpha * x[j]) * A[i + j * lda]`. |
| `kernel_02` | Column-major AXPY form with reusable `temp` and `templ`, using copy-then-multiply steps. |
| `kernel_03` | Column-major AXPY form with reusable `temp` and `templ`, assigning expression results into those objects. |
| `kernel_04` | Column-major AXPY form with loop-local temporary objects, matching the loop-local product-object style used in Raxpy. |
| `kernel_openmp_01` | Row-partitioned OpenMP direct-expression form. It avoids races on `y[i]` by giving each row to one thread. |
| `kernel_openmp_02` | Row-partitioned OpenMP form with per-thread reusable `temp` and `templ`, using copy-then-multiply steps. |
| `kernel_openmp_03` | Row-partitioned OpenMP form with per-thread reusable `temp` and `templ`, assigning expression results into those objects. |

The serial kernels use the same column-oriented update order as the reference
BLAS-style `Rgemv` routine, so they are close to a sequence of Raxpy updates.
The OpenMP kernels deliberately switch to row partitioning because the
column-oriented update would otherwise write the same `y[i]` from multiple
threads.  This makes the OpenMP kernels safe and keeps the benchmark focused on
GMP arithmetic and wrapper temporary behavior instead of synchronization.

## Recorded go.sh Sample

![Rgemv serial benchmark](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemv.png)

![Rgemv OpenMP benchmark](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemv.png)

The committed sample run uses the original `go.sh` dimensions:

```text
M = 4000, N = 4000, precision = 512
```

Results are stored in [../results_raw/Linux_Ryzen_3970X_32-Core/](../results_raw/Linux_Ryzen_3970X_32-Core/):

- [Raw log](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331.log)
- [Serial plot](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemv.png)
- [Serial PDF](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_serial_Rgemv.pdf)
- [OpenMP plot](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemv.png)
- [OpenMP PDF](../results_raw/Linux_Ryzen_3970X_32-Core/benchmark_20260430_081331_Linux_Ryzen_3970X_32-Core_openmp_Rgemv.pdf)

That sample predates the current `kernel_03`, `kernel_04`, and
`kernel_openmp_03` split.  It also predates the row-partitioned OpenMP rewrite
that removes loop-local `mpf_init`/`mpf_clear` from the timed inner loops.
New runs should use the current `go.sh` or the common runner so the expanded
kernel set is included.

## Recorded Refactored Sample

The focused local run used:

```text
M = 4000
N = 4000
precision = 512
OMP_NUM_THREADS = 32
OMP_PLACES = cores
OMP_PROC_BIND = spread
repeat = 1
```

Results are stored in this directory:

- [Raw log](results_raw/rgemv_gmp_m4000_n4000_p512_20260516_122201.log)
- [CSV summary](results_raw/rgemv_gmp_m4000_n4000_p512_summary_20260516_122201.csv)

All variants in this run report `Result OK`.

| Variant | MFLOPS | Interpretation |
|---------|-------:|----------------|
| `C_native_01` | 31.421 | Raw serial baseline. |
| `C_native_openmp_01` | 241.372 | Raw OpenMP baseline. |
| `kernel_01_orig` | 16.973 | Direct expression form; product expression materializes in the inner loop. |
| `kernel_01_mkII` | 16.016 | Same source shape as upstream `kernel_01`; slowest default wrapper path in this run. |
| `kernel_01_mkII_FIXED_PRECISION_FASTPATH` | 16.633 | Fastpath does not remove the source-shape cost here. |
| `kernel_02_orig` | 29.775 | Reusable `temp`/`templ` with copy-then-multiply steps. |
| `kernel_02_mkII` | 29.958 | Same class as C native, but still carries an explicit copy pattern. |
| `kernel_02_mkII_FIXED_PRECISION_FASTPATH` | 29.680 | Same class as default `kernel_02`. |
| `kernel_03_orig` | 32.014 | Best serial variant in this run. |
| `kernel_03_mkII` | 31.475 | Best serial mkII shape; same broad class as C native. |
| `kernel_03_mkII_FIXED_PRECISION_FASTPATH` | 31.392 | Same class as default `kernel_03`. |
| `kernel_04_orig` | 26.754 | Loop-local product objects are slower. |
| `kernel_04_mkII` | 23.400 | Loop-local temporary lifetime is costly for default mkII. |
| `kernel_04_mkII_FIXED_PRECISION_FASTPATH` | 26.581 | Fastpath helps, but this remains behind `kernel_03`. |
| `kernel_openmp_01_orig` | 234.026 | Row-partitioned OpenMP direct-expression form. |
| `kernel_openmp_01_mkII` | 226.587 | Same class, slightly slower in this single run. |
| `kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH` | 228.345 | Same class as default OpenMP 01. |
| `kernel_openmp_02_orig` | 235.763 | Row-partitioned reusable copy-then-multiply form. |
| `kernel_openmp_02_mkII` | 236.324 | Same class as upstream OpenMP 02. |
| `kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH` | 237.787 | Same class as default OpenMP 02. |
| `kernel_openmp_03_orig` | 241.843 | Best OpenMP variant in this run. |
| `kernel_openmp_03_mkII` | 240.741 | Best OpenMP mkII shape; effectively C native OpenMP class. |
| `kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH` | 240.693 | Same class as default OpenMP 03. |

The main serial result matches Raxpy: `kernel_03` is the useful wrapper shape.
It keeps product storage outside the inner loop and assigns expression results
into reusable objects.  `kernel_01` is much slower because the direct nested
expression causes inner-loop product materialization.  `kernel_04` is slower
because it deliberately makes loop-local product objects.

The main OpenMP result is that row partitioning fixes the earlier OpenMP 02
correctness problem and puts all OpenMP 01/02/03 variants into the same
230-242 MFLOPS class.  The best mkII OpenMP path is `kernel_openmp_03_mkII` at
240.741 MFLOPS, essentially the same as raw C native OpenMP in this single run.

## Kernel Shapes

The timed body is `_Rgemv()` in each benchmark executable.  The `Rgemv()`
helper in `Rgemv.hpp` is the post-run correctness reference and should not be
mixed with the timed-kernel source-shape comparison.

| Variant | Timed source shape | Temporary policy | Hotpath meaning |
|---------|--------------------|------------------|-----------------|
| `C_native_01` | Column-major `temp_b = alpha * x[j]`; inner `prod = temp_b * A[i + j*lda]; y[i] += prod`. | Raw `mpf_t` `temp_b` and `prod` initialized once. | Serial raw-GMP baseline: one outer multiply plus one inner multiply/add per matrix element. |
| `C_native_openmp_01` | Row-partitioned OpenMP; each thread owns a subset of `y[i]`. | Raw `mpf_t` temporaries initialized once per thread. | Parallel raw-GMP baseline without races on `y`. |
| `kernel_01` | Column-major `y[i] += (alpha * x[j]) * A[i + j * lda]`. | Direct nested expression. | Tests expression-template materialization in the inner loop. |
| `kernel_02` | Column-major `temp = alpha; temp *= x[j]; templ = temp; templ *= A[i + j*lda]; y[i] += templ`. | Reusable `temp` and `templ`, copy-then-multiply. | Avoids loop-local construction but pays explicit copies. |
| `kernel_03` | Column-major `temp = alpha * x[j]; templ = temp * A[i + j*lda]; y[i] += templ`. | Reusable `temp` and `templ`, expression assignment. | Best serial wrapper shape: reusable storage and direct product assignment. |
| `kernel_04` | Column-major loop-local `mpf_class temp` and `mpf_class templ`. | Loop-local product objects. | Allocation/lifetime stress case; expected to be slower. |
| `kernel_openmp_01` | Row-partitioned direct expression. | One independent `y[i]` per thread chunk. | Safe OpenMP version of direct-expression source. |
| `kernel_openmp_02` | Row-partitioned reusable copy-then-multiply. | Private reusable `temp` and `templ` per thread. | Safe OpenMP version of `kernel_02`. |
| `kernel_openmp_03` | Row-partitioned reusable expression assignment. | Private reusable `temp` and `templ` per thread. | Safe OpenMP version of `kernel_03`; best OpenMP wrapper shape here. |

The serial and OpenMP source shapes are intentionally not identical.  Serial
Rgemv follows the BLAS-like column-major AXPY order.  OpenMP uses row
partitioning because the column-major update would make all threads contend on
the same `y` vector.

## Hotpath Disassembly

The snippets below are from Release binaries under
`build_bench_release/benchmarks/gmp/02_Rgemv/`.  They focus on the timed
`_Rgemv()` loop or, for OpenMP, the outlined loop body.

### `C_native_01`

Raw C native initializes `temp_b` and `prod` once, scales `y` once, then uses a
column-major AXPY loop.  The inner loop is one `mpf_mul` plus one `mpf_add`.

```asm
56c0: mov    0x8(%rsp),%rdx       # x[j]
56c5: mov    0x20(%rsp),%rsi      # alpha
56ca: lea    0x40(%rsp),%rdi      # temp_b
56cf: call   __gmpf_mul@plt       # temp_b = alpha * x[j]

5700: mov    %r14,%rdx            # A[i + j*lda]
5703: lea    0x40(%rsp),%rsi      # temp_b
5708: mov    %rbp,%rdi            # prod
570f: call   __gmpf_mul@plt       # prod = temp_b * A
5714: mov    %rbx,%rsi            # y[i]
5717: mov    %rbx,%rdi            # y[i]
571a: mov    %rbp,%rdx            # prod
571d: call   __gmpf_add@plt       # y[i] += prod
5722: add    $0x18,%r14           # A++
5726: add    $0x18,%rbx           # y++
572d: jne    5700
```

### `kernel_01_mkII`

`kernel_01` is the direct nested-expression source shape.  The hotpath shows
why it is slower: the inner loop initializes and clears a product temporary and
enters expression evaluation before the final multiply/add.

```asm
57d0: mov    0x10(%rsp),%rax
57d5: mov    %rbx,%rdi            # y[i], used for precision
57e7: call   __gmpf_get_prec@plt
57ec: mov    %rbp,%rdi            # loop-local product
57f5: call   __gmpf_init2@plt
57fa: mov    0x18(%rsp),%rsi      # binary expression node
5805: call   mpf_evaluate<mul_op,...>
580a: mov    %r12,%rdx            # A[i + j*lda]
580d: mov    %rbp,%rsi            # evaluated alpha*x[j]
5813: call   __gmpf_mul@plt
5818: mov    %rbp,%rdx
581b: mov    %rbx,%rsi
5821: call   __gmpf_add@plt
5826: mov    %rbp,%rdi
5835: call   __gmpf_clear@plt
583f: jne    57d0
```

This explains the measured gap: `kernel_01_mkII` is 16.016 MFLOPS while
`kernel_03_mkII` is 31.475 MFLOPS in the recorded run.

### `kernel_03_mkII`

`kernel_03` keeps `temp` and `templ` outside the loop.  After one-time
initialization, the inner loop is the same call class as C native: one
`mpf_mul` plus one `mpf_add`.

```asm
57c0: mov    0x8(%rsp),%rdx       # x[j]
57c5: mov    0x20(%rsp),%rsi      # alpha
57ca: lea    0x40(%rsp),%rdi      # temp
57cf: call   __gmpf_mul@plt       # temp = alpha * x[j]

5800: mov    %r12,%rdx            # A[i + j*lda]
5803: lea    0x40(%rsp),%rsi      # temp
5808: mov    %r13,%rdi            # templ
580b: call   __gmpf_mul@plt       # templ = temp * A
5810: mov    %r13,%rdx            # templ
5813: mov    %rbx,%rsi            # y[i]
5816: mov    %rbx,%rdi            # y[i]
5819: call   __gmpf_add@plt       # y[i] += templ
581e: add    $0x1,%rbp
5822: add    $0x18,%r12           # A++
5826: add    $0x18,%rbx           # y++
582d: jne    5800
```

This is why `kernel_03_mkII` is in the C native serial performance class.

### `kernel_openmp_03_mkII`

The OpenMP path is row-partitioned.  Each thread initializes private `temp` and
`templ` once.  For each owned row, it first scales `y[i]`, then loops over
columns.  The inner column loop has two multiplies and one add:

```asm
4020: mov    0x30(%r15),%rdx      # beta
4024: mov    %r14,%rsi            # y[i]
4027: mov    %r14,%rdi            # y[i]
402f: call   __gmpf_mul@plt       # y[i] *= beta

4070: mov    0x10(%rbx),%rsi      # alpha
4074: mov    0x8(%rsp),%rdi       # temp
4079: mov    %r15,%rdx            # x[j]
4084: call   __gmpf_mul@plt       # temp = alpha * x[j]
4089: mov    0x8(%rsp),%rsi       # temp
408e: mov    %r13,%rdx            # A[i + j*lda]
4091: mov    %rbp,%rdi            # templ
4094: call   __gmpf_mul@plt       # templ = temp * A
4099: mov    %rbp,%rdx            # templ
409c: mov    %r14,%rsi            # y[i]
409f: mov    %r14,%rdi            # y[i]
40a2: call   __gmpf_add@plt       # y[i] += templ
40a7: add    0x18(%rsp),%r13      # next A row-stride access
40b1: jne    4070
```

Compared with serial `kernel_03`, this row-partitioned OpenMP path recomputes
`alpha * x[j]` for each row instead of once per column.  That is the price paid
to avoid races on `y[i]`.  Despite the extra outer-loop work, it reaches
240.741 MFLOPS on 32 threads in the recorded run.

## Lessons Learned

- The Raxpy README structure maps well to Rgemv, but Rgemv needs one extra
  distinction: serial and OpenMP use different traversal orders for correctness
  and race avoidance.
- Serial Rgemv should be read as a sequence of column-major Raxpy updates.
  With that shape, `kernel_03` is the useful wrapper source form.
- Direct nested expressions in `kernel_01` are not free.  The disassembly shows
  loop-local initialization, expression evaluation, and clearing in the hot
  path.
- Reusable expression-assignment temporaries in `kernel_03` reduce the hot path
  to the same GMP call class as C native.
- Loop-local product objects in `kernel_04` remain useful as a stress case, but
  they are not the optimization target.
- OpenMP Rgemv should partition rows unless a separate reduction or buffering
  scheme is introduced.  Column-major parallel AXPY would otherwise race on
  `y[i]`.
- The current row-partitioned OpenMP kernels are correctness-clean: every
  variant in the recorded refactored run reports `Result OK`.
