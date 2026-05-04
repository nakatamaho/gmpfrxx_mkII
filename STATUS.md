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
- mpfrxx::mpfr_class currently uses fixed Phase 1 defaults recorded at that time; Phase 3 replaces the precision default with a configurable 512-bit wrapper-owned default.

Phase 2 status:
DONE

Implemented features:
- Added MPFR scalar expression operands for signed and unsigned standard integral types except bool.
- Added MPFR scalar expression operands for float and double.
- Rejected bool, long double, __int128, and unsigned __int128 as MPFR expression scalar leaves.
- Normalized MPFR scalar leaves to ABI-stable storage:
  - signed integrals -> std::int64_t
  - unsigned integrals -> std::uint64_t
  - float and double -> double
- Preserved the scalar/scalar boundary: expression-template operators require at least one MPFR object or MPFR expression operand.
- Added minimal RAII storage and exact integral construction for gmpxx::mpz_class.
- Implemented integral-to-MPFR scalar evaluation through a gmpxx::mpz_class temporary and mpfr_set_z().
- Implemented float/double scalar evaluation through mpfr_set_d().
- Verified std::uint64_t max and std::int64_t min convert exactly without negating the signed minimum value.
- Ported the allocation-count test from the sibling gmpxx_mkII repository as tests/test_mpfr_alloc_count.cpp.
- Added allocation-aware MPFR expression evaluation for non-aliasing destinations:
  - simple leaf-only assignments such as dst = a + b allocate zero times
  - left-associated addition chains such as dst = a + b + c + d allocate zero times
  - compound two-sided expressions such as dst = (a + b) * (c + d) use one temporary mpfr_t
- Kept aliasing assignments on the safe temporary path.

Missing features:
- None for Phase 2.

Tests added:
- tests/test_et_contract_mpfr_scalar.cpp
- tests/test_mpfr_scalar_eval.cpp
- tests/test_mpfr_long_width_dispatch.cpp
- tests/test_mpfr_alloc_count.cpp
- tests/compile_fail/test_bool_scalar.cpp
- tests/compile_fail/test_long_double_scalar.cpp
- tests/compile_fail/test_int128_scalar.cpp
- tests/compile_fail/test_scalar_scalar_et_operator.cpp

Exact commands run:
- sed -n '1,260p' include/gmpfrxx_mkII/detail/integer_conversion.hpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,420p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,220p' tests/CMakeLists.txt
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+mpfrxx::mpfr_class\\s+operator|mpfr_class\\s+operator\\+|mpfr_class\\s+operator-|mpfr_class\\s+operator\\*|mpfr_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>" include tests CMakeLists.txt cmake
- git diff --stat
- sed -n '1,260p' ../gmpxx_mkII/tests/test_alloc_count.cpp
- rg -n "alloc_count|mp_set_memory_functions|allocation" ../gmpxx_mkII/tests ../gmpxx_mkII/include 2>/dev/null
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- cmake --build build -j && ctest --test-dir build --output-on-failure -R test_mpfr_alloc_count
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+mpfrxx::mpfr_class\\s+operator|mpfr_class\\s+operator\\+|mpfr_class\\s+operator-|mpfr_class\\s+operator\\*|mpfr_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- git diff --stat

Pass/fail result:
- CMake configure: PASS
- Initial build: FAIL because is_mpfr_object_or_node_v referenced result_type for non-expression operands during SFINAE substitution.
- Fixed is_mpfr_object_or_node with partial specializations instead of a variable-template boolean expression.
- Build after trait fix: PASS
- Initial source scan: FAIL because generic integer_conversion.hpp contained MPFR-specific trait names and is included by gmpxx_mkII.h.
- Moved MPFR scalar trait names into mpfr_impl.hpp, leaving integer_conversion.hpp generic.
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 20/20 tests passed
- Eager MPFR operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h>: PASS, no matches
- Initial migrated allocation-count test: FAIL because the first allocation-aware implementation evaluated both sides of (a + b) * (c + d) into separate temporary mpfr_t objects.
- Updated compound non-aliasing evaluation to evaluate the left side directly into the destination and only the right side into one temporary mpfr_t.
- Restored the migrated allocation-count expectation for (a + b) * (c + d) to 1; simple addition chains still assert zero allocations.
- Final ctest --test-dir build --output-on-failure after the migrated allocation-count test: PASS, 21/21 tests passed
- Final eager MPFR operator scan after allocation-aware evaluation: PASS, no matches
- Final source scan for forbidden GMP-header MPFR/MPC includes and symbols after allocation-aware evaluation: PASS, no matches

