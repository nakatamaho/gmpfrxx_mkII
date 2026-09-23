# Changes for 1.5.0

## 1.5.0 - 2026-09-23

libQD3 adapter and MPC compatibility feature release. The public source
archive is distributed as `gmpfrxx_mkII.1.5.0.tar.xz`; the runtime version APIs
report `1.5.0`.

### libQD3 real adapters

- Add MPFR adapters for libQD3 `ds_real`, `ts_real`, `qs_real`, and `td_real`.
- Add the optional `edd_real` adapter when libQD3 was configured with EDD
  support.
- Add `gmpfrxx_mkII/adapters/single_real.hpp` as an umbrella header for the
  single-real adapters.
- Import every component limb into MPFR so extended-precision values are not
  reduced to only their leading component.
- Add GMP-only and MPFR-backed adapter QA, including all-limb expected-value
  comparisons and libQD3 source/generated include-directory handling.

### MPC 1.3/1.4 compatibility

- Use the native `mpc_log2` API when `MPC_VERSION >= MPC_VERSION_NUM(1, 4, 0)`.
- Use an MPC 1.3-compatible `mpc_log` plus `mpc_div_fr` fallback when native
  `mpc_log2` is unavailable.
- Add focused math coverage for both MPC API paths.

### Release verification

- Verify the release with GMP, MPFR, and MPC enabled.
- Verify the libQD3 adapter tests with and without MPFR oracle support.
