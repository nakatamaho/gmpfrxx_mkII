# GMP mpq Canonicalization Notes

Status: Complete

This note records GMP rational canonicalization facts and the resulting `mpq_class` policy.

Primary upstream source:

- `../source/gmp.texi`

Generated references:

- `../generated/gmp.txt`
- `../generated/gmp.html`
- `../generated/gmp.info`

## Upstream canonical form

GMP rational numbers are stored as `mpq_t`.

Canonical form means:

1. numerator and denominator have no common factors;
2. denominator is positive;
3. zero is represented as `0/1`.

GMP rational arithmetic functions assume canonical operands and canonicalize their results.

## `mpq_canonicalize`

Use `mpq_canonicalize(q)` to remove common factors and make the denominator positive.

## Assignment and mutation

Pure assignment functions do not necessarily canonicalize the destination:

```text
mpq_set
mpq_set_z
mpq_set_ui
mpq_set_si
mpq_set_str
```

`mpq_set_ui`, `mpq_set_si`, and `mpq_set_str` can require `mpq_canonicalize` if the resulting fraction is not already canonical.

Direct numerator/denominator access or mutation via `mpq_numref`, `mpq_denref`, `mpq_set_num`, or `mpq_set_den` can bypass canonical form. Canonicalize before arithmetic, comparison, or public observation unless canonicality is proven.

## `gmpfrxx_mkII` policy

`mpq_class` constructors and public assignment operations must leave the object canonical before returning.

Examples:

```cpp
mpq_class q(2, 4);  // 1/2
mpq_class z(0, 7);  // 0/1
```

String construction must check the GMP parse return value and then canonicalize fraction inputs when needed.

Expression-template evaluation must preserve exact rational semantics and must not route rationals through `double`, `long double`, `mpf_class`, or decimal strings internally.
