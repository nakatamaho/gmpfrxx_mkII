#!/usr/bin/env bash
#
# Copyright (c) 2026
#      Nakata, Maho
#      All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
benchmark_root="$(cd "${script_dir}/.." && pwd)"
repo_dir="$(cd "${benchmark_root}/.." && pwd)"

build_dir="${1:-${repo_dir}/build_phase5}"
precision="${2:-512}"
rdot_n="${3:-100000000}"
output_dir="${4:-${benchmark_root}/mpfr/results_raw}"
repeat_count="${5:-10}"
raxpy_n="${6:-${rdot_n}}"
rgemv_m="${7:-4000}"
rgemv_n="${8:-4000}"
rgemm_m="${9:-500}"
rgemm_k="${10:-500}"
rgemm_n="${11:-500}"

export OMP_NUM_THREADS="${OMP_NUM_THREADS:-32}"
export OMP_PLACES="${OMP_PLACES:-cores}"
export OMP_PROC_BIND="${OMP_PROC_BIND:-spread}"
benchmark_command_prefix="${BENCH_COMMAND_PREFIX:-${BENCH_NUMACTL:-}}"

mkdir -p "${output_dir}"
log_file="${output_dir}/benchmark_$(date +%Y%m%d_%H%M%S).log"

benchmark_dir="${build_dir}/benchmarks"
if [[ ! -d "${benchmark_dir}" ]]; then
    echo "Benchmark directory not found: ${benchmark_dir}" >&2
    echo "Build first: cmake --build ${build_dir} -j" >&2
    exit 1
fi

run_one() {
    local label="$1"
    shift
    local subdir="$1"
    shift
    local exe="${benchmark_dir}/${subdir}/$1"
    shift

    if [[ ! -x "${exe}" ]]; then
        echo "Executable not found: ${exe}" >&2
        exit 1
    fi

    for ((run = 1; run <= repeat_count; ++run)); do
        local command=("${exe}" "$@")
        if [[ -n "${benchmark_command_prefix}" ]]; then
            local prefix=()
            read -r -a prefix <<<"${benchmark_command_prefix}"
            command=("${prefix[@]}" "${command[@]}")
        fi
        command=(env "MPFRXX_DEFAULT_PRECISION_BITS=${precision}" "${command[@]}")

        echo "COMMAND ${label} ${command[*]}"
        echo "RUN ${run}/${repeat_count}"
        /usr/bin/time -f "WALL_SECONDS %e" "${command[@]}"
        echo
    done
}

