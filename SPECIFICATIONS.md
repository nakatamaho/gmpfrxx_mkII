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
It includes `mpfrxx_mkII.h` automatically and is layered on top of the real
MPFR wrapper.

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

`gmpxx::mpf_class` and `gmpxx::mpfc_class` are backed by GMP `mpf_t`. They do
not model IEEE signed zero, NaN, or infinity. MPF-backed math functions
therefore cannot reproduce IEEE-754 signed-zero branch cuts. In particular,
`gmpxx::atan2(0, 0)` returns `+0`; the four IEEE cases `atan2(+0,+0)`,
`atan2(-0,+0)`, `atan2(+0,-0)`, and `atan2(-0,-0)` are not distinguishable in
the MPF backend. Use `mpfrxx::mpfr_class` or `mpfrxx::mpc_class` when signed-zero
branch behavior is part of the numeric contract.

## Scalar Common Types

The wrapper-defined `std::common_type` specializations follow the same scalar
policy as expression scalar leaves. Supported real scalar leaves include
non-`bool` integral types up to 64 bits, `float`, and `double`. Integral scalar
leaves are normalized before expression evaluation to signed or unsigned 64-bit
storage, so platform-dependent public input types such as `long` and
`unsigned long` are accepted only through that normalization policy rather than
through GMP's native `long` ABI. These scalar leaves promote with
`mpfrxx::mpfr_class` to `mpfrxx::mpfr_class`, and with `mpfrxx::mpc_class` to
`mpfrxx::mpc_class`.

`bool`, `wchar_t`, `char16_t`, `char32_t`, `long double`, and `__int128` remain
outside the expression scalar-leaf contract and must not become common-type
promotion backdoors. `wchar_t`, `char16_t`, and `char32_t` are character types,
not numeric scalar operands for this wrapper.

When `mpcxx_mkII.h` or `gmpfrxx_mkII.h` is included, `std::complex<double>` is
accepted as an MPC scalar operand. It promotes with `mpfrxx::mpc_class` to
`mpfrxx::mpc_class`; it also promotes with `mpfrxx::mpfr_class` to
`mpfrxx::mpc_class` because the result has a complex component. This support is
MPC-only and must not make `mpfrxx_mkII.h` depend on GNU MPC by itself.

## MPQ Stream Input and Arithmetic Readiness

`gmpxx::mpq_class` and raw `mpq_ptr` stream extraction intentionally follow
upstream `gmpxx.h` stream behavior.  Extraction uses `mpq_set_str`-compatible
semantics, preserves the parsed numerator and denominator, and does not call
`mpq_canonicalize`.  This means input such as `2/4` remains `2/4` at the
stream layer, and structurally invalid raw rationals such as `1/0` or `0/0`
are accepted by stream extraction rather than setting `failbit`.  The stream
layer is a raw serialization compatibility boundary, not an arithmetic-ready
normalization boundary.

Constructors, `set_str`, expression evaluation, and arithmetic-producing paths
must continue to reject zero denominators and/or produce canonical rationals.
Before a value read through stream extraction is passed to GMP rational
arithmetic, users must validate and canonicalize it.  For wrapper objects, call
`mpq_class::canonicalize()`, which first rejects a zero denominator with
`std::domain_error` and then calls `mpq_canonicalize`.  For raw `mpq_t` values
read through `operator>>(std::istream&, mpq_ptr)`, use
`gmpxx::mpq_canonicalize_checked(mpq_ptr)`.  Use
`mpq_class::has_zero_denominator()` or
`gmpxx::mpq_has_zero_denominator(mpq_srcptr)` when a caller needs to inspect
the raw stream representation without mutating it.

If `mpq_class::canonicalize()` or `gmpxx::mpq_canonicalize_checked(...)`
throws because the denominator is zero, the target remains in its original raw
stream state.  Such a value is not arithmetic-ready; the caller must replace it
or repair the denominator before passing it to GMP rational arithmetic.

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

