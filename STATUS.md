Prompt 0 status:
DONE
Completed gates:
- CMake configures successfully.
- Tests build successfully.
- CTest passes.
- No public arithmetic operators exist.
- No expression-template implementation exists.
- mpf_class uses mpf_init2() and wrapper-owned thread-local precision.
- mpf_class does not call mpf_set_default_prec().
- mpfr_class uses mpfr_init2().
- mpfr_class stores only mpfr_t.
- mpfr_class does not store precision, rounding mode, emin, or emax as members.
- MPFR rounding mode is accessible through thread-local helpers.
- MPFR emin and emax are accessible through thread-local helpers.
- current_eval_context() exposes rnd, emin, and emax.
- Scoped guards restore previous values.
- Thread tests prove independent precision, rounding, and exponent-range settings.
- STATUS.md records completed and missing gates.
Missing gates:
- None.
Commands run:
- pwd && rg --files
- git status --short
- sed -n '1,240p' AGENTS.md
- sed -n '1,220p' docs/precision_policy.md
- sed -n '1,220p' docs/header_boundary.md
- mkdir -p include/gmpfrxx_mkII/detail tests
- cmake -S . -B build
- rg -n "operator\\+|operator-|operator\\*|operator/|expr.hpp|expression template|mpz_class|mpq_class|mpfc_class|mpc_class|to_string|std::ostream|mpf_set_default_prec|mpfr_set_emin|mpfr_set_emax" CMakeLists.txt include tests STATUS.md
- cmake --build build
- ctest --test-dir build --output-on-failure
- rg -n "mpf_init2|mpfr_init2|mpf_set_default_prec|mpfr_set_emin|mpfr_set_emax|mpfr_t value_|mpfr_prec_t .*;|mpfr_rnd_t .*;|mpfr_exp_t .*;" include tests STATUS.md
- rg --files include tests | sort
- git diff -- CMakeLists.txt include tests STATUS.md
- ctest --test-dir build --output-on-failure
- git status --short
- git diff --stat -- CMakeLists.txt include tests STATUS.md
- rm -rf build
Known limitations:
- Prompt 0 intentionally does not implement arithmetic operators, expression templates, comparisons, streaming, string conversion, math functions, or integer/rational/complex wrapper classes.
- MPFR helper names keep the required Prompt 0 default_* API, but the stored values are wrapper-owned thread-local construction/evaluation settings.

Prompt 1 status:
PARTIAL
Completed gates:
- Prompt 1 specification has been added to PROMPTS.md.
- double constructors and with_prec(bits, double) have been added for mpf_class and mpfr_class.
- Minimal same-family binary expression nodes have been added for +, -, *, and /.
- Tests for Prompt 1 have been added.
Missing gates:
- CMake configure has not been rerun after Prompt 1.
- Test build has not been rerun after Prompt 1.
- CTest has not been rerun after Prompt 1.
- Scope scans have not been run after Prompt 1.
Commands run:
- rg --files include tests | sort
- git status --short
- apply_patch for Prompt 1 expression implementation and tests
- apply_patch for PROMPTS.md and STATUS.md
Known limitations:
- Prompt 1 intentionally does not implement unary operators, compound assignment, comparisons, streaming, string conversion, math functions, scalar promotion, mixed-family expressions, nested expression evaluation, or integer/rational/complex wrapper classes.
