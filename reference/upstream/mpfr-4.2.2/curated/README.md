# curated/

This directory contains human-curated implementation notes derived from GNU MPFR 4.2.2 documentation.

These notes are implementation references for `gmpfrxx::mpfr_class`.

Dependency boundary:

- `mpfr_class` must use MPFR and may include `<mpfr.h>`.
- `mpfrxx_mkII.h` and `gmpfrxx_mkII.h` may expose `mpfr_class` and link MPFR.
- `mpf_class` and `mpfc_class` must not use MPFR.
- `gmpxx_mkII.h` must not include `<mpfr.h>` and must not link MPFR.

Project default precision policy:

- Raw upstream MPFR initializes its default precision to 53 bits.
- `gmpfrxx_mkII` uses a project default precision initialized to **512 bits** for `mpfr_class` default construction.
- Wrapper-owned `mpfr_t` objects should normally be initialized with `mpfr_init2` and the project default precision instead of relying on raw `mpfr_init`.

Primary upstream source:

- `../source/mpfr.texi`

Generated references:

- `../generated/mpfr.txt`
- `../generated/mpfr.html`
- `../generated/mpfr.info`
