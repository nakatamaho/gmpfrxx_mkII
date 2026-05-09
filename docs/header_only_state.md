# Header-Only State Boundary

`gmpfrxx_mkII` is a C++17 header-only library. Some scratch
pools and caches are implemented as `inline` function-local `thread_local` or
function-local `static` state. That is appropriate for a single executable image,
but it is not a process-wide singleton contract across all dynamic-library
configurations.

On Windows DLL builds, and on some macOS dylib or plugin configurations, each
image that instantiates the headers may own a separate copy of this state. An
executable, a DLL, and a plugin can therefore observe different wrapper default
scratch pools and cache contents even though they run in the same process.

## Affected State

The current header-only wrapper-owned state includes:

- MPF and MPFR thread scratch pools
- `pi_cache()`
- `log_two_cache()`
- `trig_constant_cache()`

These names are implementation details, but their boundary behavior affects
performance caches.

MPF default precision is not wrapper-owned mutable state. The MPF-facing public
default APIs route to GMP's `mpf_get_default_prec()` and
`mpf_set_default_prec()`. The environment reload API sets GMP's default from
`MPFXX_DEFAULT_PREC_BITS`, falling back to 512 bits when the variable is absent
or invalid. Runtime mutation is a user-managed process-global policy: callers
must synchronize their own use.

MPFR default precision, default rounding mode, and `emin`/`emax` are not
wrapper-owned state. The MPFR-facing public default APIs route to libmpfr:

- `mpfr_get_default_prec()` / `mpfr_set_default_prec()`
- `mpfr_get_default_rounding_mode()` / `mpfr_set_default_rounding_mode()`
- `mpfr_get_emin()` / `mpfr_set_emin()`
- `mpfr_get_emax()` / `mpfr_set_emax()`

Those values follow MPFR's own thread-safety model. With a thread-safe MPFR
build they are MPFR TLS; without one they are MPFR global state.

MPC does not provide a separate default precision or default rounding state API
in the MPC headers used by this project. The MPC-facing default APIs therefore
do not keep wrapper-owned state either. They derive both component precisions
from `mpfr_get_default_prec()` and both component rounding modes from
`mpfr_get_default_rounding_mode()`. Unequal MPC component precision remains
available only through explicit object construction such as `with_precision`.

## Contract

Wrapper-owned scratch pools and caches are per thread or per
header-instantiating image. They are not guaranteed to be process-wide across
executable, shared-library, or plugin boundaries.

Objects carry their own GMP, MPFR, or MPC value state and precision after
construction. Passing already-constructed wrapper objects across a boundary does
not require the receiver to share the sender's wrapper scratch or cache state.
The remaining risk is code on the receiving side that relies on warmed caches.

## Required Application Guidance

Applications using multiple dynamic images must choose one of these patterns:

1. Prefer explicit object factories such as `with_precision(...)` at API
   boundaries instead of relying on default construction.
2. Pass fully constructed objects, explicit precision values, or explicit
   options across ABI boundaries.
3. Treat MPF math caches as per-image performance caches, not shared state.

Recommended boundary API shape:

```cpp
struct numeric_options {
    mp_bitcnt_t mpf_precision_bits;
    mpfr_prec_t mpfr_precision_bits;
    mpfr_rnd_t mpfr_rounding_mode;
    mpfr_prec_t mpc_real_precision_bits;
    mpfr_prec_t mpc_imag_precision_bits;
    mpc_rnd_t mpc_rounding_mode;
};

void initialize_numeric_defaults(const numeric_options& options);
```

Call the initializer in each image that constructs wrapper objects.

## Non-Goals

The header-only build does not provide a single process-wide compiled anchor for
defaults or caches. Providing that would require an optional compiled support
library with exported state and ABI rules. Until such a mode exists, this
document is the intended contract for dynamic-library users.

## Practical Rule

Use default constructors only inside a boundary where defaults have been set
locally. At public ABI boundaries, pass explicit precision or already-constructed
objects.
