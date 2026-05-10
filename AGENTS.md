
# AGENTS.md

## Project

This repository implements `gmpfrxx_mkII`, a from-scratch C++17 header-only expression-template wrapper library for GMP, MPFR, and MPC.

Public namespaces:

```cpp
namespace gmpxx;   // GMP-only public API
namespace mpfrxx;  // MPFR/MPC public API
```

Internal implementation namespace:

```cpp
namespace gmpfrxx_mkII::detail;
```

Do not use `namespace gmpfrxx` as a public API namespace.

The implementation is independent and must not include `gmpxx.h` or link `libgmpxx`.

## Header Roles

The header roles are strict.

```text
gmpxx_mkII.h
  GMP-only header.
  Exposes:
    gmpxx::mpz_class
    gmpxx::mpq_class
    gmpxx::mpf_class
    optionally later: gmpxx::mpfc_class
  Depends on:
    GMP only
  Must not include:
    <mpfr.h>
    <mpc.h>
  Must not reference:
    mpfr_*
    mpc_*
    mpc_t

mpfrxx_mkII.h
  MPFR/MPC header.
  Exposes:
    mpfrxx::mpz_class  // alias to gmpxx::mpz_class
    mpfrxx::mpq_class  // alias to gmpxx::mpq_class
    mpfrxx::mpfr_class
    mpfrxx::mpc_class
  Depends on:
    GMP + MPFR + MPC
  Must not expose:
    gmpxx::mpf_class
    gmpxx::mpfc_class
  Must not include:
    gmpxx_mkII.h
    gmpfrxx_mkII/detail/mpf_impl.hpp
    gmpfrxx_mkII/detail/mpfc_impl.hpp
  May include:
    <gmp.h>
    <mpfr.h>
    <mpc.h>
    gmpfrxx_mkII/detail/zq_impl.hpp
    gmpfrxx_mkII/detail/mpfr_impl.hpp
    gmpfrxx_mkII/detail/mpc_impl.hpp

gmpfrxx_mkII.h
  Full combined header / aggregator.
  Exposes:
    gmpxx::mpz_class
    gmpxx::mpq_class
    gmpxx::mpf_class
    optionally later: gmpxx::mpfc_class
    mpfrxx::mpfr_class
    mpfrxx::mpc_class
  Depends on:
    GMP + MPFR + MPC
  May include:
    gmpxx_mkII.h
    mpfrxx_mkII.h
    <mpc.h>
    gmpfrxx_mkII/detail/mpc_impl.hpp
```

## Namespace Rules

`mpfrxx::mpz_class` and `mpfrxx::mpq_class` must be aliases, not separate types.

Correct:

```cpp
namespace gmpxx {
    class mpz_class;
    class mpq_class;
}

namespace mpfrxx {
    using ::gmpxx::mpz_class;
    using ::gmpxx::mpq_class;
    class mpfr_class;
    class mpc_class;
}
```

Wrong:

```cpp
namespace mpfrxx {
    class mpz_class; // wrong if distinct from gmpxx::mpz_class
    class mpq_class; // wrong if distinct from gmpxx::mpq_class
}
```

## Implementation Rules

1. Use C++17.
2. Do not require C++20 concepts.
3. Do not include `gmpxx.h`.
4. Do not link `libgmpxx`.
5. Do not bridge to an existing `gmpxx_mkII.h` implementation.
6. Generate `gmpxx_mkII.h`, `mpfrxx_mkII.h`, and `gmpfrxx_mkII.h` from scratch.
7. Use GMP C API from `<gmp.h>`.
8. Use MPFR C API from `<mpfr.h>` only in MPFR/MPC implementation paths.
9. Use MPC C API from `<mpc.h>` only in MPC implementation paths.
10. Do not use MPC for `gmpxx::mpf_class` or `gmpxx::mpfc_class`.
11. Keep code, comments, identifiers, tests, and documentation in English.
12. Update `STATUS.md` after each phase.
13. Run CMake/CTest after each phase.

## Private Headers

Implementation headers must live under:

```text
include/gmpfrxx_mkII/detail/
```

Do not create `include/detail/`.

Recommended private headers:

```text
config.hpp
type_traits.hpp
integer_conversion.hpp
environment.hpp
expr.hpp
eval_context.hpp
zq_impl.hpp
mpfr_impl.hpp
mpc_impl.hpp
mpf_impl.hpp
mpfc_impl.hpp
math_mpfr.hpp
math_mpc.hpp
math_mpf.hpp
math_mpfc.hpp
```

## Expression Template Rule

Public arithmetic operators must return expression nodes, not eager public numeric objects.

Example:

```cpp
auto e = a + b;
static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(e)>);
```

The following must be false for expression-forming operators:

```cpp
std::is_same_v<decltype(e), gmpxx::mpf_class>
std::is_same_v<decltype(e), mpfrxx::mpfr_class>
std::is_same_v<decltype(e), mpfrxx::mpc_class>
```

Materialization is allowed only in:

```text
construction from expression
assignment from expression
explicit evaluation helpers
compound assignment operators
documented eager math functions
stream output of expressions
comparison of expressions
```

Expression node lifetime rule:

Do NOT bind expression templates to `auto` and let them outlive their operands.
Expression nodes may hold raw references to lvalue operands.

```cpp
// BAD: e holds raw references to a and b.
auto e = a + b;
mutate_or_destroy(a);
mpf_class r = e;  // dangling if a was destroyed; changed value if mutated

// GOOD: evaluate immediately.
mpf_class r = a + b;
```

`[[nodiscard]]` on expression node types is only a diagnostic aid for discarded
expressions such as `a + b;`. It is not a lifetime-safety mechanism for
`auto e = a + b;`.

Do not implement eager arithmetic as the primary API:

```cpp
friend mpfrxx::mpfr_class operator+(const mpfrxx::mpfr_class&, const mpfrxx::mpfr_class&);
friend mpfrxx::mpc_class operator+(const mpfrxx::mpc_class&, const mpfrxx::mpc_class&);
friend gmpxx::mpf_class operator+(const gmpxx::mpf_class&, const gmpxx::mpf_class&);
```

Every arithmetic phase must include static-assert tests proving expression-node return types.

## Integer Policy

Do not expose GMP's `long` limitation.

Rules:

1. Do not blindly cast public integral inputs to `long`.
2. `std::uint64_t{UINT64_MAX}` must convert exactly.
3. `std::int64_t{INT64_MIN}` must convert exactly without evaluating `-value`.
4. For `gmpxx::mpz_class`, use `mpz_import` or an equivalent chunk path when needed.
5. For `gmpxx::mpq_class`, convert numerator and denominator through `gmpxx::mpz_class`.
6. For `gmpxx::mpf_class`, convert integral values through `gmpxx::mpz_class` and `mpf_set_z`.
7. For `mpfrxx::mpfr_class`, convert integral values through `gmpxx::mpz_class` and `mpfr_set_z`.
8. For `mpfrxx::mpc_class`, convert real scalar operands through the MPFR conversion path.

Reject as expression scalar leaves:

```text
bool
long double
__int128
```

`__int128` may be supported for direct `gmpxx::mpz_class` construction and comparison, but not as an expression scalar leaf.

## Precision and Environment

### MPFR

`mpfrxx::mpfr_class` uses MPFR precision, exponent range, and rounding.

The initial wrapper-owned MPFR default precision is 512 bits.

Environment variables:

```text
MPFRXX_DEFAULT_PRECISION_BITS
MPFRXX_EMIN
MPFRXX_EMAX
MPFRXX_ROUNDING_MODE
```

Allowed rounding strings:

```text
RNDN RNDZ RNDU RNDD RNDA RNDF
MPFR_RNDN MPFR_RNDZ MPFR_RNDU MPFR_RNDD MPFR_RNDA MPFR_RNDF
```

Existing-object assignment preserves destination precision. New expression materialization uses max leaf precision.

### MPC

`mpfrxx::mpc_class` is MPC-backed and exposed by `mpfrxx_mkII.h` and `gmpfrxx_mkII.h`.

Environment variables:

