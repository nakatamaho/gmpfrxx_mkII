# MPFR wrapper reference for gmpfrxx_mkII

Status: Complete

This file is a project-specific, curated reference extracted from the MPFR
manual. It is not a replacement for the upstream manual.

Primary upstream source:

- `../source/mpfr.texi`

Generated references:

- `../generated/mpfr.txt`
- `../generated/mpfr.html`
- `../generated/mpfr.info`

## Rules for gmpfrxx_mkII

### `mpfr_class` lifetime

- `mpfr_class` must own exactly one initialized `mpfr_t`.
- Every initialized `mpfr_t` must be cleared exactly once.
- Copy construction and copy assignment must create independent MPFR objects.
- Move construction and move assignment must not cause double-clear.
- Moved-from objects must remain destructible.
- Constructors must leave the object in a destructible state even if conversion
  or parsing fails.

### Precision

- Default construction must initialize the underlying `mpfr_t` at the project default precision, initially 512 bits.
- Construction with explicit precision must initialize the underlying `mpfr_t`
  at that precision.
- Do not provide `mpfr_class(mpfr_prec_t)` as a precision-only constructor. A
  single integral argument constructs a numeric value at the current project default
  precision, initially 512 bits.
- Precision-only construction must use an explicit tag or named factory, for
  example `mpfr_class::with_prec(bits)`.
- Assignment from an expression must preserve the destination object's current
  precision.
- Existing-object assignment evaluates the RHS at the destination precision.
- New expression materialization uses the maximum precision across the
  `mpfr_class` leaves in the expression tree.
- Expression evaluation temporaries must use the selected evaluation precision:
  destination precision for assignment, expression-derived precision for new
  materialization.
- `prec_tag` must remain a disambiguation mechanism for precision-based
  construction.

### Rounding

- Unless a public API explicitly exposes rounding control, use the project's
  default rounding mode consistently.
- The initial wrapper default rounding mode should be `MPFR_RNDN`.
- Do not use `MPFR_RNDF` as the default rounding mode; upstream MPFR documents
  its ternary value and inexact flag as unspecified.
- Do not silently mix rounding modes inside one expression evaluation path.
- All MPFR arithmetic calls must pass an explicit rounding mode.
- All MPFR calls returning `int` must have their ternary return value captured
  internally.

### Input and output arguments

- MPFR functions generally place output arguments before input arguments.
- Wrapper implementation should follow this consistently when calling MPFR C
  APIs.
- Avoid decimal string round-trips for internal conversion paths.

### Aliasing and compound assignment

- MPFR supports common in-place arithmetic patterns.
- Compound assignments should use MPFR functions in-place where appropriate.
- Self-compound operations must remain correct.

### Interaction with GMP types

- Construction or assignment from `mpz_class` should use `mpfr_set_z`.
- Construction or assignment from `mpq_class` should use `mpfr_set_q`.
- Standard integral values may be canonicalized through `mpz_class` and then
  converted with `mpfr_set_z`.
- Do not convert GMP integers or rationals through decimal strings internally.
- Conversion must respect the destination or temporary MPFR precision policy of
  `gmpfrxx_mkII`.
- `mpf_class` belongs to the GMP-only floating family; do not accept `mpf +
  mpfr` or `mpfr + mpf` by implicit conversion.

### Expression-template participation

- `mpfr_class` is a leaf expression type.
- Leaf RAII wrapper operands should be stored by `const&`.
- Intermediate expressions should be stored by value.
- Scalar operands must not produce dangling references.
- `bool` must not satisfy the scalar concept.
- Evaluation must not create dangling references to temporaries.

## Review checklist

Before changing MPFR wrapper code, check:

- no uninitialized `mpfr_t` is passed to MPFR
- no initialized `mpfr_t` is leaked
- no initialized `mpfr_t` is cleared twice
- moved-from objects are still valid for destruction
- destination precision is preserved on assignment
- expression temporaries cannot dangle
- scalar overloads do not accidentally accept `bool`
- MPFR calls use explicit rounding modes
- MPFR ternary return values are captured internally
- `MPFR_RNDF` is not used as the default rounding mode
- GMP-to-MPFR conversions avoid string round-trips
- existing-object assignment evaluates at destination precision
- new materialization evaluates at expression-derived precision
- tests cover the changed behavior
