# Reference Layout

This repository separates project-owned documentation from upstream reference material.

## Project Documentation

Project-owned design and development documents live under:

```text
docs/
  design.md
  precision_policy.md
  header_boundary.md
  compatibility.md
  reference_layout.md
  dev/
    codex_prompt_pack.md
```

These files describe the wrapper policy, public API boundaries, precision behavior, compatibility expectations, and development workflow.

## Upstream Reference Root

Upstream reference material lives under:

```text
reference/upstream/
```

The current upstream versions are:

```text
reference/upstream/gmp-6.3.0/
reference/upstream/mpfr-4.2.2/
reference/upstream/mpc-1.3.1/
```

Do not create a second upstream reference tree under `docs/reference/`.

## Per-upstream Layout

Each upstream reference directory may contain:

```text
source/
generated/
curated/
```

Meaning:

| Directory | Meaning | Edit policy |
|---|---|---|
| `source/` | Original upstream source or documentation material | Do not edit manually |
| `generated/` | Mechanically generated extracts | Do not edit manually |
| `curated/` | Human-curated implementation notes | Prefer for design decisions |

## Curated Notes

When implementing behavior that depends on upstream semantics, prefer curated notes before raw source.

For GMP behavior, prefer:

```text
reference/upstream/gmp-6.3.0/curated/
```

For MPFR behavior, prefer:

```text
reference/upstream/mpfr-4.2.2/curated/
```

For MPC behavior, prefer:

```text
reference/upstream/mpc-1.3.1/curated/
```

## MPC Reference Policy

The MPC reference tree is production reference material for `gmpfrxx::mpc_class`.

`mpc_class` is the MPFR/MPC complex type:

- it may include `<mpc.h>`
- it links MPC
- it stores `mpc_t`
- it calls `mpc_*` functions

The same MPC material is forbidden as an implementation dependency for the GMP-only family:

- `gmpxx_mkII.h` must not include `<mpc.h>`
- `mpf_class` must not use MPC
- `mpfc_class` must not use MPC
- the `gmpxx_mkII` target must not link MPC

`mpfc_class` and `mpc_class` are intentionally incompatible unless a future explicit conversion policy is added.
