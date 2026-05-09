# Precision and Rounding Policy

This document records precision and rounding policies for `gmpfrxx_mkII`.

## `mpf_class` Precision

GMP's `mpf_set_default_prec()` model is process-global in the tested GMP build.
It is not suitable for per-thread numeric defaults.

`gmpfrxx_mkII` routes MPF default precision through GMP's
`mpf_get_default_prec()` and `mpf_set_default_prec()`. This makes GMP's
process-global MPF default precision the single source of truth and avoids
header-only DSO-local wrapper default state. Callers that mutate it at runtime
are responsible for their own synchronization.

Initial project default: **512 bits**, unless `MPFXX_DEFAULT_PREC_BITS` is set
in the process environment before numeric code runs.

Rules:

1. `mpf_class()` uses the current GMP MPF default precision through `mpf_get_default_prec()`. The reload API sets it from `MPFXX_DEFAULT_PREC_BITS` when that parses as a positive precision, otherwise 512 bits.
2. Existing-object assignment preserves the destination precision.
3. New expression materialization uses the maximum precision of all `mpf_class` leaves in the expression tree.
4. Intermediate temporaries use the selected evaluation precision.
5. Runtime MPF default setter and environment reload APIs call `mpf_set_default_prec()`.

### Boundary policy

MPF default precision is user-managed process-global ambient state. This
preserves the convenience API and shares one default through libgmp rather than
through header-defined wrapper storage. Applications that need deterministic
precision across threads must synchronize default mutations or use explicit
object factories such as `mpf_class::with_precision(...)`.

## `mpfc_class` Precision

`mpfc_class` is built from two `mpf_class` components.

Rules:

1. Real and imaginary parts should normally have the same precision.
2. Existing-object assignment preserves destination component precision.
3. New expression materialization uses the maximum precision of all `mpf_class` and `mpfc_class` leaves.
4. `mpfc_class` must not use MPC.
5. MPC environment variables and MPC rounding defaults must not affect `mpfc_class`.

## `mpfr_class` Precision and Rounding

`mpfr_class` uses MPFR precision and rounding facilities. Its ordinary default
construction uses `mpfr_get_default_prec()` from libmpfr.

MPFR owns default precision, default rounding mode, and `emin`/`emax` state.
When MPFR is built with thread-safety enabled, that state is MPFR TLS. This
wrapper does not keep separate MPFR default precision or rounding storage.
`mpfrxx::set_default_precision_bits()` and
`mpfrxx::set_default_rounding_mode()` are thin wrappers around
`mpfr_set_default_prec()` and `mpfr_set_default_rounding_mode()`.

Recommended API:

```cpp
namespace gmpfrxx {

class mpfr_defaults {
public:
    static mpfr_prec_t get_default_precision();
    static void set_default_precision(mpfr_prec_t precision);

    static mpfr_rnd_t get_default_rounding_mode();
    static void set_default_rounding_mode(mpfr_rnd_t rnd);

    static mpfr_exp_t get_emin();
    static void set_emin(mpfr_exp_t emin);

    static mpfr_exp_t get_emax();
    static void set_emax(mpfr_exp_t emax);
};

}
```

Rules:

1. `mpfr_class()` uses `mpfr_defaults::get_default_precision()`, which reads `mpfr_get_default_prec()`. Prefer `mpfr_init2` over raw `mpfr_init` for wrapper-owned objects when the implementation needs explicit precision.
2. A single integral argument constructs a numeric value at the current project default precision. It must not be interpreted as a precision-only constructor.
3. Precision-only construction must use an explicit tag or a named factory such as `mpfr_class::with_prec(bits)`.
4. Value-plus-precision construction may use an unambiguous two-argument form such as `mpfr_class(value, precision)`.
5. The default rounding mode should be `MPFR_RNDN` unless changed through the wrapper API.
6. Existing-object assignment preserves destination precision.
7. New expression materialization uses the maximum precision of all `mpfr_class` leaves in the expression tree.
8. Arithmetic must receive MPFR ternary return values internally, even if the public API does not expose them yet.
9. Wrapper default `emin`/`emax` setters route to `mpfr_set_emin()` and `mpfr_set_emax()` and therefore follow MPFR's thread-safety model.

## `mpc_class` Precision and Rounding

`mpc_class` wraps GNU MPC `mpc_t`.

MPC is the complex arithmetic library built on MPFR and GMP. MPFR itself is real-valued and does not provide a complex scalar type.

`mpc_class` is intentionally incompatible with `mpfc_class`. `mpc_class` belongs to the MPFR/MPC family, while `mpfc_class` belongs to the GMP-only `mpf_class` family. Their precision and rounding policies must not be merged implicitly.

### Component precision

An MPC number has separate real and imaginary component precisions.

Rules:

