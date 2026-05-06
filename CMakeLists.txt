cmake_minimum_required(VERSION 3.16)

project(gmpfrxx_mkII LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)

find_package(Git QUIET)
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)
find_package(MPC REQUIRED)

set(GMPFRXX_MKII_GIT_COMMIT_HASH "unknown")
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short=12 HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE GMPFRXX_MKII_GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gmpfrxx_mkII/detail/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/gmpfrxx_mkII/detail/version.hpp"
    @ONLY
)

add_library(gmpxx_mkII INTERFACE)
target_include_directories(gmpxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gmpxx_mkII INTERFACE GMP::GMP)
target_compile_features(gmpxx_mkII INTERFACE cxx_std_17)
add_library(gmpxx_mkII::gmpxx_mkII ALIAS gmpxx_mkII)

add_library(mpfrxx_mkII INTERFACE)
target_include_directories(mpfrxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(mpfrxx_mkII INTERFACE GMP::GMP MPFR::MPFR)
target_compile_features(mpfrxx_mkII INTERFACE cxx_std_17)
add_library(mpfrxx_mkII::mpfrxx_mkII ALIAS mpfrxx_mkII)

add_library(mpcxx_mkII INTERFACE)
target_include_directories(mpcxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(mpcxx_mkII INTERFACE mpfrxx_mkII::mpfrxx_mkII MPC::MPC)
target_compile_features(mpcxx_mkII INTERFACE cxx_std_17)
add_library(mpcxx_mkII::mpcxx_mkII ALIAS mpcxx_mkII)

add_library(gmpfrxx_mkII INTERFACE)
target_include_directories(gmpfrxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gmpfrxx_mkII INTERFACE
    gmpxx_mkII::gmpxx_mkII
    mpfrxx_mkII::mpfrxx_mkII
    mpcxx_mkII::mpcxx_mkII
)
target_compile_features(gmpfrxx_mkII INTERFACE cxx_std_17)
add_library(gmpfrxx_mkII::gmpfrxx_mkII ALIAS gmpfrxx_mkII)

add_subdirectory(examples)
add_subdirectory(benchmarks)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
