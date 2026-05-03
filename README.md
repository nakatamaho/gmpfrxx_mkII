# gmpfrxx_mkII

`gmpfrxx_mkII` is a C++17 header-only C++ wrapper project for GMP,
MPFR, and GNU MPC.

The public namespace is:

```cpp
namespace gmpfrxx
```

The project intentionally keeps a source-level feel close to GMP's `gmpxx.h`
for `mpz_class`, `mpq_class`, and `mpf_class`, but it is a new wrapper. It is
not ABI-compatible with `gmpxx.h`, does not include `gmpxx.h`, and does not link
`libgmpxx`.

## Current Development Priority

The current priority is **expression-template repair**, not feature expansion.

Earlier implementation attempts produced eager public arithmetic in several
places. That is not an acceptable final state for this project. Before adding
new math functions, formatting, API polish, or compatibility extras, implement
Prompt 0 from `PROMPTS.md` and make public arithmetic expression-template based.

Use Codex from the repository root with:

```text
Implement Prompt 0 from ./PROMPTS.md exactly. Do not implement later prompts.
```

If any ET blocker remains, continue with:

```text
Continue Prompt 0 from ./PROMPTS.md. Fix the remaining ET blockers. Do not add new features.
```

Do not mark ET as done unless the public arithmetic operators return expression
nodes and evaluation happens only through wrapper construction, wrapper
assignment, or compound assignment.

## Public Types

| Type | Backend | Meaning |
|---|---|---|
| `gmpfrxx::mpz_class` | GMP `mpz_t` | Integer |
| `gmpfrxx::mpq_class` | GMP `mpq_t` | Rational, always canonicalized after arithmetic |
| `gmpfrxx::mpf_class` | GMP `mpf_t` | GMP floating-point value |
| `gmpfrxx::mpfc_class` | two `mpf_class` components | GMP-only complex value |
| `gmpfrxx::mpfr_class` | MPFR `mpfr_t` | MPFR real floating-point value |
| `gmpfrxx::mpc_class` | GNU MPC `mpc_t` | MPFR/MPC complex value |

`mpfc_class` and `mpc_class` are separate families. `mpfc_class` must remain
GMP-only and must not use MPC. `mpc_class` must use GNU MPC and must not be
implemented as `std::complex<mpfr_class>`.

## Public Headers

```text
include/gmpxx_mkII.h
include/mpfrxx_mkII.h
include/gmpfrxx_mkII.h
```

| Header | Public types | Required dependencies |
|---|---|---|
| `gmpxx_mkII.h` | `mpz_class`, `mpq_class`, `mpf_class`, `mpfc_class` | GMP only |
| `mpfrxx_mkII.h` | `mpz_class`, `mpq_class`, `mpfr_class`, `mpc_class` | GMP + MPFR + MPC |
| `gmpfrxx_mkII.h` | all public types | GMP + MPFR + MPC |

`gmpxx_mkII.h` must not include `<mpfr.h>` or `<mpc.h>`, must not mention
`mpfr_*` or `mpc_*` symbols, and must not require MPFR or MPC at link time.

## Expression-Template Contract

This project is expression-template first.

For supported wrapper operands, public arithmetic operators must build lazy
expression nodes:

```cpp
a + b
-a
a + b * c - d
z + q + f1 * f2
z + q + r1 * r2
fc1 * fc2 + f
c1 * c2 + r
```

`decltype(a + b)` must be an expression node, not a materialized wrapper:

```cpp
static_assert(gmpfrxx::detail::is_expression_node_v<decltype(a + b)>);
static_assert(!std::is_same<decltype(a + b), gmpfrxx::mpf_class>::value);
```

Arithmetic operators must not call GMP, MPFR, or MPC arithmetic routines merely
to return a public wrapper result. They should only construct expression nodes.

Wrapper objects are created by explicit materialization:

```cpp
gmpfrxx::mpf_class x = a + b;
gmpfrxx::mpfr_class y = r1 * r2 + 1;
gmpfrxx::mpc_class z = c1 * c2 + r;
```

