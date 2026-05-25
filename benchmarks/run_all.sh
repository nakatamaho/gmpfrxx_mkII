#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2-Clause

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_dir="$(cd "${script_dir}/.." && pwd)"

build_dir="${1:-${repo_dir}/build_bench_release}"
precision="${2:-512}"
repeat_count="${3:-10}"
rdot_n="${4:-10000000}"
raxpy_n="${5:-${rdot_n}}"
rgemv_m="${6:-4000}"
rgemv_n="${7:-4000}"
output_root="${8:-}"
run_id="${RUN_ALL_ID:-run_all_p${precision}_repeat${repeat_count}_$(date +%Y%m%d_%H%M%S)}"

export OMP_NUM_THREADS="${OMP_NUM_THREADS:-32}"
export OMP_PLACES="${OMP_PLACES:-cores}"
export OMP_PROC_BIND="${OMP_PROC_BIND:-spread}"

run_subdir() {
    local label="$1"
    shift
    local runner="$1"
    shift
    local out_dir="$1"
    shift

    if [[ ! -x "${runner}" ]]; then
        echo "Runner not found or not executable: ${runner}" >&2
        exit 1
    fi

    if compgen -G "${out_dir}/summary_*.csv" >/dev/null; then
        echo "SKIP_EXISTING ${label} ${out_dir}"
        return
    fi

    mkdir -p "${out_dir}"
    echo "===== ${label} ====="
    echo "RUNNER ${runner} $* ${out_dir}"
    "${runner}" "$@" "${out_dir}"
}

output_dir_for() {
    local rel_dir="$1"
    local flat_label="${rel_dir//\//_}"

    if [[ -n "${output_root}" ]]; then
        printf "%s/%s\n" "${output_root}" "${flat_label}"
    else
        printf "%s/%s/results_raw/%s\n" "${script_dir}" "${rel_dir}" "${run_id}"
    fi
}


echo "RUN_ALL build_dir=${build_dir} precision=${precision} repeat=${repeat_count}"
echo "RUN_ALL rdot_n=${rdot_n} raxpy_n=${raxpy_n} rgemv_m=${rgemv_m} rgemv_n=${rgemv_n}"
echo "RUN_ALL run_id=${run_id}"
if [[ -n "${output_root}" ]]; then
    echo "RUN_ALL output_root=${output_root}"
else
    echo "RUN_ALL output_root=per-benchmark results_raw"
fi
echo "OPENMP_AFFINITY OMP_NUM_THREADS=${OMP_NUM_THREADS} OMP_PLACES=${OMP_PLACES} OMP_PROC_BIND=${OMP_PROC_BIND}"
if [[ -n "${BENCH_COMMAND_PREFIX:-${BENCH_NUMACTL:-}}" ]]; then
    echo "BENCH_COMMAND_PREFIX ${BENCH_COMMAND_PREFIX:-${BENCH_NUMACTL:-}}"
fi
echo "SKIP gemm benchmarks: gmp/03_Rgemm mpfr/03_Rgemm"
echo

run_subdir "gmp/00_Rdot" \
    "${script_dir}/gmp/00_Rdot/run_repeat.sh" \
    "$(output_dir_for "gmp/00_Rdot")" \
    "${build_dir}" "${rdot_n}" "${precision}" "${repeat_count}"

run_subdir "gmp/01_Raxpy" \
    "${script_dir}/gmp/01_Raxpy/run_repeat.sh" \
    "$(output_dir_for "gmp/01_Raxpy")" \
    "${build_dir}" "${raxpy_n}" "${precision}" "${repeat_count}"

run_subdir "gmp/02_Rgemv" \
    "${script_dir}/gmp/02_Rgemv/run_repeat.sh" \
    "$(output_dir_for "gmp/02_Rgemv")" \
    "${build_dir}" "${rgemv_m}" "${rgemv_n}" "${precision}" "${repeat_count}"

run_subdir "mpfr/00_Rdot" \
    "${script_dir}/mpfr/00_Rdot/run_repeat.sh" \
    "$(output_dir_for "mpfr/00_Rdot")" \
    "${build_dir}" "${rdot_n}" "${precision}" "${repeat_count}"

run_subdir "mpfr/01_Raxpy" \
    "${script_dir}/mpfr/01_Raxpy/run_repeat.sh" \
    "$(output_dir_for "mpfr/01_Raxpy")" \
    "${build_dir}" "${raxpy_n}" "${precision}" "${repeat_count}"

run_subdir "mpfr/02_Rgemv" \
    "${script_dir}/mpfr/02_Rgemv/run_repeat.sh" \
    "$(output_dir_for "mpfr/02_Rgemv")" \
    "${build_dir}" "${rgemv_m}" "${rgemv_n}" "${precision}" "${repeat_count}"

echo "RUN_ALL_DONE run_id=${run_id}"