Known issues:
- Phase 2 intentionally does not implement direct scalar constructors for mpfrxx::mpfr_class.
- Phase 2 intentionally does not implement MPFR environment variables, rounding configuration beyond fixed MPFR_RNDN, exponent-range handling, mpz/mpq mixed expressions, MPC arithmetic, MPF arithmetic, comparisons, streaming, or math functions.

Phase 3 status:
DONE

Implemented features:
- Added MPFR defaults API in namespace mpfrxx:
  - mpfrxx::default_options()
  - mpfrxx::default_precision_bits()
  - mpfrxx::set_default_precision_bits()
  - mpfrxx::default_rounding_mode()
  - mpfrxx::set_default_rounding_mode()
  - mpfrxx::default_emin()
  - mpfrxx::default_emax()
  - mpfrxx::set_default_exponent_range()
  - mpfrxx::reload_mpfr_defaults_from_environment()
- Added MPFR environment loading from:
  - MPFRXX_DEFAULT_PRECISION_BITS
  - MPFRXX_EMIN
  - MPFRXX_EMAX
  - MPFRXX_ROUNDING_MODE
- Renamed the Phase 3 MPFR environment variables in AGENTS.md and PROMPTS.md from the previous long project-prefixed MPFR names to the shorter MPFRXX_* names.
- Set the initial wrapper-owned MPFR default precision to 512 bits.
- Documented that the initial wrapper-owned GMP mpf_class default precision is also 512 bits for the later MPF phase.
- Added rounding string parsing for RNDN, RNDZ, RNDU, RNDD, RNDA, RNDF and their MPFR_* spellings.
- Replaced fixed MPFR_RNDN evaluation with the current wrapper-owned MPFR rounding mode.
- Added MPFR exponent-range defaults and a guard that applies MPFR emin/emax during wrapper evaluation, then restores the previous MPFR global range.
- Extended eval_context to carry precision, rounding mode, emin, and emax.

Missing features:
- None for Phase 3.

Tests added:
- tests/test_mpfr_defaults.cpp
- tests/test_mpfr_environment.cpp
- tests/test_mpfr_environment_invalid.cpp

Exact commands run:
- rg -n for previous long project-prefixed MPFR env names and MPFRXX names in AGENTS.md PROMPTS.md STATUS.md docs include tests CMakeLists.txt cmake
- git status --short
- sed -n '1,220p' include/gmpfrxx_mkII/detail/environment.hpp
- sed -n '1,120p' include/gmpfrxx_mkII/detail/eval_context.hpp
- sed -n '1,540p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,90p' tests/CMakeLists.txt
- rg -n "default precision|DEFAULT_PRECISION|53|512|MPF_DEFAULT|MPFRXX_DEFAULT" AGENTS.md PROMPTS.md docs include tests STATUS.md
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+mpfrxx::mpfr_class\\s+operator|mpfr_class\\s+operator\\+|mpfr_class\\s+operator-|mpfr_class\\s+operator\\*|mpfr_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>" include tests CMakeLists.txt cmake
- rg -n for previous long project-prefixed MPFR env names and obsolete short rounding env spelling in AGENTS.md PROMPTS.md STATUS.md docs include tests CMakeLists.txt cmake

Pass/fail result:
- CMake configure: PASS
- cmake --build build -j: PASS
- ctest --test-dir build --output-on-failure: PASS, 24/24 tests passed
- Eager MPFR operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h>: PASS, no matches
- Source scan for old MPFR env var names and obsolete short rounding env spelling: PASS, no matches

Known issues:
- Phase 3 intentionally does not implement direct scalar constructors for mpfrxx::mpfr_class.
- Phase 3 intentionally does not implement mpz/mpq mixed expressions, MPC arithmetic, MPF arithmetic, comparisons, streaming, or math functions.
- MPFR exponent-range guards use MPFR's process-global exponent range during the guarded operation and restore the previous values afterward.

