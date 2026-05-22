
# PROMPTS.md — MPFR/MPC in mpfrxx_mkII / Namespace-split / Scratch / ET-first Plan

This prompt set rebuilds `gmpfrxx_mkII` from scratch.

Public namespaces:

```cpp
namespace gmpxx
namespace mpfrxx
```

Internal namespace:

```cpp
namespace gmpfrxx_mkII::detail
```

Do not use `namespace gmpfrxx` as a public API namespace.

Header roles:

```text
gmpxx_mkII.h    = GMP only
mpfrxx_mkII.h   = GMP + MPFR + MPC
gmpfrxx_mkII.h  = full combined aggregator
```

This means:

```text
gmpxx_mkII.h:
  gmpxx::mpz_class
  gmpxx::mpq_class
  gmpxx::mpf_class
  optional gmpxx::mpfc_class

mpfrxx_mkII.h:
  mpfrxx::mpz_class  // alias to gmpxx::mpz_class
  mpfrxx::mpq_class  // alias to gmpxx::mpq_class
  mpfrxx::mpfr_class
  mpfrxx::mpc_class

gmpfrxx_mkII.h:
  full aggregator
  includes gmpxx_mkII.h and mpfrxx_mkII.h
```

Implementation order:

```text
Phase 0    Skeleton, CMake, header boundaries, namespace split
Phase 1    mpfrxx::mpfr_class expression-template core
Phase 2    MPFR scalar leaves and portable integer conversion
Phase 3    MPFR defaults, environment, rounding, emin/emax
Phase 4    Minimal gmpxx::mpz_class / gmpxx::mpq_class exact wrappers
Phase 5    mpz/mpq + mpfr mixed ET evaluation
Phase 6    mpfrxx::mpc_class ET core
Phase 7    MPC scalar/mixed mpfr/mpz/mpq support and environment
Phase 8    Scratch gmpxx_mkII.h: mpz/mpq/mpf ET implementation
Phase 9    Cross-header coexistence and forbidden mixed-family ops
Phase 10   Optional GMP-only gmpxx::mpfc_class
Phase 11   GMP-only mpfc_class math, if mpfc_class is kept
Phase 12   Documentation, examples, and benchmarks
```

Main rule:

```text
Public arithmetic operators must return expression nodes from the beginning.
Never implement eager binary arithmetic first.
```

---

# Generic Phase Prompt

Use this exact pattern with Codex CLI:

```text
Read AGENTS.md, STATUS.md, and PROMPTS.md.

Implement Phase N from PROMPTS.md only.

Do not proceed to Phase N+1.

Keep the implementation C++17.

Preserve the ET contract:
public arithmetic operators must return expression nodes, not eager public numeric classes.

Use public namespaces:
  gmpxx
  mpfrxx

Use internal namespace:
  gmpfrxx_mkII::detail

Do not use public namespace gmpfrxx.

Respect header roles:
  gmpxx_mkII.h    = GMP only
  mpfrxx_mkII.h   = GMP + MPFR + MPC
  gmpfrxx_mkII.h  = full combined aggregator

Do not bridge to an existing gmpxx_mkII.h implementation.
Generate the current phase from scratch inside this repository.

Add or update all tests required by the phase.

Run:
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
  cmake --build build -j
  ctest --test-dir build --output-on-failure

Update STATUS.md with:
- implemented features
- missing features
- tests added
- exact commands run
- pass/fail result
- known issues
```

---

# Non-negotiable ET Contract

For expression-capable classes, public arithmetic operators must return expression nodes.

Example:

```cpp
mpfrxx::mpfr_class a(256), b(256), c(256);
auto e = a + b * c;

static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(e)>);
static_assert(!std::is_same_v<decltype(e), mpfrxx::mpfr_class>);
static_assert(std::is_same_v<typename decltype(e)::result_type,
                             mpfrxx::mpfr_class>);
```

Do not implement eager primary operators such as:

```cpp
friend mpfrxx::mpfr_class operator+(const mpfrxx::mpfr_class&,
                                    const mpfrxx::mpfr_class&);
```

