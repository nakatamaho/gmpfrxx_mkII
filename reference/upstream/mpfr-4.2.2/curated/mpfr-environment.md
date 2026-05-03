# MPFR Environment Notes

Status: Complete

This note records MPFR environment facts and the resulting `gmpfrxx_mkII` wrapper policy.

Primary upstream source:

- `../source/mpfr.texi`

Generated references:

- `../generated/mpfr.txt`
- `../generated/mpfr.html`
- `../generated/mpfr.info`

Relevant upstream sections:

- Memory Handling
- Initialization Functions
- Rounding-Related Functions
- Exception Related Functions

## Upstream environment facts

MPFR has mutable internal data that are not passed as ordinary function parameters:

```text
flags
exponent range
default precision
default rounding mode
caches
```

These data are global if MPFR was not compiled as thread-safe. If MPFR was compiled with `--enable-thread-safe`, they are per-thread using thread-local storage.

The initial values of thread-local data after a thread is created depend on the compiler and thread implementation. A wrapper must not assume that TLS initialization gives a project-specific policy unless it explicitly sets the values it needs.

`mpfr_buildopt_tls_p()` returns nonzero when MPFR was compiled as thread-safe using compiler-level thread-local storage.

## Default precision

`mpfr_set_default_prec(prec)` sets the default precision to exactly `prec` bits for subsequent `mpfr_init` and `mpfr_inits` calls. Existing initialized variables are unaffected.

The initial raw upstream MPFR default precision is 53 bits.

`mpfr_get_default_prec()` returns the current MPFR default precision in bits.

`gmpfrxx_mkII` must not mistake this upstream 53-bit initial value for the project policy. The project default precision for `mpfr_class` is library-owned and initially 512 bits. Wrapper internals should prefer `mpfr_init2` with the project-selected precision. Public default construction uses `mpfr_defaults::get_default_precision()`, initially 512 bits, unless a future API explicitly documents direct synchronization with raw MPFR defaults.

## Default rounding mode

`mpfr_set_default_rounding_mode(rnd)` sets the MPFR default rounding mode.

`mpfr_get_default_rounding_mode()` returns the current MPFR default rounding mode.

The initial default rounding mode is round-to-nearest, i.e. `MPFR_RNDN`.

Arithmetic wrappers should still pass an explicit rounding mode to each MPFR call. The default rounding mode is a policy value to read, not an excuse to call MPFR APIs without a rounding argument.

## Exponent range

`mpfr_get_emin()` and `mpfr_get_emax()` return the current smallest and largest allowed exponents.

`mpfr_set_emin(exp)` and `mpfr_set_emax(exp)` set the current exponent range endpoints. They return nonzero if `exp` is outside the range accepted by the implementation; in that case the endpoint is not changed. They return zero on success.

After changing the exponent range, it is the caller's responsibility to ensure that any input value used in subsequent operations is in the new exponent range, for example with `mpfr_check_range` where appropriate. Using a value outside the new exponent range can have undefined behavior unless a specific function documents otherwise.

`mpfr_set_emin` and `mpfr_set_emax` do not check the condition `emin <= emax` across successive calls. If `emin > emax` and a floating-point value needs to be produced as output, behavior is undefined.

Set exponent limits early, before computations that should obey that range.

## `gmpfrxx::mpfr_defaults`

Define a wrapper API that centralizes MPFR environment access:

```cpp
namespace gmpfrxx {

class mpfr_defaults {
public:
    static mpfr_prec_t get_default_precision();  // initially 512 bits
    static void set_default_precision(mpfr_prec_t precision);

    static mpfr_rnd_t get_default_rounding_mode();
    static void set_default_rounding_mode(mpfr_rnd_t rnd);

    static mpfr_exp_t get_emin();
    static void set_emin(mpfr_exp_t emin);

    static mpfr_exp_t get_emax();
    static void set_emax(mpfr_exp_t emax);

    static bool has_thread_local_environment();
};

}
```

Implementation mapping:

```text
get_default_precision       -> project-owned default precision, initially 512 bits
set_default_precision       -> project-owned default precision update; optionally also call mpfr_set_default_prec only if the implementation documents synchronization
get_default_rounding_mode   -> mpfr_get_default_rounding_mode
set_default_rounding_mode   -> mpfr_set_default_rounding_mode
get_emin                    -> mpfr_get_emin
set_emin                    -> mpfr_set_emin
get_emax                    -> mpfr_get_emax
set_emax                    -> mpfr_set_emax
has_thread_local_environment -> mpfr_buildopt_tls_p
```

`set_emin` and `set_emax` should not silently ignore MPFR errors. Either throw, assert in debug builds and return a status in no-exception builds, or expose a checked `bool try_set_*` API. Do not present a failed MPFR range update as successful.

## Scoped environment helpers

Provide RAII helpers for temporary environment changes:

```cpp
class scoped_mpfr_default_precision;
class scoped_mpfr_rounding_mode;
class scoped_mpfr_emin;
class scoped_mpfr_emax;
class scoped_mpfr_environment;
```

Each helper must save the old value on construction and restore it on destruction.

Because MPFR environment data may be global in non-thread-safe builds, these helpers should be documented as affecting the current MPFR environment, not as isolated purely local values.

Library functions should not leave changed default precision, rounding mode, or exponent range behind unless their public purpose is explicitly to change that environment.

## Cache cleanup

MPFR may allocate caches for constants and internal use.

Expose or document a cleanup hook that calls one of the MPFR cleanup APIs where appropriate, for example at test shutdown or application shutdown. Do not call cache cleanup inside ordinary arithmetic operations.
