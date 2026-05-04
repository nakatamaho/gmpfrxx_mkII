# gmpfrxx_mkII

`gmpfrxx_mkII` is a C++17 header-only wrapper library for GMP, MPFR, and GNU MPC.

This is a from-scratch implementation. It does not include `gmpxx.h`, does not
link `libgmpxx`, and does not bridge to an existing C++ wrapper implementation.
The implementation uses the GMP, MPFR, and MPC C APIs directly.

## Public Namespaces

Public API namespaces:

```cpp
namespace gmpxx;   // GMP-only API
namespace mpfrxx;  // MPFR/MPC API
```

Internal implementation namespace:

```cpp
namespace gmpfrxx_mkII::detail;
```

`namespace gmpfrxx` is not a public API namespace.

## Header Roles

| Header | Public API | Dependencies |
|---|---|---|
| `gmpxx_mkII.h` | `gmpxx::mpz_class`, `gmpxx::mpq_class`, `gmpxx::mpf_class`, `gmpxx::mpfc_class` | GMP only |
| `mpfrxx_mkII.h` | `mpfrxx::mpz_class`, `mpfrxx::mpq_class`, `mpfrxx::mpfr_class`, `mpfrxx::mpc_class` | GMP + MPFR + MPC |
| `gmpfrxx_mkII.h` | Combined aggregator | GMP + MPFR + MPC |

`mpfrxx::mpz_class` and `mpfrxx::mpq_class` are aliases to the GMP-only
`gmpxx` types, not separate wrapper classes.

`gmpxx_mkII.h` is intentionally GMP-only. It must not include `<mpfr.h>` or
`<mpc.h>`, must not reference `mpfr_*` or `mpc_*` symbols, and must not require
MPFR or MPC at link time.

## Public Types

| Type | Backend | Header |
|---|---|---|
| `gmpxx::mpz_class` | GMP `mpz_t` | `gmpxx_mkII.h`, `mpfrxx_mkII.h`, `gmpfrxx_mkII.h` |
| `gmpxx::mpq_class` | GMP `mpq_t` | `gmpxx_mkII.h`, `mpfrxx_mkII.h`, `gmpfrxx_mkII.h` |
| `gmpxx::mpf_class` | GMP `mpf_t` | `gmpxx_mkII.h`, `gmpfrxx_mkII.h` |
| `gmpxx::mpfc_class` | two `gmpxx::mpf_class` components | `gmpxx_mkII.h`, `gmpfrxx_mkII.h` |
| `mpfrxx::mpfr_class` | MPFR `mpfr_t` | `mpfrxx_mkII.h`, `gmpfrxx_mkII.h` |
| `mpfrxx::mpc_class` | GNU MPC `mpc_t` | `mpfrxx_mkII.h`, `gmpfrxx_mkII.h` |

`gmpxx::mpfc_class` is GMP-only and separate from `mpfrxx::mpc_class`.
`mpfrxx::mpc_class` is MPC-backed. Mixed arithmetic between these families is
intentionally rejected.

## Expression Templates

Public arithmetic operators return expression nodes. They do not eagerly return
public numeric wrapper objects.

```cpp
#include <gmpxx_mkII.h>

#include <type_traits>

int main()
{
    auto a = gmpxx::mpf_class::with_precision(128, 1.0);
    auto b = gmpxx::mpf_class::with_precision(128, 2.0);

    auto expr = a + b;
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(expr)>);
    static_assert(!std::is_same_v<decltype(expr), gmpxx::mpf_class>);

    gmpxx::mpf_class value = expr;
}
```

Materialization is allowed through construction, assignment, explicit evaluation
helpers, compound assignment operators, documented eager math functions, stream
output of expressions, and comparison of expressions.

## Precision Policy

Default floating precision is project-owned and starts at 512 bits for
`gmpxx::mpf_class`, `gmpxx::mpfc_class`, `mpfrxx::mpfr_class`, and
`mpfrxx::mpc_class` components.

Single scalar constructor arguments are values, not precision selectors.

```cpp
mpfrxx::mpfr_class x(53);  // value 53 at the current default precision
```

Use named factories for precision-only construction.

```cpp
auto f = gmpxx::mpf_class::with_precision(256);
auto fc = gmpxx::mpfc_class::with_precision(256, 320);
auto r = mpfrxx::mpfr_class::with_precision(256);
auto c = mpfrxx::mpc_class::with_precision(256, 320);
```

Existing-object assignment preserves destination precision. New expression
materialization uses the maximum relevant leaf precision.

## Environment

Supported environment variables:

```text
MPFXX_DEFAULT_PREC_BITS
MPFRXX_DEFAULT_PRECISION_BITS
MPFRXX_EMIN
MPFRXX_EMAX
MPFRXX_ROUNDING_MODE
MPFRXX_MPC_DEFAULT_PRECISION_BITS
MPFRXX_MPC_REAL_PRECISION_BITS
MPFRXX_MPC_IMAG_PRECISION_BITS
MPFRXX_MPC_ROUNDING_MODE
MPFRXX_MPC_REAL_ROUNDING_MODE
MPFRXX_MPC_IMAG_ROUNDING_MODE
```

The fixed-precision fast path is a compile-time option:

```text
GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
```

It is not a runtime environment variable.

## Examples

GMP-only:

```cpp
#include <gmpxx_mkII.h>

#include <iostream>

int main()
{
    gmpxx::mpz_class z("ffffffff", 16);
    gmpxx::mpq_class q("6/8");
    auto f = gmpxx::mpf_class::with_precision(128, 1.5);

    gmpxx::mpf_class value = z + q + f;
    std::cout << value << '\n';
}
```

MPFR/MPC:

```cpp
#include <mpfrxx_mkII.h>

#include <iostream>

int main()
{
    auto r = mpfrxx::mpfr_class::with_precision(128, 1.25);
    auto c = mpfrxx::mpc_class::with_precision(128, 128);
    c = r + mpfrxx::mpz_class(2);

    std::cout << r.to_double() << '\n';
}
```

## Building

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

CMake interface targets:

```text
gmpxx_mkII       -> GMP only
mpfrxx_mkII      -> GMP + MPFR + MPC
gmpfrxx_mkII     -> GMP + MPFR + MPC
```

The build also compiles examples and small benchmark programs.

## Benchmarks

Benchmarks are simple standalone executables under `benchmarks/`. They are
intended for quick local comparisons while developing expression-template
evaluation paths, not for stable published performance numbers.

```sh
cmake --build build -j
./build/benchmarks/bench_gmp_arithmetic
./build/benchmarks/bench_mpfr_mpc_arithmetic
```

## Known Limitations

- `gmpxx::mpfc_class` math APIs are present, but the current MPF transcendental
  helpers are GMP-only double-backed approximations. Full arbitrary-precision
  MPF transcendental algorithms are still future work.
- Stream input is not implemented for all wrapper families.
- The project is compatibility-oriented, not a full `gmpxx.h` clone.

## Repository Layout

```text
include/      public headers and header-only implementation
tests/        runtime, compile-fail, and header-boundary tests
examples/     buildable example programs
benchmarks/   small local benchmark programs
cmake/        CMake find modules
docs/         design notes
STATUS.md     phase-by-phase implementation status
PROMPTS.md    implementation phase prompts
AGENTS.md     repository-local development instructions
```
