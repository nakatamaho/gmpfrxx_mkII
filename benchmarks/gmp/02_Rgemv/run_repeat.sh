#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_dir="$(cd "${script_dir}/../../.." && pwd)"

build_dir="${1:-${repo_dir}/build_bench_release}"
m="${2:-4000}"
n="${3:-4000}"
precision="${4:-512}"
repeat_count="${5:-10}"
output_dir="${6:-${script_dir}/results_raw/rgemv_gmp_m${m}_n${n}_p${precision}_repeat${repeat_count}_$(date +%Y%m%d_%H%M%S)}"

export OMP_NUM_THREADS="${OMP_NUM_THREADS:-32}"
export OMP_PLACES="${OMP_PLACES:-cores}"
export OMP_PROC_BIND="${OMP_PROC_BIND:-spread}"
benchmark_command_prefix="${BENCH_COMMAND_PREFIX:-${BENCH_NUMACTL:-}}"

benchmark_dir="${build_dir}/benchmarks/gmp/02_Rgemv"
if [[ ! -d "${benchmark_dir}" ]]; then
    echo "Benchmark directory not found: ${benchmark_dir}" >&2
    echo "Build first: cmake --build ${build_dir} -j" >&2
    exit 1
fi

mkdir -p "${output_dir}"
log_file="${output_dir}/benchmark_rgemv_gmp_m${m}_n${n}_p${precision}_repeat${repeat_count}.log"

executables=(
    "Rgemv_gmp_C_native_01"
    "Rgemv_gmp_C_native_02"
    "Rgemv_gmp_C_native_03"
    "Rgemv_gmp_C_native_04"
    "Rgemv_gmp_C_native_openmp_01"
    "Rgemv_gmp_C_native_openmp_02"
    "Rgemv_gmp_C_native_openmp_03"
    "Rgemv_gmp_C_native_openmp_04"
    "Rgemv_gmp_C_native_openmp_05"
    "Rgemv_gmp_C_native_openmp_06"
    "Rgemv_gmp_C_native_openmp_07"
    "Rgemv_gmp_kernel_01_orig"
    "Rgemv_gmp_kernel_01_mkII"
    "Rgemv_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_openmp_01_orig"
    "Rgemv_gmp_kernel_openmp_01_mkII"
    "Rgemv_gmp_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_02_orig"
    "Rgemv_gmp_kernel_02_mkII"
    "Rgemv_gmp_kernel_02_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_03_orig"
    "Rgemv_gmp_kernel_03_mkII"
    "Rgemv_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_04_orig"
    "Rgemv_gmp_kernel_04_mkII"
    "Rgemv_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_openmp_02_orig"
    "Rgemv_gmp_kernel_openmp_02_mkII"
    "Rgemv_gmp_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_openmp_03_orig"
    "Rgemv_gmp_kernel_openmp_03_mkII"
    "Rgemv_gmp_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_openmp_04_orig"
    "Rgemv_gmp_kernel_openmp_04_mkII"
    "Rgemv_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_openmp_05_orig"
    "Rgemv_gmp_kernel_openmp_05_mkII"
    "Rgemv_gmp_kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_openmp_06_orig"
    "Rgemv_gmp_kernel_openmp_06_mkII"
    "Rgemv_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH"
    "Rgemv_gmp_kernel_openmp_07_orig"
    "Rgemv_gmp_kernel_openmp_07_mkII"
    "Rgemv_gmp_kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH"
)

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
    if [[ -n "${benchmark_command_prefix}" ]]; then
        echo "BENCH_COMMAND_PREFIX ${benchmark_command_prefix}"
    fi
    echo

    for exe in "${executables[@]}"; do
        run_one "${exe}"
    done
} 2>&1 | tee "${log_file}"

python3 "${script_dir}/plot_repeat_summary.py" "${log_file}" \
    --output-dir "${output_dir}" \
    --output-prefix "rgemv_gmp_m${m}_n${n}_p${precision}_repeat${repeat_count}" \
    --title-prefix "GMP Rgemv m=${m} n=${n} precision=${precision} repeat=${repeat_count}"