1. `mpc_class()` uses `mpfr_get_default_prec()` for both components by default; the initial component precision is 512 bits when MPFR has not been changed.
2. A single integral argument constructs a numeric complex value at the current project default component precisions. It must not mean equal-component precision.
3. Two integral arguments construct a numeric complex value, not a pair of precisions.
4. Precision-only construction must use explicit factories or tags, for example `mpc_class::with_prec(bits)` and `mpc_class::with_prec2(real_bits, imag_bits)`.
5. `mpc_class::with_prec(bits)` initializes both components to the same precision with `mpc_init2`.
6. `mpc_class::with_prec2(real_bits, imag_bits)` initializes unequal component precisions with `mpc_init3`.
7. Existing-object assignment preserves both destination component precisions.
8. New expression materialization chooses a pair of evaluation precisions.
9. For complex expressions, real-valued leaves promoted into a complex expression contribute their precision to both component evaluation precisions unless a more precise analysis is implemented.
10. Use `mpc_get_prec2` to query component precisions.
11. Use `mpc_get_prec` only when equal-component precision has already been established or when zero-for-unequal is intended.
12. `mpc_set_prec` destroys the previous value by setting components to NaN; do not use it for precision-preserving assignment.
13. The wrapper does not store independent MPC default real and imaginary precisions. Compatibility APIs that accept two default precisions collapse them to the maximum precision representable by MPFR's single default precision.

### Rounding

MPC operations take an explicit `mpc_rnd_t`.

A complex rounding mode is a pair of real rounding modes. The wrapper does not
own a separate default MPC rounding state. The default MPC rounding pair is
derived from `mpfr_get_default_rounding_mode()` for both components.

```cpp
namespace gmpfrxx {

class mpc_defaults {
public:
    static mpc_rnd_t get_default_rounding_mode();
    static void set_default_rounding_mode(mpc_rnd_t rnd);
};

class scoped_mpc_rounding_mode {
public:
    explicit scoped_mpc_rounding_mode(mpc_rnd_t rnd);
    ~scoped_mpc_rounding_mode();
};

}
```

Rules:

1. The initial default MPC rounding mode is `MPC_RNDNN` when MPFR's default rounding mode is `MPFR_RNDN`.
2. `mpc_defaults`-style APIs should route to `mpfr_get_default_rounding_mode()` and `mpfr_set_default_rounding_mode()` rather than wrapper-owned storage.
3. Arithmetic must pass an explicit `mpc_rnd_t` to every MPC operation.
4. MPC inexact return values must be captured internally.
5. Use `MPC_INEX_RE` and `MPC_INEX_IM` to decompose the returned inexact status.
6. Compatibility APIs that accept separate real and imaginary default rounding modes can only update MPFR's single default rounding mode when both modes are equal.

## Existing-object Assignment vs New Materialization

This distinction is mandatory for all floating-point wrappers.

### Existing object

```cpp
auto out = mpfr_class::with_prec(256);
mpfr_class a(1, 512), b(2, 512);
out = a + b;
```

`out` remains 256-bit because its destination precision is 256 bits.

Do not write this example as `mpfr_class out(256)`. Under the constructor policy above, that expression means value `256` at the current project default precision, not a 256-bit precision-only object. With the project default policy in this pack, it is initially 512-bit unless the caller changes the default.

For `mpc_class`, both destination component precisions are preserved.

### New materialization

```cpp
auto out = a + b;
```

The new object uses expression-derived precision.

For `mpc_class`, the new object uses expression-derived real and imaginary component precisions.

## Math Function Materialization

Math functions materialize wrapper objects eagerly.

For `mpf_class` and `mpfc_class`, materialization uses the input precision for unary functions and the maximum relevant operand precision for binary functions such as `pow` and `atan2`.

For `mpc_class`, unary math functions preserve the input real and imaginary component precisions. Binary `pow(mpc_class, mpc_class)` uses the pairwise maximum real and imaginary component precisions. Mixed `pow(mpc_class, mpfr_class)` and `pow(mpfr_class, mpc_class)` first promote the real operand to an MPC value and then apply the same pairwise precision rule.

Math functions are not lazy ET nodes in the current implementation.

## Fixed-Precision Fast Path

The canonical compile option is:

```text
GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
```

The user promises that `mpf_class`, `mpfc_class`, `mpfr_class`, and `mpc_class` object precisions are stable during expression evaluation.

Rules:

1. In normal builds, precision must be computed correctly from current object state.
2. In fast-path builds, precision metadata may be cached or assumed stable.
3. Violating the fast-path precision-stability contract makes numerical results unspecified.
4. The library must remain memory-safe even when the user violates the contract.
5. Do not scatter `#ifdef GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` throughout the codebase.
6. Put preprocessor checks in `detail/config.hpp` and expose a constexpr flag.
