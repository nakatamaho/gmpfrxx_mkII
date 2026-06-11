# Auto-fetch and build GMP, MPFR, and MPC only when system packages are absent.
# The fallback builds static Autotools libraries into a private build-tree prefix
# and then re-runs the existing Find*.cmake modules against that prefix.

include(CMakeParseArguments)
include(ProcessorCount)

option(GMPFRXX_MKII_DEPS_AUTO_FETCH
    "Fetch and build GMP/MPFR/MPC when find_package cannot locate them" ON)
set(GMPFRXX_MKII_DEPS_PREFIX "${CMAKE_BINARY_DIR}/_deps/gmpfrxx_mkii"
    CACHE PATH "Install prefix for auto-built GMP/MPFR/MPC dependencies")
set(GMPFRXX_MKII_DEPS_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/_deps/downloads"
    CACHE PATH "Download directory for auto-built GMP/MPFR/MPC archives")
set(GMPFRXX_MKII_DEPS_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/src"
    CACHE PATH "Source extraction directory for auto-built GMP/MPFR/MPC archives")
set(GMPFRXX_MKII_DEPS_BUILD_DIR "${CMAKE_BINARY_DIR}/_deps/build"
    CACHE PATH "Build directory for auto-built GMP/MPFR/MPC archives")
set(GMPFRXX_MKII_DEPS_HOST "" CACHE STRING
    "Optional Autotools --host value for cross-building auto-built dependencies")
set(GMPFRXX_MKII_DEPS_CFLAGS "-std=gnu17" CACHE STRING
    "CFLAGS used for auto-built GMP/MPFR/MPC dependencies")
set(GMPFRXX_MKII_DEPS_CPPFLAGS "" CACHE STRING
    "CPPFLAGS used for auto-built GMP/MPFR/MPC dependencies")
set(GMPFRXX_MKII_DEPS_LDFLAGS "" CACHE STRING
    "LDFLAGS used for auto-built GMP/MPFR/MPC dependencies")

set(GMPFRXX_MKII_DEPS_GMP_URL
    "https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz"
    CACHE STRING "URL used when auto-fetching GMP")
set(GMPFRXX_MKII_DEPS_GMP_SHA256
    "a3c2b80201b89e68616f4ad30bc66aee4927c3ce50e33929ca819d5c43538898"
    CACHE STRING "SHA256 for the auto-fetched GMP archive")
set(GMPFRXX_MKII_DEPS_GMP_SOURCE_SUBDIR "gmp-6.3.0"
    CACHE STRING "Top-level source directory in the GMP archive")

set(GMPFRXX_MKII_DEPS_MPFR_URL
    "https://www.mpfr.org/mpfr-4.2.2/mpfr-4.2.2.tar.bz2"
    CACHE STRING "URL used when auto-fetching MPFR")
set(GMPFRXX_MKII_DEPS_MPFR_SHA256
    "9ad62c7dc910303cd384ff8f1f4767a655124980bb6d8650fe62c815a231bb7b"
    CACHE STRING "SHA256 for the auto-fetched MPFR archive")
set(GMPFRXX_MKII_DEPS_MPFR_SOURCE_SUBDIR "mpfr-4.2.2"
    CACHE STRING "Top-level source directory in the MPFR archive")

set(GMPFRXX_MKII_DEPS_MPC_URL
    "https://ftp.gnu.org/gnu/mpc/mpc-1.4.1.tar.xz"
    CACHE STRING "URL used when auto-fetching MPC")
set(GMPFRXX_MKII_DEPS_MPC_SHA256
    "91204cd32f164bd3b7c992d4a6a8ce6519511aadab30f78b6982d0bf8d73e931"
    CACHE STRING "SHA256 for the auto-fetched MPC archive")
set(GMPFRXX_MKII_DEPS_MPC_SOURCE_SUBDIR "mpc-1.4.1"
    CACHE STRING "Top-level source directory in the MPC archive")

ProcessorCount(_gmpfrxx_mkii_processor_count)
if(_gmpfrxx_mkii_processor_count EQUAL 0)
    set(_gmpfrxx_mkii_processor_count 2)
endif()
set(GMPFRXX_MKII_DEPS_BUILD_JOBS "${_gmpfrxx_mkii_processor_count}"
    CACHE STRING "Parallel job count for auto-built GMP/MPFR/MPC")