Schedule-change MPF status:
DONE

Implemented features:
- Implemented gmpxx::mpf_class as an RAII wrapper around mpf_t.
- Added wrapper-owned GMP MPF default precision API:
  - gmpxx::default_mpf_precision_bits()
  - gmpxx::set_default_mpf_precision_bits()
  - gmpxx::reload_default_mpf_precision_bits_from_environment()
- Set the initial wrapper-owned GMP MPF default precision to 512 bits.
- Added MPF default precision environment loading from MPFXX_DEFAULT_PREC_BITS.
- Used mpf_init2() for wrapper-owned MPF construction.
- Did not call mpf_set_default_prec().
- Added MPF expression-template operators for +, -, *, /, unary +, and unary -.
- Public MPF arithmetic operators return expression nodes, not eager gmpxx::mpf_class objects.
- Added MPF scalar expression operands for signed and unsigned standard integrals except bool, plus float and double.
- Normalized MPF scalar leaves to std::int64_t, std::uint64_t, or double.
- Evaluated integral MPF scalar leaves through gmpxx::mpz_class temporaries and mpf_set_z().
- New MPF expression materialization uses the maximum effective precision of MPF object leaves.
- Assignment into existing MPF objects preserves destination effective precision.
- Added alias-safe MPF expression assignment using temporaries when the destination appears in the source expression.
- Added compile-fail tests for forbidden MPF/MPFR and MPF/MPC mixed-family operations.

Missing features:
- Full mpz/mpq + mpf mixed exact promotion is not implemented yet.
- MPF environment variable handling is not implemented yet.

Tests added:
- tests/test_et_contract_mpf.cpp
- tests/test_mpf_basic.cpp
- tests/test_mpf_precision_policy.cpp
- tests/test_mpf_aliasing.cpp
- tests/compile_fail/test_mpf_plus_mpfr.cpp
- tests/compile_fail/test_mpfr_plus_mpf.cpp
- tests/compile_fail/test_mpf_plus_mpc.cpp
- tests/compile_fail/test_mpc_plus_mpf.cpp

Exact commands run:
- sed -n '1,220p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,540p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,120p' include/gmpxx_mkII.h
- sed -n '1,130p' tests/CMakeLists.txt
- git status --short
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- ./build/tests/test_mpf_precision_policy
- gdb -batch -ex run -ex bt --args ./build/tests/test_mpf_precision_policy
- nl -ba tests/test_mpf_precision_policy.cpp
- gdb -batch -ex 'break tests/test_mpf_precision_policy.cpp:13' -ex run -ex 'print default_value.precision()' --args ./build/tests/test_mpf_precision_policy
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+gmpxx::mpf_class\\s+operator|mpf_class\\s+operator\\+|mpf_class\\s+operator-|mpf_class\\s+operator\\*|mpf_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- git diff --stat

Pass/fail result:
- CMake configure: PASS
- Initial cmake --build build -j after MPF implementation: PASS
- Initial ctest --test-dir build --output-on-failure after MPF implementation: FAIL, test_mpf_precision_policy aborted.
- Root cause: GMP mpf_init2(160) reports an effective precision of 192 via mpf_get_prec() because GMP rounds precision to its internal limb allocation. This does not change the requested wrapper default, which is 512 unless set explicitly or loaded from MPFXX_DEFAULT_PREC_BITS.
- Fixed the MPF precision test to compare effective precision and destination preservation instead of requiring exact requested precision from mpf_get_prec().
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 32/32 tests passed
- Eager MPF operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPF scalar constructors are intentionally not implemented; value construction currently uses with_precision(bits, value) or set(value).
- GMP may report effective MPF precision greater than the requested value through mpf_get_prec().
- MPFXX_DEFAULT_PREC_BITS controls the requested wrapper default precision; mpf_get_prec() reports GMP's effective precision.
- mpz/mpq + mpf mixed-family promotion remains for the later exact-wrapper phase.

Phase 4 status:
DONE