Materialization is allowed only in construction, assignment, explicit evaluation helpers, compound assignment, documented eager math functions, stream output, and comparisons.

Numerical tests alone are insufficient.

---

# Phase 0 — Skeleton, CMake, Header Boundaries, Namespace Split

Goal:

Create the repository skeleton.

Do not implement eager arithmetic.

Do not implement real `gmpxx::mpf_class`.

Required layout:

```text
include/
├─ gmpxx_mkII.h
├─ mpfrxx_mkII.h
├─ gmpfrxx_mkII.h
└─ gmpfrxx_mkII/
   └─ detail/
      ├─ config.hpp
      ├─ type_traits.hpp
      ├─ integer_conversion.hpp
      ├─ environment.hpp
      ├─ expr.hpp
      ├─ eval_context.hpp
      ├─ zq_impl.hpp
      ├─ mpfr_impl.hpp
      ├─ mpc_impl.hpp
      ├─ mpf_impl.hpp
      └─ mpfc_impl.hpp

tests/
├─ test_gmp_header_smoke.cpp
├─ test_mpfr_header_smoke.cpp
├─ test_aggregator_header_smoke.cpp
├─ test_namespace_aliases.cpp
├─ test_header_boundaries.cpp
├─ test_integer_model_diagnostics.cpp
└─ compile_fail/
   ├─ mpfr_header_must_not_expose_mpf.cpp
   ├─ mpfr_header_must_not_expose_mpfc.cpp
   └─ gmp_header_must_not_expose_mpfr_or_mpc.cpp

cmake/
├─ FindGMP.cmake
├─ FindMPFR.cmake
└─ FindMPC.cmake
```

Requirements:

1. Create public namespaces `gmpxx` and `mpfrxx`.
2. Create internal namespace `gmpfrxx_mkII::detail`.
3. `zq_impl.hpp` defines shell or minimal complete classes:
   - `gmpxx::mpz_class`
   - `gmpxx::mpq_class`
4. `mpfrxx_mkII.h` exposes:
   - `mpfrxx::mpz_class` as alias to `gmpxx::mpz_class`
   - `mpfrxx::mpq_class` as alias to `gmpxx::mpq_class`
   - `mpfrxx::mpfr_class`
   - `mpfrxx::mpc_class`
5. `mpfrxx_mkII.h` includes `<gmp.h>`, `<mpfr.h>`, and `<mpc.h>`.
6. `mpfrxx_mkII.h` must not expose `gmpxx::mpf_class` or `gmpxx::mpfc_class`.
7. `gmpxx_mkII.h` must not include `<mpfr.h>` or `<mpc.h>`.
8. `gmpfrxx_mkII.h` may include both public headers.
9. Define CMake interface targets:
    - `gmpxx_mkII` links GMP only.
    - `mpfrxx_mkII` links GMP, MPFR, and MPC.
    - `gmpfrxx_mkII` links GMP, MPFR, and MPC.
10. Add compile-fail tests proving header boundaries.
11. Add namespace tests proving:
    - `std::is_same_v<mpfrxx::mpz_class, gmpxx::mpz_class>`
    - `std::is_same_v<mpfrxx::mpq_class, gmpxx::mpq_class>`

Run CMake/CTest and update `STATUS.md`.

Do not proceed to Phase 1.

---

# Phase 1 — `mpfrxx::mpfr_class` Expression-template Core

Goal:

Implement `mpfrxx::mpfr_class` as the first real numeric class.

Do not implement `gmpxx::mpf_class`.

Do not implement full `mpfrxx::mpc_class` arithmetic yet.

Required expression infrastructure:

```text
include/gmpfrxx_mkII/detail/expr.hpp
include/gmpfrxx_mkII/detail/eval_context.hpp
```

Required expression node types:

```text
object_leaf
scalar_leaf
unary_expr
binary_expr
```

Required operation tags:

```text
add_op
sub_op
mul_op
div_op
neg_op
pos_op
```

Required traits:

