# GMP gmpxx.h Compatibility Notes

Status: Complete

This note records which `gmpxx.h` behaviors `gmpfrxx_mkII` follows and where it intentionally differs.

Primary upstream source:

- `../source/gmp.texi`

Generated references:

- `../generated/gmp.txt`
- `../generated/gmp.html`
- `../generated/gmp.info`

## Compatibility scope

`gmpfrxx_mkII` should be source-familiar to `gmpxx.h` users for `mpz_class`, `mpq_class`, and `mpf_class`, but it is not ABI-compatible and does not include or link `gmpxx.h` / `libgmpxx`.

The public namespace is `gmpfrxx`, not the global namespace.

## Header boundary

`gmpxx_mkII.h` exposes GMP-only types and must not expose MPFR or MPC symbols.

`mpfrxx_mkII.h` exposes `mpfr_class` and `mpc_class` and may depend on GMP + MPFR + MPC.

## `mpf_class` default precision

GMP's C interface has a process-global default precision controlled by `mpf_set_default_prec`. `gmpfrxx_mkII` intentionally replaces this with a library-owned thread-local default precision initialized to **512 bits**.

```cpp
mpf_class x(42);  // numeric value 42 using the project default precision, initially 512 bits
```

Precision-only construction must use a tag or factory:

```cpp
auto x = mpf_class::with_prec(256);
```

Value-plus-precision construction may use an unambiguous two-argument form:

```cpp
mpf_class x(42, 256);  // numeric value 42 with at least 256-bit precision
```

## Assignment precision

Follow `gmpxx.h` here: assignment into an existing `mpf_class` preserves destination precision and stores the value at that precision.

```cpp
auto out = mpf_class::with_prec(128);
mpf_class a(1, 512), b(2, 512);
out = a + b;
assert(out.get_prec() == 128);
```

## New materialization precision

A new `mpf_class` materialized from an expression uses expression-derived precision. For a binary `mpf_class` expression, that is normally the maximum precision of the floating operands.

## Intentionally redesigned areas

- Do not implement legacy pre-C++17 compatibility paths.
- Do not let ET operators capture scalar/scalar arithmetic such as `1 + 2`.
- Reject `bool`, `long double`, and generic `__int128` as expression scalar leaves.
- `__int128` may be supported only by explicit compiler-gated constructors/comparisons.
- Math functions may materialize results immediately instead of preserving all lazy `gmpxx.h` math-function expression behavior.