Implemented features:
- Implemented gmpxx::mpq_class as an RAII wrapper around mpq_t.
- Extended gmpxx::mpz_class and gmpxx::mpq_class with construction and assignment from expression nodes.
- Added exact GMP-only expression-template arithmetic for mpz/mpq:
  - mpz + mpz -> gmpxx::mpz_class
  - mpz/mpq combinations -> gmpxx::mpq_class
  - mpq + mpq -> gmpxx::mpq_class
  - division produces gmpxx::mpq_class
- Added mpq canonicalization for construction and expression materialization.
- Preserved mpfrxx::mpz_class and mpfrxx::mpq_class as aliases to the gmpxx exact classes.
- Added MPFR mixed exact operand support:
  - mpz/mpq + mpfr -> mpfrxx::mpfr_class expression nodes
  - mpfr + mpz/mpq -> mpfrxx::mpfr_class expression nodes
- Exact mpz/mpq leaves do not contribute MPFR precision; mixed exact/MPFR materialization keeps the MPFR leaf precision.
- Kept exact mpz/mpq implementation in zq_impl.hpp, with no MPFR/MPC references in GMP-only headers.

Missing features:
- Stronger exact/MPFR mixed expression evaluation from Phase 5 is not implemented yet.
- mpz/mpq + mpf promotion remains to be completed with the GMP-only mixed MPF phase.

Tests added:
- tests/test_et_contract_zq_mpfr.cpp
- tests/test_mpz_basic.cpp
- tests/test_mpq_basic.cpp
- tests/test_mpq_canonicalization.cpp
- tests/test_mixed_zq_mpfr_promotion.cpp

Exact commands run:
- sed -n '1,220p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,540p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,120p' tests/CMakeLists.txt
- git status --short
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- gdb -batch -ex run -ex bt --args ./build/tests/test_mixed_zq_mpfr_promotion
- nl -ba tests/test_mixed_zq_mpfr_promotion.cpp
- cmake --build build -j && ctest --test-dir build --output-on-failure -R test_mixed_zq_mpfr_promotion
- ctest --test-dir build --output-on-failure
- rg -n for eager mpz/mpq/mpfr/mpf arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- git diff --stat

Pass/fail result:
- CMake configure: PASS
- cmake --build build -j: PASS
- Initial ctest --test-dir build --output-on-failure: FAIL, test_mixed_zq_mpfr_promotion aborted.
- Root cause: exact mpz/mpq leaves were initially counted as MPFR default-precision leaves, so z + r(160) materialized at 512 bits instead of preserving the MPFR leaf precision.
- Fixed exact mpz/mpq leaves to contribute zero MPFR precision in mixed exact/MPFR expressions.
- Focused mixed promotion test rerun: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 37/37 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- Phase 5 still needs to strengthen mixed exact/MPFR evaluation policy.
- Exact expression division by zero is not given a wrapper-specific diagnostic yet; it follows GMP behavior.

Phase 5 status:
DONE

Implemented features:
- Strengthened mixed exact/MPFR evaluation for exact-only subexpressions embedded in MPFR expressions.
- Exact mpz-result subexpressions are now evaluated with the GMP exact mpz evaluator and converted to MPFR once with mpfr_set_z().
- Exact mpq-result subexpressions are now evaluated with the GMP exact mpq evaluator and converted to MPFR once with mpfr_set_q().
- Kept public arithmetic expression-template based; no eager public mpz/mpq/mpfr/mpf arithmetic operators were added.
- Kept GMP-only headers free of MPFR/MPC includes and symbols.

Missing features:
- mpz/mpq + mpf promotion remains to be completed with the GMP-only mixed MPF phase.
- MPC arithmetic is still a later phase.
- Exact expression division by zero still follows GMP behavior without a wrapper-specific diagnostic.

Tests added:
- tests/test_mixed_zq_mpfr_exact_subexpression.cpp

