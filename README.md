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

`mpfrxx::mpc_class` supports equality and inequality only.  Complex ordering
operators such as `<`, `<=`, `>`, and `>=` are intentionally not defined.  MPC
values with a NaN component compare unequal to themselves, matching the usual
IEEE-style NaN equality rule.

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

Expression nodes borrow lvalue wrapper operands and own rvalue wrapper
operands.  This keeps inner-loop expressions such as `dst = a + b` and
`acc += x * y` allocation-light for existing objects.  A saved expression that
contains lvalue operands must not outlive those operands, and evaluating it
after those operands have been modified observes their current values.  Materialize
to a wrapper object if a stable snapshot is needed.

### Expression Template Pitfall

Do not bind expression templates to `auto` and let them outlive their operands.
Expression nodes may hold raw references to lvalue operands.

```cpp
// BAD: e holds raw references to a and b.
auto e = a + b;
mutate_or_destroy(a);
gmpxx::mpf_class r = e;  // dangling if a was destroyed; changed value if mutated

// GOOD: evaluate immediately.
gmpxx::mpf_class r = a + b;
```

The expression-node types are marked `[[nodiscard]]`, so a standalone discarded
expression such as `a + b;` can trigger a compiler diagnostic. This does not
make storing expression nodes safe; it only catches accidental ignored
expressions.

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

### GMP MPF Default Context

`gmpxx::mpf_class` has two GMP-only default-context modes.

Frozen-env mode is the default.  It is header-only and reads the default MPF
precision from the environment on first use in each linked image.  The
environment is treated as immutable after process startup / first use; changing
it at runtime is unsupported.  This mode does not follow GMP's process-global
`mpf_set_default_prec()` state, and default construction uses explicit
`mpf_init2()` with the wrapper precision.

External-provider mode is opt-in with:

```cpp
GMPXX_MKII_DEFAULT_CONTEXT_MODE=GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER
```

It requires exactly one linked provider implementation, such as the
`gmpxx_mkII_default_context_provider` shared library target.  The provider owns
mutable storage and may use `thread_local` internally.  A missing provider is a
link error.  Do not embed the provider object file into multiple shared
libraries; the provider must be a single dedicated shared library, or one
designated owner such as MPLAPACK.

Changing the default precision affects only subsequently constructed objects.
Assignment to an existing `mpf_class` preserves the left-hand-side precision.

### MPFR Default Context

`mpfrxx::mpfr_class` routes default precision, rounding mode, and exponent
range through libmpfr's default state.  The wrapper does not define
header-owned `thread_local` MPFR default storage.

The wrapper initial default precision is 512 bits.  On wrapper default access,
gmpfrxx_mkII initializes libmpfr's default precision from
`MPFRXX_DEFAULT_PRECISION_BITS` when it is valid, otherwise from the 512-bit
wrapper default, only if libmpfr's current thread-local default state still has
MPFR's library-initial values.  Runtime calls such as
`mpfrxx::set_default_precision_bits()` update libmpfr's thread-local default
state and affect only subsequently constructed MPFR objects in that thread.

This policy requires an MPFR build with TLS support.  CMake checks
`mpfr_buildopt_tls_p()` and fails configuration if libmpfr does not report TLS.
Shared-library users must ensure all images use the same libmpfr shared
library; wrapper-owned DSO-local MPFR default storage and DSO-local
initialization flags are intentionally avoided.

### MPC Default Context

`mpfrxx::mpc_class` default precision and rounding are derived from the same
libmpfr default state used by `mpfrxx::mpfr_class`.  The wrapper does not keep
separate mutable default state for MPC real and imaginary components.

Default MPC real and imaginary precision therefore must match, and default MPC
real and imaginary rounding modes must match.  Calls such as
`mpfrxx::set_default_mpc_precision_bits(real, imag)` and
`mpfrxx::set_default_mpc_rounding_mode(real, imag)` throw
`std::invalid_argument` when the two component values differ.  Use explicit
`mpfrxx::mpc_class::with_precision(real, imag)` construction when an individual
object needs different real and imaginary precision.

## Environment

Supported environment variables:

```text
GMPXX_MKII_DEFAULT_MPF_PREC_BITS
GMPFRXX_MKII_DEFAULT_MPF_PREC_BITS
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

For GMP MPF defaults, `GMPXX_MKII_DEFAULT_MPF_PREC_BITS` has priority over
`GMPFRXX_MKII_DEFAULT_MPF_PREC_BITS`, which has priority over the legacy
`MPFXX_DEFAULT_PREC_BITS` name.  Invalid GMP MPF precision environment values
fail fast.

The fixed-precision fast path is a compile-time option:

```text
GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
```

It is not a runtime environment variable.

Compound-assignment multiply fusion is controlled by separate compile-time
options because it can change rounding semantics:

```text
GMPXX_ENABLE_FMA
MPFRXX_ENABLE_FMA
```

`GMPXX_ENABLE_FMA` enables the GMP MPF `a += b * c` and `a -= b * c` scratch
paths. `MPFRXX_ENABLE_FMA` enables MPFR `mpfr_fma`, `mpfr_fms`,
`mpfr_fmma`, and `mpfr_fmms` paths for supported fused expression shapes.

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

The build also compiles examples and the GMP benchmark families.

## Benchmarks

Benchmarks are standalone executables under `benchmarks/`, split by numeric
backend.  The current benchmark families live under `benchmarks/gmp/`; shared
runner and plotting helpers live under `benchmarks/common/`.

```sh
cmake --build build -j
benchmarks/common/run_benchmarks.sh build 128 8 8 4 4 3 3 3
```

## Known Limitations

- `gmpxx::mpfc_class` math APIs are present, but the current MPF transcendental
  helpers are GMP-only double-backed approximations. Full arbitrary-precision
  MPF transcendental algorithms are still future work.
- `gmpxx::mpfc_class` is a GMP-only complex convenience type, not a replacement
  for MPC. Division uses a Smith-style scaled formula, but MPF-backed
  multiplication and inverse/transcendental formulas still do not provide
  MPC-style correctly rounded complex arithmetic or systematic guard-bit
  evaluation. Cancellation-sensitive workloads should use `mpfrxx::mpc_class`.
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
