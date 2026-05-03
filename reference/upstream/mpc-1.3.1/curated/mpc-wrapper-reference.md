# MPC wrapper reference for gmpfrxx_mkII

Status: Complete

This file is a project-specific, curated reference extracted from the MPC manual.
It is not a replacement for the upstream manual.

Primary upstream source:

- `../source/mpc.texi`

Generated references:

- `../generated/mpc.txt`
- `../generated/mpc.html`
- `../generated/mpc.info`

## Scope

MPC provides complex arithmetic built on MPFR real components.

`gmpfrxx_mkII` uses MPC for:

```cpp
gmpfrxx::mpc_class
```

`gmpfrxx_mkII` does not use MPC for:

```cpp
gmpfrxx::mpfc_class
```

## Object lifetime

- `mpc_class` must own exactly one initialized `mpc_t`.
- Every initialized `mpc_t` must be cleared exactly once.
- Copy construction and copy assignment must create independent MPC objects.
- Move construction and move assignment must not cause double-clear.
- Moved-from objects must remain destructible.

## Initialization

Use:

```text
mpc_init2(z, prec)
```

when real and imaginary parts have the same precision.

Use:

```text
mpc_init3(z, real_prec, imag_prec)
```

when real and imaginary parts may have different precisions.

Do not leave an `mpc_t` uninitialized.

Do not initialize an already initialized `mpc_t` without clearing it first.

## Precision

- MPC values have precision for real and imaginary components.
- Default construction must use the project MPFR default precision for both components, initially 512 bits.
- Construction with explicit precision must define both component precisions deliberately.
- Assignment from expressions should preserve the destination object's component precisions.
- Temporary precision must be chosen explicitly and consistently.
- Use `mpc_get_prec2` for general component precision queries.
- Use `mpc_get_prec` only when equal component precision is required or when zero-for-unequal is intentionally handled.
- Do not use `mpc_set_prec` for ordinary assignment because it discards the old value by setting components to NaN.

## Rounding

- MPC operations use `mpc_rnd_t`.
- A complex rounding mode is a pair of real rounding modes.
- The first rounding component controls the real part.
- The second rounding component controls the imaginary part.
- The wrapper's initial default rounding mode should be `MPC_RNDNN`.
- Wrapper APIs should make the chosen default rounding policy explicit.

## Return values

Most MPC functions return an `int` describing real and imaginary inexactness.

The wrapper must receive the return value internally.

Use:

```text
MPC_INEX_RE(value)
MPC_INEX_IM(value)
```

when decomposing inexact status.

The public API may expose status later, but status must not be accidentally discarded inside the implementation.

## Interoperation with MPFR

- Real-valued `mpfr_class` values can be promoted to `mpc_class` using MPC/MPFR APIs.
- The imaginary part of a real input is exact positive zero unless a different policy is explicitly documented.
- Real and imaginary parts should be converted through MPC/MPFR APIs directly.
- Do not use decimal string round-trips for internal conversion.
- Avoid implicit conversions that lose precision unexpectedly.

## Component access

MPC provides:

```text
mpc_real
mpc_imag
mpc_realref
mpc_imagref
```

The first public API should prefer safe value-returning accessors:

```cpp
mpfr_class real() const;
mpfr_class imag() const;
```

Do not expose mutable references to internal `mpfr_t` components without a separate design review.

## Expression-template participation

- `mpc_class` expression-template integration must track a pair of component precisions.
- Do not force complex expressions into a real-only `MpfrExpr` interface.
- Real-valued leaves promoted into complex expressions should contribute precision to both component precision calculations unless a more precise analysis is implemented.
- Scalar overloads must remain unambiguous and must not accept `bool`.

## Review checklist

Before changing MPC-related wrapper code, check:

- no uninitialized `mpc_t` is passed to MPC
- no initialized `mpc_t` is leaked
- no initialized `mpc_t` is cleared twice
- moved-from objects are still valid for destruction
- component precision behavior is explicit
- rounding behavior is explicit
- MPC return values are captured internally
- complex expression templates are not mixed accidentally with real expressions
- tests cover construction, destruction, copy, move, arithmetic, precision, rounding, and scalar interoperation
