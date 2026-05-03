# Design

This document records the core design decisions for `gmpfrxx_mkII`.

## Scope

`gmpfrxx_mkII` is a from-scratch C++17 header-only wrapper library for the GMP, MPFR, and MPC C APIs.

It deliberately does not use GMP's `gmpxx.h` or `libgmpxx`.

All public types are source-level APIs only. The project is not ABI-compatible with GMP's `gmpxx.h`, and users must recompile code that uses this library.

The public namespace is:

```cpp
namespace gmpfrxx
```

Public classes must not be introduced in the global namespace, and the project does not provide default global aliases.

## Public Headers

```text
include/
├─ gmpxx_mkII.h       # GMP only: mpz/mpq/mpf/mpfc
├─ mpfrxx_mkII.h      # GMP + MPFR + MPC: mpz/mpq/mpfr/mpc
└─ gmpfrxx_mkII.h     # aggregator: all public types
```

### `gmpxx_mkII.h`

Exposes:

```cpp
gmpfrxx::mpz_class
gmpfrxx::mpq_class
gmpfrxx::mpf_class
gmpfrxx::mpfc_class
```

Dependencies:

```text
GMP only
```

Forbidden:

- include `<mpfr.h>`
- include `<mpc.h>`
- reference `mpfr_*`
- reference `mpc_*`
- link `libmpfr`
- link `libmpc`

### `mpfrxx_mkII.h`

Exposes:

```cpp
gmpfrxx::mpz_class
gmpfrxx::mpq_class
gmpfrxx::mpfr_class
gmpfrxx::mpc_class
```

Dependencies:

```text
GMP + MPFR + MPC
```

Forbidden:

```cpp
gmpfrxx::mpf_class
gmpfrxx::mpfc_class
```

This header is allowed to include `<mpfr.h>` and `<mpc.h>`.

### `gmpfrxx_mkII.h`

Exposes:

```cpp
gmpfrxx::mpz_class
gmpfrxx::mpq_class
gmpfrxx::mpf_class
gmpfrxx::mpfc_class
gmpfrxx::mpfr_class
gmpfrxx::mpc_class
```

Dependencies:

```text
GMP + MPFR + MPC
```

This is an aggregator header.

## Type Mapping

```text
mpz_t   -> gmpfrxx::mpz_class
mpq_t   -> gmpfrxx::mpq_class
mpf_t   -> gmpfrxx::mpf_class
mpfr_t  -> gmpfrxx::mpfr_class
mpc_t   -> gmpfrxx::mpc_class
```

`mpfc_class` is not a C-library type mapping. It is a project-defined GMP-only complex type:

```cpp
class mpfc_class {
    mpf_class real_;
    mpf_class imag_;
};
```

## Real and Complex Families

The library deliberately has two separate floating-point families.

### GMP-only family

```text
mpf_class
mpfc_class
```

This family is available from `gmpxx_mkII.h` and must remain GMP-only.

`mpfc_class` is not MPC. It exists so that a user who only wants GMP and GMP-like floating-point arithmetic can still use a complex type without linking MPFR or MPC.

### MPFR/MPC family

```text
mpfr_class
mpc_class
```

This family is available from `mpfrxx_mkII.h` and `gmpfrxx_mkII.h`.

`mpfr_class` is real-valued. MPFR itself does not define a complex scalar.

`mpc_class` is the complex counterpart and must wrap GNU MPC `mpc_t`:

```cpp
#include <mpc.h>

class mpc_class {
    mpc_t value_;
};
```

Do not implement `mpc_class` as:

```cpp
std::complex<mpfr_class>
```

That representation would lose MPC's rounding-mode and inexact-result semantics.

## `mpfc_class` vs `mpc_class`

These types must not be blurred.

| Type | Backend | Header | Link dependency | Purpose |
|---|---|---|---|---|
| `mpfc_class` | two `mpf_class` components | `gmpxx_mkII.h`, `gmpfrxx_mkII.h` | `-lgmp` | GMP-only complex arithmetic |
| `mpc_class` | `mpc_t` | `mpfrxx_mkII.h`, `gmpfrxx_mkII.h` | `-lgmp -lmpfr -lmpc` | MPFR/MPC complex arithmetic |

Forbidden conversions by default:

```text
mpfc + mpc
mpc + mpfc
mpfc + mpfr
mpfr + mpfc
mpf + mpc
mpc + mpf
```

These can be added later only with an explicit conversion and precision policy.

## `mpc_class` Construction and Component Access

Required constructors and factories should include:

```cpp
mpc_class();
static mpc_class with_prec(mpfr_prec_t precision);
static mpc_class with_prec2(mpfr_prec_t real_precision, mpfr_prec_t imag_precision);
mpc_class(const mpfr_class& real);
mpc_class(const mpfr_class& real, const mpfr_class& imag);
mpc_class(const mpz_class& real);
mpc_class(const mpz_class& real, const mpz_class& imag);
mpc_class(const mpq_class& real);
mpc_class(const mpq_class& real, const mpq_class& imag);
```

