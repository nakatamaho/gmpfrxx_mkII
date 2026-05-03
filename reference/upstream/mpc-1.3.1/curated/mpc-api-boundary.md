# MPC API Boundary

Status: Complete

This file records where GNU MPC API use is allowed and where it is forbidden.

## Allowed

MPC API use is allowed for:

```text
include/gmpfrxx_mkII/detail/mpc_impl.hpp
include/gmpfrxx_mkII/detail/math_mpc.hpp
tests that exercise mpc_class
mpfrxx_mkII.h
gmpfrxx_mkII.h
```

Allowed MPC headers and symbols:

```cpp
#include <mpc.h>
```

```text
mpc_t
mpc_init2
mpc_init3
mpc_clear
mpc_set
mpc_set_z
mpc_set_q
mpc_set_fr
mpc_set_fr_fr
mpc_get_prec
mpc_get_prec2
mpc_real
mpc_imag
mpc_realref
mpc_imagref
mpc_add
mpc_sub
mpc_mul
mpc_div
mpc_neg
mpc_sqr
MPC_RNDNN
MPC_INEX_RE
MPC_INEX_IM
```

Use exact function availability from the targeted MPC version; do not invent API names.

## Forbidden

MPC API use is forbidden for:

```text
gmpxx_mkII.h
include/gmpfrxx_mkII/detail/mpf_impl.hpp
include/gmpfrxx_mkII/detail/mpfc_impl.hpp
include/gmpfrxx_mkII/detail/math_mpf.hpp
include/gmpfrxx_mkII/detail/math_mpfc.hpp
```

`mpfc_class` must not call MPC functions, even if `mpc_class` exists in the same library.

Forbidden shortcut:

```cpp
// Wrong for mpfc_class
mpc_t tmp;
mpc_sin(tmp, ...);
```

## Source Scan Tests

Production source scans should verify:

1. No `<mpc.h>` inclusion appears in the GMP-only header path.
2. No `mpc_*` symbol appears in the `mpfc_class` implementation.
3. `gmpxx_mkII` link tests do not require `-lmpc`.
4. `mpfrxx_mkII` link tests do require `-lmpc` once `mpc_class` exists.

Reference documentation under `reference/upstream/mpc-1.3.1/` is exempt from source scans.
