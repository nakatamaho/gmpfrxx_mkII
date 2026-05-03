# curated/

This directory contains human-curated implementation notes derived from GNU GMP 6.3.0 documentation.

These notes are implementation references for the GMP-side types:

```cpp
gmpfrxx::mpz_class
gmpfrxx::mpq_class
gmpfrxx::mpf_class
gmpfrxx::mpfc_class
```

Dependency boundary:

- GMP-side types may include `<gmp.h>`.
- `gmpxx_mkII.h` must remain GMP-only and must not include `<mpfr.h>` or `<mpc.h>`.
- `mpfr_class` is documented under the MPFR curated directory.

Primary upstream source:

- `../source/gmp.texi`

Generated references:

- `../generated/gmp.txt`
- `../generated/gmp.html`
- `../generated/gmp.info`

Project default precision policy:

- `gmpfrxx_mkII` does not follow GMP's process-global `mpf_set_default_prec()` model for `mpf_class`.
- The project owns a thread-local `mpf_class` default precision.
- The initial project default precision is **512 bits**.
- Public default construction of `mpf_class` should use `mpf_init2(x, gmpxx_defaults::get_default_mpf_precision())`.
