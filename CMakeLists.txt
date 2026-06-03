cmake_minimum_required(VERSION 3.16)

project(gmpfrxx_mkII VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)
include(CheckCXXSourceCompiles)

find_package(Git QUIET)
find_program(XZ_EXECUTABLE xz)
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)
find_package(MPC REQUIRED)

set(GMPFRXX_MKII_MPFR_HAS_BUILDOPT_TLS_P 0)
set(GMPFRXX_MKII_MPFR_BUILDOPT_TLS 0)
set(GMPFRXX_MKII_MPC_HAS_BUILDOPT_TLS_P 0)
set(GMPFRXX_MKII_MPC_BUILDOPT_TLS 0)

set(CMAKE_REQUIRED_INCLUDES "${MPFR_INCLUDE_DIR}")
set(CMAKE_REQUIRED_LIBRARIES "${MPFR_LIBRARY}")
check_cxx_source_compiles("
#include <mpfr.h>
int main() { return mpfr_buildopt_tls_p() ? 0 : 1; }
" GMPFRXX_MKII_HAVE_MPFR_BUILDOPT_TLS_P)
unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)

if(GMPFRXX_MKII_HAVE_MPFR_BUILDOPT_TLS_P)
    set(GMPFRXX_MKII_MPFR_HAS_BUILDOPT_TLS_P 1)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cmake_checks")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cmake_checks/mpfr_tls.cpp" "
#include <mpfr.h>
int main() { return mpfr_buildopt_tls_p() ? 0 : 1; }
")
    try_run(
        GMPFRXX_MKII_MPFR_TLS_RUN_RESULT
        GMPFRXX_MKII_MPFR_TLS_COMPILE_RESULT
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_checks"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_checks/mpfr_tls.cpp"
        CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${MPFR_INCLUDE_DIR}"
        LINK_LIBRARIES "${MPFR_LIBRARY}"
    )
    if(GMPFRXX_MKII_MPFR_TLS_COMPILE_RESULT AND GMPFRXX_MKII_MPFR_TLS_RUN_RESULT EQUAL 0)
        set(GMPFRXX_MKII_MPFR_BUILDOPT_TLS 1)
    endif()
endif()

set(CMAKE_REQUIRED_INCLUDES "${MPC_INCLUDE_DIR};${MPFR_INCLUDE_DIR};${GMP_INCLUDE_DIR}")
set(CMAKE_REQUIRED_LIBRARIES "${MPC_LIBRARY};${MPFR_LIBRARY};${GMP_LIBRARY}")
check_cxx_source_compiles("
#include <mpc.h>
int main() { return mpc_buildopt_tls_p() ? 0 : 1; }
" GMPFRXX_MKII_HAVE_MPC_BUILDOPT_TLS_P)
unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)

if(GMPFRXX_MKII_HAVE_MPC_BUILDOPT_TLS_P)
    set(GMPFRXX_MKII_MPC_HAS_BUILDOPT_TLS_P 1)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cmake_checks")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cmake_checks/mpc_tls.cpp" "
#include <mpc.h>
int main() { return mpc_buildopt_tls_p() ? 0 : 1; }
")
    try_run(
        GMPFRXX_MKII_MPC_TLS_RUN_RESULT
        GMPFRXX_MKII_MPC_TLS_COMPILE_RESULT
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_checks"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_checks/mpc_tls.cpp"
        CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${MPC_INCLUDE_DIR};${MPFR_INCLUDE_DIR};${GMP_INCLUDE_DIR}"
        LINK_LIBRARIES "${MPC_LIBRARY};${MPFR_LIBRARY};${GMP_LIBRARY}"
    )
    if(GMPFRXX_MKII_MPC_TLS_COMPILE_RESULT AND GMPFRXX_MKII_MPC_TLS_RUN_RESULT EQUAL 0)
        set(GMPFRXX_MKII_MPC_BUILDOPT_TLS 1)
    endif()
endif()

message(STATUS "MPFR buildopt TLS probe: has API=${GMPFRXX_MKII_MPFR_HAS_BUILDOPT_TLS_P}, tls=${GMPFRXX_MKII_MPFR_BUILDOPT_TLS}")
message(STATUS "MPC buildopt TLS probe: has API=${GMPFRXX_MKII_MPC_HAS_BUILDOPT_TLS_P}, tls=${GMPFRXX_MKII_MPC_BUILDOPT_TLS}")

