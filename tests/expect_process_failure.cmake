if(NOT DEFINED TEST_EXECUTABLE)
    message(FATAL_ERROR "TEST_EXECUTABLE is required")
endif()

if(NOT DEFINED TEST_ENVIRONMENT)
    set(TEST_ENVIRONMENT "")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env ${TEST_ENVIRONMENT} "${TEST_EXECUTABLE}"
    RESULT_VARIABLE result
)

if(result EQUAL 0)
    message(FATAL_ERROR "process unexpectedly succeeded: ${TEST_EXECUTABLE}")
endif()
