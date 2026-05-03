# MPC Expression Evaluation Policy

Status: Complete

This note records expression-evaluation rules for `gmpfrxx::mpc_class`.

## Type promotion

Allowed:

```text
mpc + mpc       -> mpc
mpc + mpfr      -> mpc
mpfr + mpc      -> mpc
mpc + mpz/mpq   -> mpc
mpz/mpq + mpc   -> mpc
```

Forbidden by default:

```text
mpc + mpf
mpf + mpc
mpc + mpfc
mpfc + mpc
```

Rationale:

- `mpf_class` belongs to the GMP-only floating family.
- `mpfc_class` belongs to the GMP-only complex family.
- `mpc_class` belongs to the MPFR/MPC floating family.
- Silent cross-family conversion would hide precision and rounding choices.

## Precision propagation

Default `mpc_class` construction uses the project MPFR default precision for both components, initially 512 bits.

For new `mpc_class` materialization, compute:

```text
real evaluation precision
imaginary evaluation precision
```

For assignment into an existing `mpc_class`, use the destination object's current component precisions.

Real-valued leaves promoted into complex expressions should contribute their precision to both component precision calculations unless a more precise expression analysis is implemented.

## Temporaries

Use:

```text
mpc_init2
```

for equal component precision temporaries.

Use:

```text
mpc_init3
```

for unequal component precision temporaries.

Do not use `mpc_set_prec` for ordinary temporary reuse unless discarding the previous value and NaN-reset behavior is explicitly intended.

## Aliasing

MPC supports using the same variable as input and output for many operations, but the wrapper evaluator must still be aliasing-safe.

Use temporaries when expression shape would otherwise destroy a still-needed operand.

## Return status

Every MPC arithmetic call returns an inexact status.

Capture it internally even if the public API does not expose it yet.
