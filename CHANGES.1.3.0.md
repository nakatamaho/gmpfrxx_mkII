# Changes for 1.3.0

## 1.3.0 - 2026-08-07

MPLAPACK integration maintenance release. The public source archive is
distributed as `gmpfrxx_mkII.1.3.0.tar.xz`; the runtime version APIs report
`1.3.0`.

### Compatibility

- Guarded binary128 adapter tests with
  `GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128`.
- Avoided instantiating `_Float128` or `__float128` MPFR conversion paths when
  the compiler exposes binary128 native types but the linked MPFR build does
  not provide `mpfr_set_float128` / `mpfr_get_float128`.
- Kept binary80 adapter coverage unchanged.

### Public API

- No new public wrapper API was added.
- Existing binary128 adapter types remain available only for configurations
  that can route native binary128 values through MPFR float128 support.

### Release Verification

- Linux standalone CMake Release build passed with
  `GMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC`.
- Full CTest passed 189/189 tests.
- A focused build with `-U__SIZEOF_FLOAT128__` passed the binary real and
  complex adapter tests, confirming that binary128 native tests are skipped
  when MPFR float128 support is not exposed.
