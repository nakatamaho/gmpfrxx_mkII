# PRECISION-only Fast Path

This note describes the `PRECISION`-only build mode used by the GMP and MPFR
benchmarks.

In implementation terms, `PRECISION` means:

```text
GMPFRXX_MKII_FAST_FIXED_PREC
```

and nothing else.  In particular, `PRECISION` alone does not imply:

```text
GMPFRXX_MKII_FAST_STABLE_RND
GMPFRXX_MKII_ENABLE_FMA
```

Benchmark names use slightly different suffixes for historical reasons:

| Backend | Benchmark suffix | Compile option |
|---------|------------------|----------------|
| GMP | `FIXED_PRECISION_FASTPATH` | `GMPFRXX_MKII_FAST_FIXED_PREC` |
| MPFR | `PRECISION` | `GMPFRXX_MKII_FAST_FIXED_PREC` |

`PRECISION_FMA`, `ROUNDING_PRECISION`, and
`ROUNDING_PRECISION_FMA` are not precision-only modes.  They combine the fixed
precision fast path with additional source or build changes.

## Contract

The fixed precision fast path is a performance contract for controlled hot
loops.

It assumes that participating floating objects keep the precision expected by
the kernel.  Existing-object assignment still preserves the destination
precision; the fast path does not change the public precision policy.

The optimization is useful for benchmark kernels such as:

```cpp
acc += x[i] * y[i];
y[i] += alpha * x[i];
```

where the product temporary has the same precision on every iteration.

It is not a semantic replacement for arbitrary expression evaluation.  Generic
expressions may still need intermediate materialization according to the normal
expression-template evaluation rules.

## What It Removes

Without the fast path, expression-form products often materialize a scoped
temporary inside the timed loop:

```text
init temporary
mul into temporary
add temporary into destination
clear temporary
```

With `GMPFRXX_MKII_FAST_FIXED_PREC`, selected direct product compound paths use
a per-thread scratch object instead:

```text
borrow thread-local scratch
mul into scratch
add scratch into destination
release scratch slot
```

After the first scratch initialization, the steady hot path avoids repeated
`mpf_init2` / `mpf_clear` or `mpfr_init2` / `mpfr_clear` for the product
temporary.

Scratch initialization, precision growth, and fallback code may still exist in
the binary.  The relevant question in disassembly is whether those calls are on
the steady element loop.

## When Thread-local Scratch Exists

Thread-local scratch storage is part of the precision fast path implementation,
but it is not part of every build or every expression path.

The intended control flow is:

```cpp
if constexpr (build_options::fast_fixed_precision) {
    mpf_thread_scratch product(precision);   // or mpfr_thread_scratch
    // borrow from the thread-local scratch pool
} else {
    scoped_mpf_temporary product(precision); // or scoped_mpfr_temporary
    // construct and clear a local temporary
}
```

In a normal build without `GMPFRXX_MKII_FAST_FIXED_PREC`, the selected branch is
the scoped temporary path.  The thread-local scratch pool may still be defined
in the header, but the normal expression hot path does not borrow from it.

Even in a precision-fastpath build, thread-local scratch is used only by the
selected direct product compound paths.  Other expression shapes may still use
the ordinary materialization path, and very large precisions or exhausted
scratch slots fall back to a local temporary.

So the correct reading is:

```text
thread-local scratch is available in the implementation,
but it is used only by selected GMPFRXX_MKII_FAST_FIXED_PREC paths.
```

## What It Does Not Remove

`PRECISION` alone does not cache MPFR rounding mode.

For MPFR, a plain `PRECISION` target still obtains the current default rounding
mode when the operation needs it.  Use the source-level `ROUNDING` variant, now
implemented with `with_rounding`, when the timed loop should use a rounding mode
captured before the loop.

`PRECISION` alone also does not enable FMA.

For MPFR, `mpfr_fma` appears only in FMA-enabled targets whose source shape can
be captured by the expression-template FMA path and whose build enables:

```text
GMPFRXX_MKII_ENABLE_FMA
```

This distinction matters because split multiply-add and FMA have different
rounding semantics.

## GMP: Precision-only Shape

Representative source:

```cpp
mpf_class acc(0.0, precision);
for (int64_t i = 0; i < n; ++i) {
    acc += dx[i] * dy[i];
}
```

Baseline GMP expression materialization contains temporary initialization and
cleanup in the element loop:

```asm
# Rdot_gmp_kernel_01_mkII
.L_gmp_baseline_element:
    call   __gmpf_get_prec@plt
    call   __gmpf_init2@plt
    call   __gmpf_mul@plt
    call   __gmpf_add@plt
    call   __gmpf_clear@plt
    cmp    <i>,<n>
    jne    .L_gmp_baseline_element
```

Here the element loop runs from `.L_gmp_baseline_element` through the branch
back to `.L_gmp_baseline_element`.  The branch target is the next iteration
precision read and product-temporary construction.

The precision-only target changes the steady path to a thread-local scratch
temporary:

```asm
# Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH
.L_gmp_precision_element:
    call   __gmpf_get_prec@plt
    cmpb   $0x0,%fs:<tls scratch active flag>
    jne    .L_gmp_precision_fallback
    cmp    <scratch precision>,<lhs precision>
    jne    .L_gmp_precision_resize_or_fallback
.L_gmp_precision_steady_scratch:
    call   __gmpf_mul@plt
    call   __gmpf_add@plt
    movb   $0x0,%fs:<tls scratch active flag>
    cmp    <i>,<n>
    jne    .L_gmp_precision_element
```

Here `.L_gmp_precision_element` is the outer element loop.  The steady scratch
subpath starts at `.L_gmp_precision_steady_scratch` after the active-slot and
precision checks.  The final `jne` jumps back to
`.L_gmp_precision_element`, not to the middle of the scratch path.

