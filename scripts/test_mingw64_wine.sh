#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
BUILD_DIR=${1:-build-mingw64-wine}
CTEST_REGEX=${2:-${GMPFRXX_MKII_MINGW_CTEST_REGEX:-}}
BUILD_TYPE=${GMPFRXX_MKII_MINGW_BUILD_TYPE:-Debug}
BUILD_TARGET=${GMPFRXX_MKII_MINGW_BUILD_TARGET:-all}
DEPS_ROOT=${GMPFRXX_MKII_MINGW_DEPS_ROOT:-/home/docker/mplapack/external/i}
TOOLCHAIN=${GMPFRXX_MKII_MINGW_TOOLCHAIN:-${ROOT_DIR}/cmake/toolchains/mingw64-wine.cmake}

if [[ ${BUILD_DIR} != /* ]]; then
    BUILD_DIR=${ROOT_DIR}/${BUILD_DIR}
fi

require_command() {
    local name=$1
    if ! command -v "${name}" >/dev/null 2>&1; then
        echo "error: required command not found: ${name}" >&2
        exit 1
    fi
}

require_path() {
    local path=$1
    if [[ ! -e ${path} ]]; then
        echo "error: required path not found: ${path}" >&2
        exit 1
    fi
}

require_command cmake
require_command ctest
require_command wine
require_command winepath
require_command x86_64-w64-mingw32-gcc
require_command x86_64-w64-mingw32-g++

GMP_PREFIX=${GMPFRXX_MKII_MINGW_GMP_PREFIX:-${DEPS_ROOT}/GMP}
MPFR_PREFIX=${GMPFRXX_MKII_MINGW_MPFR_PREFIX:-${DEPS_ROOT}/MPFR}
MPC_PREFIX=${GMPFRXX_MKII_MINGW_MPC_PREFIX:-${DEPS_ROOT}/MPC}
DEPS_AUTO_FETCH=${GMPFRXX_MKII_DEPS_AUTO_FETCH:-ON}

have_prebuilt_deps=0
if [[ -e ${GMP_PREFIX}/include/gmp.h &&
      -e ${GMP_PREFIX}/lib/libgmp.dll.a &&
      -e ${GMP_PREFIX}/bin/libgmp-10.dll &&
      -e ${MPFR_PREFIX}/include/mpfr.h &&
      -e ${MPFR_PREFIX}/lib/libmpfr.dll.a &&
      -e ${MPFR_PREFIX}/bin/libmpfr-6.dll &&
      -e ${MPC_PREFIX}/include/mpc.h &&
      -e ${MPC_PREFIX}/lib/libmpc.dll.a &&
      -e ${MPC_PREFIX}/bin/libmpc-3.dll ]]; then
    have_prebuilt_deps=1
fi

if [[ ${have_prebuilt_deps} -eq 0 ]]; then
    case ${DEPS_AUTO_FETCH} in
        OFF|off|FALSE|false|0|NO|no)
            require_path "${GMP_PREFIX}/include/gmp.h"
            require_path "${GMP_PREFIX}/lib/libgmp.dll.a"
            require_path "${GMP_PREFIX}/bin/libgmp-10.dll"
            require_path "${MPFR_PREFIX}/include/mpfr.h"
            require_path "${MPFR_PREFIX}/lib/libmpfr.dll.a"
            require_path "${MPFR_PREFIX}/bin/libmpfr-6.dll"
            require_path "${MPC_PREFIX}/include/mpc.h"
            require_path "${MPC_PREFIX}/lib/libmpc.dll.a"
            require_path "${MPC_PREFIX}/bin/libmpc-3.dll"
            ;;
        *)
            echo "Prebuilt MinGW GMP/MPFR/MPC DLL dependencies not found; CMake auto-fetch fallback is enabled."
            ;;
    esac
fi

mkdir -p "${BUILD_DIR}"

wine_paths=("$(winepath -w "${BUILD_DIR}")")
if [[ ${have_prebuilt_deps} -eq 1 ]]; then
    wine_paths+=("$(winepath -w "${GMP_PREFIX}/bin")")
    wine_paths+=("$(winepath -w "${MPFR_PREFIX}/bin")")
    wine_paths+=("$(winepath -w "${MPC_PREFIX}/bin")")
fi
wine_joined_path=$(IFS=';'; echo "${wine_paths[*]}")
if [[ -n ${WINEPATH:-} ]]; then
    export WINEPATH="${wine_joined_path};${WINEPATH}"
else
    export WINEPATH="${wine_joined_path}"
fi
export WINEDEBUG=${WINEDEBUG:--all}

echo "ROOT_DIR=${ROOT_DIR}"
echo "BUILD_DIR=${BUILD_DIR}"
echo "DEPS_ROOT=${DEPS_ROOT}"
echo "BUILD_TYPE=${BUILD_TYPE}"
echo "BUILD_TARGET=${BUILD_TARGET}"
echo "DEPS_AUTO_FETCH=${DEPS_AUTO_FETCH}"
echo "HAVE_PREBUILT_DEPS=${have_prebuilt_deps}"
echo "WINEPATH=${WINEPATH}"

cmake_args=(
    -S "${ROOT_DIR}"
    -B "${BUILD_DIR}"
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN}"
    -DGMPFRXX_MKII_MINGW_DEPS_ROOT="${DEPS_ROOT}"
    -DGMPFRXX_MKII_MINGW_GMP_PREFIX="${GMP_PREFIX}"
    -DGMPFRXX_MKII_MINGW_MPFR_PREFIX="${MPFR_PREFIX}"
    -DGMPFRXX_MKII_MINGW_MPC_PREFIX="${MPC_PREFIX}"
    -DGMPFRXX_MKII_DEPS_AUTO_FETCH="${DEPS_AUTO_FETCH}"
)

if [[ ${have_prebuilt_deps} -eq 1 ]]; then
    cmake_args+=(
        -DGMP_INCLUDE_DIR="${GMP_PREFIX}/include"
        -DGMP_LIBRARY="${GMP_PREFIX}/lib/libgmp.dll.a"
        -DMPFR_INCLUDE_DIR="${MPFR_PREFIX}/include"
        -DMPFR_LIBRARY="${MPFR_PREFIX}/lib/libmpfr.dll.a"
        -DMPC_INCLUDE_DIR="${MPC_PREFIX}/include"
        -DMPC_LIBRARY="${MPC_PREFIX}/lib/libmpc.dll.a"
    )
fi

dependency_cache_vars=(
    GMPFRXX_MKII_DEPS_GMP_URL
    GMPFRXX_MKII_DEPS_GMP_SHA256
    GMPFRXX_MKII_DEPS_MPFR_URL
    GMPFRXX_MKII_DEPS_MPFR_SHA256
    GMPFRXX_MKII_DEPS_MPC_URL
    GMPFRXX_MKII_DEPS_MPC_SHA256
    GMPFRXX_MKII_DEPS_CFLAGS
    GMPFRXX_MKII_DEPS_CPPFLAGS
    GMPFRXX_MKII_DEPS_LDFLAGS
)
for cache_var in "${dependency_cache_vars[@]}"; do
    if [[ -n ${!cache_var:-} ]]; then
        cmake_args+=("-D${cache_var}=${!cache_var}")
    fi
done

cmake "${cmake_args[@]}"

cmake --build "${BUILD_DIR}" --target "${BUILD_TARGET}" -j "${CMAKE_BUILD_PARALLEL_LEVEL:-$(nproc)}"

ctest_args=(--test-dir "${BUILD_DIR}" --output-on-failure)
if [[ -n ${CTEST_REGEX} ]]; then
    ctest_args+=(-R "${CTEST_REGEX}")
fi
ctest "${ctest_args[@]}"