find_program(GMPFRXX_MKII_DEPS_MAKE_PROGRAM NAMES gmake make)
find_program(GMPFRXX_MKII_DEPS_SH_PROGRAM NAMES sh bash)

function(gmpfrxx_mkii_require_auto_fetch package_name)
    if(NOT GMPFRXX_MKII_DEPS_AUTO_FETCH)
        message(FATAL_ERROR
            "${package_name} was not found and GMPFRXX_MKII_DEPS_AUTO_FETCH is OFF. "
            "Install ${package_name} or enable the auto-fetch fallback."
        )
    endif()
    if(NOT GMPFRXX_MKII_DEPS_SH_PROGRAM)
        message(FATAL_ERROR
            "${package_name} was not found and no POSIX shell was found. "
            "The auto-fetch fallback builds Autotools packages and requires sh."
        )
    endif()
    if(NOT GMPFRXX_MKII_DEPS_MAKE_PROGRAM)
        message(FATAL_ERROR
            "${package_name} was not found and no make program was found. "
            "The auto-fetch fallback builds Autotools packages and requires make."
        )
    endif()
endfunction()

function(gmpfrxx_mkii_download_and_extract dep_name url sha256 source_subdir out_source_dir)
    file(MAKE_DIRECTORY "${GMPFRXX_MKII_DEPS_DOWNLOAD_DIR}")
    file(MAKE_DIRECTORY "${GMPFRXX_MKII_DEPS_SOURCE_DIR}")
    get_filename_component(_archive_name "${url}" NAME)
    set(_archive "${GMPFRXX_MKII_DEPS_DOWNLOAD_DIR}/${_archive_name}")
    set(_source_dir "${GMPFRXX_MKII_DEPS_SOURCE_DIR}/${source_subdir}")

    if(EXISTS "${_archive}")
        file(SHA256 "${_archive}" _actual_sha256)
        if(NOT "${_actual_sha256}" STREQUAL "${sha256}")
            message(FATAL_ERROR
                "Existing ${dep_name} archive has SHA256 ${_actual_sha256}, expected ${sha256}: ${_archive}"
            )
        endif()
    else()
        message(STATUS "Fetching ${dep_name}: ${url}")
        file(DOWNLOAD "${url}" "${_archive}"
            EXPECTED_HASH "SHA256=${sha256}"
            TLS_VERIFY ON
            SHOW_PROGRESS
            STATUS _download_status
            LOG _download_log
        )
        list(GET _download_status 0 _download_result)
        if(NOT _download_result EQUAL 0)
            list(GET _download_status 1 _download_message)
            message(FATAL_ERROR
                "Failed to download ${dep_name}: ${_download_message}\n${_download_log}"
            )
        endif()
    endif()

    if(NOT EXISTS "${_source_dir}/configure")
        file(REMOVE_RECURSE "${_source_dir}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E tar xf "${_archive}"
            WORKING_DIRECTORY "${GMPFRXX_MKII_DEPS_SOURCE_DIR}"
            RESULT_VARIABLE _extract_result
        )
        if(NOT _extract_result EQUAL 0)
            message(FATAL_ERROR "Failed to extract ${dep_name}: ${_archive}")
        endif()
    endif()

    if(NOT EXISTS "${_source_dir}/configure")
        message(FATAL_ERROR
            "${dep_name} archive did not produce expected source directory: ${_source_dir}"
        )
    endif()

    set(${out_source_dir} "${_source_dir}" PARENT_SCOPE)
endfunction()

function(gmpfrxx_mkii_autotools_env out_env)
    set(_env)
    if(CMAKE_C_COMPILER)
        list(APPEND _env "CC=${CMAKE_C_COMPILER}")
    endif()
    if(CMAKE_CXX_COMPILER)
        list(APPEND _env "CXX=${CMAKE_CXX_COMPILER}")
    endif()
    if(GMPFRXX_MKII_DEPS_CFLAGS)
        list(APPEND _env "CFLAGS=${GMPFRXX_MKII_DEPS_CFLAGS}")
    endif()
    if(GMPFRXX_MKII_DEPS_CPPFLAGS)
        list(APPEND _env "CPPFLAGS=${GMPFRXX_MKII_DEPS_CPPFLAGS}")
    endif()
    if(GMPFRXX_MKII_DEPS_LDFLAGS)
        list(APPEND _env "LDFLAGS=${GMPFRXX_MKII_DEPS_LDFLAGS}")
    endif()
    if(CMAKE_AR)
        list(APPEND _env "AR=${CMAKE_AR}")
    endif()
    if(CMAKE_RANLIB)
        list(APPEND _env "RANLIB=${CMAKE_RANLIB}")
    endif()
    set(${out_env} ${_env} PARENT_SCOPE)
