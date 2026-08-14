# Changes for 1.4.0

## 1.4.0 - 2026-08-14

GMP comparison and shared-library compatibility maintenance release. The
public source archive is distributed as `gmpfrxx_mkII.1.4.0.tar.xz`; the
runtime version APIs report `1.4.0`.

### GMP MPF comparisons

- Route comparisons between two materialized `gmpxx::mpf_class` values directly
  through GMP's native `mpf_cmp`.
- Avoid expression-template materialization for this common comparison path,
  including values with exponents far outside the binary64 range.
- Add a regression test covering finite MPF values whose `double` conversion
  overflows.

### Shared-library compatibility

- Keep expression-building operators and internal expression helpers local to
  each translation unit while preserving the public numeric classes and APIs.
- Preserve the public ABI boundary: `gmpxx::mpf_class` itself is not marked
  hidden, and no public wrapper layout or symbol is changed.

### Release verification

- Release CMake build and CTest pass with GMP, MPFR, and MPC enabled.
- The focused comparison regression test passes.