```cpp
gmpfrxx_mkII::detail::is_expression_node<T>
gmpfrxx_mkII::detail::is_expression_node_v<T>
gmpfrxx_mkII::detail::expression_result_type<T>
gmpfrxx_mkII::detail::expression_result_type_t<T>
```

`mpfrxx::mpfr_class` must wrap `mpfr_t` with RAII.

Required operators returning expression nodes:

```text
+
-
*
/
unary +
unary -
```

Required materialization:

```cpp
mpfrxx::mpfr_class x = a + b * c;
out = a + b * c;
```

Rules:

1. Construction from expression materializes a new `mpfr_class`.
2. Assignment to existing `mpfr_class` evaluates into destination precision.
3. New materialization uses max precision of all `mpfr_class` leaves.
4. Internal temporaries use selected evaluation precision.
5. Aliasing must be safe.

Required tests:

```text
tests/test_et_contract_mpfr.cpp
tests/test_mpfr_expression_eval.cpp
tests/test_mpfr_precision_policy.cpp
tests/test_mpfr_aliasing.cpp
```

Do not proceed to Phase 2.

---

# Phase 2 — MPFR Scalar Leaves and Portable Integer Conversion

Goal:

Add scalar expression operands to the MPFR ET core.

Expression scalar leaves may include signed/unsigned standard integrals except bool, plus float and double.

Reject:

```text
bool
long double
__int128 as expression scalar
```

Normalize scalar leaves to ABI-stable storage:

```cpp
std::int64_t
std::uint64_t
double
```

Scalar/scalar expressions must not be intercepted.

For integral-to-MPFR conversion:

```text
integral -> gmpxx::mpz_class temporary -> mpfr_set_z
```

Required tests:

```text
tests/test_et_contract_mpfr_scalar.cpp
tests/test_mpfr_scalar_eval.cpp
tests/test_mpfr_long_width_dispatch.cpp
tests/compile_fail/test_bool_scalar.cpp
tests/compile_fail/test_long_double_scalar.cpp
tests/compile_fail/test_int128_scalar.cpp
```

Do not proceed to Phase 3.

---

# Phase 3 — MPFR Defaults, Environment, Rounding, Emin/Emax

Goal:

Add explicit MPFR default and environment handling.

Add an MPFR defaults API in `namespace mpfrxx`.

The initial wrapper-owned MPFR default precision is 512 bits.

Environment variables:

```text
MPFRXX_DEFAULT_PRECISION_BITS
MPFRXX_DEFAULT_EMIN
MPFRXX_DEFAULT_EMAX
MPFRXX_DEFAULT_ROUNDING_MODE
```

Required tests:

```text
tests/test_mpfr_defaults.cpp
tests/test_mpfr_environment.cpp
tests/test_mpfr_environment_invalid.cpp
```

Do not proceed to Phase 4.

---

# Phase 4 — Minimal `gmpxx::mpz_class` and `gmpxx::mpq_class`

Goal:

Add exact `gmpxx::mpz_class` and `gmpxx::mpq_class` wrappers so `mpfrxx_mkII.h` exposes `mpz/mpq/mpfr/mpc`.

Expose them in `mpfrxx` by alias only:

```cpp
namespace mpfrxx {
    using ::gmpxx::mpz_class;
    using ::gmpxx::mpq_class;
}
```

`mpq_class` must canonicalize results.

Required promotions:

```text
mpz + mpz       -> gmpxx::mpz_class
mpz + mpq       -> gmpxx::mpq_class
mpq + mpq       -> gmpxx::mpq_class
mpz/mpq + mpfr  -> mpfrxx::mpfr_class
mpfr + mpz/mpq  -> mpfrxx::mpfr_class
```

All public arithmetic returns expression nodes.

Required tests:

```text
tests/test_et_contract_zq_mpfr.cpp
tests/test_mpz_basic.cpp
tests/test_mpq_basic.cpp
tests/test_mpq_canonicalization.cpp
tests/test_mixed_zq_mpfr_promotion.cpp
tests/test_namespace_aliases.cpp
```

Do not proceed to Phase 5.

---

