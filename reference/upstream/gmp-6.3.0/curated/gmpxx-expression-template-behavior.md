# GMP gmpxx.h Expression Template Behavior Notes

Status: Complete

This note records upstream `gmpxx.h` expression-template behavior that matters for `gmpfrxx_mkII`.

Primary upstream source:

- `../source/gmp.texi`

Generated references:

- `../generated/gmp.txt`
- `../generated/gmp.html`
- `../generated/gmp.info`

## Upstream facts

GMP's C++ interface uses expression templates so C++ syntax can map to GMP C functions while avoiding avoidable temporaries.

For `mpf_class`, upstream documentation distinguishes assignment into an existing destination from construction of a new object.

### Existing destination

When an expression needs temporary intermediate `mpf_class` values for assignment into an existing destination, those temporaries have the same precision as the destination.

`operator=` stores a value but does not copy or change the destination precision.

### New object construction

A new `mpf_class` constructed from an `mpf_class` or expression gets precision from the source value or expression. For binary expressions, the expression precision is the higher precision of the two operands.

### Builtin and exact operands

Builtin values, `mpz_class`, and `mpq_class` used to construct an `mpf_class` without explicit precision use the default `mpf` precision in upstream `gmpxx.h`.

`gmpfrxx_mkII` follows the source-level idea but changes the source of the default precision: the project default is thread-local and initially 512 bits.

## `gmpfrxx_mkII` conclusions

Follow:

1. Assignment into an existing `mpf_class` preserves destination precision.
2. Temporaries required by assignment use destination precision.
3. New `mpf_class` materialization from an expression uses expression-derived precision.
4. Binary `mpf_class` expression precision is the maximum participating `mpf_class` leaf precision.
5. A single integral constructor argument is a numeric value, not precision-only construction.

Intentionally differ:

1. Do not include `gmpxx.h`.
2. Do not link `libgmpxx`.
3. Use namespace `gmpfrxx`.
4. Use a thread-local `mpf_class` default precision initialized to 512 bits instead of GMP's process-global default.
5. Reject ambiguous scalar leaves such as `bool`, `long double`, and generic `__int128` in expression templates.
6. Permit immediate materialization of math-function results.

## Lifetime policy

- RAII wrapper leaves may be stored by `const&` when expressions are immediately evaluated.
- Intermediate expression nodes should be stored by value.
- Scalar leaves should be normalized into safe value types.
- Expression objects are not intended to be durable views.

## Fused operations

For integer expressions, use `mpz_addmul` and `mpz_submul` only when the expression shape is safe and profitable.
