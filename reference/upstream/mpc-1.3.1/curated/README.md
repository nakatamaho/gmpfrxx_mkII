# curated/

This directory contains human-curated implementation notes derived from GNU MPC 1.3.1 documentation.

MPC is the required backend for `gmpfrxx::mpc_class`.

Dependency boundary:

- `mpfrxx_mkII.h` and `gmpfrxx_mkII.h` may include `<mpc.h>`.
- `mpfrxx_mkII` and `gmpfrxx_mkII` link MPC.
- `mpc_class` may store `mpc_t` and call `mpc_*` functions.
- `mpfc_class` must not use MPC.
- `gmpxx_mkII.h` must not include `<mpc.h>` or expose `mpc_class`.

Allowed reference use:

- Terminology checks.
- Comparison with complex arithmetic conventions.
- Complex arithmetic and rounding policy design.

Primary upstream source:

- `../source/mpc.texi`

Generated references:

- `../generated/mpc.txt`
- `../generated/mpc.html`
- `../generated/mpc.info`