endfunction()

function(gmpfrxx_mkii_build_autotools_dependency dep_name)
    cmake_parse_arguments(ARG
        ""
        "URL;SHA256;SOURCE_SUBDIR;PREFIX;INSTALL_HEADER;INSTALL_LIBRARY"
        "CONFIGURE_ARGS"
        ${ARGN}
    )

    if(EXISTS "${ARG_PREFIX}/include/${ARG_INSTALL_HEADER}" AND
       EXISTS "${ARG_PREFIX}/lib/${ARG_INSTALL_LIBRARY}")
        message(STATUS "Using previously auto-built ${dep_name}: ${ARG_PREFIX}")
        return()
    endif()

    gmpfrxx_mkii_download_and_extract(
        "${dep_name}"
        "${ARG_URL}"
        "${ARG_SHA256}"
        "${ARG_SOURCE_SUBDIR}"
        _source_dir
    )

    set(_build_dir "${GMPFRXX_MKII_DEPS_BUILD_DIR}/${ARG_SOURCE_SUBDIR}")
    file(REMOVE_RECURSE "${_build_dir}")
    file(MAKE_DIRECTORY "${_build_dir}")
    file(MAKE_DIRECTORY "${ARG_PREFIX}")

    set(_host_args)
    if(GMPFRXX_MKII_DEPS_HOST)
        list(APPEND _host_args "--host=${GMPFRXX_MKII_DEPS_HOST}")
    endif()

    gmpfrxx_mkii_autotools_env(_autotools_env)
    message(STATUS "Configuring ${dep_name} into ${ARG_PREFIX}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env ${_autotools_env}
            "${GMPFRXX_MKII_DEPS_SH_PROGRAM}" "${_source_dir}/configure"
            "--prefix=${ARG_PREFIX}"
            ${_host_args}
            ${ARG_CONFIGURE_ARGS}
        WORKING_DIRECTORY "${_build_dir}"
        RESULT_VARIABLE _configure_result
    )
    if(NOT _configure_result EQUAL 0)
        message(FATAL_ERROR "Failed to configure ${dep_name}; see ${_build_dir}/config.log")
    endif()

    message(STATUS "Building ${dep_name}")
    execute_process(
        COMMAND "${GMPFRXX_MKII_DEPS_MAKE_PROGRAM}" "-j${GMPFRXX_MKII_DEPS_BUILD_JOBS}"
        WORKING_DIRECTORY "${_build_dir}"
        RESULT_VARIABLE _build_result
    )
    if(NOT _build_result EQUAL 0)
        message(FATAL_ERROR "Failed to build ${dep_name}")
    endif()

    message(STATUS "Installing ${dep_name}")
    execute_process(
        COMMAND "${GMPFRXX_MKII_DEPS_MAKE_PROGRAM}" install
        WORKING_DIRECTORY "${_build_dir}"
        RESULT_VARIABLE _install_result
    )
    if(NOT _install_result EQUAL 0)
        message(FATAL_ERROR "Failed to install ${dep_name}")
    endif()

    if(NOT EXISTS "${ARG_PREFIX}/include/${ARG_INSTALL_HEADER}" OR
       NOT EXISTS "${ARG_PREFIX}/lib/${ARG_INSTALL_LIBRARY}")
        message(FATAL_ERROR
            "${dep_name} installed successfully but expected files were not found under ${ARG_PREFIX}"
        )
    endif()
endfunction()

function(gmpfrxx_mkii_cache_dependency_paths package_name prefix library)
    set(${package_name}_INCLUDE_DIR "${prefix}/include" CACHE PATH
        "${package_name} include directory" FORCE)
    set(${package_name}_LIBRARY "${prefix}/lib/${library}" CACHE FILEPATH
        "${package_name} library" FORCE)
