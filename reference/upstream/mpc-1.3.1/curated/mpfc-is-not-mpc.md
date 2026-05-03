# `mpfc_class` Is Not MPC

Status: Complete

`gmpfrxx::mpfc_class` is a GMP-only complex floating-point type.

Recommended representation:

```cpp
class mpfc_class {
    mpf_class real_;
    mpf_class imag_;
};
```

It is not:

```cpp
class mpfc_class {
    mpc_t value_;
};
```

## Required Dependency Boundary

`mpfc_class` belongs to:

```text
gmpxx_mkII.h
gmpfrxx_mkII.h
```

It must not belong to:

```text
mpfrxx_mkII.h
```

## Required Link Boundary

Code using only:

```cpp
#include <gmpxx_mkII.h>
```

must compile and link with GMP only:

```text
-lgmp
```

It must not require:

```text
-lmpfr
-lmpc
```

## Relationship to `mpc_class`

`mpc_class` exists separately and wraps GNU MPC `mpc_t`.

That does not relax the rule for `mpfc_class`.

`mpfc_class` must not use MPC internally, even if `mpc_class` is implemented elsewhere in the repository.

## Precision Policy

`mpfc_class` precision is derived from its `mpf_class` components.

Rules:

1. Real and imaginary parts should normally have the same precision.
2. Existing-object assignment preserves destination component precision.
3. New materialization uses the maximum precision of all `mpf_class` and `mpfc_class` leaves.
4. Math functions may materialize results immediately.
5. Math functions must not call MPC.
