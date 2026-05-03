# GMP mpf Precision Policy Notes

Status: Complete

This note records upstream facts about GMP `mpf_t` precision and the resulting `gmpfrxx_mkII` policy for `mpf_class`.

Primary upstream source:

- `../source/gmp.texi`

Generated references:

- `../generated/gmp.txt`
- `../generated/gmp.html`
- `../generated/gmp.info`

## Upstream precision facts

`mpf_t` objects must be initialized before use.

- `mpf_init(x)` initializes `x` to zero using GMP's current default floating precision.
- `mpf_init2(x, prec)` initializes `x` to zero and sets its precision to be at least `prec` bits.
- `mpf_get_prec(x)` returns the current precision in bits.
- `mpf_set_prec(x, prec)` sets precision to at least `prec` bits and truncates the stored value.
- `mpf_set_prec_raw(x, prec)` is a low-level effective-precision change with restoration requirements.
- `mpf_set_default_prec(prec)` sets GMP's process-global default for later `mpf_init` calls.
- `mpf_get_default_prec()` returns GMP's current process-global default precision.

## Upstream assignment facts

Assignment functions set an already initialized destination:

```text
mpf_set
mpf_set_ui
mpf_set_si
mpf_set_d
mpf_set_z
mpf_set_q
mpf_set_str
```

Use `mpf_set_z` and `mpf_set_q` for exact GMP integer and rational conversion into an initialized `mpf_t`.

There are no `mpf_add_z`, `mpf_mul_z`, or analogous `mpf_*_q` arithmetic APIs. Use an initialized temporary and `mpf_set_z` or `mpf_set_q` when needed.

## Upstream C++ interface facts

For GMP's `mpf_class`:

- an explicit precision constructor argument is used in bits;
- builtin values, `mpz_class`, and `mpq_class` without explicit precision use the default `mpf` precision;
- construction from an `mpf_class` or expression uses the source expression precision;
- binary expression precision is the higher precision of the operands;
- assignment into an existing `mpf_class` stores a value but does not copy or change the destination precision.

## `gmpfrxx_mkII` default precision policy

`gmpfrxx_mkII` does not expose GMP's process-global `mpf_set_default_prec()` as the public `mpf_class` policy.

Instead, it owns a library-local thread-local default precision.

Initial value:

```text
512 bits
```

Recommended API:

```cpp
class gmpxx_defaults {
public:
    static mp_bitcnt_t get_default_mpf_precision() noexcept;
    static void set_default_mpf_precision(mp_bitcnt_t precision);
};

class scoped_mpf_default_precision;
```

Implementation rule:

```text
mpf_class() -> mpf_init2(value_, gmpxx_defaults::get_default_mpf_precision())
```

Do not call `mpf_set_default_prec` or `mpf_get_default_prec` in ordinary wrapper code.

## Constructor policy

A single integral constructor argument constructs a numeric value at the current project default precision.

```cpp
mpf_class x(42);  // value 42 using the project default precision, initially 512 bits
```

Precision-only construction must use a tag or named factory:

```cpp
auto x = mpf_class::with_prec(256);
```

Value-plus-precision construction may use an unambiguous two-argument form:

```cpp
mpf_class x(42, 256);  // value 42 with at least 256-bit precision
```

## Assignment and materialization policy

Existing-object assignment preserves destination precision:

```cpp
auto out = mpf_class::with_prec(128);
mpf_class a(1, 512), b(2, 512);
out = a + b;
assert(out.get_prec() == 128);
```

Do not call `mpf_set_prec` during ordinary assignment.

New expression materialization uses expression-derived precision. Exact `mpz_class`, `mpq_class`, and supported integral leaves do not raise floating precision by themselves.

## `mpfc_class` policy

`mpfc_class` is a GMP-only complex type built from two `mpf_class` components. Default construction should initialize both components at the current project `mpf_class` default precision, initially 512 bits. It must not use MPFR or MPC.
