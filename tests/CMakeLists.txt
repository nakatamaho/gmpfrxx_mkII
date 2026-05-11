function(add_phase0_test name target)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE ${target})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_phase0_test(test_gmp_header_smoke gmpxx_mkII)
add_phase0_test(test_mpfr_header_smoke mpfrxx_mkII)
add_phase0_test(test_mpc_header_smoke mpcxx_mkII)
add_phase0_test(test_aggregator_header_smoke gmpfrxx_mkII)
add_phase0_test(test_namespace_aliases mpfrxx_mkII)
add_phase0_test(test_header_boundaries gmpfrxx_mkII)
target_compile_definitions(test_header_boundaries PRIVATE
    GMPFRXX_MKII_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)
add_phase0_test(test_library_tls_detection gmpfrxx_mkII)
add_phase0_test(test_version_info gmpfrxx_mkII)
add_phase0_test(test_integer_model_diagnostics gmpxx_mkII)
add_phase0_test(test_gmpxx_mkII gmpxx_mkII)
add_phase0_test(test_mpfrxx_mkII mpcxx_mkII)
add_phase0_test(test_abi_fingerprint gmpfrxx_mkII)
add_phase0_test(test_alias_safety gmpfrxx_mkII)
add_phase0_test(test_temporary_expression_lifetime gmpfrxx_mkII)
add_phase0_test(test_comparisons gmpxx_mkII)
add_phase0_test(test_compound_assign gmpxx_mkII)
add_phase0_test(test_common_type gmpfrxx_mkII)
add_phase0_test(test_exception_support gmpxx_mkII)
add_phase0_test(test_random gmpxx_mkII)
add_phase0_test(test_type_conversions gmpxx_mkII)
add_phase0_test(test_unary_minus_simplification gmpfrxx_mkII)
add_phase0_test(test_user_defined_literals gmpxx_mkII)
add_phase0_test(test_et_contract_mpfr mpfrxx_mkII)
add_phase0_test(test_mpfr_user_defined_literals mpcxx_mkII)
add_phase0_test(test_mpfr_expression_eval mpfrxx_mkII)
add_phase0_test(test_mpfr_precision_policy mpfrxx_mkII)
add_phase0_test(test_mpfr_aliasing mpfrxx_mkII)
add_phase0_test(test_mpfr_random mpfrxx_mkII)
add_phase0_test(test_et_contract_mpfr_scalar mpfrxx_mkII)
add_phase0_test(test_mpfr_scalar_eval mpfrxx_mkII)
add_phase0_test(test_mpfr_long_width_dispatch mpfrxx_mkII)
add_phase0_test(test_mpfr_alloc_count mpfrxx_mkII)
set_tests_properties(test_mpfr_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mpfr_fixed_precision_materialization mpfrxx_mkII)
target_compile_definitions(test_mpfr_fixed_precision_materialization PRIVATE
    GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
    MPFRXX_ENABLE_FMA
)
add_phase0_test(test_mpfr_fixed_precision_fma_alloc_count mpfrxx_mkII)
target_compile_definitions(test_mpfr_fixed_precision_fma_alloc_count PRIVATE
    GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
    MPFRXX_ENABLE_FMA
)
set_tests_properties(test_mpfr_fixed_precision_fma_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mpfr_type_conversions mpfrxx_mkII)
add_phase0_test(test_mpfr_comparisons mpfrxx_mkII)
add_phase0_test(test_mpfr_compound_assign mpfrxx_mkII)
target_compile_definitions(test_mpfr_compound_assign PRIVATE
    MPFRXX_ENABLE_FMA
)
add_phase0_test(test_mpfr_rounding_scope mpfrxx_mkII)
add_executable(test_mpfr_rounding_scope_stable test_mpfr_rounding_scope.cpp)
target_link_libraries(test_mpfr_rounding_scope_stable PRIVATE mpfrxx_mkII)
target_compile_definitions(test_mpfr_rounding_scope_stable PRIVATE
    GMPFRXX_MKII_ASSUME_STABLE_MPFR_ROUNDING_MODE
)
add_test(NAME test_mpfr_rounding_scope_stable COMMAND test_mpfr_rounding_scope_stable)
add_phase0_test(test_mpfr_exception_support mpcxx_mkII)
add_phase0_test(test_mpfr_defaults mpfrxx_mkII)
add_phase0_test(test_mpfr_initialization_sentinel mpfrxx_mkII)
add_phase0_test(test_mpfr_environment mpfrxx_mkII)
add_phase0_test(test_mpfr_environment_invalid mpfrxx_mkII)
add_phase0_test(test_mpfr_string_io mpfrxx_mkII)
add_phase0_test(test_mpfr_math mpfrxx_mkII)
add_phase0_test(test_mpfr_transcendent_functions mpfrxx_mkII)
add_phase0_test(test_et_contract_mpf gmpxx_mkII)
add_phase0_test(test_mpf_basic gmpxx_mkII)
add_phase0_test(test_mpf_precision_policy gmpxx_mkII)
add_phase0_test(test_mpf_default_precision_env gmpxx_mkII)
set_tests_properties(test_mpf_default_precision_env PROPERTIES
    ENVIRONMENT "GMPXX_MKII_DEFAULT_MPF_PREC_BITS=1024"
)
add_executable(test_mpf_default_precision_invalid_env test_mpf_default_precision_invalid_env.cpp)
target_link_libraries(test_mpf_default_precision_invalid_env PRIVATE gmpxx_mkII)
add_test(
    NAME test_mpf_default_precision_invalid_env
    COMMAND "${CMAKE_COMMAND}"
        -DTEST_EXECUTABLE=$<TARGET_FILE:test_mpf_default_precision_invalid_env>
        -DTEST_ENVIRONMENT=GMPXX_MKII_DEFAULT_MPF_PREC_BITS=abc
        -P "${CMAKE_CURRENT_SOURCE_DIR}/expect_process_failure.cmake"
)
add_phase0_test(test_mpf_external_provider gmpxx_mkII_default_context_provider)
add_phase0_test(test_mpf_aliasing gmpxx_mkII)
add_phase0_test(test_mpf_string_io gmpxx_mkII)
add_phase0_test(test_mpf_alloc_count gmpxx_mkII)
set_tests_properties(test_mpf_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mpf_scalar_alloc_count gmpxx_mkII)
set_tests_properties(test_mpf_scalar_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mpf_fixed_precision_materialization gmpxx_mkII)
target_compile_definitions(test_mpf_fixed_precision_materialization PRIVATE
    GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
)
add_phase0_test(test_mpf_fixed_precision_tls_scratch gmpxx_mkII)
target_compile_definitions(test_mpf_fixed_precision_tls_scratch PRIVATE
    GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
)
set_tests_properties(test_mpf_fixed_precision_tls_scratch PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mpf_long_width_dispatch gmpxx_mkII)
add_phase0_test(test_mpf_numeric_equivalence gmpxx_mkII)
add_phase0_test(test_mpf_power_of_two_fusion gmpxx_mkII)
add_phase0_test(test_mpf_thread_safety gmpxx_mkII)
add_phase0_test(test_mpf_pi gmpxx_mkII)
add_phase0_test(test_mpf_transcendent_functions gmpxx_mkII)
add_phase0_test(test_mpf_math_functions gmpxx_mkII)
add_phase0_test(test_mpf_extended_transcendent_functions gmpxx_mkII)
add_phase0_test(test_construction_copy gmpfrxx_mkII)
add_phase0_test(test_et_contract_mpfc gmpxx_mkII)
add_phase0_test(test_mpfc_basic gmpxx_mkII)
add_phase0_test(test_mpfc_precision_policy gmpxx_mkII)
set_tests_properties(test_mpfc_precision_policy PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mpfc_header_boundary gmpxx_mkII)
add_phase0_test(test_mpfc_math gmpxx_mkII)
add_phase0_test(test_mpfc_io gmpxx_mkII)
add_phase0_test(test_et_contract_zq_mpf gmpxx_mkII)
add_phase0_test(test_mixed_zq_mpf_promotion gmpxx_mkII)
add_phase0_test(test_mixed_type_arithmetic gmpfrxx_mkII)
add_phase0_test(test_et_contract_zq_mpfr mpfrxx_mkII)
add_phase0_test(test_mpz_basic gmpxx_mkII)
add_phase0_test(test_mpz_arithmetic gmpxx_mkII)
add_phase0_test(test_mpz_addmul_fusion mpfrxx_mkII)
add_phase0_test(test_mpz_addmul_alloc_count mpfrxx_mkII)
set_tests_properties(test_mpz_addmul_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mpq_basic gmpxx_mkII)
add_phase0_test(test_mpq_arithmetic gmpxx_mkII)
add_phase0_test(test_mpq_canonicalization gmpxx_mkII)
add_phase0_test(test_zq_string_io gmpxx_mkII)
add_phase0_test(test_mpz_mpq_alloc_count mpfrxx_mkII)
set_tests_properties(test_mpz_mpq_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mixed_zq_mpfr_promotion mpfrxx_mkII)
add_phase0_test(test_mixed_zq_mpfr_exact_subexpression mpfrxx_mkII)
add_phase0_test(test_mpfr_numeric_equivalence mpfrxx_mkII)
add_phase0_test(test_mpfr_power_of_two_fusion mpfrxx_mkII)
add_phase0_test(test_mpfr_scalar_alloc_count mpfrxx_mkII)
set_tests_properties(test_mpfr_scalar_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)
add_phase0_test(test_mpfr_thread_safety mpfrxx_mkII)
add_phase0_test(test_et_contract_mpc mpcxx_mkII)
add_phase0_test(test_mpc_basic mpcxx_mkII)
add_phase0_test(test_mpc_aliasing mpcxx_mkII)
add_phase0_test(test_mpc_defaults mpcxx_mkII)
add_phase0_test(test_mpc_environment mpcxx_mkII)
add_phase0_test(test_mpc_environment_invalid mpcxx_mkII)
add_phase0_test(test_mpc_precision_policy mpcxx_mkII)
add_phase0_test(test_mpc_io mpcxx_mkII)
add_phase0_test(test_mpc_math mpcxx_mkII)
add_phase0_test(test_mpc_alloc_count mpcxx_mkII)
target_compile_definitions(test_mpc_alloc_count PRIVATE
    GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
)
set_tests_properties(test_mpc_alloc_count PROPERTIES RUN_SERIAL TRUE LABELS alloc-count)