The important change is that `__gmpf_init2` and `__gmpf_clear` are no longer on
the steady product-temporary path.  The loop still reads the destination
precision and still has thread-local scratch bookkeeping.

This bookkeeping is not a map or container lookup.  It is the minimal state
management needed to reuse a per-thread scratch object safely:

```text
read the TLS scratch slot state
check whether the slot is already active
check whether the slot has enough precision
mark the slot active while the product temporary is borrowed
mark the slot inactive after the add
```

On Linux x86-64 this appears as `%fs:<offset>` TLS loads/stores and a few
branches.  It remains because the wrapper must still know which scratch object
can be reused for this expression evaluation.  `PRECISION` removes repeated
GMP object construction for the product temporary; it does not make the wrapper
state-free.

GMP has no MPFR-style rounding lookup in this path, so there is no separate
`ROUNDING` optimization class.

## MPFR: Precision-only Shape

Representative source:

```cpp
mpfr_class acc(0.0, precision);
for (int64_t i = 0; i < n; ++i) {
    acc += dx[i] * dy[i];
}
```

Baseline MPFR expression materialization contains both rounding lookup and
temporary initialization/cleanup in the element loop:

```asm
# Rdot_mpfr_kernel_01
.L_mpfr_baseline_element:
    call   mpfr_get_default_rounding_mode@plt
    call   mpfr_init2@plt
    call   mpfr_mul@plt
    call   mpfr_add@plt
    call   mpfr_clear@plt
    cmp    <i>,<n>
    jne    .L_mpfr_baseline_element
```

Here the `jne` returns to `.L_mpfr_baseline_element`, so both the default
rounding lookup and product temporary construction are repeated for the next
element.

The precision-only caller loop still obtains the rounding mode, but delegates
the product materialization to the fixed-precision helper:

```asm
# Rdot_mpfr_kernel_01_PRECISION
.L_mpfr_precision_element:
    call   mpfr_get_default_rounding_mode@plt
    mov    <lhs precision>,%rdx
    call   mpfr_compound_assign_with_context...
    cmp    <i>,<n>
    jne    .L_mpfr_precision_element
```

Here the element loop is the caller loop from `.L_mpfr_precision_element` to the
branch back to `.L_mpfr_precision_element`.  The helper has its own scratch
subpath; returning from the helper resumes the caller loop at the induction
update and branch.

The helper steady path performs split multiply-add through thread-local scratch:

```asm
# fixed-precision helper steady path
.L_mpfr_precision_helper_steady_scratch:
    call   mpfr_mul@plt
    call   mpfr_add@plt
    movb   $0x0,<tls scratch active flag>
    ret
```

The important change is that `mpfr_init2` and `mpfr_clear` are no longer on the
steady product-temporary path.  The important non-change is that
`mpfr_get_default_rounding_mode` remains unless the source is changed to a
`ROUNDING` variant.

## Comparison

| Backend | Precision-only target | Removes steady product init/clear | Removes rounding lookup | Enables FMA | Remaining steady overhead |
|---------|-----------------------|-----------------------------------|-------------------------|-------------|---------------------------|
| GMP | `*_FIXED_PRECISION_FASTPATH` | Yes | Not applicable | No | destination precision read, TLS scratch-slot bookkeeping |
| MPFR | `*_PRECISION` | Yes | No | No | destination precision read, default rounding lookup, TLS scratch-slot bookkeeping |

For MPFR, the closest raw-C-like FMA path is not `PRECISION`; it is the combined
rounding, precision, and FMA path, for example:

```text
Rdot_mpfr_kernel_01_ROUNDING_PRECISION_FMA
```

That target has cached rounding and one `mpfr_fma` call per element.  It is a
different optimization class from `PRECISION` alone.

## Verification Checklist

Build representative benchmark targets:

```bash
cmake --build build_bench_release --target \
  Rdot_gmp_kernel_01_mkII \
  Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH \
  Rdot_mpfr_kernel_01 \
  Rdot_mpfr_kernel_01_PRECISION \
  Rdot_mpfr_kernel_01_ROUNDING \
  Rdot_mpfr_kernel_01_ROUNDING_PRECISION \
  Rdot_mpfr_kernel_01_ROUNDING_PRECISION_FMA -j
```

Inspect representative hot paths:

```bash
objdump -Cd --no-show-raw-insn \
  build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH

objdump -Cd --no-show-raw-insn \
  build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_PRECISION
```

Addresses are build-specific.  Check the call sequence, not the numeric
addresses.

Expected observations:

| Target class | Expected steady product path |
|--------------|------------------------------|
| GMP baseline expression | `mpf_init2`, `mpf_mul`, `mpf_add`, `mpf_clear` |
| GMP precision-only | `mpf_mul`, `mpf_add` through scratch; no steady `mpf_init2` / `mpf_clear` |
| MPFR baseline expression | `mpfr_get_default_rounding_mode`, `mpfr_init2`, `mpfr_mul`, `mpfr_add`, `mpfr_clear` |
| MPFR precision-only | `mpfr_get_default_rounding_mode`, helper call, then scratch `mpfr_mul` / `mpfr_add` |
| MPFR rounding precision FMA | cached rounding and one `mpfr_fma` per element |

## Practical Rule

Use `PRECISION` to test the cost of product-temporary lifetime.

Use `ROUNDING` to test the cost of MPFR default rounding lookup.

Use `FMA` to test a semantic change from split multiply-add to fused
multiply-add.

Do not interpret `PRECISION` as "the C-native MPFR loop."  It only removes the
fixed-precision temporary construction cost; it does not remove every wrapper
or MPFR environment cost.
