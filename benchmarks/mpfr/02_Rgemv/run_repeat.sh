#!/usr/bin/env bash
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
repo_dir="$(cd "${script_dir}/../../.." && pwd)"

build_dir="${1:-${repo_dir}/build_bench_release}"
m="${2:-4000}"
n="${3:-4000}"
precision="${4:-512}"
repeat_count="${5:-10}"
output_dir="${6:-${script_dir}/results_raw/rgemv_mpfr_m${m}_n${n}_p${precision}_repeat${repeat_count}_$(date +%Y%m%d_%H%M%S)}"

export OMP_NUM_THREADS="${OMP_NUM_THREADS:-32}"
export OMP_PLACES="${OMP_PLACES:-cores}"
export OMP_PROC_BIND="${OMP_PROC_BIND:-spread}"
benchmark_command_prefix="${BENCH_COMMAND_PREFIX:-${BENCH_NUMACTL:-}}"
smoke_enabled="${BENCH_SMOKE:-1}"
smoke_check_enabled="${BENCH_SMOKE_CHECK:-1}"
smoke_nocheck_enabled="${BENCH_SMOKE_NOCHECK:-1}"
smoke_m="${BENCH_SMOKE_M:-13}"
smoke_n="${BENCH_SMOKE_N:-13}"

benchmark_dir="${build_dir}/benchmarks/mpfr/02_Rgemv"
if [[ ! -d "${benchmark_dir}" ]]; then
    echo "Benchmark directory not found: ${benchmark_dir}" >&2
    echo "Build first: cmake --build ${build_dir} -j" >&2
    exit 1
fi

mkdir -p "${output_dir}"
log_file="${output_dir}/benchmark_rgemv_mpfr_m${m}_n${n}_p${precision}_repeat${repeat_count}.log"

executables=(
    "Rgemv_mpfr_C_native_01"
    "Rgemv_mpfr_C_native_01_FMA"
    "Rgemv_mpfr_C_native_02"
    "Rgemv_mpfr_C_native_02_FMA"
    "Rgemv_mpfr_C_native_03"
    "Rgemv_mpfr_C_native_04"
    "Rgemv_mpfr_C_native_openmp_01"
    "Rgemv_mpfr_C_native_openmp_01_FMA"
    "Rgemv_mpfr_C_native_openmp_02"
    "Rgemv_mpfr_C_native_openmp_02_FMA"
    "Rgemv_mpfr_C_native_openmp_03"
    "Rgemv_mpfr_C_native_openmp_04"
    "Rgemv_mpfr_C_native_openmp_04_FMA"
    "Rgemv_mpfr_C_native_openmp_05"
    "Rgemv_mpfr_C_native_openmp_05_FMA"
    "Rgemv_mpfr_C_native_openmp_06"
    "Rgemv_mpfr_C_native_openmp_06_FMA"
    "Rgemv_mpfr_C_native_openmp_07"
    "Rgemv_mpfr_C_native_openmp_07_FMA"
)

append_cpp_family() {
    local base="$1"
    executables+=(
        "${base}"
        "${base}_PRECISION"
        "${base}_ROUNDING"
        "${base}_ROUNDING_PRECISION"
        "${base}_ROUNDING_FMA_CAPTURE_FMA"
        "${base}_ROUNDING_FMA_CAPTURE_PRECISION_FMA"
    )
}

for variant in 01 02 03 04; do
    append_cpp_family "Rgemv_mpfr_kernel_${variant}"
done

for variant in 01 02 03 04 05 06 07; do
    append_cpp_family "Rgemv_mpfr_kernel_openmp_${variant}"
done