Do not provide precision-only constructors of the form `mpc_class(mpfr_prec_t)` or `mpc_class(mpfr_prec_t, mpfr_prec_t)`. Those signatures collide with value construction such as `mpc_class(42)` and `mpc_class(1, 2)`. Precision-only construction must use the named factories or explicit precision tags.

Standard integer constructors must use the portable integer conversion policy and must construct values at the current project default component precisions, initially 512 bits, unless an explicit value-plus-precision overload is used.

Component access should be safe:

```cpp
mpfr_class real() const;
mpfr_class imag() const;
```

Returning mutable references to MPC's internal `mpfr_t` components is dangerous and should not be part of the first public API. If a future low-level API exposes component references, it must document lifetime, precision mutation, aliasing, and invalidation rules.

## Math Functions

Math functions materialize results eagerly.

GMP-only math functions for `mpf_class` and `mpfc_class` live in the GMP-only header path and must not use MPFR, MPC, or double-precision fallback as their production implementation.

MPC-backed math functions for `mpc_class` live in the MPFR/MPC header path and call GNU MPC directly. Every MPC math call must receive an explicit `mpc_rnd_t`, and the ternary inexact return value must be captured internally.

The implementation does not preserve lazy `gmpxx.h`-style math-function expression nodes. For example, a math function such as `sqrt(f)` returns a materialized wrapper object.

## Precision Design

### `mpf_class`

- Existing-object assignment preserves destination precision.
- New expression materialization uses the maximum precision of all `mpf_class` leaves.
- The default constructor uses the library-owned thread-local GMP-style default precision, initially 512 bits.

### `mpfc_class`

- Component precision is derived from the contained `mpf_class` objects.
- Existing-object assignment preserves destination component precisions.
- New expression materialization uses the maximum precision of all `mpf_class` and `mpfc_class` leaves.

### `mpfr_class`

- Existing-object assignment preserves destination precision.
- New expression materialization uses the maximum precision of all `mpfr_class` leaves.
- Default construction uses the project MPFR default precision, initially 512 bits.
- Arithmetic uses an explicit or documented MPFR rounding mode.

### `mpc_class`

- MPC values have separate real and imaginary precisions.
- Default construction uses the project MPFR default precision for both components, initially 512 bits.
- Equal precision values may use `mpc_init2`.
- Unequal component precision values must use `mpc_init3`.
- Existing-object assignment preserves both destination component precisions.
- New expression materialization uses an explicit pair of evaluation precisions.
- Use `mpc_get_prec2` when component precisions may differ.
- Do not rely on `mpc_get_prec` except for the equal-precision convenience case.

## Rounding Design

### MPFR

`mpfr_class` uses MPFR rounding modes, normally defaulting to `MPFR_RNDN` unless changed by wrapper environment helpers.

MPFR ternary return values must be received internally even if the public API does not expose them initially.

### MPC

`mpc_class` uses `mpc_rnd_t`.

A complex rounding mode is a pair of real rounding modes. The project default should be library-owned and thread-local, initially:

```cpp
MPC_RNDNN
```

MPC return values must be received internally. Use:

```cpp
MPC_INEX_RE(value)
MPC_INEX_IM(value)
```

when decomposing real and imaginary inexactness.

## Expression Templates

Expression-template nodes are intended for immediate evaluation.

Object leaves may initially be stored by const reference.

Scalar leaves must be stored by value and normalized to ABI-stable storage such as:

```cpp
std::int64_t
std::uint64_t
```

Do not accept:

```text
bool
long double
__int128
```

as expression scalar leaves.

The lifetime policy is intentionally conservative: expression objects are temporary views for immediate evaluation, not long-lived containers. Object leaves are references to existing wrapper objects, while scalar leaves are copied into normalized storage. Users should not store expression objects for later use.

## Type Promotion

```text
mpz + mpz       -> mpz
mpz + mpq       -> mpq
mpq + mpz       -> mpq
mpq + mpq       -> mpq

mpz/mpq + mpf   -> mpf
mpf + mpz/mpq   -> mpf
mpf + mpf       -> mpf

mpz/mpq + mpfr  -> mpfr
mpfr + mpz/mpq  -> mpfr
mpfr + mpfr     -> mpfr

mpfc + mpfc     -> mpfc
mpfc + mpf      -> mpfc
mpf + mpfc      -> mpfc
mpfc + mpz/mpq  -> mpfc
mpz/mpq + mpfc  -> mpfc

mpc + mpc       -> mpc
mpc + mpfr      -> mpc
mpfr + mpc      -> mpc
mpc + mpz/mpq   -> mpc
mpz/mpq + mpc   -> mpc
```

Forbidden:

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

## Dependency Graph

```text
gmpxx_mkII
  -> GMP

mpfrxx_mkII
  -> GMP
  -> MPFR
  -> MPC

gmpfrxx_mkII
  -> GMP
  -> MPFR
  -> MPC
```

## Reference Layout

```text
reference/upstream/
├─ gmp-6.3.0/
├─ mpfr-4.2.2/
└─ mpc-1.3.1/
```

For MPC-related implementation decisions, use:

```text
reference/upstream/mpc-1.3.1/curated/
```

The MPC reference is required implementation material for `mpc_class`. MPC remains forbidden for `mpfc_class` and for `gmpxx_mkII.h`.