### Generative Random Expressions

Random getters follow the upstream `gmpxx.h` policy: they return generator
expression nodes, not pure value snapshots. This applies to
`gmpxx::gmp_randclass::get_z_bits(...)`,
`gmpxx::gmp_randclass::get_z_range(...)`,
`gmpxx::gmp_randclass::get_f(...)`, the matching integer getters on
`mpfrxx::gmp_randclass`, and the MPFR floating getters such as
`mpfrxx::gmp_randclass::get_fr...(...)`. Evaluating such a node consumes the
associated `gmp_randstate_t` and may produce a fresh random value.

The wrapper does not guarantee that a random expression appearing more than
once in an expression tree, or evaluated through more than one materialization
path, is generated exactly once. The generation count and evaluation order are
not a semantic contract. If one random draw must be reused, materialize it
first:

```cpp
mpfrxx::mpfr_class r = state.get_fr(prec);
mpfrxx::mpfr_class y = r + r;
```

`gmp_randclass` random generation is not internally synchronized.
`random_mpz_expr`, `random_mpf_expr`, and `random_mpfr_expr` may share the same
underlying `gmp_randstate_t`; callers must use a state from one thread at a
time or protect it with an external mutex.

## Move Semantics

Public floating wrappers must be nothrow move-constructible.

`gmpxx::mpf_class`, `mpfrxx::mpfr_class`, and `mpfrxx::mpc_class` must not add
an object-ownership flag or any other per-object layout state around the
backend C object. Their size and alignment are part of the performance
contract:

```cpp
sizeof(gmpxx::mpf_class) == sizeof(mpf_t)
alignof(gmpxx::mpf_class) == alignof(mpf_t)

sizeof(mpfrxx::mpfr_class) == sizeof(mpfr_t)
alignof(mpfrxx::mpfr_class) == alignof(mpfr_t)

sizeof(mpfrxx::mpc_class) == sizeof(mpc_t)
alignof(mpfrxx::mpc_class) == alignof(mpc_t)
```

This preserves dense array layout for BLAS-like kernels. For example,
`mpfrxx::mpfr_class` arrays must have the same stride as `mpfr_t`, not a larger
stride caused by a wrapper-side `bool valid_` and padding. Likewise,
`gmpxx::mpf_class` arrays must keep the same stride as `mpf_t`.

Move construction keeps the moved-from source as a valid backend object by
initializing destination storage and swapping backend values. This may allocate,
but it avoids wrapper-side ownership metadata and keeps dense-array layout.

Move assignment to a valid destination preserves the left-hand-side precision in normal builds.
This matches ordinary assignment semantics for existing objects.

When `GMPFRXX_MKII_FAST_FIXED_PREC` is enabled, floating wrapper
move assignment is allowed to assume that the source and destination precisions
match. A precision mismatch in `gmpxx::mpf_class`, `mpfrxx::mpfr_class`, or
`mpfrxx::mpc_class` move assignment is a caller contract violation and is
outside the normal left-hand-side precision preservation guarantee. Debug builds
may assert this precondition; release builds may omit the check and keep the
fast swap path. This macro must only be enabled for programs or benchmark kernels
that maintain a fixed precision discipline for all participating floating
objects.

After a floating wrapper object has been moved from, it remains valid for
destruction, assignment, and swap, but its numeric value and precision are
valid but unspecified. Portable code must not read a moved-from wrapper for
numeric meaning or precision policy decisions before assigning a new value to
it. Container reallocation can therefore still be proportional to active
precision; performance-critical arrays should reserve capacity or use stable
storage when relocation cost matters.

`gmpxx::mpfc_class` follows the MPF move behavior through its two
`gmpxx::mpf_class` components.

## Compile-Time Fast-Path Options

Fast-path options are compile-time contracts. They must be enabled deliberately
for a translation unit or benchmark target; they are not environment variables
and they must not silently change the default precision policy.

