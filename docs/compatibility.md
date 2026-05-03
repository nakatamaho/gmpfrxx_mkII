# Compatibility

`gmpfrxx_mkII` is intentionally familiar to users of GMP's `gmpxx.h`, but it is not ABI-compatible with `gmpxx.h`.

Users must recompile.

## Independence from `gmpxx.h`

`gmpfrxx_mkII` must not:

- include `gmpxx.h`
- use `libgmpxx`
- rely on GMP's C++ wrapper internals

It is implemented from scratch on top of the GMP, MPFR, and MPC C APIs.

## API Compatibility Goal

The public API for:

```text
mpz_class
mpq_class
mpf_class
```

should be reasonably familiar to users of GMP's `gmpxx.h`.

However, exact source compatibility is not guaranteed.

The public API also adds MPFR and MPC wrapper types that do not exist in `gmpxx.h`:

```text
mpfr_class
mpc_class
```

and a GMP-only complex extension:

```text
mpfc_class
```

## ABI Compatibility

There is no ABI compatibility with GMP's `gmpxx.h`.

This library defines its own classes:

```cpp
gmpfrxx::mpz_class
gmpfrxx::mpq_class
gmpfrxx::mpf_class
gmpfrxx::mpfc_class
gmpfrxx::mpfr_class
gmpfrxx::mpc_class
```

They are not interchangeable with global `mpz_class`, `mpq_class`, or `mpf_class` from `gmpxx.h`.

## Intentional Differences from `gmpxx.h`

### Namespace

`gmpxx.h` exposes classes in the global namespace.

`gmpfrxx_mkII` uses:

```cpp
namespace gmpfrxx
```

### Default `mpf_class` Precision

`gmpxx.h` follows GMP's process-global `mpf_set_default_prec()`.

`gmpfrxx_mkII` uses a library-owned thread-local default precision, initially 512 bits.

### Expression Templates

`gmpfrxx_mkII` uses expression templates, but storage, lifetime, scalar, and precision policies are explicitly redesigned.

Expression objects are intended for immediate evaluation.

Object leaves are references to existing wrapper objects. Scalar leaves are copied by value into normalized signed or unsigned 64-bit storage. `__int128` may be accepted by explicit `mpz_class` construction, but it is intentionally not an expression scalar leaf.

### Math Functions

Math functions may materialize results.

The library does not need to preserve all lazy math-function expression cases from `gmpxx.h`, such as:

```cpp
sqrt(1 / f)
```

evaluating entirely as an LHS-precision lazy expression.

### Scalar Policy

`gmpfrxx_mkII` intentionally avoids exposing GMP's `long`-based limitations.

Standard integer values such as:

```text
std::int64_t
std::uint64_t
```

and other supported standard integral types must convert exactly on both LP64 and LLP64 platforms.

Expression scalar leaves are normalized to:

```text
std::int64_t
std::uint64_t
```

`bool` is rejected as a numeric scalar.

`long double` is not an expression scalar leaf.

`__int128` may be supported for direct `mpz_class` construction and comparison, but is not an expression scalar leaf.

## Complex Types

### `mpfc_class`

`mpfc_class` is an intentional extension.

It is a GMP-only complex floating-point type built from two `mpf_class` components.

It is not MPC and must not link `libmpc`.

It is exposed by:

```text
gmpxx_mkII.h
gmpfrxx_mkII.h
```

It is not exposed by:

```text
mpfrxx_mkII.h
```

### `mpc_class`

`mpc_class` is an intentional extension.

It is the MPFR/MPC complex floating-point type and wraps GNU MPC `mpc_t`.

It is not `std::complex<mpfr_class>`.

It must link `libmpc`.

It is exposed by:

```text
mpfrxx_mkII.h
gmpfrxx_mkII.h
```

It is not exposed by:

```text
gmpxx_mkII.h
```

## Dependency Boundaries

### GMP-only

Code including only:

```cpp
#include <gmpxx_mkII.h>
```

must require only:

```text
-lgmp
```

It must not require:

```text
-lmpfr
-lmpc
```

### MPFR/MPC

Code including:

```cpp
#include <mpfrxx_mkII.h>
```

or:

```cpp
#include <gmpfrxx_mkII.h>
```

may require:

```text
-lgmp -lmpfr -lmpc
```

## Forbidden Implicit Mixing

The following are intentionally forbidden unless a later explicit conversion policy is added:

```text
mpf + mpfr
mpfr + mpf
mpfc + mpfr
mpfr + mpfc
mpfc + mpc
mpc + mpfc
mpf + mpc
mpc + mpf
```

The point is not that conversions are impossible. The point is that silent conversion would hide a precision and rounding decision.

## MPC Documentation Status

MPC documentation under:

```text
reference/upstream/mpc-1.3.1/
```

is implementation reference for `mpc_class`.

It remains forbidden as an implementation dependency for `mpfc_class` and for `gmpxx_mkII.h`.

`mpc_class` and `mpfc_class` are intentionally incompatible. The former is MPC-backed and belongs to the MPFR/MPC family; the latter is GMP-only and belongs to the `mpf_class` family. Mixed operations between them remain forbidden unless a later explicit conversion policy is added.
