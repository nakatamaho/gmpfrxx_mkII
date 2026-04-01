set(TEST_NAMES
    test_construction
    test_arithmetic
    test_mixed_scalar
    test_compound
    test_precision
)

foreach(t IN LISTS TEST_NAMES)
    add_executable(${t} ${t}.cpp)
    target_link_libraries(${t} PRIVATE gmpfrxx_mkII)
    add_test(NAME ${t} COMMAND ${t})
endforeach()