| Option | Applies to | Required caller contract | Optimization class |
|--------|------------|--------------------------|--------------------|
| `GMPFRXX_MKII_FAST_FIXED_PREC` | `gmpxx::mpf_class`, `mpfrxx::mpfr_class`, `mpfrxx::mpc_class` | All participating floating objects in the hot path have matching fixed precision. Mismatch is a contract violation. | Swap-based move assignment and bounded scratch reuse may skip precision-restoration work. |
| `GMPFRXX_MKII_FAST_STABLE_RND` | MPFR default-rounding paths; MPC paths that inherit MPFR default rounding | The default rounding mode is stable during the numeric kernel, except through wrapper APIs that refresh the cache. | Repeated MPFR default-rounding reads may be replaced by a cached thread-local value. |
| `GMPFRXX_MKII_ENABLE_FMA` | MPFR expression and compound-assignment paths | Fused MPFR rounding semantics are acceptable for supported expression shapes. | `mpfr_fma`, `mpfr_fmma`, `mpfr_fmms`, and related fused operations may replace materialized multiply-plus-add forms. |

The three options are independent. Fixed precision does not imply stable
rounding, stable rounding does not imply fused arithmetic, and FMA does not
imply any fixed-precision contract.

## Fused Compound Assignment

The fixed-precision fastpath flag must not control multiply-fusion semantics.
These rewrites are controlled by explicit compile-time options:

```text
GMPFRXX_MKII_ENABLE_FMA
```

GMP MPF has no wrapper fused multiply-add option. `a += b * c` and
`a -= b * c` may use a direct multiply-then-add/subtract specialization, but it
still follows MPF's two-step rounding behavior. When
`GMPFRXX_MKII_FAST_FIXED_PREC` is enabled, the direct
leaf-product compound path may reuse a header-local thread scratch object for
the intermediate product. This is a performance-only cache; it must not define
public default precision state or change MPF precision semantics. In this mode
the scratch slot may keep its active precision between uses; it is restored only
when reallocated or cleared. Fixed-precision callers are expected to keep the
same effective precision through the hot loop, so the cache must not restore
the raw precision on every release.

Fixed-precision scratch pools are bounded performance caches.  If a pool is
exhausted by a deep expression tree, the implementation may fall back to a
scoped backend temporary with normal initialization and cleanup.  Pool
exhaustion must not change observable arithmetic results, precision policy, or
exception behavior.

`GMPFRXX_MKII_ENABLE_FMA` enables MPFR's fused operations for supported expression
shapes. `a += b * c` maps to `mpfr_fma`. `a -= b * c` maps to
`mpfr_fma` with an exactly negated multiplicand, so signed-zero behavior is
left to MPFR's fused-add semantics instead of applying `mpfr_neg` to an
already rounded result. Materializing `a * b + c * d` maps to `mpfr_fmma`,
and materializing `a * b - c * d` maps to `mpfr_fmms`.

These options are separate from the
`GMPFRXX_MKII_FAST_FIXED_PREC` macro because enabling FMA
changes numeric semantics, especially for MPFR where fused evaluation performs
one rounding instead of materializing the product and then adding or
subtracting.

## Stable MPFR Rounding Fast Path

`GMPFRXX_MKII_FAST_STABLE_RND` is an opt-in performance
macro for code that does not change the MPFR default rounding mode during a
numeric kernel on the same thread.

When enabled, wrapper arithmetic reads a thread-local cached rounding value
for `current_eval_context()`. The cache is initialized to `MPFR_RNDN`, then
refreshed during wrapper first-use initialization, when wrapper environment
defaults are applied, and when callers use
`mpfrxx::set_default_rounding_mode(...)`. If raw MPFR code sets a non-initial
rounding mode before first wrapper access in a thread, first-use initialization
must synchronize the cache to that existing MPFR TLS rounding mode without
overwriting the raw MPFR state. Direct calls to
`mpfr_set_default_rounding_mode(...)` after wrapper first use are outside this
fast-path contract unless the caller refreshes the wrapper state by using the
wrapper setter before continuing.

