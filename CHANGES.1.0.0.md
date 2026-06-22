# Changes for 1.0.0

## 1.0.0 - 2026-06-19

Initial stable API release of `gmpfrxx_mkII`. The public source archive is
distributed as `gmpfrxx_mkII.1.0.0.tar.xz`; the runtime version APIs report
`1.0.0`.

### Public API

- Provides C++17 expression-template wrappers for GMP integer, rational, and
  floating-point types in `gmpxx`.
- Provides MPFR real wrappers in `mpfrxx`.
- Provides MPC complex wrappers through the opt-in `mpcxx_mkII.h` header.
- Keeps `gmpxx_mkII.h` GMP-only, `mpfrxx_mkII.h` GMP+MPFR-only, and
  `gmpfrxx_mkII.h` as the full GMP+MPFR+MPC aggregator.
- Supports component-selectable CMake builds with `GMPFRXX_MKII_COMPONENTS=GMP`,
  `GMP,MPFR`, or `GMP,MPFR,MPC`.
- Provides version APIs through `gmpxx::version()`, `mpfrxx::version()`,
  `gmpxx::git_commit_hash()`, and `mpfrxx::git_commit_hash()`.

### Precision and Environment

- Uses a 512-bit initial default precision policy for GMP MPF, MPFR, and MPC
  wrapper defaults.
- Preserves destination precision for assignment to existing floating-point
  objects.
- Uses libmpfr TLS-backed default state for MPFR precision, rounding, and
  exponent range.
- Keeps GMP MPF default precision wrapper-owned and independent from
  `mpf_set_default_prec()`.

### Correctness and Portability

- Rejects unsupported scalar expression leaves such as `bool`, `long double`,
  and `__int128`.
- Normalizes supported integer scalar leaves through 64-bit storage and exact
  GMP conversion paths.
- Keeps raw stream extraction for `mpq_class` compatible with upstream GMP C++
  behavior while documenting canonicalization requirements for arithmetic use.
- Provides compile-fail tests for header separation and forbidden mixed GMP
  MPF / MPFR / MPC arithmetic.

### Benchmarks and Documentation

- Includes GMP and MPFR benchmark families for Rdot, Raxpy, Rgemv, and Rgemm.
- Documents source-level variant shapes, C native equivalent kernels,
  representative hotpath disassembly, and benchmark lessons learned.
- Provides `PRECISION.md` for the fixed-precision fastpath model and
  disassembly signatures.
