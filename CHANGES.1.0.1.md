# Changes for 1.0.1

## 1.0.1 - 2026-06-22

Maintenance release after `v1.0.0`. The public source archive is distributed as
`gmpfrxx_mkII.1.0.1.tar.xz`; the runtime version APIs report `1.0.1`.

### Public API

- Added `gmpxx::ldexp` for GMP `mpf_class` values and expression operands.
- Added `mpfrxx::ldexp` for MPFR `mpfr_class` values and expression operands.
- Added `gmpxx::frexp`, `gmpxx::ilogb`, `gmpxx::logb`, and `gmpxx::modf` for GMP `mpf_class` values and expression operands.
- Added `mpfrxx::frexp`, `mpfrxx::ilogb`, `mpfrxx::logb`, and `mpfrxx::modf` for MPFR `mpfr_class` values and expression operands.
- Preserved source precision for returned decomposition values and destination precision for `modf` integer-part outputs.

### Tests and Build Coverage

- Extended the GMP and MPFR power-of-two fusion tests for `ldexp`.
- Added `test_mpf_decomposition_functions` and `test_mpfr_decomposition_functions`.
- Registered the new tests in full builds and in component-selectable `GMP` and `GMP,MPFR` builds.

### Release Verification

- Linux Release build passed with 185/185 CTest tests.
- MinGW/Wine build passed; the full CTest run passed 181/183 runnable tests with two allocation-counter tests disabled, and the two provider-DLL-path-sensitive tests passed when rerun with the provider DLL and MinGW runtime directories on `WINEPATH`.

### Notes

- `ilogb` and `logb` throw `std::domain_error` for zero and non-regular MPFR inputs instead of returning C floating-point sentinel values.
- No decomposition overloads were added for `mpz_class`, `mpq_class`, `mpc_class`, or `mpfc_class`.