# Phase 5 — mpz/mpq + mpfr Mixed ET Evaluation

Goal:

Strengthen mixed exact/MPFR expression evaluation.

Example:

```cpp
mpfrxx::mpz_class z = 3;
mpfrxx::mpq_class q(z, 7);
mpfrxx::mpfr_class r(256);

r = z + q + r * r;
```

Must build expression nodes, promote to `mpfrxx::mpfr_class`, use MPFR conversion APIs, preserve destination precision, and use max leaf precision for new materialization.

Required tests:

```text
tests/test_mixed_zq_mpfr_eval.cpp
tests/test_mixed_zq_mpfr_precision.cpp
tests/test_mixed_zq_mpfr_aliasing.cpp
```

Do not proceed to Phase 6.

---

# Phase 6 — `mpfrxx::mpc_class` ET Core

Goal:

Implement MPC-backed `mpfrxx::mpc_class`.

`mpc_class` belongs to `mpfrxx_mkII.h` and `gmpfrxx_mkII.h`.

Wrap `mpc_t`.

Required operators returning expression nodes:

```text
mpc + mpc
mpc - mpc
mpc * mpc
mpc / mpc
unary + mpc
unary - mpc
mpc + mpfr
mpfr + mpc
```

Required tests:

```text
tests/test_et_contract_mpc.cpp
tests/test_mpc_basic.cpp
tests/test_mpc_precision_policy.cpp
tests/test_mpc_aliasing.cpp
```

Do not proceed to Phase 7.

---

# Phase 7 — MPC Scalar/Mixed mpz/mpq/mpfr Support and Environment

Goal:

Add scalar and exact mixed operands to `mpfrxx::mpc_class`, and implement MPC-specific environment variables.

Required promotions:

```text
mpc + mpc     -> mpfrxx::mpc_class
mpc + mpfr    -> mpfrxx::mpc_class
mpfr + mpc    -> mpfrxx::mpc_class
mpc + mpz/mpq -> mpfrxx::mpc_class
mpz/mpq + mpc -> mpfrxx::mpc_class
mpc + scalar  -> mpfrxx::mpc_class
scalar + mpc  -> mpfrxx::mpc_class
```

Forbidden:

```text
mpc + mpf
mpf + mpc
mpc + mpfc
mpfc + mpc
```

Environment variables:

```text
MPCXX_DEFAULT_PRECISION_BITS
MPCXX_DEFAULT_REAL_PRECISION_BITS
MPCXX_DEFAULT_IMAG_PRECISION_BITS
MPCXX_DEFAULT_ROUNDING_MODE
MPCXX_DEFAULT_REAL_ROUNDING_MODE
MPCXX_DEFAULT_IMAG_ROUNDING_MODE
```

Required tests:

```text
tests/test_et_contract_mpc_mixed.cpp
tests/test_mpc_mixed_eval.cpp
tests/test_mpc_environment.cpp
tests/test_mpc_environment_invalid.cpp
tests/compile_fail/test_mpc_plus_mpf.cpp
tests/compile_fail/test_mpf_plus_mpc.cpp
```

Do not proceed to Phase 8.

---

# Phase 8 — Scratch `gmpxx_mkII.h`: GMP-only mpz/mpq/mpf ET

Goal:

Generate `gmpxx_mkII.h` from scratch.

It must expose:

```text
gmpxx::mpz_class
gmpxx::mpq_class
gmpxx::mpf_class
```

It must not include `<mpfr.h>` or `<mpc.h>`.

Implement `gmpxx::mpf_class` as RAII wrapper around `mpf_t`.

The initial wrapper-owned GMP `mpf_class` default precision is 512 bits.

Environment variable:

```text
GMPXX_DEFAULT_MPF_PRECISION_BITS
```

Rules:

1. Wrapper-owned default precision.
2. Do not call `mpf_set_default_prec()`.
3. Existing-object assignment preserves destination precision.
4. New expression materialization uses max precision of `mpf_class` leaves.
5. Integral construction uses `gmpxx::mpz_class` then `mpf_set_z`.
6. All public arithmetic returns expression nodes.

