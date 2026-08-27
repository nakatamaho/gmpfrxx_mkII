# Changes for 1.4.1

## 1.4.1 - 2026-08-27

GMP-only MPFC division portability maintenance release.  The public source
archive is distributed as `gmpfrxx_mkII.1.4.1.tar.xz`; the runtime version APIs
report `1.4.1`.

### MPFC division branch selection

- Replace the `mpf_get_d_2exp()` plus binary64 conversion used to choose the
  Smith-division branch with an exact comparison of GMP MPF exponent metadata
  and limbs.
- Avoid routing the branch decision through `long` on Windows LLP64, where
  `long` is 32-bit even for a 64-bit target.
- Preserve the allocation-free Smith division path and its public API.

### Regression coverage

- Add the dedicated `test_mpfc_division_smoke` CTest executable.
- Exercise MPFC division with a 3,000,000,000-bit power-of-two exponent so the
  LLP64 exponent-width path is covered directly.