Exact commands run:
- git status --short
- tail -120 STATUS.md
- rg -n "TODO|Phase 5|missing|mpf|mpq|mpz|mpfr_set_q|mpf_set_q|result_type|promot" include tests STATUS.md
- sed -n '1,620p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,470p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,220p' tests/test_mixed_zq_mpfr_promotion.cpp
- sed -n '1,180p' tests/CMakeLists.txt
- sed -n '1,220p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '1,220p' tests/test_et_contract_zq_mpfr.cpp
- rg -n "mpfr_evaluate\\(|mpq_evaluate\\(|mpz_evaluate\\(" include/gmpfrxx_mkII/detail tests
- temporary /tmp MPFR probe to find a compact exact-subexpression rounding regression
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_mixed_zq_mpfr_exact_subexpression
- rg -n for eager mpz/mpq/mpfr/mpf arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- ctest --test-dir build --output-on-failure
- git diff -- include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/CMakeLists.txt tests/test_mixed_zq_mpfr_exact_subexpression.cpp
- git status --short

Pass/fail result:
- cmake --build build -j: PASS
- New exact-subexpression regression test: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 38/38 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- mpz/mpq + mpf mixed-family promotion remains for the GMP-only mixed MPF phase.
- MPC arithmetic is not implemented yet.

Phase 6 status:
DONE

Implemented features:
- Implemented GMP-only mixed exact/MPF expression promotion.
- Added mpz/mpq object leaves as valid MPF expression operands.
- Added exact mpz/mpq expression leaves as valid MPF expression operands when at least one side of a binary expression is MPF.
- Added mpz/mpq + mpf, mpf + mpz/mpq, and exact-subexpression + mpf support for +, -, *, and /.
- Exact mpz leaves are converted to MPF through mpf_set_z().
- Exact mpq leaves and mpq-result subexpressions are converted by evaluating numerator and denominator as MPF values and dividing, because GMP MPF has no mpf_set_q().
- Exact mpz/mpq leaves do not contribute MPF precision; mixed exact/MPF materialization keeps the MPF leaf precision.
- Kept MPF/MPFR and MPF/MPC mixed-family operations forbidden.
- Kept public arithmetic expression-template based; no eager public mpz/mpq/mpf arithmetic operators were added.
- Kept GMP-only headers free of MPFR/MPC includes and symbols.

Missing features:
- MPC arithmetic is still a later phase.
- Exact expression division by zero still follows GMP behavior without a wrapper-specific diagnostic.
- MPQ-to-MPF conversion currently uses two temporary mpf_t values for numerator and denominator.

Tests added:
- tests/test_et_contract_zq_mpf.cpp
- tests/test_mixed_zq_mpf_promotion.cpp

Exact commands run:
- sed -n '1,620p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,260p' tests/test_et_contract_mpf.cpp
- sed -n '1,260p' tests/test_mpf_basic.cpp
- git status --short
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_et_contract_zq_mpf|test_mixed_zq_mpf_promotion"
- rg -n for eager mpz/mpq/mpfr/mpf arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j: PASS
- New mixed exact/MPF tests: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 40/40 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPC arithmetic is not implemented yet.
- MPQ-to-MPF conversion allocates temporary MPF numerator and denominator storage.

Phase 7 status:
DONE

Implemented features:
- Implemented mpfrxx::mpc_class as an MPC-backed RAII wrapper around mpc_t.
- Added copy construction, move construction, copy assignment, move assignment, and destruction for mpfrxx::mpc_class.
- Added with_precision(bits), with_precision(bits, real, imag), precision(), real_to_double(), imag_to_double(), and mpc_data() helpers.
- Added MPC expression-template arithmetic for +, -, *, /, unary +, and unary -.
- Added type promotion to mpc_class when at least one operand is an MPC object or MPC expression node.
- Added mixed MPC operands for mpfr_class, mpz_class, mpq_class, and supported scalar leaves.
- Converted MPFR operands through mpc_set_fr(), exact mpz operands through mpc_set_z(), and exact mpq operands through mpc_set_q().
- Kept mpfr + mpfr as MPFR arithmetic and exact-only arithmetic as exact arithmetic; MPC promotion requires an MPC operand.
- Kept MPF/MPFR and MPF/MPC mixed-family operations forbidden.
- Kept public arithmetic expression-template based; no eager public mpc/mpfr/mpf/mpz/mpq arithmetic operators were added.
- Kept GMP-only headers free of MPFR/MPC includes and symbols.