run_variants() {
    local kernel="$1"
    local subdir="$2"
    shift 2
    local args=("$@")
    local executables=()

    case "${kernel}" in
    Rdot)
        executables=(
            "Rdot_mpfr_C_native_01"
            "Rdot_mpfr_C_native_01_FMA"
            "Rdot_mpfr_C_native_02"
            "Rdot_mpfr_C_native_03"
            "Rdot_mpfr_C_native_04"
            "Rdot_mpfr_C_native_05"
            "Rdot_mpfr_C_native_06"
            "Rdot_mpfr_C_native_openmp_01"
            "Rdot_mpfr_C_native_openmp_01_FMA"
            "Rdot_mpfr_C_native_openmp_02"
            "Rdot_mpfr_C_native_openmp_03"
            "Rdot_mpfr_C_native_openmp_04"
            "Rdot_mpfr_C_native_openmp_05"
            "Rdot_mpfr_C_native_openmp_06"
            "Rdot_mpfr_kernel_01"
            "Rdot_mpfr_kernel_01_PRECISION"
            "Rdot_mpfr_kernel_01_ROUNDING"
            "Rdot_mpfr_kernel_01_ROUNDING_PRECISION"
            "Rdot_mpfr_kernel_01_ROUNDING_FMA_CAPTURE_FMA"
            "Rdot_mpfr_kernel_01_ROUNDING_FMA_CAPTURE_PRECISION_FMA"
            "Rdot_mpfr_kernel_02"
            "Rdot_mpfr_kernel_02_PRECISION"
            "Rdot_mpfr_kernel_03"
            "Rdot_mpfr_kernel_03_PRECISION"
            "Rdot_mpfr_kernel_03_ROUNDING"
            "Rdot_mpfr_kernel_03_ROUNDING_PRECISION"
            "Rdot_mpfr_kernel_04"
            "Rdot_mpfr_kernel_04_PRECISION"
            "Rdot_mpfr_kernel_05"
            "Rdot_mpfr_kernel_05_PRECISION"
            "Rdot_mpfr_kernel_06"
            "Rdot_mpfr_kernel_06_PRECISION"
            "Rdot_mpfr_kernel_openmp_01"
            "Rdot_mpfr_kernel_openmp_01_PRECISION"
            "Rdot_mpfr_kernel_openmp_01_ROUNDING"
            "Rdot_mpfr_kernel_openmp_01_ROUNDING_PRECISION"
            "Rdot_mpfr_kernel_openmp_01_ROUNDING_FMA_CAPTURE_FMA"
            "Rdot_mpfr_kernel_openmp_01_ROUNDING_FMA_CAPTURE_PRECISION_FMA"
            "Rdot_mpfr_kernel_openmp_02"
            "Rdot_mpfr_kernel_openmp_02_PRECISION"
            "Rdot_mpfr_kernel_openmp_03"
            "Rdot_mpfr_kernel_openmp_03_PRECISION"
            "Rdot_mpfr_kernel_openmp_03_ROUNDING"
            "Rdot_mpfr_kernel_openmp_03_ROUNDING_PRECISION"
            "Rdot_mpfr_kernel_openmp_04"
            "Rdot_mpfr_kernel_openmp_04_PRECISION"
            "Rdot_mpfr_kernel_openmp_05"
            "Rdot_mpfr_kernel_openmp_05_PRECISION"
            "Rdot_mpfr_kernel_openmp_06"
            "Rdot_mpfr_kernel_openmp_06_PRECISION"
        )
        ;;
    Raxpy)
        executables=(
            "Raxpy_mpfr_C_native_01"
            "Raxpy_mpfr_C_native_01_FMA"
            "Raxpy_mpfr_C_native_packed_custom_layout_FMA"
            "Raxpy_mpfr_C_native_openmp_01"
            "Raxpy_mpfr_C_native_openmp_01_FMA"
            "Raxpy_mpfr_kernel_01"
            "Raxpy_mpfr_kernel_01_PRECISION"
            "Raxpy_mpfr_kernel_01_PRECISION_FMA"
            "Raxpy_mpfr_kernel_01_ROUNDING"
            "Raxpy_mpfr_kernel_01_ROUNDING_PRECISION"
            "Raxpy_mpfr_kernel_01_ROUNDING_PRECISION_FMA"
            "Raxpy_mpfr_kernel_02"
            "Raxpy_mpfr_kernel_02_PRECISION"
            "Raxpy_mpfr_kernel_02_ROUNDING"
            "Raxpy_mpfr_kernel_02_ROUNDING_PRECISION"
            "Raxpy_mpfr_kernel_03"
            "Raxpy_mpfr_kernel_03_PRECISION"
            "Raxpy_mpfr_kernel_03_ROUNDING"
            "Raxpy_mpfr_kernel_03_ROUNDING_PRECISION"
            "Raxpy_mpfr_kernel_04"
            "Raxpy_mpfr_kernel_04_PRECISION"
            "Raxpy_mpfr_kernel_04_ROUNDING"
            "Raxpy_mpfr_kernel_04_ROUNDING_PRECISION"
            "Raxpy_mpfr_kernel_openmp_01"
            "Raxpy_mpfr_kernel_openmp_01_PRECISION"
            "Raxpy_mpfr_kernel_openmp_01_PRECISION_FMA"
            "Raxpy_mpfr_kernel_openmp_01_ROUNDING"
            "Raxpy_mpfr_kernel_openmp_01_ROUNDING_PRECISION"
            "Raxpy_mpfr_kernel_openmp_01_ROUNDING_PRECISION_FMA"
            "Raxpy_mpfr_kernel_openmp_02"
            "Raxpy_mpfr_kernel_openmp_02_PRECISION"
            "Raxpy_mpfr_kernel_openmp_02_ROUNDING"
            "Raxpy_mpfr_kernel_openmp_02_ROUNDING_PRECISION"
            "Raxpy_mpfr_kernel_openmp_03"
            "Raxpy_mpfr_kernel_openmp_03_PRECISION"
            "Raxpy_mpfr_kernel_openmp_03_ROUNDING"
            "Raxpy_mpfr_kernel_openmp_03_ROUNDING_PRECISION"
            "Raxpy_mpfr_kernel_openmp_04"
            "Raxpy_mpfr_kernel_openmp_04_PRECISION"
            "Raxpy_mpfr_kernel_openmp_04_ROUNDING"
            "Raxpy_mpfr_kernel_openmp_04_ROUNDING_PRECISION"
        )
        ;;
    Rgemv)
        executables=(
            "Rgemv_mpfr_C_native_01"
            "Rgemv_mpfr_C_native_openmp_01"
            "Rgemv_mpfr_kernel_01_mkII"
            "Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA"
            "Rgemv_mpfr_kernel_02_mkII"
            "Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH_FMA"
            "Rgemv_mpfr_kernel_openmp_01_mkII"
            "Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH_FMA"
            "Rgemv_mpfr_kernel_openmp_02_mkII"
            "Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA"
        )
        ;;
    Rgemm)
        executables=(
            "Rgemm_mpfr_C_native_01"
            "Rgemm_mpfr_C_native_02"
            "Rgemm_mpfr_C_native_openmp_01"
            "Rgemm_mpfr_C_native_openmp_02"
            "Rgemm_mpfr_kernel_01_mkII"
            "Rgemm_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemm_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA"
            "Rgemm_mpfr_kernel_02_mkII"
            "Rgemm_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemm_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH_FMA"
            "Rgemm_mpfr_kernel_03_mkII"
            "Rgemm_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemm_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH_FMA"
            "Rgemm_mpfr_kernel_openmp_01_mkII"
            "Rgemm_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemm_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH_FMA"
            "Rgemm_mpfr_kernel_openmp_02_mkII"
            "Rgemm_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemm_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA"
            "Rgemm_mpfr_kernel_openmp_03_mkII"
            "Rgemm_mpfr_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH"
            "Rgemm_mpfr_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH_FMA"
        )
        ;;
    esac

    for exe in "${executables[@]}"; do
        run_one "${kernel} ${exe#${kernel}_mpfr_}" "${subdir}" "${exe}" "${args[@]}"
    done
}

