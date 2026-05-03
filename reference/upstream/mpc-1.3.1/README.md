# MPC 1.3.1 Reference Notes

This directory contains upstream reference material and implementation notes for GNU MPC 1.3.1.

## Purpose

This directory is the implementation reference for `gmpfrxx::mpc_class`.

`gmpfrxx_mkII` uses MPC for:

```text
mpc_class
```

`gmpfrxx_mkII` must not use MPC for:

```text
mpfc_class
```

In particular:

- `mpc_class` wraps `mpc_t`.
- `mpc_class` belongs to `mpfrxx_mkII.h` and `gmpfrxx_mkII.h`.
- `mpfrxx_mkII` and `gmpfrxx_mkII` may link `libmpc`.
- `mpfc_class` is a GMP-only complex floating-point class.
- `mpfc_class` must not use `mpc_t`.
- `gmpxx_mkII.h` must not include `<mpc.h>`.
- `gmpxx_mkII` must not link `libmpc`.

## Directory Policy

```text
source/     upstream original material; do not edit manually
generated/  mechanically generated extracts; do not edit manually
curated/    human-curated implementation notes; prefer these for design decisions
```

## Important Rule

Use MPC documentation directly for `mpc_class` design.

Use MPC documentation only for comparison and terminology when working on `mpfc_class`.

Do not copy the MPC dependency into `mpfc_class`.