Missing features:
- MPC default precision and rounding environment variables are not implemented yet.
- MPC real/imag independent precision support is not implemented yet; Phase 7 uses same precision for both components.
- MPC math functions, comparisons, and streaming are not implemented yet.
- Exact expression division by zero still follows GMP behavior without a wrapper-specific diagnostic.

Tests added:
- tests/test_et_contract_mpc.cpp
- tests/test_mpc_basic.cpp
- tests/test_mpc_aliasing.cpp

Exact commands run:
- sed -n '1,260p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '1,140p' include/mpfrxx_mkII.h
- sed -n '1,180p' tests/CMakeLists.txt
- tail -90 STATUS.md
- git status --short
- rg -n "mpc_(init|set|add|sub|mul|div|real|imag|get|clear|swap|neg|conj|set_.*fr|set_.*q|set_.*z)" /usr/include /usr/local/include
- sed -n '1,220p' tests/test_aggregator_header_smoke.cpp
- sed -n '1,80p' include/gmpfrxx_mkII.h
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_et_contract_mpc|test_mpc_basic|test_mpc_aliasing"
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_et_contract_mpc|test_mpc_basic|test_mpc_aliasing"
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial cmake --build build -j: PASS
- Initial new MPC tests: FAIL, test_mpc_aliasing expected the wrong imaginary result.
- Fixed test_mpc_aliasing expectation for (1+2i)*(3-i)+(1+2i) from 10i to 7i.
- Final cmake --build build -j: PASS
- Final new MPC tests: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 43/43 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPC environment variables remain for a later phase.
- MPC uses MPC_RNDNN directly in Phase 7.
- MPC real and imaginary component precision are currently the same.

Phase 8 status:
DONE

Implemented features:
- Renamed planned MPC environment variables from long GMPFRXX_MKII_MPC_* names to shorter MPFRXX_MPC_* names in AGENTS.md and PROMPTS.md:
  - MPFRXX_MPC_DEFAULT_PRECISION_BITS
  - MPFRXX_MPC_REAL_PRECISION_BITS
  - MPFRXX_MPC_IMAG_PRECISION_BITS
  - MPFRXX_MPC_ROUNDING_MODE
  - MPFRXX_MPC_REAL_ROUNDING_MODE
  - MPFRXX_MPC_IMAG_ROUNDING_MODE
- Added MPC default options API in namespace mpfrxx:
  - mpfrxx::default_mpc_options()
  - mpfrxx::default_mpc_precision_bits()
  - mpfrxx::default_mpc_real_precision_bits()
  - mpfrxx::default_mpc_imag_precision_bits()
  - mpfrxx::set_default_mpc_precision_bits(bits)
  - mpfrxx::set_default_mpc_precision_bits(real_bits, imag_bits)
  - mpfrxx::default_mpc_rounding_mode()
  - mpfrxx::default_mpc_real_rounding_mode()
  - mpfrxx::default_mpc_imag_rounding_mode()
  - mpfrxx::set_default_mpc_rounding_mode(rounding)
  - mpfrxx::set_default_mpc_rounding_mode(real_rounding, imag_rounding)
  - mpfrxx::reload_mpc_defaults_from_environment()
- MPC defaults inherit current MPFR defaults unless MPFRXX_MPC_* variables are set.
- MPC default precision can be configured independently for real and imaginary components.
- MPC default rounding can be configured independently for real and imaginary components.
- Updated mpc_class construction and expression materialization to use mpc_init3(real_precision, imag_precision).
- Added mpc_class::real_precision() and mpc_class::imag_precision().
- Existing-object MPC expression assignment preserves destination real and imaginary precision.
- New MPC expression materialization uses max real and max imaginary precision across leaves.

Missing features:
- MPC math functions, comparisons, and streaming are not implemented yet.
- Exact expression division by zero still follows GMP behavior without a wrapper-specific diagnostic.

Tests added:
- tests/test_mpc_defaults.cpp
- tests/test_mpc_environment.cpp
- tests/test_mpc_environment_invalid.cpp
- tests/test_mpc_precision_policy.cpp