This option does not change precision, exponent range, or object-lifetime
semantics. It only removes repeated default-rounding reads from wrapper
arithmetic. It is intended for BLAS-like kernels whose rounding mode is fixed
at function entry. Code that intentionally changes rounding mode between
individual arithmetic operations must not enable this macro.

`mpfrxx::rounding_mode_scope` changes the calling thread's MPFR default
rounding mode for the lifetime of the scope and restores the previous value on
destruction. It also updates the stable-rounding cache, so stable-rounding
builds can place one scope around a numeric loop and avoid repeated MPFR
default-rounding getter calls in wrapper arithmetic. Nested scopes are
supported.

Eager MPFR math functions, such as `sin`, `cos`, `exp`, `log`, and constants
such as `const_pi`, use the calling thread's current MPFR default rounding mode.
They therefore observe `mpfrxx::rounding_mode_scope` and the stable-rounding
cache, but they are not affected by target-bound `mpfrxx::with_rounding(...)`
handles. Use a rounding-mode scope or explicit future math overloads when a
math function itself needs a non-default rounding mode.

Some MPFR math functions are version-dependent backend features. Wrappers for
functions such as `powr`, unit-based trigonometric functions, `rootn_si`,
`compound_si`, `log2p1`, `exp2m1`, and intmax power helpers are exposed only
when the included `<mpfr.h>` advertises the corresponding backend entry point.
Older MPFR headers therefore omit only those wrapper functions; the rest of the
MPFR wrapper remains available. Feature detection must be centralized behind
feature-named preprocessor macros instead of scattering backend-version checks
through public wrapper declarations.

For eager MPFR math functions with multiple operands, result precision is chosen
from the MPFR object or MPFR expression operands before scalar and exact GMP
operands are materialized. Supported scalar operands and exact `mpz_class` /
`mpq_class` operands are converted at that chosen precision; they must not
implicitly promote the result to the current default precision. If no MPFR object
or MPFR expression operand contributes a positive precision, the normal wrapper
default precision is used.

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

`GMPXX_MKII_DEFAULT_CONTEXT_MODE` is a program-wide build configuration. All
translation units that include this header and are linked into one program must
use the same value. Mixing frozen-env and external-provider modes in different
translation units is outside the supported contract, even though the
implementation exposes the selected mode through `build_options` and uses
`if constexpr` inside inline functions.

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

Environment variable:

```text
GMPXX_DEFAULT_MPF_PRECISION_BITS
```

Invalid MPF precision environment values are user configuration errors, not
binary integration failures.  They must be ignored with a diagnostic on
`stderr`, and the built-in 512-bit wrapper default must be used instead.

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

## Default-state Policy

Wrapper-owned defaults must be explicit and predictable.

For MPF, the wrapper owns the default precision and must not depend on GMP's
process-global `mpf_set_default_prec()` state.  GMP global default access is
available only through explicitly unsafe helper APIs and is not part of wrapper
default construction.

For MPFR, libmpfr's thread-local default state is the source of truth after the
wrapper first-use initialization boundary.  Applications may establish that
boundary explicitly with `mpfrxx::initialize_thread_defaults()` before applying
raw MPFR default-state overrides.

