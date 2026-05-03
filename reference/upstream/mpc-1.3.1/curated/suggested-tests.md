# Suggested MPC Tests

Status: Complete

These tests should ensure MPC is used for `mpc_class` and does not leak into GMP-only code.

## Header Tests

1. `gmpxx_mkII.h` exposes `mpfc_class`.
2. `gmpxx_mkII.h` does not expose `mpc_class`.
3. `mpfrxx_mkII.h` exposes `mpfr_class`.
4. `mpfrxx_mkII.h` exposes `mpc_class`.
5. `mpfrxx_mkII.h` does not expose `mpf_class`.
6. `mpfrxx_mkII.h` does not expose `mpfc_class`.
7. `gmpfrxx_mkII.h` exposes both `mpfc_class` and `mpc_class`.

## Link Tests

1. A program using `mpfc_class` from `gmpxx_mkII.h` must link with `-lgmp` only.
2. It must not require `-lmpfr`.
3. It must not require `-lmpc`.
4. A program using `mpc_class` from `mpfrxx_mkII.h` must link with `-lgmp -lmpfr -lmpc`.

## Source Scan Tests

The GMP-only header path should contain no production include of:

```cpp
#include <mpfr.h>
#include <mpc.h>
```

The `mpfc_class` implementation should contain no production use of:

```text
mpc_t
mpc_init
mpc_clear
mpc_add
mpc_mul
mpc_exp
mpc_log
mpc_pow
```

The `mpc_class` implementation should use MPC directly and should not implement core complex arithmetic through decimal string round-trips.

Reference documentation under `reference/upstream/mpc-1.3.1/` is exempt from source scans.

## Functional Tests for `mpc_class`

1. `mpc_class()` gives equal 512-bit components under the initial project default.
2. `mpc_class::with_prec(100)` gives equal 100-bit components.
3. `mpc_class::with_prec2(100, 50)` gives 100-bit real and 50-bit imaginary components.
4. `mpc_class(100)` constructs the numeric value `100 + 0i` at the current project default component precision; it is not a precision-only constructor.
5. Existing-object assignment preserves destination component precisions.
6. New materialization uses expression-derived component precision pair.
7. `real()` and `imag()` return `mpfr_class` values with expected precision and value.
8. Arithmetic captures MPC return status internally.
9. `MPC_RNDNN` is the initial default rounding mode.
10. Scoped MPC rounding mode restores the old mode after leaving scope.
