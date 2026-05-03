# MPFR Precision and Rounding Policy Notes

Status: Complete

This note records the MPFR facts and the resulting `gmpfrxx_mkII` policy for `gmpfrxx::mpfr_class`.

Primary upstream source:

- `../source/mpfr.texi`

Generated references:

- `../generated/mpfr.txt`
- `../generated/mpfr.html`
- `../generated/mpfr.info`

Relevant upstream sections:

- Rounding
- Initialization Functions
- Assignment Functions
- Rounding-Related Functions
- Exception Related Functions

## Upstream precision facts

MPFR precision is the number of bits used to store the significand.

`mpfr_init2(x, prec)` initializes `x`, sets its precision to exactly `prec` bits, and sets its value to NaN. `prec` must be between `MPFR_PREC_MIN` and `MPFR_PREC_MAX`; outside that range, behavior is undefined.

`mpfr_init(x)` initializes `x` with the current MPFR default precision and sets its value to NaN. The MPFR manual warns that another library in the same program may change this default and not restore it, so direct use of `mpfr_init2` is safer in wrapper internals when a precision is known.

`mpfr_set_default_prec(prec)` changes the raw MPFR default precision for subsequent `mpfr_init` and `mpfr_inits` calls. Already initialized variables are unaffected. The initial raw upstream MPFR default precision is 53 bits.

`mpfr_get_default_prec()` returns the current raw MPFR default precision in bits. `gmpfrxx_mkII` uses a project default precision initialized to 512 bits for ordinary wrapper construction, so wrapper-owned objects should not rely on raw `mpfr_init` unless synchronization is explicitly documented.

`mpfr_get_prec(x)` returns the current precision of `x` in bits.

`mpfr_set_prec(x, prec)` sets the precision of an already initialized variable to exactly `prec` bits and sets its value to NaN. The previous value is lost. If the previous value must be kept, use `mpfr_prec_round` instead.

## `gmpfrxx_mkII` default precision policy

Project initial default precision:

```text
512 bits
```

This value applies to ordinary `mpfr_class` default construction and value construction without explicit precision. Recommended implementation: `mpfr_class()` uses `mpfr_init2(value_, mpfr_defaults::get_default_precision())`.

Do not implement ordinary default construction with raw `mpfr_init` unless the implementation deliberately synchronizes MPFR's mutable default precision with the project policy and documents that choice.

## Upstream rounding facts

MPFR supports these rounding modes:

```text
MPFR_RNDN  round to nearest, ties to even
MPFR_RNDD  round toward negative infinity
MPFR_RNDU  round toward positive infinity
MPFR_RNDZ  round toward zero
MPFR_RNDA  round away from zero
MPFR_RNDF  faithful rounding
```

`MPFR_RNDF` is documented as experimental. For `MPFR_RNDF`, the ternary value and inexact flag are unspecified, and results may not be reproducible. Do not use it as the wrapper default.

`mpfr_set_default_rounding_mode(rnd)` sets the MPFR default rounding mode. The initial default rounding mode is round-to-nearest.

`mpfr_get_default_rounding_mode()` returns the current MPFR default rounding mode.

Most MPFR arithmetic functions take the destination argument first, input arguments next, and an explicit rounding mode last.

MPFR treats input variables as exact. The precision of the result is the precision of the destination variable, not the precision of the input variables.

## Upstream ternary return facts

Unless documented otherwise, MPFR functions returning `int` return a ternary value:

```text
0   stored value is the exact result
>0  stored value is greater than the exact result
<0  stored value is less than the exact result
```

A NaN result corresponds to an exact return value. An infinite result is considered inexact when obtained by overflow and exact otherwise.

The opposite of a returned ternary value is guaranteed to be representable in `int`.

## `gmpfrxx_mkII` construction policy

Do not provide a precision-only constructor of the form:

```cpp
mpfr_class(mpfr_prec_t precision);
```

A single integral argument constructs a numeric value at the current project default precision, initially 512 bits:

```cpp
mpfr_class x(42);  // value 42 using the project default precision, initially 512 bits
```

Precision-only construction must use a tag or named factory:

```cpp
auto x = mpfr_class::with_prec(256);
mpfr_class y(prec_tag{}, 256);
```

Value-plus-precision construction may use an unambiguous two-argument form:

```cpp
mpfr_class x(42, 256);  // value 42 at 256-bit precision
```

The two-argument form must not be accepted in ambiguous cases where both arguments could reasonably be interpreted as a complex value, a precision pair, or a policy object.

## `gmpfrxx_mkII` assignment policy

Existing-object assignment preserves the destination precision.

```cpp
auto out = mpfr_class::with_prec(53);
mpfr_class a(1, 200);
mpfr_class b(2, 200);
out = a + b;
```

After assignment, `out.get_prec()` remains 53.

The RHS expression must be evaluated using the selected destination precision for this assignment. The evaluator must not enlarge `out` to 200 bits just because the RHS leaves have 200-bit precision.

Use `mpfr_set_prec` only when the implementation intentionally wants to discard the current value and reset the object to NaN. Do not use `mpfr_set_prec` as part of ordinary precision-preserving assignment.

## `gmpfrxx_mkII` materialization policy

New materialization without an existing destination uses expression-derived precision.

```cpp
mpfr_class a(1, 200);
mpfr_class b(2, 80);
auto out = a + b;
```

The new `out` uses the maximum precision of the `mpfr_class` leaves in the expression tree, here 200 bits.

Exact integral or rational leaves do not raise the materialization precision by themselves. In an expression containing at least one `mpfr_class` leaf, `mpz_class`, `mpq_class`, and supported integral scalars are converted at the selected MPFR evaluation precision.

## `gmpfrxx_mkII` rounding policy

All production MPFR calls must pass an explicit `mpfr_rnd_t`.

Do not rely on hidden MPFR default rounding inside arithmetic wrappers. The wrapper may expose `mpfr_defaults::get_default_rounding_mode()` and use that value, but the value must still be passed explicitly to each MPFR call.

Initial wrapper default rounding should be `MPFR_RNDN` unless changed through the documented wrapper API.

Every ternary return value must be received internally. It may be stored in an evaluator status object, accumulated conservatively, or ignored only through a named sink that makes the discard deliberate and grep-visible.