For MPC, default precision and rounding are represented by the same MPFR
thread-local default state used by `mpfrxx::mpfr_class`; there is no separate
wrapper-owned MPC default TLS state.

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
MPFRXX_DEFAULT_EMIN
MPFRXX_DEFAULT_EMAX
MPFRXX_DEFAULT_ROUNDING_MODE
```

These environment values are applied to MPFR's thread-local default state. Invalid
MPFR environment values are ignored with a diagnostic written to `stderr`; the
corresponding built-in or current thread-local fallback remains in effect.

### Per-Thread Initialization

Wrapper initial MPFR defaults are applied at most once per linked image and per
thread.  The initialization path applies environment-derived MPFR defaults when
an `MPFRXX_*` environment variable is present, and otherwise applies the
wrapper's builtin 512-bit initial precision policy.  This behavior is shared by
`mpfrxx_mkII.h` and `gmpfrxx_mkII.h`.

Implicit first-use initialization applies only if the calling thread's libmpfr
default state still has MPFR's library-initial values:

```text
precision = 53
rounding  = MPFR_RNDN
emin      = MPFR_EMIN_DEFAULT
emax      = MPFR_EMAX_DEFAULT
```

The wrapper uses a DSO-local `thread_local` one-shot flag to avoid repeatedly
probing and applying defaults from hot evaluation-context paths.  The flag does
not store mutable MPFR default precision, rounding, `emin`, or `emax`; MPFR's
own TLS state remains the source of truth for those values.

Because the one-shot flag is DSO-local, two linked images that include the
header-only wrapper can each perform their own first-use initialization in the
same thread. Raw MPFR default-state mutation between those first-use points is
not a cross-DSO synchronization mechanism. Applications that need deterministic
defaults across shared-library boundaries should establish a startup boundary
for every participating image and thread, or avoid raw MPFR default-state
mutation after wrapper use begins.

The library-initial sentinel cannot distinguish untouched MPFR state from
application code that deliberately set all MPFR default fields back to the same
library-initial values before first wrapper access in that linked image.  In
that case, the wrapper applies its initial-default policy.  Applications that
need an explicit boundary can call `mpfrxx::initialize_thread_defaults()` first
and then apply raw MPFR default-state changes such as
`mpfr_set_default_prec(...)`.  After the one-shot initialization has run, later
raw MPFR default-state changes in the same thread are not overwritten by
implicit wrapper default access.

Wrapper default-state accesses trigger the one-shot initialization check.
Examples of such accesses include:

- `mpfrxx::initialize_thread_defaults()`
- default construction of `mpfrxx::mpfr_class`
- `mpfrxx::default_options()`
- `mpfrxx::default_precision_bits()`
- `mpfrxx::set_default_precision_bits(...)`
- `mpfrxx::set_default_rounding_mode(...)`
- `mpfrxx::set_default_exponent_range(...)`

This means a worker thread receives the wrapper's initial 512-bit policy on
first use if its libmpfr default state still equals MPFR's library-initial
state. If application code or another linked image has already changed any MPFR
default field in that thread to a non-library-initial value, wrapper access
must leave the existing libmpfr state alone. Stable-rounding builds must still
synchronize their cached rounding value to the existing MPFR TLS rounding mode
during that first-use boundary.

### Runtime Mutation

Default setters affect only the calling thread's MPFR TLS state:

```cpp
mpfrxx::set_default_precision_bits(...)
mpfrxx::set_default_rounding_mode(...)
mpfrxx::set_default_exponent_range(...)
```

They affect only subsequently constructed objects in the calling thread.
Assignment to an existing object preserves the destination object's precision.

`mpfrxx::initialize_thread_defaults()` runs the wrapper one-shot first-use
default initialization for the calling thread without constructing an MPFR
object.

`mpfrxx::reload_mpfr_defaults_from_environment()` reloads environment-derived
defaults for the calling thread only. It does not broadcast changes to other
threads.

Applications that want consistent runtime defaults across worker threads must
call the setter or reload API inside each worker thread, or pass explicit
precision and rounding context to their own numeric kernels.

### Explicit Rounding and Evaluation Contexts

`mpfrxx::with_rounding(value, rounding_mode)` returns a lightweight non-owning
handle to `value` plus an explicit MPFR rounding mode. The handle must not
outlive the referenced `mpfrxx::mpfr_class`, and must not be stored, returned,
or used after that object is destroyed or moved from.

The MPFR rounding handle is target-bound. It applies to assignment and compound
assignment through that handle, carries no public precision field, and lets the
destination `mpfr_class` determine the result precision exactly as direct
`mpfr_mul`, `mpfr_add`, or `mpfr_fma` calls do. It does not create a dynamic
rounding scope and does not affect independently materialized eager math
functions such as `sin(x)`, `exp(x)`, or `const_pi(precision)`.

No public MPFR precision context is provided. Builds that enable fixed-precision
fastpaths may still treat fixed precision as an internal caller contract in hot
expression paths, but that is controlled by build options rather than by a
public MPFR precision-context object.

MPC keeps the target-bound precision-and-rounding context API because MPC needs
separate real and imaginary component precision. `mpfrxx::mpc_evaluation_context`
stores real precision, imaginary precision, and an `mpc_rnd_t`;
`mpfrxx::with_context(mpc_value, context)` returns a non-owning handle to an
`mpfrxx::mpc_class`. Assignment and compound assignment through that handle
evaluate with the cached `mpc_rnd_t` and target component precisions, avoiding
per-operation default MPC rounding lookup in fixed-context loops. The handle may
also be constructed from separate real and imaginary MPFR rounding modes, which
are packed with `MPC_RND(real, imag)`.

The MPC context handle must not outlive the referenced object. Normal builds
check that the context precision matches the target, and fixed-precision
fastpath builds may treat a mismatch as caller contract violation. It is not a
dynamic MPC math-function scope and does not change MPC or MPFR defaults.

### MPFR Comparisons

`mpfrxx::cmp(...)` is a total-order-style helper only for ordered MPFR values.
If either operand is NaN after materialization, `cmp` throws
`std::domain_error` for an unordered comparison.  Equality and relational
operators are NaN-aware predicates instead: equality is false for unordered
operands, inequality is true, and ordering predicates are false when the
comparison is unordered.

## Environment Policy

Environment variables are process startup configuration for this wrapper.
Callers must finalize `GMPXX_*`, `MPFRXX_*`, and `MPCXX_*` variables before
first wrapper default access and before worker threads begin wrapper numeric
work. Concurrent `setenv`, `unsetenv`, or `putenv` calls racing with wrapper
default reads are outside the supported contract.

Invalid user environment values must not terminate the process.  They should be
ignored with a diagnostic, and the relevant built-in default should be used.

Provider ABI mismatches and internally inconsistent external default-context
records may terminate the process after emitting a diagnostic, because they
indicate binary integration errors rather than user configuration typos.

## MPC Default State

MPC defaults inherit MPFR's libmpfr-owned TLS defaults unless an MPC-specific
precision or rounding override is installed for the current thread and linked
image:

- without an MPC precision override, default real precision equals MPFR default precision
- without an MPC precision override, default imaginary precision equals MPFR default precision
- without an MPC rounding override, default real rounding equals MPFR default rounding
- without an MPC rounding override, default imaginary rounding equals MPFR default rounding

MPC-specific overrides affect only `mpfrxx::mpc_class` defaults. They do not
change `mpfrxx::mpfr_class` defaults and do not write MPFR's thread-local default
precision or rounding mode.

Assigning a real value to `mpfrxx::mpc_class`, including `mpfr_class`,
`mpz_class`, `mpq_class`, and supported scalar inputs, sets the imaginary
component to `+0`, following GNU MPC's `mpc_set_fr`-family semantics.

MPC default precision may be symmetric or asymmetric when explicitly requested
through the MPC precision API:

```cpp
mpfrxx::set_default_mpc_precision_bits(precision)
mpfrxx::set_default_mpc_precision_bits(real, imag)
```

All precision arguments must be valid MPFR precisions. The one-argument API
installs a symmetric MPC precision override. The two-argument API installs the
specified real and imaginary precision override, including asymmetric precision
when `real != imag`.

MPC default rounding may also be symmetric or asymmetric when explicitly
requested through the MPC rounding API:

```cpp
mpfrxx::set_default_mpc_rounding_mode(rounding)
mpfrxx::set_default_mpc_rounding_mode(real, imag)
```

The one-argument API installs a symmetric MPC rounding override. The two-argument
API installs the specified real and imaginary rounding override, including
asymmetric rounding when `real != imag`. The resulting `mpc_rnd_t` is formed with
`MPC_RND(real, imag)`.

### Windows MPC Division Workaround

On `_WIN32` builds, `mpfrxx::mpc_class` division may use a scaled finite-value
workaround instead of calling `mpc_div` directly.  This is a portability guard
for MinGW/MPC builds where direct MPC division can overflow near MPFR's
32-bit-style exponent limits even when the exact quotient is finite and small.

The workaround is intentionally narrow. It is considered only when all real and
imaginary input components are finite MPFR numbers, the divisor is not
`0 + 0i`, and at least one input component has an exponent within a small margin
of the current MPFR `emin` or `emax`.  In that case the implementation performs
componentwise max-scaling in MPFR temporaries: all input components are divided
by `max(abs(real(divisor)), abs(imag(divisor)))` before forming the bounded
denominator `cr^2 + di^2`.  The final real and imaginary components are rounded
using the requested MPC rounding mode. Non-Windows builds call `mpc_div`
directly.

This workaround is not a separate mathematical semantics for complex division.
It is a platform compatibility path intended to avoid spurious exponent-edge
failure.  Last-bit differences from a direct `mpc_div` path are possible because
the scaled path uses finite MPFR temporaries and then applies the requested
component rounding.

MPC environment variables follow the same rule:

```text
MPCXX_DEFAULT_PRECISION_BITS
MPCXX_DEFAULT_REAL_PRECISION_BITS
MPCXX_DEFAULT_IMAG_PRECISION_BITS
MPCXX_DEFAULT_ROUNDING_MODE
MPCXX_DEFAULT_REAL_ROUNDING_MODE
MPCXX_DEFAULT_IMAG_ROUNDING_MODE
```

If any `MPCXX_*` variable is present, first use of the MPC default API
or default `mpc_class` construction reads the MPC environment once for the
current linked image and thread. Valid MPC precision variables install an
MPC-specific precision override. Valid MPC rounding variables install an
MPC-specific rounding override. If no valid MPC-specific variable in a category
is present, that category continues to inherit the current MPFR default.

`MPCXX_DEFAULT_PRECISION_BITS` and `MPCXX_DEFAULT_ROUNDING_MODE` set both
components. `MPCXX_DEFAULT_REAL_*` and `MPCXX_DEFAULT_IMAG_*` override their respective
component only; the other component inherits the corresponding MPC default or,
if none was provided, the MPFR default. Therefore asymmetric MPC environment
precision and rounding are valid and affect only `mpfrxx::mpc_class` defaults.
Invalid MPC environment values are ignored with a diagnostic written to `stderr`;
the inherited MPFR default or the valid component-specific fallback remains in
effect.

`mpfrxx::reload_mpc_defaults_from_environment()` explicitly reloads MPC
environment defaults into the MPC-specific override state. It does not change
`mpfrxx::mpfr_class` default precision or rounding mode.

## Rationale

MPFR already owns default precision, rounding mode, and exponent range. MPC uses
that MPFR state as the inherited default. MPC-specific override state is kept
separate because MPC has two component precisions and two component rounding
modes, which cannot be represented by MPFR's scalar default state.

Using `libmpfr` as the source of truth avoids wrapper-owned DSO-local TLS
state. Requiring MPFR TLS support makes per-thread defaults well-defined. The
only wrapper responsibility is to apply the wrapper's initial environment
policy to each thread's MPFR TLS state before that thread observes or mutates
the defaults.