run_smoke_command() {
    local mode="$1"
    local exe="$2"
    shift 2
    local command=("$@")

    if [[ -n "${benchmark_command_prefix}" ]]; then
        local prefix=()
        read -r -a prefix <<<"${benchmark_command_prefix}"
        command=("${prefix[@]}" "${command[@]}")
    fi
    command=(env "MPFRXX_DEFAULT_PRECISION_BITS=${precision}" "${command[@]}")

    echo "SMOKE_${mode} Rgemv ${exe} ${command[*]}"
    if "${command[@]}" >/dev/null; then
        echo "SMOKE_${mode}_OK Rgemv ${exe}"
    else
        echo "SMOKE_${mode}_FAILED Rgemv ${exe}" >&2
        return 1
    fi
}

run_smoke_one() {
    local exe="$1"
    local path="${benchmark_dir}/${exe}"
    if [[ ! -x "${path}" ]]; then
        echo "Executable not found: ${path}" >&2
        exit 1
    fi

    if [[ "${smoke_nocheck_enabled}" != "0" ]]; then
        run_smoke_command NOCHECK "${exe}" "${path}" "${smoke_m}" "${smoke_n}" "${precision}" -nocheck
    fi
    if [[ "${smoke_check_enabled}" != "0" ]]; then
        run_smoke_command CHECK "${exe}" "${path}" "${smoke_m}" "${smoke_n}" "${precision}"
    fi
}

run_smoke_tests() {
    if [[ "${smoke_enabled}" == "0" ]]; then
        echo "SMOKE_SKIP Rgemv BENCH_SMOKE=0"
        return
    fi

    echo "SMOKE_BEGIN Rgemv m=${smoke_m} n=${smoke_n} precision=${precision}"
    for exe in "${executables[@]}"; do
        run_smoke_one "${exe}"
    done
    echo "SMOKE_DONE Rgemv"
}

run_one() {
    local exe="$1"
    local path="${benchmark_dir}/${exe}"
    if [[ ! -x "${path}" ]]; then
        echo "Executable not found: ${path}" >&2
        exit 1
    fi

    for ((run = 1; run <= repeat_count; ++run)); do
        local command=("${path}" "${m}" "${n}" "${precision}")
        if [[ -n "${benchmark_command_prefix}" ]]; then
            local prefix=()
            read -r -a prefix <<<"${benchmark_command_prefix}"
            command=("${prefix[@]}" "${command[@]}")
        fi
        command=(env "MPFRXX_DEFAULT_PRECISION_BITS=${precision}" "${command[@]}")

        echo "COMMAND Rgemv ${exe} ${command[*]}"
        echo "RUN ${run}/${repeat_count}"
        /usr/bin/time -f "WALL_SECONDS %e" "${command[@]}"
        echo
    done
}

{
    uname -a
    if [[ -r /proc/cpuinfo ]]; then
        grep 'model name' /proc/cpuinfo | head -1
    else
        uname -m
    fi
    echo "BENCHMARK_PARAMS precision=${precision} rgemv_m=${m} rgemv_n=${n} repeat=${repeat_count}"
    echo "OPENMP_AFFINITY OMP_NUM_THREADS=${OMP_NUM_THREADS} OMP_PLACES=${OMP_PLACES} OMP_PROC_BIND=${OMP_PROC_BIND}"
    echo "DEFAULT_PRECISION_ENV MPFRXX_DEFAULT_PRECISION_BITS=${precision}"
    echo "SMOKE_TESTS enabled=${smoke_enabled} check=${smoke_check_enabled} nocheck=${smoke_nocheck_enabled} m=${smoke_m} n=${smoke_n}"
    if [[ -n "${benchmark_command_prefix}" ]]; then
        echo "BENCH_COMMAND_PREFIX ${benchmark_command_prefix}"
    fi
    echo

    run_smoke_tests
    echo

    for exe in "${executables[@]}"; do
        run_one "${exe}"
    done
} 2>&1 | tee "${log_file}"

python3 "${script_dir}/plot_repeat_summary.py" "${log_file}" \
    --output-dir "${output_dir}" \
    --output-prefix "rgemv_mpfr_m${m}_n${n}_p${precision}_repeat${repeat_count}" \
    --title-prefix "MPFR Rgemv m=${m} n=${n} precision=${precision} repeat=${repeat_count}"
