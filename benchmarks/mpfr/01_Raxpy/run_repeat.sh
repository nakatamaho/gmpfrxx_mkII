#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_dir="$(cd "${script_dir}/../../.." && pwd)"

build_dir="${1:-${repo_dir}/build_bench_release}"
n="${2:-10000000}"
precision="${3:-512}"
repeat_count="${4:-10}"
output_dir="${5:-${script_dir}/results_raw/raxpy_mpfr_n${n}_p${precision}_repeat${repeat_count}_$(date +%Y%m%d_%H%M%S)}"

export OMP_NUM_THREADS="${OMP_NUM_THREADS:-32}"
export OMP_PLACES="${OMP_PLACES:-cores}"
export OMP_PROC_BIND="${OMP_PROC_BIND:-spread}"
benchmark_command_prefix="${BENCH_COMMAND_PREFIX:-${BENCH_NUMACTL:-}}"

benchmark_dir="${build_dir}/benchmarks/mpfr/01_Raxpy"
if [[ ! -d "${benchmark_dir}" ]]; then
    echo "Benchmark directory not found: ${benchmark_dir}" >&2
    echo "Build first: cmake --build ${build_dir} -j" >&2
    exit 1
fi

mkdir -p "${output_dir}"
log_file="${output_dir}/benchmark_raxpy_mpfr_n${n}_p${precision}_repeat${repeat_count}.log"

executables=(
    "Raxpy_mpfr_C_native_01"
    "Raxpy_mpfr_C_native_01_FMA"
    "Raxpy_mpfr_C_native_packed_custom_layout_FMA"
    "Raxpy_mpfr_C_native_openmp_01"
    "Raxpy_mpfr_C_native_openmp_01_FMA"
)

append_cpp_variant_family() {
    local base="$1"
    executables+=(
        "${base}"
        "${base}_PRECISION"
        "${base}_ROUNDING"
        "${base}_ROUNDING_PRECISION"
    )
}

append_cpp_fma_variant_family() {
    local base="$1"
    append_cpp_variant_family "${base}"
    executables+=(
        "${base}_PRECISION_FMA"
        "${base}_ROUNDING_PRECISION_FMA"
    )
}

append_cpp_fma_variant_family "Raxpy_mpfr_kernel_01"
for variant in 02 03 04; do
    append_cpp_variant_family "Raxpy_mpfr_kernel_${variant}"
done

append_cpp_fma_variant_family "Raxpy_mpfr_kernel_openmp_01"
for variant in 02 03 04; do
    append_cpp_variant_family "Raxpy_mpfr_kernel_openmp_${variant}"
done
run_one() {
    local exe="$1"
    local path="${benchmark_dir}/${exe}"
    if [[ ! -x "${path}" ]]; then
        echo "Executable not found: ${path}" >&2
        exit 1
    fi

    for ((run = 1; run <= repeat_count; ++run)); do
        local command=("${path}" "${n}" "${precision}")
        if [[ -n "${benchmark_command_prefix}" ]]; then
            local prefix=()
            read -r -a prefix <<<"${benchmark_command_prefix}"
            command=("${prefix[@]}" "${command[@]}")
        fi
        command=(env "MPFRXX_DEFAULT_PRECISION_BITS=${precision}" "${command[@]}")

        echo "COMMAND Raxpy ${exe} ${command[*]}"
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
    echo "BENCHMARK_PARAMS precision=${precision} raxpy_n=${n} repeat=${repeat_count}"
    echo "OPENMP_AFFINITY OMP_NUM_THREADS=${OMP_NUM_THREADS} OMP_PLACES=${OMP_PLACES} OMP_PROC_BIND=${OMP_PROC_BIND}"
    echo "DEFAULT_PRECISION_ENV MPFRXX_DEFAULT_PRECISION_BITS=${precision}"
    if [[ -n "${benchmark_command_prefix}" ]]; then
        echo "BENCH_COMMAND_PREFIX ${benchmark_command_prefix}"
    fi
    echo

    for exe in "${executables[@]}"; do
        run_one "${exe}"
    done
} 2>&1 | tee "${log_file}"

python3 "${script_dir}/plot_repeat_summary.py" "${log_file}"     --output-dir "${output_dir}"     --output-prefix "raxpy_mpfr_n${n}_p${precision}_repeat${repeat_count}"     --title-prefix "MPFR Raxpy N=${n} precision=${precision} repeat=${repeat_count}"