```text
MPFRXX_MPC_DEFAULT_PRECISION_BITS
MPFRXX_MPC_REAL_PRECISION_BITS
MPFRXX_MPC_IMAG_PRECISION_BITS
MPFRXX_MPC_ROUNDING_MODE
MPFRXX_MPC_REAL_ROUNDING_MODE
MPFRXX_MPC_IMAG_ROUNDING_MODE
```

MPC defaults inherit MPFR defaults unless MPC-specific variables are set.

MPC environment variables must not affect `gmpxx::mpfc_class`.

### MPF

`gmpxx::mpf_class` uses wrapper-owned default precision, not GMP's process-global default precision.

The initial wrapper-owned GMP `mpf_class` default precision is 512 bits.

Do not call `mpf_set_default_prec()` from this library.

Environment variable:

```text
MPFXX_DEFAULT_PREC_BITS
```

Existing-object assignment preserves destination precision. New expression materialization uses max leaf precision.

## Fixed-Precision Fast Path

Canonical option:

```text
GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
```

Use preprocessor checks only in `detail/config.hpp`, then expose a constexpr flag such as:

```cpp
gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath
```

The rest of the implementation must use `if constexpr`.

Do not scatter `#ifdef GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`.

## Type Promotion

Required result types:

```text
gmpxx::mpz + gmpxx::mpz       -> gmpxx::mpz_class
gmpxx::mpz + gmpxx::mpq       -> gmpxx::mpq_class
gmpxx::mpq + gmpxx::mpq       -> gmpxx::mpq_class

mpfrxx::mpz/mpq + mpfrxx::mpfr -> mpfrxx::mpfr_class
mpfrxx::mpfr + mpfrxx::mpfr    -> mpfrxx::mpfr_class

mpfrxx::mpc + mpfrxx::mpc      -> mpfrxx::mpc_class
mpfrxx::mpc + mpfrxx::mpfr     -> mpfrxx::mpc_class
mpfrxx::mpc + mpfrxx::mpz/mpq  -> mpfrxx::mpc_class

gmpxx::mpz/mpq + gmpxx::mpf   -> gmpxx::mpf_class
gmpxx::mpf + gmpxx::mpf       -> gmpxx::mpf_class
```

Forbidden:

```text
gmpxx::mpf + mpfrxx::mpfr
mpfrxx::mpfr + gmpxx::mpf
gmpxx::mpf + mpfrxx::mpc
mpfrxx::mpc + gmpxx::mpf
```

If `gmpxx::mpfc_class` is implemented, it remains separate from `mpfrxx::mpc_class`.

## CMake Rules

Required interface targets:

```text
gmpxx_mkII       -> GMP only
mpfrxx_mkII      -> GMP + MPFR + MPC
gmpfrxx_mkII     -> GMP + MPFR + MPC
```

Provide:

```text
cmake/FindGMP.cmake
cmake/FindMPFR.cmake
cmake/FindMPC.cmake
```

Required commands:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Required Compile-Fail Tests

At minimum:

```text
mpfrxx_mkII.h must not expose gmpxx::mpf_class
mpfrxx_mkII.h must not expose gmpxx::mpfc_class
gmpxx_mkII.h must not expose mpfrxx::mpfr_class
gmpxx_mkII.h must not expose mpfrxx::mpc_class

gmpxx::mpf + mpfrxx::mpfr must not compile
mpfrxx::mpfr + gmpxx::mpf must not compile
gmpxx::mpf + mpfrxx::mpc must not compile
mpfrxx::mpc + gmpxx::mpf must not compile

bool scalar rejected
long double scalar rejected
__int128 scalar leaf rejected
scalar/scalar ET operators not provided
```

## Source Scan Tests

Production code for `gmpxx_mkII.h` must not contain:

```cpp
#include <mpfr.h>
#include <mpc.h>
```

Production code must not contain:

```cpp
#include <gmpxx.h>
```

`mpc_*` symbols are allowed only in MPC-specific implementation files such as:

```text
include/gmpfrxx_mkII/detail/mpc_impl.hpp
include/gmpfrxx_mkII/detail/math_mpc.hpp
```

## Status Discipline

Update `STATUS.md` after each phase with:

```text
phase name
implemented features
missing features
tests added
exact commands run
pass/fail result
known issues
```

Do not claim completion without tests.
