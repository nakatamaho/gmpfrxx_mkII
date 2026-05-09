<!-- SPDX-License-Identifier: BSD-2-Clause -->

# MPFR Benchmarks

This directory is reserved for MPFR/MPC benchmark families.

The GMP/MPF benchmark families live under `benchmarks/gmp/`.  Future MPFR/MPC
benchmarks should mirror that layout:

- `00_Rdot/`
- `01_Raxpy/`
- `02_Rgemv/`
- `03_Rgemm/`

Raw MPFR/MPC benchmark results belong under:

```text
benchmarks/mpfr/results_raw/
```

Do not write MPFR/MPC results to the top-level `benchmarks/results_raw/`
directory.

Run the current MPFR Rdot benchmark family from the repository root:

```bash
benchmarks/common/run_mpfr_benchmarks.sh build_bench_release 512 \
    100000000 benchmarks/mpfr/results_raw/Linux_Ryzen_3970X_32-Core 10
```
