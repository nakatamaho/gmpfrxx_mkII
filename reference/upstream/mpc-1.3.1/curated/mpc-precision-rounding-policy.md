# MPC Precision and Rounding Policy

Status: Complete

This note records the precision and rounding policy for `gmpfrxx::mpc_class`.

## Precision model

An MPC value has two MPFR-backed component precisions:

```text
real precision
imaginary precision
```

Use equal component precision when possible, but do not assume equality. Default construction uses the project MPFR default precision for both components; the initial project value is 512 bits.

## Default construction policy

`mpc_class()` initializes both components at `mpfr_defaults::get_default_precision()`.

Initial project default:

```text
512 bits
```

`mpc_class(42)` means numeric value `42 + 0i` at the current project default component precision. It must not mean precision construction.

## Initialization

Use:

```text
mpc_init2(z, prec)
```

for equal component precision.

Use:

```text
mpc_init3(z, real_prec, imag_prec)
```

for separate component precision.

## Precision query

Use:

```text
mpc_get_prec2(&real_prec, &imag_prec, z)
```

for general precision queries.

`mpc_get_prec(z)` is only a convenience for the equal-precision case and returns zero if the real and imaginary precisions differ.

## Assignment

Existing-object assignment preserves destination component precisions.

New expression materialization computes a component precision pair.

## Rounding

MPC operations take `mpc_rnd_t`.

A complex rounding mode is a pair of real rounding modes.

The wrapper's default MPC rounding mode is library-owned and thread-local.

Initial value:

```text
MPC_RNDNN
```

Do not silently derive this from MPFR's current rounding mode unless the public policy is explicitly changed.

## Return values

MPC return values must be captured internally.

Use:

```text
MPC_INEX_RE(value)
MPC_INEX_IM(value)
```

when the implementation needs real and imaginary inexactness separately.
