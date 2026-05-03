# MPC Boundary Policy

GNU MPC 1.3.1 is an implementation dependency for `gmpfrxx::mpc_class`.

## Hard Rule

`mpc_class` must use MPC directly.

Allowed for `mpc_class` implementation and MPFR-side public headers:

```cpp
#include <mpc.h>
```

Allowed link dependency for `mpfrxx_mkII` and `gmpfrxx_mkII`:

```text
-lmpc
```

Allowed internal storage for `mpc_class`:

```cpp
mpc_t value_;
```

## GMP-only Boundary

`gmpfrxx_mkII` has a GMP-only public header:

```text
gmpxx_mkII.h
```

This header must remain GMP-only.

If `mpfc_class` used MPC, then GMP-only complex arithmetic would silently become an MPC dependency. That violates the header boundary policy.

## Forbidden GMP-only Use

The GMP-only header path must not:

- include `<mpc.h>`
- expose `mpc_class`
- link `libmpc`
- store `mpc_t`
- call `mpc_*` functions

These restrictions apply to `gmpxx_mkII.h`, `mpf_class`, and `mpfc_class`, not to `mpc_class`.
