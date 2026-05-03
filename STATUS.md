Phase 0 status:
DONE

Implemented features:
- Created the Phase 0 public headers: include/gmpxx_mkII.h, include/mpfrxx_mkII.h, and include/gmpfrxx_mkII.h.
- Created private implementation headers under include/gmpfrxx_mkII/detail/.
- Added public namespaces gmpxx and mpfrxx.
- Added internal namespace gmpfrxx_mkII::detail.
- Added shell complete classes for gmpxx::mpz_class, gmpxx::mpq_class, gmpxx::mpf_class, mpfrxx::mpfr_class, and mpfrxx::mpc_class.
- Added mpfrxx::mpz_class and mpfrxx::mpq_class aliases to the gmpxx exact classes.
- Added CMake interface targets: gmpxx_mkII links GMP only, mpfrxx_mkII links GMP/MPFR/MPC, and gmpfrxx_mkII links GMP/MPFR/MPC.
- Added cmake/FindGMP.cmake, cmake/FindMPFR.cmake, and cmake/FindMPC.cmake.
- Added Phase 0 smoke, namespace, header-boundary, integer-model, and compile-fail tests.

Missing features:
- None for Phase 0.

Tests added:
- tests/test_gmp_header_smoke.cpp
- tests/test_mpfr_header_smoke.cpp
- tests/test_aggregator_header_smoke.cpp
- tests/test_namespace_aliases.cpp
- tests/test_header_boundaries.cpp
- tests/test_integer_model_diagnostics.cpp
- tests/compile_fail/mpfr_header_must_not_expose_mpf.cpp
- tests/compile_fail/mpfr_header_must_not_expose_mpfc.cpp
- tests/compile_fail/gmp_header_must_not_expose_mpfr_or_mpc.cpp

Exact commands run:
- git status --short
- rg --files
- sed -n '1,260p' STATUS.md
- rg --files include tests cmake CMakeLists.txt 2>/dev/null
- sed -n '1,260p' CMakeLists.txt
- find include tests cmake -maxdepth 4 -type f -print
- sed -n '140,235p' PROMPTS.md
- mkdir -p include/gmpfrxx_mkII/detail tests/compile_fail cmake
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "#include <gmpxx\\.h>" include tests CMakeLists.txt cmake
- git diff --stat

Pass/fail result:
- Initial CTest run failed because test_header_boundaries read source paths relative to the build directory.
- Fixed test_header_boundaries by passing GMPFRXX_MKII_SOURCE_DIR from CMake.
- Final cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 9/9 tests passed
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h>: PASS, no matches

Known issues:
- Phase 0 intentionally does not implement arithmetic operators, expression-template nodes, numeric storage, conversion, comparisons, streaming, math functions, environment handling, or real GMP/MPFR/MPC RAII behavior.
- gmpxx::mpf_class, mpfrxx::mpfr_class, and mpfrxx::mpc_class are shell classes only in this phase.

Phase 1 status:
DONE

Implemented features:
- Implemented mpfrxx::mpfr_class as an RAII wrapper around mpfr_t.
- Added copy construction, move construction, copy assignment, move assignment, and destruction for mpfrxx::mpfr_class.
- Added precision allocation through mpfr_init2().
- Kept raw precision construction out of the public constructor API; precision-specific objects are created with mpfr_class::with_precision(bits).
- Added with_precision(bits), with_precision(bits, double), set(double), to_double(), precision(), and mpfr_data() helpers for Phase 1 tests and evaluation.
- Added expression node types in include/gmpfrxx_mkII/detail/expr.hpp:
  - object_leaf
  - scalar_leaf
  - unary_expr
  - binary_expr
- Added operation tags:
  - add_op
  - sub_op
  - mul_op
  - div_op
  - neg_op
  - pos_op
- Added expression traits:
  - is_expression_node
  - is_expression_node_v
  - expression_result_type
  - expression_result_type_t
- Added MPFR expression operators for +, -, *, /, unary +, and unary -.
- Public MPFR arithmetic operators return expression nodes, not eager mpfrxx::mpfr_class objects.
- Added expression materialization through mpfr_class construction from expression.
- Added expression assignment into existing mpfr_class while preserving destination precision.
- New expression materialization uses the maximum precision of MPFR object leaves.
- Binary expression evaluation uses temporaries at the selected evaluation precision, which makes assignment aliasing safe.

Missing features:
- None for Phase 1.

Tests added:
- tests/test_et_contract_mpfr.cpp
- tests/test_mpfr_expression_eval.cpp
- tests/test_mpfr_precision_policy.cpp
- tests/test_mpfr_aliasing.cpp

Exact commands run:
- sed -n '1,220p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/eval_context.hpp
- sed -n '1,220p' tests/CMakeLists.txt
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+mpfrxx::mpfr_class\\s+operator|mpfr_class\\s+operator\\+|mpfr_class\\s+operator-|mpfr_class\\s+operator\\*|mpfr_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "#include <gmpxx\\.h>" include tests CMakeLists.txt cmake
- git diff --stat
- git status --short
- rg -n "mpfr_class\\s+[A-Za-z_][A-Za-z0-9_]*\\([0-9]|mpfr_class\\([0-9]" include tests
- rg -n "explicit mpfr_class\\(mpfr_prec_t|mpfr_class\\(mpfr_prec_t" include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- CMake configure: PASS
- Initial build: FAIL because mpfr_class(int) was ambiguous between precision construction and double value construction.
- Removed the direct double constructor and kept with_precision(bits, double) plus set(double) for Phase 1 value setup.
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 13/13 tests passed
- Public numeric precision constructor scan: PASS, no matches
- Eager MPFR operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h>: PASS, no matches

Known issues:
- Phase 1 intentionally does not implement scalar expression operands, bool/long double/__int128 rejection tests, MPFR environment variables, MPFR exponent-range handling, exact mpz/mpq wrappers beyond Phase 0 shells, MPC arithmetic, MPF arithmetic, comparisons, streaming, or math functions.
- mpfrxx::mpfr_class currently uses fixed Phase 1 defaults: 53-bit default precision and MPFR_RNDN rounding.
