cmake_minimum_required(VERSION 3.16)
project(gmpfrxx_mkII_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find GMP and MPFR
find_library(GMP_LIB gmp REQUIRED)
find_library(MPFR_LIB mpfr REQUIRED)
find_path(GMP_INCLUDE gmp.h REQUIRED)
find_path(MPFR_INCLUDE mpfr.h REQUIRED)

# Header-only "library"
add_library(gmpfrxx_mkII INTERFACE)
target_include_directories(gmpfrxx_mkII INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${GMP_INCLUDE} ${MPFR_INCLUDE})
target_link_libraries(gmpfrxx_mkII INTERFACE ${MPFR_LIB} ${GMP_LIB})

enable_testing()

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