Exact commands run:
- rg -n "#define MPC_RND|MPC_RND\\(|MPC_RND_RE|MPC_RND_IM|typedef.*mpc_rnd_t|mpc_rnd_t" /usr/include/mpc.h
- sed -n '1,260p' include/gmpfrxx_mkII/detail/environment.hpp
- sed -n '1,180p' tests/test_mpfr_environment.cpp
- sed -n '1,620p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- git status --short
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpc_defaults|test_mpc_environment|test_mpc_environment_invalid|test_mpc_precision_policy|test_mpc_basic|test_mpc_aliasing"
- rg -n "GMPFRXX_MKII_MPC|MPFRXX_MPC" AGENTS.md PROMPTS.md STATUS.md include tests docs README.md
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpc_defaults|test_mpc_environment|test_mpc_environment_invalid|test_mpc_precision_policy|test_mpc_basic|test_mpc_aliasing"
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- ctest --test-dir build --output-on-failure
- rg -n "GMPFRXX_MKII_MPC" AGENTS.md PROMPTS.md STATUS.md include tests docs README.md
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake

Pass/fail result:
- Initial cmake --build build -j: PASS
- Initial new/affected MPC tests: FAIL, test_mpc_environment_invalid expected precision "1" to be invalid, but MPFR_PREC_MIN is 1 in this environment.
- Fixed invalid precision test value from "1" to "0".
- Final cmake --build build -j: PASS
- Final new/affected MPC tests: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 47/47 tests passed
- Old long MPC environment variable scan: PASS, no matches in tracked files scanned
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPC math functions, comparisons, and streaming remain for later phases.
- mpc_class::precision() returns the common precision when real and imaginary precision match, otherwise the smaller component precision; use real_precision() and imag_precision() for exact component values.

Phase 9 status:
DONE

Implemented features:
- Added gmpxx::mpf_class construction from decimal strings with explicit precision:
  - mpf_class(const char*, mp_bitcnt_t)
  - mpf_class(const std::string&, mp_bitcnt_t)
- Added gmpxx::mpf_class::set(const char*) and set(const std::string&).
- Added gmpxx::mpf_class::get_str() and get_str(exp, base, digits) for decimal/string formatting.
- Added gmpxx::mpf_class::set_str(const char*, base) and set_str(const std::string&, base), preserving the destination value when parsing fails.
- Added std::ostream output for gmpxx::mpf_class using gmp_asprintf() and basic ostream formatting flags.
- Added internal GMP-allocated string RAII cleanup for strings returned by GMP formatting functions.
- Added examples/example01.cpp demonstrating explicit-precision MPF construction, expression-template addition, and stream output.
- Added examples/CMakeLists.txt and wired examples into the top-level CMake build.

Missing features:
- MPF stream input is not implemented yet.
- Stream output does not yet fully emulate every gmpxx.h formatting edge case.
- String constructors are currently MPF-only; mpz/mpq/mpfr/mpc string constructors remain for later phases.

Tests added:
- tests/test_mpf_string_io.cpp
- CTest entry for example01

Exact commands run:
- sed -n '1,190p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,220p' CMakeLists.txt
- git status --short
- sed -n '1,120p' README.md
- rg -n "Copyright|All rights reserved|Redistribution" tests include docs README.md | head -40
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpf_string_io|example01"
- ./build/examples/example01
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests examples
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples CMakeLists.txt cmake
- ctest --test-dir build --output-on-failure
- rg -n "mpf_class|mpf_get_str|mpf_set_str|operator<<|get_str|void set\\(|set_str|string" ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '617,1015p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '2280,2675p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '4180,4235p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpf_string_io|example01"
- ./build/examples/example01
- nm -C build/examples/example01 | rg "gmp_allocated_string|set_str|get_str|to_string|operator<<|gmp_asprintf|__gmpf_set_str|__gmpf_get_str"
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests examples
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples CMakeLists.txt cmake

Pass/fail result:
- cmake --build build -j: PASS
- New MPF string I/O and example tests: PASS
- example01 output: 1.5 + 2.5 = 4
- Final ctest --test-dir build --output-on-failure: PASS, 49/49 tests passed
- Final ctest after gmpxx_mkII.h.in refinement: PASS, 49/49 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPF output formatting now covers basic ostream precision, fixed/scientific, uppercase, showpoint, showpos, width, fill, and alignment flags, but is still not a complete gmpxx.h stream-format clone.