function(add_compile_fail_test name target)
    set(source "${CMAKE_CURRENT_SOURCE_DIR}/compile_fail/${name}.cpp")
    set(output "${CMAKE_CURRENT_BINARY_DIR}/${name}.o")
    add_test(
        NAME compile_fail_${name}
        COMMAND ${CMAKE_CXX_COMPILER}
            -std=c++17
            -I${CMAKE_SOURCE_DIR}/include
            -I${GMP_INCLUDE_DIR}
            -I${MPFR_INCLUDE_DIR}
            -I${MPC_INCLUDE_DIR}
            -c "${source}"
            -o "${output}"
    )
    set_tests_properties(compile_fail_${name} PROPERTIES WILL_FAIL TRUE)
endfunction()

add_compile_fail_test(mpfr_header_must_not_expose_mpf mpfrxx_mkII)
add_compile_fail_test(mpfr_header_must_not_expose_mpfc mpfrxx_mkII)
add_compile_fail_test(mpfr_header_must_not_expose_mpc mpfrxx_mkII)
add_compile_fail_test(mpc_header_requires_mpfr_first mpcxx_mkII)
add_compile_fail_test(gmp_header_must_not_expose_mpfr_or_mpc gmpxx_mkII)
add_compile_fail_test(test_bool_scalar mpfrxx_mkII)
add_compile_fail_test(test_long_double_scalar mpfrxx_mkII)
add_compile_fail_test(test_int128_scalar mpfrxx_mkII)
add_compile_fail_test(test_scalar_scalar_et_operator mpfrxx_mkII)
add_compile_fail_test(test_mpf_plus_mpfr gmpfrxx_mkII)
add_compile_fail_test(test_mpfr_plus_mpf gmpfrxx_mkII)
add_compile_fail_test(test_mpf_plus_mpc gmpfrxx_mkII)
add_compile_fail_test(test_mpc_plus_mpf gmpfrxx_mkII)
add_compile_fail_test(test_mpfc_plus_mpfr gmpfrxx_mkII)
add_compile_fail_test(test_mpfr_plus_mpfc gmpfrxx_mkII)
add_compile_fail_test(test_mpfc_plus_mpc gmpfrxx_mkII)
add_compile_fail_test(test_mpc_plus_mpfc gmpfrxx_mkII)

add_test(
    NAME link_fail_mpf_external_provider_missing
    COMMAND ${CMAKE_CXX_COMPILER}
        -std=c++17
        -DGMPXX_MKII_DEFAULT_CONTEXT_MODE=GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER
        -I${CMAKE_SOURCE_DIR}/include
        -I${CMAKE_BINARY_DIR}/generated
        -I${GMP_INCLUDE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/test_mpf_external_provider_missing.cpp"
        ${GMP_LIBRARY}
        -o "${CMAKE_CURRENT_BINARY_DIR}/test_mpf_external_provider_missing"
)
set_tests_properties(link_fail_mpf_external_provider_missing PROPERTIES WILL_FAIL TRUE)
