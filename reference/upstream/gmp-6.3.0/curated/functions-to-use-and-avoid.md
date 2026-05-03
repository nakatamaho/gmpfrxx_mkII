# GMP Functions to Use and Avoid

Status: Complete

This note records GMP C APIs that `gmpfrxx_mkII` should use, restrict, or avoid.

Primary upstream source:

- `../source/gmp.texi`

Generated references:

- `../generated/gmp.txt`
- `../generated/gmp.html`
- `../generated/gmp.info`

## General rule

Use the GMP C API directly. Do not include `gmpxx.h` and do not link `libgmpxx`.

Allowed in GMP-only code:

```cpp
#include <gmp.h>
```

Forbidden in the GMP-only header path:

```cpp
#include <mpfr.h>
#include <mpc.h>
```

Forbidden symbols in the GMP-only header path:

```text
mpfr_*
mpc_*
```

## Integer APIs

Use these lifetime and assignment APIs:

```text
mpz_init
mpz_init2
mpz_clear
mpz_swap
mpz_set
mpz_set_ui
mpz_set_si
mpz_set_str
```

Use these arithmetic APIs as needed:

```text
mpz_add
mpz_sub
mpz_mul
mpz_neg
mpz_abs
mpz_cmp
mpz_addmul
mpz_submul
```

Use `mpz_import` for portable fixed-width integer conversion when the value may not fit in `long` or `unsigned long`. Apply sign explicitly with `mpz_neg` when needed.

Do not blindly cast C++ integers to `long` before calling `mpz_set_si` or `mpz_set_ui`.

Avoid `mpz_set_d`, `mpz_set_q`, and `mpz_set_f` for exact standard-integer construction because they truncate or go through non-integer semantics.

## Rational APIs

Use:

```text
mpq_init
mpq_clear
mpq_swap
mpq_set
mpq_set_z
mpq_set_ui
mpq_set_si
mpq_set_str
mpq_canonicalize
mpq_add
mpq_sub
mpq_mul
mpq_div
mpq_neg
mpq_cmp
```

Call `mpq_canonicalize` after construction or mutation paths that can leave numerator and denominator non-canonical.

Do not convert rationals through decimal strings internally.

## Floating APIs

Use:

```text
mpf_init2
mpf_clear
mpf_swap
mpf_get_prec
mpf_set
mpf_set_ui
mpf_set_si
mpf_set_z
mpf_set_q
mpf_set_str
mpf_add
mpf_sub
mpf_mul
mpf_div
mpf_neg
mpf_abs
mpf_sqrt
mpf_cmp
```

Use `mpf_init2` for wrapper-owned `mpf_t` objects because the project owns a thread-local default precision, initially 512 bits.

Avoid ordinary use of:

```text
mpf_init
mpf_inits
mpf_set_default_prec
mpf_get_default_prec
```

The public `mpf_class` default precision comes from `gmpxx_defaults`, not GMP's process-global default.

Restrict:

```text
mpf_set_prec
mpf_set_prec_raw
```

`mpf_set_prec` truncates the stored value and is not part of ordinary precision-preserving assignment. `mpf_set_prec_raw` has restoration requirements and should not be used in the initial implementation.

Forbidden nonexistent APIs:

```text
mpf_add_z
mpf_sub_z
mpf_z_sub
mpf_mul_z
mpf_div_z
mpf_add_q
mpf_sub_q
mpf_mul_q
mpf_div_q
```

Use an explicitly initialized temporary plus `mpf_set_z` or `mpf_set_q` instead.

## GMP-only complex type

`mpfc_class` is built from two `mpf_class` components and must not use MPFR or MPC.
