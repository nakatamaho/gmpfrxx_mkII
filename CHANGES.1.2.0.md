# Changes for 1.2.0

## 1.2.0 - 2026-07-28

MPLAPACK integration release. The public source archive is distributed as
`gmpfrxx_mkII.1.2.0.tar.xz`; the runtime version APIs report `1.2.0`.

### Public API

- Added implicit real-scalar construction for GMP and MPFR complex wrappers.
- Added component accessors for MPC and complex expression operands.
- Added typed integer extraction helpers for GMP and MPFR wrapper values.
- Added MPFR default exponent-range accessors.
- Added MPFR-compatible random helpers and shared random-state coverage.

### Adapters

- Added semantic binary80 and binary128 import adapters for platform-specific
  extended floating-point types.
- Added explicit adapter headers for GMP, QD, DD, and complex-like external
  values.
- Kept QD/DD adapter tests optional: they run only when QD headers and libqd
  are available, and QD complex support is not a default dependency.

### Compatibility

- Kept GMP, MPFR, and MPC header boundaries explicit.
- Kept mixed GMP MPF / MPFR / MPC arithmetic ill-formed unless callers use
  explicit conversion paths.
- Made generated version information robust when the source tree is built
  outside a Git checkout.

### Release Verification

- Linux standalone CMake build passed with `GMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC`.
- CTest passed 154/154 tests.
