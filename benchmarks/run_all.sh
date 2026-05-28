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
repo_dir="$(cd "${script_dir}/.." && pwd)"

build_dir="${1:-${repo_dir}/build_bench_release}"
precision_arg="${2:-512,1024}"
repeat_count="${3:-10}"
rdot_n="${4:-10000000}"
raxpy_n="${5:-${rdot_n}}"
rgemv_m="${6:-4000}"
rgemv_n="${7:-4000}"
output_root="${8:-}"
run_stamp="${RUN_ALL_STAMP:-$(date +%Y%m%d_%H%M%S)}"

precisions=()
case "${precision_arg}" in
    all | both)
        precision_list="512 1024"
        ;;
    *)
        precision_list="${precision_arg//,/ }"
        ;;
esac

for precision in ${precision_list}; do
    if [[ ! "${precision}" =~ ^[0-9]+$ ]]; then
        echo "Invalid precision '${precision}' in '${precision_arg}'" >&2
        exit 1
    fi
    precisions+=("${precision}")
done

if [[ "${#precisions[@]}" -eq 0 ]]; then
    echo "No precision values requested" >&2
    exit 1
fi

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

run_id_for_precision() {
    local precision="$1"

    if [[ -n "${RUN_ALL_ID:-}" ]]; then
        if [[ "${#precisions[@]}" -gt 1 ]]; then
            printf "%s_p%s\n" "${RUN_ALL_ID}" "${precision}"
        else
            printf "%s\n" "${RUN_ALL_ID}"
        fi
    else
        printf "run_all_p%s_repeat%s_%s\n" "${precision}" "${repeat_count}" "${run_stamp}"
    fi
}

output_dir_for() {
    local rel_dir="$1"
    local run_id="$2"
    local flat_label="${rel_dir//\//_}"

    if [[ -n "${output_root}" ]]; then
        if [[ "${#precisions[@]}" -gt 1 ]]; then
            printf "%s/%s/%s\n" "${output_root}" "${flat_label}" "${run_id}"
        else
            printf "%s/%s\n" "${output_root}" "${flat_label}"
        fi
    else
        printf "%s/%s/results_raw/%s\n" "${script_dir}" "${rel_dir}" "${run_id}"
    fi
}

run_precision_suite() {
    local precision="$1"
    local run_id
    run_id="$(run_id_for_precision "${precision}")"

    echo "RUN_ALL_PRECISION_BEGIN precision=${precision} run_id=${run_id}"

    run_subdir "gmp/00_Rdot" \
        "${script_dir}/gmp/00_Rdot/run_repeat.sh" \
        "$(output_dir_for "gmp/00_Rdot" "${run_id}")" \
        "${build_dir}" "${rdot_n}" "${precision}" "${repeat_count}"

    run_subdir "gmp/01_Raxpy" \
        "${script_dir}/gmp/01_Raxpy/run_repeat.sh" \
        "$(output_dir_for "gmp/01_Raxpy" "${run_id}")" \
        "${build_dir}" "${raxpy_n}" "${precision}" "${repeat_count}"

    run_subdir "gmp/02_Rgemv" \
        "${script_dir}/gmp/02_Rgemv/run_repeat.sh" \
        "$(output_dir_for "gmp/02_Rgemv" "${run_id}")" \
        "${build_dir}" "${rgemv_m}" "${rgemv_n}" "${precision}" "${repeat_count}"

    run_subdir "mpfr/00_Rdot" \
        "${script_dir}/mpfr/00_Rdot/run_repeat.sh" \
        "$(output_dir_for "mpfr/00_Rdot" "${run_id}")" \
        "${build_dir}" "${rdot_n}" "${precision}" "${repeat_count}"

    run_subdir "mpfr/01_Raxpy" \
        "${script_dir}/mpfr/01_Raxpy/run_repeat.sh" \
        "$(output_dir_for "mpfr/01_Raxpy" "${run_id}")" \
        "${build_dir}" "${raxpy_n}" "${precision}" "${repeat_count}"

    run_subdir "mpfr/02_Rgemv" \
        "${script_dir}/mpfr/02_Rgemv/run_repeat.sh" \
        "$(output_dir_for "mpfr/02_Rgemv" "${run_id}")" \
        "${build_dir}" "${rgemv_m}" "${rgemv_n}" "${precision}" "${repeat_count}"

    echo "RUN_ALL_PRECISION_DONE precision=${precision} run_id=${run_id}"
    echo
}


echo "RUN_ALL build_dir=${build_dir} precisions=${precisions[*]} repeat=${repeat_count}"
echo "RUN_ALL rdot_n=${rdot_n} raxpy_n=${raxpy_n} rgemv_m=${rgemv_m} rgemv_n=${rgemv_n}"
echo "RUN_ALL run_stamp=${run_stamp}"
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

for precision in "${precisions[@]}"; do
    run_precision_suite "${precision}"
done

echo "RUN_ALL_DONE precisions=${precisions[*]} run_stamp=${run_stamp}"
