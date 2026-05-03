# MPC Dependency Policy

Status: Complete

GNU MPC is an implementation dependency for `gmpfrxx::mpc_class`.

It is not an implementation dependency for `gmpfrxx::mpfc_class`.

## Required MPC Use

`mpc_class` must wrap exactly one initialized `mpc_t`:

```cpp
class mpc_class {
    mpc_t value_;
};
```

`mpc_class` belongs to:

```text
mpfrxx_mkII.h
gmpfrxx_mkII.h
```

These headers and targets may include and link:

```cpp
#include <mpc.h>
```

```text
-lmpc
```

## Forbidden MPC Use

`mpfc_class` is GMP-only complex and must not use MPC.

`mpfc_class` belongs to:

```text
gmpxx_mkII.h
gmpfrxx_mkII.h
```

When used through `gmpxx_mkII.h`, it must require only:

```text
-lgmp
```

`gmpxx_mkII.h` must not include:

```cpp
#include <mpfr.h>
#include <mpc.h>
```

and must not reference:

```text
mpfr_*
mpc_*
```

## Design Rule

Do not implement `mpc_class` as `std::complex<mpfr_class>`.

MPC carries explicit complex rounding-mode and inexact-result semantics that would be lost in a plain `std::complex` wrapper.