{
    uname -a
    if [[ -r /proc/cpuinfo ]]; then
        grep 'model name' /proc/cpuinfo | head -1
    else
        uname -m
    fi
    echo "BENCHMARK_PARAMS precision=${precision} rdot_n=${rdot_n} raxpy_n=${raxpy_n} rgemv_m=${rgemv_m} rgemv_n=${rgemv_n} rgemm_m=${rgemm_m} rgemm_k=${rgemm_k} rgemm_n=${rgemm_n} repeat=${repeat_count}"
    echo "OPENMP_AFFINITY OMP_NUM_THREADS=${OMP_NUM_THREADS} OMP_PLACES=${OMP_PLACES} OMP_PROC_BIND=${OMP_PROC_BIND}"
    echo "DEFAULT_PRECISION_ENV MPFRXX_DEFAULT_PRECISION_BITS=${precision}"
    if [[ -n "${benchmark_command_prefix}" ]]; then
        echo "BENCH_COMMAND_PREFIX ${benchmark_command_prefix}"
    fi
    echo

    run_variants Rdot mpfr/00_Rdot "${rdot_n}" "${precision}"
    run_variants Raxpy mpfr/01_Raxpy "${raxpy_n}" "${precision}"
    run_variants Rgemv mpfr/02_Rgemv "${rgemv_m}" "${rgemv_n}" "${precision}"
    run_variants Rgemm mpfr/03_Rgemm "${rgemm_m}" "${rgemm_k}" "${rgemm_n}" "${precision}"
} 2>&1 | tee "${log_file}"

python3 "${script_dir}/plot.py" "${log_file}" --output-dir "${output_dir}" --backend MPFR --kernels Rdot Raxpy Rgemv Rgemm
