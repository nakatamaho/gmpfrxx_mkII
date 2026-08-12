# Changes for 1.3.2

## 1.3.2 - 2026-08-12

i386 portability maintenance release. The public source archive is distributed
as `gmpfrxx_mkII.1.3.2.tar.xz`; the runtime version APIs report `1.3.2`.

### Compatibility

- Fixed GCC 12 parsing failures on i386 by avoiding the combination of
  `[[nodiscard]]` and the GNU hidden-visibility attribute on expression node
  class declarations.
- Preserved hidden visibility for the expression-building operators and leaf
  implementation types introduced in `1.3.1`.
- Made scalar allocation-count tests account for 32-bit `long` targets, where
  signed 64-bit scalar evaluation must materialize through `mpz_class`.
- Skipped the MPC division regression case on platforms where `mpfr_exp_t` is
  itself 32-bit and cannot provide headroom beyond the tested exponent edge.
- Disabled the `__int128` compile-fail scalar test when the compiler/target
  does not provide C++ `__int128`.

### Release Verification

- Full CTest passed 153/153 tests in the MPLAPACK `mplapack-tier1-debian-i386`
  Docker image with GCC 12.4.0.
- Full CTest passed 153/153 tests in the MPLAPACK `mplapack-tier1-debian-i386`
  Docker image with GCC 14.2.0.
