# gmpfrxx_mkII Specifications

This file records behavioral contracts that are easy to lose when the
implementation is header-only and spans GMP, MPFR, MPC, shared libraries, and
thread-local library state.

## Header Roles

Public headers are split by required numeric backend and link dependency.

| Header | Public API | Dependencies |
| --- | --- | --- |
| `gmpxx_mkII.h` | `gmpxx::mpz_class`, `gmpxx::mpq_class`, `gmpxx::mpf_class`, `gmpxx::mpfc_class` | GMP only |
| `mpfrxx_mkII.h` | `mpfrxx::mpz_class`, `mpfrxx::mpq_class`, `mpfrxx::mpfr_class` | GMP + MPFR |
| `mpcxx_mkII.h` | `mpfrxx::mpc_class` | GMP + MPFR + MPC |
| `gmpfrxx_mkII.h` | combined aggregator | GMP + MPFR + MPC |

`gmpxx_mkII.h` is intentionally GMP-only. It must not include `<mpfr.h>` or
`<mpc.h>`, must not reference `mpfr_*`, `mpc_*`, or `mpc_t` symbols, and must
not require MPFR or MPC at link time.

`mpfrxx_mkII.h` is intentionally the real MPFR header. It requires GMP and
MPFR, but it must not require GNU MPC. Applications that only use real MPFR
arithmetic should not need to include `<mpc.h>` or link `-lmpc`.

`mpcxx_mkII.h` is the opt-in complex MPC header. It is used when
`mpfrxx::mpc_class` is needed and therefore requires GNU MPC and `-lmpc`.
It is layered on top of `mpfrxx_mkII.h`.

`gmpfrxx_mkII.h` is a convenience aggregator for users that want all supported
types and accept the full GMP + MPFR + MPC dependency set.

The GMP complex type is different by design: `gmpxx::mpfc_class` is implemented
as two `gmpxx::mpf_class` components in the GMP-only wrapper. It remains
header-only and does not require GNU MPC, so it is kept in `gmpxx_mkII.h`
rather than split into a separate GMP-complex header.

## Public Types

| Type | Backend | Header |
| --- | --- | --- |
| `gmpxx::mpz_class` | GMP `mpz_t` | `gmpxx_mkII.h`, `mpfrxx_mkII.h`, `gmpfrxx_mkII.h` |
| `gmpxx::mpq_class` | GMP `mpq_t` | `gmpxx_mkII.h`, `mpfrxx_mkII.h`, `gmpfrxx_mkII.h` |
| `gmpxx::mpf_class` | GMP `mpf_t` | `gmpxx_mkII.h`, `gmpfrxx_mkII.h` |
| `gmpxx::mpfc_class` | two `gmpxx::mpf_class` components | `gmpxx_mkII.h`, `gmpfrxx_mkII.h` |
| `mpfrxx::mpfr_class` | MPFR `mpfr_t` | `mpfrxx_mkII.h`, `gmpfrxx_mkII.h` |
| `mpfrxx::mpc_class` | GNU MPC `mpc_t` | `mpcxx_mkII.h`, `gmpfrxx_mkII.h` |

`mpfrxx::mpz_class` and `mpfrxx::mpq_class` are aliases to the GMP-only
`gmpxx` types, not separate wrapper classes:

```cpp
namespace mpfrxx {
using ::gmpxx::mpz_class;
using ::gmpxx::mpq_class;
}
```

`gmpxx::mpfc_class` is GMP-only and separate from `mpfrxx::mpc_class`.
`mpfrxx::mpc_class` is MPC-backed. Mixed arithmetic between these families is
intentionally rejected.

`mpfrxx::mpc_class` supports equality and inequality only. Complex ordering
operators such as `<`, `<=`, `>`, and `>=` are intentionally not defined. MPC
values with a NaN component compare unequal to themselves, matching the usual
IEEE-style NaN equality rule.

## Expression Template Lifetime

