# gmpfrxx_mkII Specifications

This file records behavioral contracts that are easy to lose when the
implementation is header-only and spans GMP, MPFR, MPC, shared libraries, and
thread-local library state.

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

Environment-derived MPFR defaults are applied once per thread, not once per
process.

The first wrapper default-state access in a thread initializes that thread's
MPFR TLS state. Examples of such accesses include:

- default construction of `mpfrxx::mpfr_class`
- `mpfrxx::default_options()`
- `mpfrxx::default_precision_bits()`
- `mpfrxx::set_default_precision_bits(...)`
- `mpfrxx::set_default_rounding_mode(...)`
- `mpfrxx::set_default_exponent_range(...)`

This means a worker thread receives the wrapper's initial 512-bit policy on
first use even if another thread has already changed its own MPFR default
precision.

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
