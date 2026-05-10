<!-- SPDX-License-Identifier: BSD-2-Clause -->

# Benchmarks

The benchmark tree is split by numeric backend.

- `gmp/`: eager BLAS-like GMP/MPF benchmark programs ported to this
  repository.  The top-level CMake build creates raw `mpf_t`, upstream
  `gmpxx.h`, `gmpxx_mkII`, `gmpxx_mkII` with
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`, and OpenMP variants where available.
- `mpfr/`: reserved for MPFR/MPC benchmarks.
- `common/`: shared runner and plotting helpers.

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

Run the full sample dimensions inherited from `go.sh`:

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

Generated result directories such as `benchmarks/gmp/results_raw/` and
`benchmarks/mpfr/results_raw/` are kept available for curated commits.  Check
the staged file list before committing a large benchmark run.

GMP benchmark directories:

- [gmp/00_Rdot](gmp/00_Rdot/README.md): dot product, `sum_i x_i * y_i`.
- [gmp/01_Raxpy](gmp/01_Raxpy/README.md): AXPY, `y_i = y_i + alpha * x_i`.
- [gmp/02_Rgemv](gmp/02_Rgemv/README.md): dense matrix-vector multiply.
- [gmp/03_Rgemm](gmp/03_Rgemm/README.md): dense matrix-matrix multiply.