Forbidden:

```text
gmpxx::mpf + mpfrxx::mpfr
mpfrxx::mpfr + gmpxx::mpf
gmpxx::mpf + mpfrxx::mpc
mpfrxx::mpc + gmpxx::mpf
```

Required tests:

```text
tests/test_et_contract_mpf.cpp
tests/test_mpf_basic.cpp
tests/test_mpf_precision_policy.cpp
tests/test_mpf_environment.cpp
tests/test_mixed_zq_mpf_promotion.cpp
tests/compile_fail/test_mpf_plus_mpfr.cpp
tests/compile_fail/test_mpfr_plus_mpf.cpp
tests/compile_fail/test_mpf_plus_mpc.cpp
tests/compile_fail/test_mpc_plus_mpf.cpp
```

Do not proceed to Phase 9.

---

# Phase 9 — Cross-header Coexistence and Forbidden Mixed-family Ops

Goal:

Harden header boundaries and mixed-family compile failures.

Required checks:

```text
include <mpfrxx_mkII.h> alone:
  mpfrxx::mpz_class
  mpfrxx::mpq_class
  mpfrxx::mpfr_class
  mpfrxx::mpc_class
  no mpf/mpfc exposure

include <gmpxx_mkII.h> alone:
  gmpxx::mpz_class
  gmpxx::mpq_class
  gmpxx::mpf_class
  no mpfr/mpc exposure

include <gmpfrxx_mkII.h>:
  all current public types visible
```

Forbidden operations:

```text
gmpxx::mpf + mpfrxx::mpfr
mpfrxx::mpfr + gmpxx::mpf
gmpxx::mpf + mpfrxx::mpc
mpfrxx::mpc + gmpxx::mpf
```

Required tests:

```text
tests/test_cross_header_coexistence.cpp
tests/test_cross_family_forbidden_ops.cpp
tests/compile_fail/test_gmp_header_exposes_mpfr.cpp
tests/compile_fail/test_gmp_header_exposes_mpc.cpp
```

Do not proceed to Phase 10.

---

# Phase 10 — Optional GMP-only `gmpxx::mpfc_class`

Goal:

Add a GMP-only complex floating-point type backed by `gmpxx::mpf_class`.

This is optional and separate from `mpfrxx::mpc_class`.

It is not MPC.

It belongs to `gmpxx_mkII.h` and `gmpfrxx_mkII.h`.

Required tests:

```text
tests/test_et_contract_mpfc.cpp
tests/test_mpfc_basic.cpp
tests/test_mpfc_precision_policy.cpp
tests/test_mpfc_header_boundary.cpp
tests/compile_fail/test_mpfc_plus_mpfr.cpp
tests/compile_fail/test_mpfr_plus_mpfc.cpp
tests/compile_fail/test_mpfc_plus_mpc.cpp
tests/compile_fail/test_mpc_plus_mpfc.cpp
```

Do not proceed to Phase 11.

---

# Phase 11 — GMP-only `gmpxx::mpfc_class` Math

Goal:

Add GMP-only complex math after `gmpxx::mpfc_class` ET arithmetic is stable.

Required functions:

```text
sqrt
exp
log
sin
cos
tan
sinh
cosh
tanh
pow
gamma
```

Do not use MPC for `mpfc_class`.

Do not use MPFR for GMP-only math functions.

---

# Phase 12 — Documentation, Examples, and Benchmarks

Goal:

Add documentation after semantics are fixed.

README must explain:

```text
from-scratch implementation
no gmpxx.h
no libgmpxx
header roles:
  gmpxx_mkII.h    = GMP only
  mpfrxx_mkII.h   = GMP + MPFR + MPC
  gmpfrxx_mkII.h  = full combined aggregator
namespace split:
  gmpxx
  mpfrxx
internal namespace:
  gmpfrxx_mkII::detail
mpfrxx::mpc_class is MPC-backed and exposed by mpfrxx_mkII.h
gmpxx::mpfc_class, if present, is GMP-only and separate from mpfrxx::mpc_class
```
