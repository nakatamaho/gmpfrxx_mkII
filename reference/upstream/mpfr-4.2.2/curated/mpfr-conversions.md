# MPFR Conversion Notes

Status: Complete

This note records conversion facts and wrapper policy for constructing and assigning `gmpfrxx::mpfr_class` values.

Primary upstream source:

- `../source/mpfr.texi`

Generated references:

- `../generated/mpfr.txt`
- `../generated/mpfr.html`
- `../generated/mpfr.info`

Relevant upstream section:

- Assignment Functions

## Upstream assignment functions

MPFR assignment functions set an already initialized destination and round toward the given rounding mode.

Relevant functions:

```text
mpfr_set
mpfr_set_ui
mpfr_set_si
mpfr_set_uj
mpfr_set_sj
mpfr_set_z
mpfr_set_q
mpfr_set_f
mpfr_set_flt
mpfr_set_d
mpfr_set_ld
mpfr_set_str
```

For GMP-backed exact values:

```text
mpfr_set_z(rop, op, rnd)  // from mpz_t
mpfr_set_q(rop, op, rnd)  // from mpq_t
```

For fixed-width C integers:

```text
mpfr_set_ui(rop, op, rnd)  // unsigned long
mpfr_set_si(rop, op, rnd)  // signed long
mpfr_set_uj(rop, op, rnd)  // uintmax_t
mpfr_set_sj(rop, op, rnd)  // intmax_t
```

All of these functions return the usual MPFR ternary value unless the manual documents a special case.

## Zero conversion

For these functions, input zero is converted to positive zero regardless of rounding mode:

```text
mpfr_set_ui
mpfr_set_si
mpfr_set_uj
mpfr_set_sj
mpfr_set_z
mpfr_set_q
mpfr_set_f
```

If a public API needs to preserve negative zero, it must use a conversion path that carries a signed floating value or explicitly call `mpfr_set_zero` with the intended sign.

## Rational conversion

`mpfr_set_q` converts an `mpq_t` to MPFR with explicit rounding.

The MPFR manual notes that `mpfr_set_q` might fail if the numerator or denominator cannot be represented as an `mpfr_t`.

The wrapper must not hide such a failure as a successful conversion. It should route the ternary/error information into the same internal status path used for other MPFR calls, and use the project's error-handling policy for exceptional conversion failure.

## Floating conversion

`mpfr_set_flt`, `mpfr_set_d`, and `mpfr_set_ld` convert already-rounded machine floating values to MPFR. They do not recover decimal source precision that was lost when the C/C++ literal or variable was created.

For exact decimal text input, use `mpfr_set_str` or a dedicated string parser API rather than converting through `double`.

Do not use machine floating conversions as production fallbacks for high-precision math functions.

`long double` must not be accepted as an expression scalar leaf under the project scalar policy. If a direct `mpfr_class(long double)` value constructor is ever provided, it must be explicit and documented separately from expression-template scalar handling.

## `gmpfrxx_mkII` integral conversion policy

For standard integral inputs, prefer a single canonical path:

```text
standard integer -> mpz_class -> mpfr_set_z
```

Reasons:

1. It avoids a large overload matrix for `long`, `long long`, `intmax_t`, and platform-specific widths.
2. It avoids accidental narrowing before the MPFR conversion.
3. It gives one exact integer conversion path shared with `mpz_class` expression leaves.
4. It keeps rounding behavior centralized in the `mpfr_set_z(..., rnd)` call.
5. It makes rejection of `bool`, `long double`, and `__int128` easier to test consistently.

Direct use of `mpfr_set_si`, `mpfr_set_ui`, `mpfr_set_sj`, or `mpfr_set_uj` is allowed in small, audited fast paths, but those fast paths must preserve the same observable behavior as the canonical `mpz_class` path.

## `gmpfrxx_mkII` GMP conversion policy

Use direct MPFR/GMP conversion APIs:

```text
mpz_class -> mpfr_set_z
mpq_class -> mpfr_set_q
```

Do not convert through decimal strings internally.

Do not implicitly convert `mpf_class` to `mpfr_class` in mixed-family expressions. Although MPFR provides `mpfr_set_f`, `mpf_class` belongs to the GMP-only floating family in this project, and `mpf + mpfr` is forbidden by default.

## Destination precision and rounding

Conversions into an existing `mpfr_class` use the destination object's current precision and the selected explicit rounding mode.

Conversions used while materializing a new `mpfr_class` use the selected materialization precision.

Every conversion call returning `int` must have its ternary return value captured internally.