Public arithmetic operators return expression nodes. Expression nodes may borrow
lvalue wrapper operands by raw reference and may own rvalue wrapper operands.

Do not bind expression templates to `auto` and let them outlive their operands.
This is a user-visible contract, not an implementation accident.

```cpp
// BAD: e holds raw references to a and b.
auto e = a + b;
mutate_or_destroy(a);
gmpxx::mpf_class r = e;  // dangling if a was destroyed; changed value if mutated

// GOOD: evaluate immediately.
gmpxx::mpf_class r = a + b;
```

Materialize to a public numeric wrapper object whenever a stable value snapshot
is needed:

```cpp
auto snapshot = gmpxx::mpf_class(a + b);
```

Expression node types should be marked `[[nodiscard]]` so standalone discarded
expressions such as `a + b;` can produce compiler diagnostics. This does not
make saved expression nodes lifetime-safe.

## Move Semantics

Public floating wrappers must be nothrow move-constructible.

Move construction of backend-owned floating objects must not allocate a fresh
backend object just to swap it with the source. The move constructors for
`gmpxx::mpf_class`, `mpfrxx::mpfr_class`, and `mpfrxx::mpc_class` steal the
backend object storage directly and leave the source wrapper in a valid
moved-from state that owns no backend object. Destructors must therefore clear
backend storage only when the wrapper currently owns valid storage.

This policy is important for expression-template rvalue capture and for
`std::vector` reallocation of high-precision values. A move from a temporary
must not add another GMP/MPFR/MPC allocation on top of the allocation already
needed to create the temporary value.

Move assignment has two precision policies:

- In the normal precision-policy build, move assignment to a valid destination
  preserves the left-hand-side precision. This matches ordinary assignment
  semantics for existing objects.
- In `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` builds, move assignment to
  a valid destination uses `mpf_swap`, `mpfr_swap`, or `mpc_swap`. This avoids
  clearing the destination's old backend storage in the hot path and
  intentionally does not preserve the left-hand-side precision.

If the destination is already moved-from and owns no valid backend object, move
assignment may directly steal the source storage because there is no valid
backend object to swap with.

`gmpxx::mpfc_class` follows the same move behavior through its two
`gmpxx::mpf_class` components.

## Fused Compound Assignment

The fixed-precision fastpath flag must not control multiply-fusion semantics.
These rewrites are controlled by explicit compile-time options:

```text
MPFRXX_ENABLE_FMA
```

GMP MPF has no wrapper fused multiply-add option. In particular, the wrapper
must not keep a header-owned TLS scratch pool for MPF compound assignment.
`a += b * c` and `a -= b * c` use ordinary MPF expression evaluation.

`MPFRXX_ENABLE_FMA` enables MPFR's fused operations for supported expression
shapes. `a += b * c` maps to `mpfr_fma`; `a -= b * c` maps to `mpfr_fms`
with the rounding-mode adjustment required for the negated result.
Materializing `a * b + c * d` maps to `mpfr_fmma`, and materializing
`a * b - c * d` maps to `mpfr_fmms`.

These options are separate from
`GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` because enabling them changes
numeric semantics, especially for MPFR where fused evaluation performs one
rounding instead of materializing the product and then adding or subtracting.

## GMP MPF Default Precision

GMP's `mpf_set_default_prec()` and `mpf_get_default_prec()` are process-global
ambient state. They are not thread-local default state.

`gmpxx::mpf_class` therefore does not use GMP's global default precision as its
normal wrapper policy. Default construction must use an explicit precision:

```cpp
mpf_init2(..., gmpxx::default_mpf_precision_bits())
```

and must not use:

```cpp
mpf_init(...)
```

when that would derive precision from GMP's ambient global default.

The wrapper exposes explicit unsafe escape hatches for code that intentionally
wants to read or mutate GMP's global C state:

```cpp
gmpxx::unsafe_get_gmp_global_mpf_precision_bits()
gmpxx::unsafe_set_gmp_global_mpf_precision_bits(...)
```

These functions are not the wrapper default-precision policy.

### Default Context Modes

There are two GMP-only default-context modes.

#### Frozen-Env Header-Only Mode

This is the default mode:

```cpp
GMPXX_MKII_DEFAULT_CONTEXT_MODE == GMPXX_MKII_DEFAULT_CONTEXT_FROZEN_ENV
```

Properties:

- header-only
- no provider object file
- no header-owned mutable TLS storage
- default MPF precision is 512 bits unless overridden by environment
- runtime mutation of wrapper defaults is unsupported
- GMP's `mpf_set_default_prec()` does not affect wrapper default construction

Environment variables are checked in this priority order:

```text
GMPXX_MKII_DEFAULT_MPF_PREC_BITS
GMPFRXX_MKII_DEFAULT_MPF_PREC_BITS
MPFXX_DEFAULT_PREC_BITS
```

The environment is intentionally treated as immutable after first use. In this
mode, `gmpxx::set_default_mpf_precision_bits(...)` is a compatibility no-op and
`gmpxx::reload_default_mpf_precision_bits_from_environment()` keeps using the
already frozen value.

Because this mode is header-only, the frozen function-local static can be
image-local at shared-library or plugin boundaries. Each linked image may
therefore observe its own frozen wrapper default. This is acceptable for the
simple header-only mode, but it is not a cross-DSO thread-default mechanism.

#### External-Provider Mode

This mode is opt-in:

```cpp
GMPXX_MKII_DEFAULT_CONTEXT_MODE == GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER
```

Properties:

- not header-only
- requires one linked default-context provider implementation
- missing provider symbols are a link error
- no inline fallback storage in headers
- the provider owns mutable default-context storage
- the provider may use `thread_local` internally

This mode is the only supported way to use mutable thread-specific wrapper MPF
defaults across library code. In this mode:

```cpp
gmpxx::set_default_mpf_precision_bits(...)
```

sets the calling thread's provider-owned default precision, and
`gmpxx::default_mpf_precision_guard` restores the previous calling-thread
default when it leaves scope.

Exactly one provider implementation must be linked into the process. Do not
put the provider object file into multiple shared libraries. If multiple DSOs
each contain their own provider implementation, each DSO can own a different
TLS/default-context instance, and code crossing those DSO boundaries will not
share the same thread default. Prefer a dedicated provider shared library, or
designate one owning library such as MPLAPACK.

Provider identity can be checked with:

```cpp
gmpxx_mkII_default_context_provider_token_v1()
```

All participating DSOs must observe the same provider token.

### Precision Lifetime

Changing the wrapper default precision affects only subsequently constructed
`gmpxx::mpf_class` objects. Assignment to an existing object preserves the
left-hand-side precision.

## MPFR Default State

MPFR default precision, default rounding mode, and default exponent range are
owned by `libmpfr`.

The wrapper must not keep separate mutable C++ storage for MPFR default
precision, rounding mode, `emin`, or `emax`.

The public MPFR default APIs route through MPFR's C API:

```cpp
mpfr_get_default_prec()
mpfr_set_default_prec(...)

mpfr_get_default_rounding_mode()
mpfr_set_default_rounding_mode(...)

mpfr_get_emin()
mpfr_get_emax()
mpfr_set_emin(...)
mpfr_set_emax(...)
```

### TLS Requirement

This project requires MPFR to provide thread-local default state.

CMake must verify that:

- `mpfr_buildopt_tls_p()` is available.
- `mpfr_buildopt_tls_p()` reports TLS enabled.

If either condition is not met, configuration must fail. The wrapper does not
provide a fallback MPFR default-state implementation for non-TLS MPFR builds.

### Initial Wrapper Defaults

The wrapper default MPFR precision is 512 bits unless overridden by:

```text
MPFRXX_DEFAULT_PRECISION_BITS
```

The wrapper also reads:

```text
MPFRXX_EMIN
MPFRXX_EMAX
MPFRXX_ROUNDING_MODE
```

These environment values are applied to MPFR's thread-local default state.

### Per-Thread Initialization

Environment-derived MPFR defaults are applied from wrapper code only while the
calling thread's libmpfr default state still has MPFR's library-initial values:

```text
precision = 53
rounding  = MPFR_RNDN
emin      = MPFR_EMIN_DEFAULT
emax      = MPFR_EMAX_DEFAULT
```

The wrapper must not use a header-owned `thread_local` initialization flag for
this decision. Inline function-local TLS can be DSO-local on some platforms,
which would allow one linked image to reapply environment defaults after
another image had already changed the same libmpfr TLS state. The libmpfr
default state itself is the sentinel and the source of truth.

Wrapper default-state accesses check this sentinel. Examples of such accesses
include:

- default construction of `mpfrxx::mpfr_class`
- `mpfrxx::default_options()`
- `mpfrxx::default_precision_bits()`
- `mpfrxx::set_default_precision_bits(...)`
- `mpfrxx::set_default_rounding_mode(...)`
- `mpfrxx::set_default_exponent_range(...)`

This means a worker thread receives the wrapper's initial 512-bit policy on
first use if its libmpfr default state is still untouched. If application code
or another linked image has already changed any MPFR default field in that
thread, wrapper access must leave the existing libmpfr state alone.

### Runtime Mutation

Default setters affect only the calling thread's MPFR TLS state:

```cpp
mpfrxx::set_default_precision_bits(...)
mpfrxx::set_default_rounding_mode(...)
mpfrxx::set_default_exponent_range(...)
```

They affect only subsequently constructed objects in the calling thread.
Assignment to an existing object preserves the destination object's precision.

`mpfrxx::reload_mpfr_defaults_from_environment()` reloads environment-derived
defaults for the calling thread only. It does not broadcast changes to other
threads.

Applications that want consistent runtime defaults across worker threads must
call the setter or reload API inside each worker thread, or pass explicit
precision and rounding context to their own numeric kernels.

## MPC Default State

`mpfrxx::mpc_class` does not own separate wrapper-side default precision or
rounding state.

MPC default precision and default rounding are derived from MPFR's
libmpfr-owned TLS default state:

- default real precision equals MPFR default precision
- default imaginary precision equals MPFR default precision
- default real rounding equals MPFR default rounding
- default imaginary rounding equals MPFR default rounding

The wrapper intentionally supports only symmetric MPC defaults.

These APIs are accepted only when both component arguments match:

```cpp
mpfrxx::set_default_mpc_precision_bits(real, imag)
mpfrxx::set_default_mpc_rounding_mode(real, imag)
```

If the arguments differ, the wrapper throws `std::invalid_argument`.

MPC environment variables follow the same rule:

```text
MPFRXX_MPC_DEFAULT_PRECISION_BITS
MPFRXX_MPC_REAL_PRECISION_BITS
MPFRXX_MPC_IMAG_PRECISION_BITS
MPFRXX_MPC_ROUNDING_MODE
MPFRXX_MPC_REAL_ROUNDING_MODE
MPFRXX_MPC_IMAG_ROUNDING_MODE
```

If real and imaginary environment defaults are both provided, they must be
equal. Asymmetric defaults are not represented by wrapper-owned TLS state.

## Rationale

MPFR already owns default precision, rounding mode, and exponent range. Keeping
a second copy in this header-only wrapper risks divergence across shared-library
or plugin boundaries.

Using `libmpfr` as the source of truth avoids wrapper-owned DSO-local TLS
state. Requiring MPFR TLS support makes per-thread defaults well-defined. The
only wrapper responsibility is to apply the wrapper's initial environment
policy to each thread's MPFR TLS state before that thread observes or mutates
the defaults.
