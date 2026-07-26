# MPLAPACK comparison adapters

The opt-in headers under `gmpfrxx_mkII/adapters/` provide a narrow boundary for
materializing finite backend results into owning MPFR or MPC wrapper objects.
They do not provide general cross-backend arithmetic.

## Real sources

Include the matching header before explicitly constructing or assigning an
`mpfrxx::mpfr_class`:

| Source family | Header | Native import |
|---|---|---|
| `double` | `mpfrxx_mkII.h` | ordinary `mpfr_set_d` path |
| `dd_real` | `gmpfrxx_mkII/adapters/dd_real.hpp` | direct two-component MPFR sum |
| `qd_real` | `gmpfrxx_mkII/adapters/qd_real.hpp` | direct four-component MPFR sum |
| binary80 wrapper | `gmpfrxx_mkII/adapters/binary80_real.hpp` | `mpfr_set_ld` |
| binary128 wrapper | `gmpfrxx_mkII/adapters/binary128_real.hpp` | `mpfr_set_float128` |
| `gmpxx::mpf_class` | `gmpfrxx_mkII/adapters/mpf_real.hpp` | `mpfr_set_f` |

MPLAPACK raw binary80 and binary128 result types are wrapped explicitly at the
adapter boundary. Raw `long double` and raw `__float128` remain rejected
expression-template scalar leaves.

## Complex sources

The complex adapters independently import both components and materialize an
owning `mpfrxx::mpc_class`:

| Source family | Header |
|---|---|
| `std::complex<double>` | `mpcxx_mkII.h` |
| `dd_complex` | `gmpfrxx_mkII/adapters/dd_complex.hpp` |
| `qd_complex` | `gmpfrxx_mkII/adapters/qd_complex.hpp` |
| binary80 complex | `gmpfrxx_mkII/adapters/binary80_complex.hpp` |
| binary128 complex | `gmpfrxx_mkII/adapters/binary128_complex.hpp` |
| `gmpxx::mpfc_class` | `gmpfrxx_mkII/adapters/mpfc_complex.hpp` |

After materialization, subtraction, norms, tolerances, and pass/fail decisions
belong entirely to MPFR/MPC comparison code.

## Numerical contract

- Finite normalized values ordinarily produced by MPLAPACK backends are in
  scope.
- dd and qd component reconstruction is a practical comparison embedding, not
  an exact-rounding proof for arbitrary noncanonical expansions.
- No extended source silently passes through binary64.
- The ordinary default precision is 512 binary bits for
  `mpfrxx::mpfr_class` and `gmpxx::mpf_class`; the complex wrappers use their
  normal 512-bit component defaults.
- Conversion and assignment do not inspect, compare, negotiate, transfer, or
  preserve precision metadata.

## Unsupported operations

The adapters do not provide reverse MPFR/MPC conversion, MPFR-to-GMP
conversion, mixed-backend arithmetic, both operand-order operators, compound
assignment with adapter operands, cross-family expression-template
composition, round-trip identity, arbitrary-precision correct-rounding
guarantees, edd/td MPLAPACK adapters, or MPLAPACK-specific utility functions.

The package does not include MPLAPACK headers and does not depend on
`mplapackint`, `castREAL_*`, `sign`, `nint`, `iceil`, `cabs1`, `pow2`, or
MPLAPACK print helpers.