or by assignment into an existing object:

```cpp
out = a + b;
out += b * c;
```

This is intentionally not a materialization test:

```cpp
auto x = a + b;
```

With true expression templates, `x` is an expression node. Use an explicit
wrapper type when a wrapper value is intended.

## Precision and Rounding Policy

Default floating precision is project-owned and starts at **512 bits** for
`mpf_class`, `mpfc_class`, `mpfr_class`, and `mpc_class` components.

Single integral constructor arguments are values, not precision selectors:

```cpp
gmpfrxx::mpfr_class x(53);  // value 53 at the current default precision
```

Precision-only construction must use named factories or explicit precision tags:

```cpp
auto x = gmpfrxx::mpfr_class::with_prec(256);
auto z = gmpfrxx::mpc_class::with_prec2(256, 320);
```

Existing-object assignment preserves destination precision:

```cpp
auto out = gmpfrxx::mpfr_class::with_prec(256);
gmpfrxx::mpfr_class a(1);
gmpfrxx::mpfr_class b(2);
out = a + b;  // out remains 256-bit
```

New materialization uses expression-derived precision:

```cpp
gmpfrxx::mpfr_class out = a + b;
```

Math functions currently materialize eagerly. Ordinary arithmetic must remain
lazy ET.

## Minimal Example

```cpp
#include <gmpxx_mkII.h>

#include <cstdint>
#include <iostream>

int main()
{
    gmpfrxx::mpz_class a = std::uint64_t{18446744073709551615ull};
    gmpfrxx::mpz_class b = 1;

    gmpfrxx::mpz_class z = a + b;  // explicit materialization

    std::cout << z.get_str() << '\n';
}
```

Compile and link with GMP:

```sh
c++ -std=c++17 -Iinclude example.cpp -lgmp
```

## Building and Testing

Standard build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Fast-path build:

```sh
cmake -S . -B build-fast -DCMAKE_BUILD_TYPE=Debug \
  -DGMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH=ON
cmake --build build-fast -j
ctest --test-dir build-fast --output-on-failure
```

CMake interface targets:

```text
gmpxx_mkII
gmpfrxx_mkII
mpfrxx_mkII
```

Dependency intent:

```text
gmpxx_mkII   -> GMP only
mpfrxx_mkII  -> GMP + MPFR + MPC
gmpfrxx_mkII -> GMP + MPFR + MPC
```

## Repository Layout

```text
include/                 public headers and header-only implementation
tests/                   runtime, compile-fail, and header-boundary tests
cmake/                   CMake find modules
docs/                    project-owned design documentation
reference/upstream/      GMP, MPFR, and MPC reference material
AGENTS.md                repository-local development instructions
PROMPTS.md               Codex implementation prompts
STATUS.md                implementation status and test record
README.md                this file
```

Reference directories use this structure:

```text
source/     upstream original material; do not edit manually
generated/  mechanically generated extracts; do not edit manually
curated/    human-curated implementation notes; prefer these for design decisions
```

When implementing behavior that depends on GMP, MPFR, or MPC semantics, check or
create the relevant note under `reference/upstream/*/curated/` before coding.

## Development Rules

- Use C++17 only.
- Keep all code, comments, tests, and project documents in English.
- Do not include `gmpxx.h`.
- Do not link `libgmpxx`.
- Use GMP, MPFR, and MPC C APIs directly.
- Keep GMP-only headers free of MPFR and MPC dependencies.
- Keep `mpfc_class` independent from MPC.
- Keep `mpc_class` backed by GNU MPC.
- Do not allow implicit conversion between `mpfc_class` and `mpc_class`.
- Prefer compile-fail tests for forbidden conversions and header-boundary rules.
- Prefer static type tests for ET behavior; runtime value tests alone are not enough.
- Keep the repository buildable at the end of every prompt.
- Update `STATUS.md` after each completed implementation prompt.
