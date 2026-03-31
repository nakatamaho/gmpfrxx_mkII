# gmpfrxx\_mkII

A modern C++20 single-header wrapper for [MPFR](https://www.mpfr.org/) and [GMP](https://gmplib.org/), built on expression templates.

**Status: early development — arithmetic core working, many features yet to come.**

## Motivation

Existing C++ wrappers for MPFR/GMP (mpreal, mpfrc++, the original gmpfrxx) are
either stuck on C++03/11 idioms, carry heavy dependencies, or silently create
temporaries that destroy performance in numerical linear algebra inner loops.

gmpfrxx\_mkII takes a different approach:

- **Expression templates** eliminate intermediate `mpfr_t` allocations in compound expressions like `a + b * c - d`.
- **C++20 concepts** (`MpfrExpr`, `Scalar`) give clear error messages instead of SFINAE walls.
- **Single header** (`gmpfrxx_mkII.h`) — drop it in and go.
- **RAII everywhere** — `mpfr_class`, `mpz_class`, `mpq_class` all manage their own memory.

The long-term goal is a **complete** wrapper suitable for MPLAPACK and general-purpose
multiprecision numerical computing.

## Requirements

| Requirement | Minimum |
|---|---|
| C++ standard | C++20 |
| Compiler | GCC ≥ 12, Clang ≥ 15 |
| MPFR | ≥ 4.0 |
| GMP | ≥ 6.0 |
| CMake (for tests) | ≥ 3.16 |

## Quick start

### Header-only usage

```cpp
#include "gmpfrxx_mkII.h"

int main() {
    mpfr_set_default_prec(256);

    mpfr_class a("3.14159265358979323846264338327950288");
    mpfr_class b("2.71828182845904523536028747135266250");

    mpfr_class c = (a + b) * (a - b);   // no intermediate mpfr_t allocated
    std::cout << c << std::endl;

    // mixed arithmetic — scalars are lazy-wrapped, never promoted early
    mpfr_class d = a * 2 + 1;

    // GMP integer / rational interop
    mpz_class  z(42);
    mpq_class  q(355, 113);
    mpfr_class e = a + z - q;
}
```

### Build & test

```bash
mkdir build && cd build
cmake ..
make -j
ctest --output-on-failure
```

## Architecture

The header is organised into six numbered sections:

| # | Section | Contents |
|---|---|---|
| 1 | **Config** | Forward declarations, `prec_tag_t`, GMP string helper |
| 2 | **Concepts** | `MpfrExpr` concept (requires `eval(mpfr_ptr, rnd)` + `get_prec()`), `Scalar` concept |
| 3 | **Expression framework** | `ExprStorage` policy, `ScalarExpr<S>`, op tags (`AddOp` etc.), `TmpMpfr`, `BinaryExpr<Op,L,R>` |
| 4 | **GMP classes** | `mpz_class`, `mpq_class` — RAII, move-aware, satisfy `MpfrExpr` |
| 5 | **mpfr\_class** | RAII over `mpfr_t`, constructors, assignment, compound assignment, I/O |
| 6 | **Operators** | `+`, `-`, `*`, `/` for all `MpfrExpr × MpfrExpr`, `MpfrExpr × Scalar`, `Scalar × MpfrExpr` combinations; unary `-` |

### Expression template flow

```
  a + b * c
       │
       ▼
  BinaryExpr<AddOp,
      mpfr_class,                      ← stored by const&
      BinaryExpr<MulOp,
          mpfr_class,                  ← stored by const&
          mpfr_class>>                 ← stored by const&
       │
       ▼  (on assignment to d)
  d.eval():
    tmp1 = b.eval(); tmp2 = c.eval()
    tmp3 = MulOp::apply(tmp1, tmp2)
    result = AddOp::apply(a, tmp3)
    mpfr_set(d, result)
```

### `ExprStorage` policy

| Type | Storage | Rationale |
|---|---|---|
| `mpfr_class`, `mpz_class`, `mpq_class` | `const T&` | Leaf RAII objects — caller guarantees lifetime |
| `BinaryExpr`, `ScalarExpr` | `T` (by value) | Intermediate nodes — must be owned to survive chaining |

This means `auto expr = a + b; mpfr_class c = expr + d;` is safe — the inner
`BinaryExpr` holding references to `a` and `b` is copied by value into the outer expression.

### Precision propagation

- **Expressions**: `get_prec()` returns `std::max` over all operands recursively.
- **Construction from expression**: inherits expression's precision.
- **Assignment to existing variable**: keeps `*this` precision, evaluates expression at that precision.
- **Compound assignment** (`+=` etc.): same as assignment — `*this` precision is preserved.

### `prec_tag` constructor

```cpp
mpfr_class a(prec_tag, 1024);  // 1024-bit precision, value = 0
mpfr_class b(1024);            // default precision, value = 1024
```

The tagged constructor avoids the ambiguity between "set precision" and
"set integer value" that plagues other wrappers.

## What works today (v0.1)

- [x] `mpfr_class` — RAII, copy/move, construction from `int`/`long`/`unsigned long`/`double`/`const char*`/`mpz_class`/`mpq_class`
- [x] `mpz_class` — RAII, copy/move, string construction, `MpfrExpr` interface
- [x] `mpq_class` — RAII, copy/move, string construction, `MpfrExpr` interface
- [x] Expression templates for `+`, `-`, `*`, `/`
- [x] Mixed `MpfrExpr × Scalar` arithmetic (both orderings)
- [x] Compound assignment (`+=`, `-=`, `*=`, `/=`) — expression and scalar overloads
- [x] Scalar compound assignment optimised (`mpfr_add_si`, `mpfr_mul_d`, etc.)
- [x] Unary minus
- [x] `ostream` output for all three classes
- [x] `prec_tag` constructor
- [x] 5 ctest suites: construction / arithmetic / mixed\_scalar / compound / precision

## Roadmap to complete wrapper

### Phase 1 — Comparison & conversion

- [ ] Comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`)
- [ ] `mpfr_class` ↔ `mpz_class` / `mpq_class` explicit conversions
- [ ] `to_string()`, `istream >>` input
- [ ] `mpf_class` (GMP float) wrapper

### Phase 2 — Mathematical functions

- [ ] Transcendentals: `sqrt`, `cbrt`, `pow`, `exp`, `exp2`, `exp10`, `log`, `log2`, `log10`
- [ ] Trigonometric: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`
- [ ] Hyperbolic: `sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh`
- [ ] Special: `erf`, `erfc`, `gamma`, `lgamma`, `zeta`, `digamma`
- [ ] Rounding / classification: `floor`, `ceil`, `trunc`, `round`, `frac`, `isnan`, `isinf`, `iszero`
- [ ] Constants: `const_pi`, `const_log2`, `const_euler`, `const_catalan`
- [ ] All functions as lazy expression nodes (return `UnaryExpr<F,E>`)

### Phase 3 — MPLAPACK integration

- [ ] `Rlamch`-compatible machine parameter extraction
- [ ] `mpfr_class` as drop-in `REAL` for MPLAPACK
- [ ] Blue's norm-scaling constants derivation
- [ ] `abs()`, `sign()`, `min()`, `max()` as expression nodes
- [ ] Implicit conversion policy control (strict / permissive modes)

### Phase 4 — Robustness & performance

- [ ] `UnaryExpr<Op,E>` node for math functions
- [ ] Fused multiply-add (`FmaOp`) exploiting `mpfr_fma`
- [ ] Move-aware expression evaluation (steal temporaries)
- [ ] Thread-local default precision / rounding mode
- [ ] Custom allocator support via `mp_set_memory_functions`
- [ ] Exception / flag mode (track inexact, overflow, etc.)
- [ ] `constexpr` where possible (GMP/MPFR permitting)

### Phase 5 — Ecosystem

- [ ] `std::numeric_limits<mpfr_class>` specialisation
- [ ] `std::hash<mpfr_class>` for unordered containers
- [ ] Formatter for `std::format` (C++20)
- [ ] pkg-config / CMake find-module
- [ ] CI matrix (GCC 12–14, Clang 15–18, MPFR 4.0–4.2)
- [ ] Comprehensive documentation with Doxygen
- [ ] Benchmark suite (vs mpreal, vs raw MPFR C)

## Design decisions & rationale

**Why not inherit from / wrap mpreal?**
mpreal predates C++11 move semantics and concepts. Retrofitting expression
templates into its API would break backward compatibility.  A clean-room design
with C++20 as the baseline avoids legacy baggage.

**Why `ExprStorage` instead of universal `const&`?**
`auto expr = a + b;` must keep the inner `BinaryExpr` alive. If `BinaryExpr`
stored sub-expressions by reference, the intermediate returned from `operator+`
would be a dangling temporary. The `ExprStorage` policy stores intermediates by
value and leaf RAII objects by reference — safe by construction.

**Why `prec_tag` instead of a named constructor?**
Named constructors (static methods) cannot participate in implicit conversion
chains. A tag type keeps the constructor in overload resolution while
disambiguating from `mpfr_class(1024)` (integer value).

**Why separate `mpz_class` / `mpq_class` instead of using GMP's `mpz_class`?**
GMP's C++ classes don't satisfy `MpfrExpr`. Rather than adapter wrappers, we
provide our own lightweight RAII classes that natively participate in expression
templates. If you need interop with GMP's classes, access the raw `mpz_t` /
`mpq_t` via `get_mpz_t()` / `get_mpq_t()`.

## Tests

| Suite | What it covers |
|---|---|
| `test_construction` | All constructors for `mpfr_class`, `mpz_class`, `mpq_class`; copy/move |
| `test_arithmetic` | `+`, `-`, `*`, `/` between `mpfr_class`; chained expressions; `mpz`/`mpq` in expressions; unary minus |
| `test_mixed_scalar` | `mpfr_class ⊕ int`, `double ⊕ mpfr_class`, chained mixed, assignment from scalar |
| `test_compound` | `+=`, `-=`, `*=`, `/=` with `mpfr_class`, scalar, expression, `mpz`, `mpq`; self-compound |
| `test_precision` | Precision propagation through expressions; `prec_tag` vs integer ctor; `set_prec`; compound precision preservation |

## License

[BSD 2-Clause](LICENSE)

This is intentionally a permissive license. MPFR (LGPL) and GMP (LGPL / GPL)
are linked dynamically — gmpfrxx\_mkII itself imposes no copyleft obligation.

## Acknowledgements

This project exists in the context of [MPLAPACK](https://github.com/nakatamaho/mplapack)
development and aims to eventually replace ad-hoc MPFR wrappers across the
MPLAPACK codebase with a single, well-tested, expression-template-based header.
