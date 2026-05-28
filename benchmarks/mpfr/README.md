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

# MPFR Benchmarks

This directory contains MPFR/MPC benchmark families.

The GMP/MPF benchmark families live under `benchmarks/gmp/`.  MPFR/MPC
benchmarks should mirror that layout:

- `00_Rdot/`
- `01_Raxpy/`
- `02_Rgemv/`
- `03_Rgemm/`

Per-family notes:

- [00_Rdot](00_Rdot/README.md) documents the MPFR dot-product kernel shapes,
  including the split serial/OpenMP `01..06` files, stable-rounding variants,
  and FMA variants.

Raw MPFR/MPC benchmark results belong under:

```text
benchmarks/mpfr/results_raw/
```

Do not write MPFR/MPC results to the top-level `benchmarks/results_raw/`
directory.

Run the current MPFR Rdot/Raxpy/Rgemv/Rgemm benchmark families from the repository root:

```bash
benchmarks/common/run_mpfr_benchmarks.sh build_bench_release 512 \
    100000000 benchmarks/mpfr/results_raw/Linux_Ryzen_3970X_32-Core 10
```

The optional sixth argument overrides the Raxpy vector size; otherwise Raxpy
uses the Rdot vector size.
The optional seventh and eighth arguments override the Rgemv row and column
counts; defaults are `4000 4000`.
The optional ninth through eleventh arguments override the Rgemm `m`, `k`,
and `n` counts; defaults are `500 500 500`.
