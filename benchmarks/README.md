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

# Benchmarks

The benchmark tree is split by numeric backend.

- `gmp/`: eager BLAS-like GMP/MPF benchmark programs ported to this
  repository.  The top-level CMake build creates raw `mpf_t`, upstream
  `gmpxx.h`, `gmpxx_mkII`, and OpenMP variants where available.  Rdot omits
  the legacy fixed-precision-fastpath target names.
- `mpfr/`: reserved for MPFR/MPC benchmarks.
- `common/`: shared runner and plotting helpers.

MPFR wrapper benchmark kernels are built in three wrapper configurations:

- `*_mkII`: normal precision-policy build.
- `*_mkII_FIXED_PRECISION_FASTPATH`: currently the same wrapper build as
  `*_mkII`; the fixed-precision fastpath macro is disabled.
- `*_mkII_FIXED_PRECISION_FASTPATH_FMA`: normal wrapper build plus
  `MPFRXX_ENABLE_FMA`, enabling MPFR fused multiply-add/subtract expression
  paths where the wrapper can safely apply them.

Rdot MPFR kernels use the shorter `*_mkII` and `*_mkII_FMA` names instead of
the legacy fixed-precision-fastpath names. They also provide
`*_mkII_STABLE_ROUNDING` and `*_mkII_STABLE_ROUNDING_FMA` variants, which
define `GMPFRXX_MKII_ASSUME_STABLE_MPFR_ROUNDING_MODE` and assume the MPFR
default rounding mode is not changed inside the benchmark kernel.

MPFR Rdot also provides raw C native FMA baselines:

- `Rdot_mpfr_C_native_01_FMA`
- `Rdot_mpfr_C_native_openmp_01_FMA`

MPFR Raxpy follows the same shorter naming policy.  It provides raw C native
FMA baselines and wrapper FMA variants:

- `Raxpy_mpfr_C_native_01_FMA`
- `Raxpy_mpfr_C_native_openmp_01_FMA`
- `Raxpy_mpfr_kernel_*_mkII_FMA`

Raw results are kept under each backend, not under the top-level benchmark
directory:

- `benchmarks/gmp/results_raw/`
- `benchmarks/mpfr/results_raw/`

Build from the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

Benchmark tiers:

| tier | Rdot/Raxpy | Rgemv | Rgemm | repeat | purpose |
|---|---:|---:|---:|---:|---|
| `smoke` | 50-2000 | 8x7 to 40x40 | 4x3x5 to 16x16x16 | 1 | build and runner validation |
| `scaled-full` | 10000000 | 1000x1000 | 200x200x200 | 10 | comparable development benchmark |
| `full` | 100000000 | 4000x4000 | 500x500x500 | 10 | publication-style benchmark |

Expected wall time on the Ryzen Threadripper 3970X benchmark host:

| tier | backend | expected wall time | notes |
|---|---|---:|---|
| `smoke` | GMP or MPFR | under 5 minutes | validates binaries, logs, and plots |
| `scaled-full` | GMP | about 38 minutes | measured as 2258.85 seconds with `Rdot/Raxpy=10000000`, `Rgemv=1000x1000`, `Rgemm=200x200x200`, repeat 10 |
| `scaled-full` | MPFR | about 46 minutes | measured as 2736.24 seconds with the same dimensions as GMP, repeat 10 |
| `scaled-full` | GMP + MPFR | about 1 hour 23 minutes | measured as 4995.09 seconds when the two backend suites are run sequentially |
| `full` | GMP or MPFR | long-running | intended for publication-style runs, not routine validation |

Run the historical full sample dimensions through the shared runner:

```bash
benchmarks/common/run_benchmarks.sh build_bench_release 512 \
    100000000 100000000 4000 4000 500 500 500 \
    benchmarks/gmp/results_raw/Linux_Ryzen_3970X_32-Core 10
```

Run the MPFR Rdot/Raxpy/Rgemv/Rgemm benchmark families with the same log and plot format:

