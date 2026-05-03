cmake_minimum_required(VERSION 3.16)

project(gmpfrxx_mkII LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)

find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)
find_package(MPC REQUIRED)

add_library(gmpxx_mkII INTERFACE)
target_include_directories(gmpxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gmpxx_mkII INTERFACE GMP::GMP)
target_compile_features(gmpxx_mkII INTERFACE cxx_std_17)

add_library(mpfrxx_mkII INTERFACE)
target_include_directories(mpfrxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(mpfrxx_mkII INTERFACE GMP::GMP MPFR::MPFR MPC::MPC)
target_compile_features(mpfrxx_mkII INTERFACE cxx_std_17)

add_library(gmpfrxx_mkII INTERFACE)
target_include_directories(gmpfrxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gmpfrxx_mkII INTERFACE GMP::GMP MPFR::MPFR MPC::MPC)
target_compile_features(gmpfrxx_mkII INTERFACE cxx_std_17)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