if(NOT GMPFRXX_MKII_MPFR_HAS_BUILDOPT_TLS_P)
    message(FATAL_ERROR
        "MPFR must provide mpfr_buildopt_tls_p(). "
        "gmpfrxx_mkII uses libmpfr's thread-local default precision, "
        "rounding mode, and exponent range as the MPFR default-state owner."
    )
endif()

if(NOT GMPFRXX_MKII_MPFR_BUILDOPT_TLS)
    message(FATAL_ERROR
        "MPFR was built without TLS support. "
        "gmpfrxx_mkII requires MPFR TLS because MPFR default precision, "
        "rounding mode, and exponent range are routed through libmpfr."
    )
endif()

set(GMPFRXX_MKII_DIST_BASENAME "${PROJECT_NAME}.${PROJECT_VERSION}-rc1")
set(GMPFRXX_MKII_DIST_TAR "${CMAKE_CURRENT_BINARY_DIR}/${GMPFRXX_MKII_DIST_BASENAME}.tar")
set(GMPFRXX_MKII_DIST_ARCHIVE "${GMPFRXX_MKII_DIST_TAR}.xz")

if(Git_FOUND AND XZ_EXECUTABLE)
    add_custom_target(dist
        COMMAND "${CMAKE_COMMAND}" -E rm -f "${GMPFRXX_MKII_DIST_TAR}" "${GMPFRXX_MKII_DIST_ARCHIVE}"
        COMMAND "${GIT_EXECUTABLE}" archive
                --worktree-attributes
                --format=tar
                "--prefix=${GMPFRXX_MKII_DIST_BASENAME}/"
                HEAD
                -o "${GMPFRXX_MKII_DIST_TAR}"
        COMMAND "${XZ_EXECUTABLE}" -z -f -9 "${GMPFRXX_MKII_DIST_TAR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Creating ${GMPFRXX_MKII_DIST_ARCHIVE} from git archive"
        VERBATIM
    )
endif()

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

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gmpfrxx_mkII/detail/build_config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/gmpfrxx_mkII/detail/build_config.hpp"
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

add_library(gmpxx_mkII_default_context_provider SHARED
    src/gmpxx_mkII_default_context_provider.cpp
)
target_include_directories(gmpxx_mkII_default_context_provider PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gmpxx_mkII_default_context_provider PUBLIC GMP::GMP)
target_compile_features(gmpxx_mkII_default_context_provider PUBLIC cxx_std_17)
target_compile_definitions(gmpxx_mkII_default_context_provider PUBLIC
    GMPXX_MKII_DEFAULT_CONTEXT_MODE=GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER
)
add_library(gmpxx_mkII::default_context_provider ALIAS gmpxx_mkII_default_context_provider)

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

function(gmpfrxx_mkii_require_link_interface name)
    cmake_parse_arguments(LINK_INTERFACE "" "" "REQUIRED;FORBIDDEN" ${ARGN})
    get_target_property(actual_links ${name} INTERFACE_LINK_LIBRARIES)
    if(NOT actual_links)
        set(actual_links "")
    endif()
    foreach(required_item IN LISTS LINK_INTERFACE_REQUIRED)
        if(NOT required_item IN_LIST actual_links)
            message(FATAL_ERROR "${name} must link ${required_item}; actual: ${actual_links}")
        endif()
    endforeach()
    foreach(forbidden_item IN LISTS LINK_INTERFACE_FORBIDDEN)
        if(forbidden_item IN_LIST actual_links)
            message(FATAL_ERROR "${name} must not link ${forbidden_item}; actual: ${actual_links}")
        endif()
    endforeach()
endfunction()

gmpfrxx_mkii_require_link_interface(
    gmpxx_mkII
    REQUIRED GMP::GMP
    FORBIDDEN MPFR::MPFR MPC::MPC
)

gmpfrxx_mkii_require_link_interface(
    mpfrxx_mkII
    REQUIRED GMP::GMP MPFR::MPFR
    FORBIDDEN MPC::MPC
)

gmpfrxx_mkii_require_link_interface(
    mpcxx_mkII
    REQUIRED mpfrxx_mkII::mpfrxx_mkII MPC::MPC
)

add_subdirectory(examples)
add_subdirectory(benchmarks)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
