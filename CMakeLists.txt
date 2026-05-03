cmake_minimum_required(VERSION 3.16)

project(gmpfrxx_mkII LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

find_path(GMP_INCLUDE_DIR NAMES gmp.h REQUIRED)
find_library(GMP_LIBRARY NAMES gmp REQUIRED)

find_path(MPFR_INCLUDE_DIR NAMES mpfr.h REQUIRED)
find_library(MPFR_LIBRARY NAMES mpfr REQUIRED)

add_library(gmpxx_mkII INTERFACE)
target_include_directories(gmpxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${GMP_INCLUDE_DIR}
)
target_link_libraries(gmpxx_mkII INTERFACE ${GMP_LIBRARY})
target_compile_features(gmpxx_mkII INTERFACE cxx_std_17)

add_library(mpfrxx_mkII INTERFACE)
target_include_directories(mpfrxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${GMP_INCLUDE_DIR}
    ${MPFR_INCLUDE_DIR}
)
target_link_libraries(mpfrxx_mkII INTERFACE ${GMP_LIBRARY} ${MPFR_LIBRARY})
target_compile_features(mpfrxx_mkII INTERFACE cxx_std_17)

add_library(gmpfrxx_mkII INTERFACE)
target_include_directories(gmpfrxx_mkII INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${GMP_INCLUDE_DIR}
    ${MPFR_INCLUDE_DIR}
)
target_link_libraries(gmpfrxx_mkII INTERFACE ${GMP_LIBRARY} ${MPFR_LIBRARY})
target_compile_features(gmpfrxx_mkII INTERFACE cxx_std_17)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