```bash
benchmarks/common/run_mpfr_benchmarks.sh build_bench_release 512 \
    100000000 benchmarks/mpfr/results_raw/Linux_Ryzen_3970X_32-Core 10
```

The common runners set OpenMP affinity defaults for repeatable OpenMP
measurements unless the caller already provides them:

```bash
OMP_NUM_THREADS=32
OMP_PLACES=cores
OMP_PROC_BIND=spread
```

Override them directly when comparing affinity policies:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=close \
    benchmarks/common/run_mpfr_benchmarks.sh build_bench_release 512 \
    100000000 benchmarks/mpfr/results_raw/Linux_Ryzen_3970X_32-Core_close 10
```

For NUMA interleave experiments, pass a command prefix.  The runner records the
prefix in the log and applies it to every timed executable:

```bash
BENCH_COMMAND_PREFIX="numactl --interleave=all" \
    benchmarks/common/run_mpfr_benchmarks.sh build_bench_release 512 \
    100000000 benchmarks/mpfr/results_raw/Linux_Ryzen_3970X_32-Core_interleave 10
```

The MPFR runner uses the Rdot size for Raxpy by default.  A sixth argument can
override the Raxpy vector size.  Seventh and eighth arguments override Rgemv
`m` and `n`.  Ninth through eleventh arguments override Rgemm `m`, `k`, and
`n`.

The final argument is the repeat count.  The default is `10`; the plotter
aggregates repeated `MFLOPS` samples by variant and draws sample standard
deviation error bars.

The plotter writes serial and OpenMP PNG graphs separately:

- `*_serial_summary.png` and `*_serial_<kernel>.png`
- `*_openmp_summary.png` and `*_openmp_<kernel>.png`

The raw log is the authoritative result.  The plotted `MFLOPS` values measure
the timed kernel body, not allocation, random initialization, or verification.
Use `WALL_SECONDS` in the log when total executable time matters.

Measure host memory bandwidth with the STREAM-like OpenMP helper:

```bash
benchmarks/common/run_stream_like.sh 100000000 20 3 \
    benchmarks/results_raw/stream_like/Linux_Ryzen_3970X_32-Core
```

The helper builds `benchmarks/common/stream_like_omp.cpp` with
`-O3 -march=native -fopenmp`, then runs Copy, Scale, Add, and Triad with
`OMP_PROC_BIND=spread` and `OMP_PROC_BIND=close`.  By default it also records a
single SMT comparison with 64 OpenMP threads.  Set `RUN_STREAM_SMT=0` to skip
that extra run.

Rdot benchmark executables install a GMP allocator counter before the first GMP
allocation and print a timed-kernel profile line:

```text
BENCH_ALLOC_COUNTS label=timed_kernel alloc=... realloc=... free=... alloc_bytes=... realloc_old_bytes=... realloc_new_bytes=... free_bytes=...
```

This counter observes actual GMP/MPFR heap traffic through
`mp_set_memory_functions()` and does not instrument wrapper internals.  If
direct init/clear operation counts are needed for diagnosis, configure with
`-DGMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS=ON` or
`-DGMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS=ON`; those opt-in counters use
benchmark-local macro wrapping and are not enabled in normal benchmark builds.

Generated result directories such as `benchmarks/gmp/results_raw/` and
`benchmarks/mpfr/results_raw/` are kept available for curated commits.  Check
the staged file list before committing a large benchmark run.

GMP benchmark directories:

- [gmp/00_Rdot](gmp/00_Rdot/README.md): dot product, `sum_i x_i * y_i`.
- [gmp/01_Raxpy](gmp/01_Raxpy/README.md): AXPY, `y_i = y_i + alpha * x_i`.
- [gmp/02_Rgemv](gmp/02_Rgemv/README.md): dense matrix-vector multiply.
- [gmp/03_Rgemm](gmp/03_Rgemm/README.md): dense matrix-matrix multiply.
