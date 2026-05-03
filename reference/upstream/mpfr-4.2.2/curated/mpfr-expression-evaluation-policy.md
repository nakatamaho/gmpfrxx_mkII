# MPFR Expression Evaluation Policy Notes

Status: Complete

This note records expression-evaluation rules for `gmpfrxx::mpfr_class`.

Primary upstream source:

- `../source/mpfr.texi`

Generated references:

- `../generated/mpfr.txt`
- `../generated/mpfr.html`
- `../generated/mpfr.info`

## Scope

`mpfr_class` is the MPFR-backed real floating-point type.

It belongs to:

```text
mpfrxx_mkII.h
gmpfrxx_mkII.h
```

It does not belong to the GMP-only header path:

```text
gmpxx_mkII.h
```

`mpfr_class` wraps `mpfr_t`. MPFR itself is real-valued and does not define a complex scalar. Complex MPFR-backed arithmetic belongs to `mpc_class`, which wraps GNU MPC `mpc_t`.

## Type promotion

Allowed in the MPFR real evaluator:

```text
mpfr + mpfr       -> mpfr
mpz/mpq + mpfr    -> mpfr
mpfr + mpz/mpq    -> mpfr
integral + mpfr   -> mpfr
mpfr + integral   -> mpfr
```

The supported integral scalar set must be the same deliberately bounded set used by the rest of the wrapper. `bool`, `long double`, and `__int128` must not be accepted as expression scalar leaves.

Crossing from real MPFR to complex MPC leaves the MPFR evaluator and enters the MPC evaluator:

```text
mpc + mpfr  -> mpc
mpfr + mpc  -> mpc
```

Forbidden by default:

```text
mpf + mpfr
mpfr + mpf
mpfc + mpfr
mpfr + mpfc
```

Rationale:

- `mpf_class` and `mpfc_class` belong to the GMP-only floating family.
- `mpfr_class` belongs to the MPFR floating family.
- `mpc_class` belongs to the MPFR/MPC complex family.
- Silent cross-family conversion would hide precision, exponent-range, and rounding choices.

## Expression leaves

`mpfr_class` leaves should be stored by `const&` in expression nodes.

Exact scalar leaves should be stored by value. Do not store references to temporary scalar wrappers that can dangle.

`mpz_class` and `mpq_class` leaves are exact inputs. They are converted to MPFR at the selected evaluation precision when the expression is evaluated.

## Selected evaluation precision

There are two different modes. Do not collapse them.

### Default construction

```cpp
mpfr_class d;
assert(d.get_prec() == 512);
```

The project default precision is initially 512 bits.

## Existing-object assignment

For assignment into an existing `mpfr_class`, the selected evaluation precision is the destination object's current precision.

```cpp
auto out = mpfr_class::with_prec(53);
mpfr_class a(1, 200);
mpfr_class b(2, 200);
out = a + b;
```

The evaluator must compute the RHS at 53-bit destination precision and store the result in `out`. `out.get_prec()` remains 53.

This is the precision-preserving assignment rule. Do not resize the destination to the maximum RHS leaf precision.

### New materialization

For materialization without an existing destination, the selected evaluation precision is the maximum precision of the `mpfr_class` leaves in the expression tree.

```cpp
mpfr_class a(1, 200);
mpfr_class b(2, 80);
auto out = a + b;
```

The materialized `out` has 200-bit precision.

If exact `mpz_class`, `mpq_class`, or supported integral leaves are present, they do not by themselves increase the selected precision. They are rounded into MPFR at the selected precision.

## Temporaries

All MPFR temporaries created while evaluating an expression must be initialized with the selected evaluation precision for that evaluation mode.

Use `mpfr_init2(tmp, selected_precision)` for temporaries.

Do not use `mpfr_init(tmp)` in evaluator internals when the precision is known, because it depends on mutable raw MPFR default precision. Use `mpfr_init2` with the selected project precision.

Do not use `mpfr_set_prec` as a temporary-reuse shortcut unless discarding the old value and setting the object to NaN is intentionally part of the algorithm.

## Rounding mode selection

All MPFR arithmetic calls must pass an explicit rounding mode.

Default evaluator policy:

```text
mpfr_defaults::get_default_rounding_mode()
```

Initial value:

```text
MPFR_RNDN
```

Scoped rounding-mode helpers may temporarily change the wrapper-visible default, but expression evaluation must capture the chosen rounding mode once at the start of evaluation and use it consistently through that evaluation.

Do not silently mix rounding modes inside a single expression evaluation path.

Do not use `MPFR_RNDF` as the default evaluator rounding mode. Its ternary value and inexact flag are unspecified by upstream MPFR documentation.

## Ternary return value collection

Every MPFR call returning `int` must have its return value captured.

Minimum acceptable internal representation:

```cpp
struct mpfr_eval_status {
    bool saw_inexact = false;
    bool saw_positive_ternary = false;
    bool saw_negative_ternary = false;
};
```

This status is not a mathematically exact ternary value for the whole expression unless the evaluator explicitly proves such a property. It is an internal diagnostic/status carrier.

If the public API later exposes operation status, define a separate exact contract then. Do not accidentally imply that a sequence of intermediate ternary values is already the ternary value of the fully fused mathematical expression.

## Aliasing

MPFR supports common in-place patterns such as using the same variable as both input and output for operations like multiplication.

The wrapper evaluator may use the destination as the MPFR `rop` when expression shape and aliasing make it safe.

Use temporaries when direct in-place evaluation would overwrite an operand that is still needed later in the expression.

Self-assignment and self-compound expressions must be correct:

```cpp
x = x * x + 1;
x += x * y;
```

## Required tests

Precision behavior:

```cpp
auto out = mpfr_class::with_prec(53);
mpfr_class a(1, 200);
mpfr_class b(2, 200);
out = a + b;
assert(out.get_prec() == 53);

auto materialized = a + b;
assert(materialized.get_prec() == 200);
```

Forbidden mixed-family expressions must fail to compile:

```text
mpf + mpfr
mpfr + mpf
mpfc + mpfr
mpfr + mpfc
```

Scalar rejection tests must include:

```text
bool
long double
__int128
```
