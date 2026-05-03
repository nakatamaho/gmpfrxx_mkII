# GMP mpz Integer Conversion Notes

Status: Complete

This note records GMP integer conversion facts and the resulting portable C++ integer policy.

Primary upstream source:

- `../source/gmp.texi`

Generated references:

- `../generated/gmp.txt`
- `../generated/gmp.html`
- `../generated/gmp.info`

## Upstream assignment facts

`mpz_set_ui` takes `unsigned long int` and `mpz_set_si` takes `signed long int`. These are not sufficient as the only path for portable `std::uint64_t` and `std::int64_t` on LLP64 platforms.

`mpz_set_d`, `mpz_set_q`, and `mpz_set_f` truncate and must not be used for exact standard-integer construction.

## Import/export facts

`mpz_import` converts unsigned word data into a non-negative integer. It does not read a sign from the input. Apply sign explicitly with `mpz_neg` when needed.

`mpz_export` exports the absolute value; the source sign is ignored and must be handled separately.

## `gmpfrxx_mkII` public integer policy

Public constructors, assignment operators, and comparisons must not blindly cast to `long` or `unsigned long`.

Required standard integer support must be exact on LP64, LLP64, and ILP32 for:

```text
std::int8_t  std::uint8_t
std::int16_t std::uint16_t
std::int32_t std::uint32_t
std::int64_t std::uint64_t
```

Recommended signed conversion:

1. detect sign without evaluating `-value` for signed minimum values;
2. convert magnitude using unsigned arithmetic with defined behavior;
3. use `mpz_import` for the magnitude;
4. apply `mpz_neg` if the original value was negative.

Reject `bool` as a generic integer constructor or expression scalar leaf.

Support for `__int128` may be added only behind compiler-gated explicit constructors/comparisons. It must not silently enter the ordinary ET scalar leaf set.

Use direct APIs for exact conversion to other project types:

```text
mpz_class -> mpq_set_z
mpz_class -> mpf_set_z
mpz_class -> mpfr_set_z
```

Do not convert through decimal strings internally.