endfunction()

function(gmpfrxx_mkii_library_dir out_var library_path)
    get_filename_component(_library_dir "${library_path}" DIRECTORY)
    set(${out_var} "${_library_dir}" PARENT_SCOPE)
endfunction()

function(gmpfrxx_mkii_find_or_build_dependencies)
    find_package(GMP QUIET)
    if(NOT GMP_FOUND)
        gmpfrxx_mkii_require_auto_fetch(GMP)
        set(_gmp_prefix "${GMPFRXX_MKII_DEPS_PREFIX}/GMP")
        gmpfrxx_mkii_build_autotools_dependency(GMP
            URL "${GMPFRXX_MKII_DEPS_GMP_URL}"
            SHA256 "${GMPFRXX_MKII_DEPS_GMP_SHA256}"
            SOURCE_SUBDIR "${GMPFRXX_MKII_DEPS_GMP_SOURCE_SUBDIR}"
            PREFIX "${_gmp_prefix}"
            INSTALL_HEADER "gmp.h"
            INSTALL_LIBRARY "libgmp.a"
            CONFIGURE_ARGS --enable-static --disable-shared
        )
        gmpfrxx_mkii_cache_dependency_paths(GMP "${_gmp_prefix}" "libgmp.a")
        find_package(GMP REQUIRED)
    endif()

    find_package(MPFR QUIET)
    if(NOT MPFR_FOUND)
        gmpfrxx_mkii_require_auto_fetch(MPFR)
        gmpfrxx_mkii_library_dir(_gmp_library_dir "${GMP_LIBRARY}")
        set(_mpfr_prefix "${GMPFRXX_MKII_DEPS_PREFIX}/MPFR")
        gmpfrxx_mkii_build_autotools_dependency(MPFR
            URL "${GMPFRXX_MKII_DEPS_MPFR_URL}"
            SHA256 "${GMPFRXX_MKII_DEPS_MPFR_SHA256}"
            SOURCE_SUBDIR "${GMPFRXX_MKII_DEPS_MPFR_SOURCE_SUBDIR}"
            PREFIX "${_mpfr_prefix}"
            INSTALL_HEADER "mpfr.h"
            INSTALL_LIBRARY "libmpfr.a"
            CONFIGURE_ARGS
                --enable-static
                --disable-shared
                --enable-thread-safe
                "--with-gmp-include=${GMP_INCLUDE_DIR}"
                "--with-gmp-lib=${_gmp_library_dir}"
        )
        gmpfrxx_mkii_cache_dependency_paths(MPFR "${_mpfr_prefix}" "libmpfr.a")
        find_package(MPFR REQUIRED)
    endif()

    find_package(MPC QUIET)
    if(NOT MPC_FOUND)
        gmpfrxx_mkii_require_auto_fetch(MPC)
        gmpfrxx_mkii_library_dir(_gmp_library_dir "${GMP_LIBRARY}")
        gmpfrxx_mkii_library_dir(_mpfr_library_dir "${MPFR_LIBRARY}")
        set(_mpc_prefix "${GMPFRXX_MKII_DEPS_PREFIX}/MPC")
        gmpfrxx_mkii_build_autotools_dependency(MPC
            URL "${GMPFRXX_MKII_DEPS_MPC_URL}"
            SHA256 "${GMPFRXX_MKII_DEPS_MPC_SHA256}"
            SOURCE_SUBDIR "${GMPFRXX_MKII_DEPS_MPC_SOURCE_SUBDIR}"
            PREFIX "${_mpc_prefix}"
            INSTALL_HEADER "mpc.h"
            INSTALL_LIBRARY "libmpc.a"
            CONFIGURE_ARGS
                --enable-static
                --disable-shared
                "--with-gmp-include=${GMP_INCLUDE_DIR}"
                "--with-gmp-lib=${_gmp_library_dir}"
                "--with-mpfr-include=${MPFR_INCLUDE_DIR}"
                "--with-mpfr-lib=${_mpfr_library_dir}"
        )
        gmpfrxx_mkii_cache_dependency_paths(MPC "${_mpc_prefix}" "libmpc.a")
        find_package(MPC REQUIRED)
    endif()
endfunction()
