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
repo_dir="$(cd "${script_dir}/.." && pwd)"

build_dir="${1:-${repo_dir}/build_bench_release}"
precision="${2:-512}"
max_n="${3:-1024}"
small_threshold="${4:-128}"
small_repeat="${5:-5}"
large_repeat="${6:-1}"
backend_filter="${7:-both}"
step_size="${8:-${RGEMM_STEP:-23}}"

if ! [[ "${step_size}" =~ ^[1-9][0-9]*$ ]]; then
    echo "Invalid positive integer step size: ${step_size}" >&2
    exit 1
fi

export OMP_NUM_THREADS="${OMP_NUM_THREADS:-32}"
export OMP_PLACES="${OMP_PLACES:-cores}"
export OMP_PROC_BIND="${OMP_PROC_BIND:-spread}"
benchmark_command_prefix="${BENCH_COMMAND_PREFIX:-${BENCH_NUMACTL:-}}"
benchmark_nocheck="${BENCH_NOCHECK:-1}"
smoke_enabled="${BENCH_SMOKE:-1}"
smoke_check_enabled="${BENCH_SMOKE_CHECK:-1}"
smoke_nocheck_enabled="${BENCH_SMOKE_NOCHECK:-1}"
smoke_n="${BENCH_SMOKE_N:-13}"

benchmark_dir="${build_dir}/benchmarks"
if [[ ! -d "${benchmark_dir}" ]]; then
    echo "Benchmark directory not found: ${benchmark_dir}" >&2
    echo "Build first: cmake --build ${build_dir} -j" >&2
    exit 1
fi

timestamp="$(date +%Y%m%d_%H%M%S)"

sizes=()
add_size() {
    local n="$1"
    if (( n >= 1 && n <= max_n )); then
        sizes+=("${n}")
    fi
}

n=1
while (( n <= max_n )); do
    add_size "${n}"
    n=$((n * 2))
done

n="${step_size}"
while (( n <= max_n )); do
    add_size "${n}"
    n=$((n + step_size))
done

mapfile -t sizes < <(printf "%s\n" "${sizes[@]}" | sort -n -u)

gmp_targets=()
for variant in 01 02 03 04 05 06; do
    gmp_targets+=(
        "Rgemm_gmp_C_native_${variant}"
        "Rgemm_gmp_C_native_openmp_${variant}"
        "Rgemm_gmp_kernel_${variant}_orig"
        "Rgemm_gmp_kernel_${variant}_mkII"
        "Rgemm_gmp_kernel_${variant}_mkII_FIXED_PRECISION_FASTPATH"
        "Rgemm_gmp_kernel_openmp_${variant}_orig"
        "Rgemm_gmp_kernel_openmp_${variant}_mkII"
        "Rgemm_gmp_kernel_openmp_${variant}_mkII_FIXED_PRECISION_FASTPATH"
    )
done

mpfr_targets=()
for variant in 01 02; do
    mpfr_targets+=(
        "Rgemm_mpfr_C_native_${variant}"
        "Rgemm_mpfr_C_native_openmp_${variant}"
        "Rgemm_mpfr_kernel_${variant}_mkII"
        "Rgemm_mpfr_kernel_${variant}_mkII_PRECISION"
        "Rgemm_mpfr_kernel_${variant}_mkII_ROUNDING"
        "Rgemm_mpfr_kernel_${variant}_mkII_ROUNDING_PRECISION"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII_PRECISION"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII_ROUNDING"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII_ROUNDING_PRECISION"
    )
done

for variant in 03 04 05 06; do
    mpfr_targets+=(
        "Rgemm_mpfr_C_native_${variant}"
        "Rgemm_mpfr_C_native_${variant}_FMA"
        "Rgemm_mpfr_C_native_openmp_${variant}"
        "Rgemm_mpfr_C_native_openmp_${variant}_FMA"
        "Rgemm_mpfr_kernel_${variant}_mkII"
        "Rgemm_mpfr_kernel_${variant}_mkII_PRECISION"
        "Rgemm_mpfr_kernel_${variant}_mkII_ROUNDING"
        "Rgemm_mpfr_kernel_${variant}_mkII_ROUNDING_PRECISION"
        "Rgemm_mpfr_kernel_${variant}_mkII_ROUNDING_FMA"
        "Rgemm_mpfr_kernel_${variant}_mkII_ROUNDING_PRECISION_FMA"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII_PRECISION"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII_ROUNDING"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII_ROUNDING_PRECISION"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII_ROUNDING_FMA"
        "Rgemm_mpfr_kernel_openmp_${variant}_mkII_ROUNDING_PRECISION_FMA"
    )
done

want_backend() {
    local backend="$1"
    [[ "${backend_filter}" == "both" || "${backend_filter}" == "all" || "${backend_filter}" == "${backend}" ]]
}

repeat_for_size() {
    local size="$1"
    if (( size <= small_threshold )); then
        echo "${small_repeat}"
    else
        echo "${large_repeat}"
    fi
}

build_targets() {
    local targets=("$@")
    if ((${#targets[@]} == 0)); then
        return
    fi
    cmake --build "${build_dir}" -j --target "${targets[@]}"
}

summarize_csv() {
    local raw_csv="$1"
    local summary_csv="$2"
    python3 - "${raw_csv}" "${summary_csv}" <<'PY'
import csv
import statistics
import sys
from collections import defaultdict

raw_csv, summary_csv = sys.argv[1:3]
groups = defaultdict(list)
with open(raw_csv, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        groups[(row["size"], row["variant"])].append(row)

with open(summary_csv, "w", newline="") as f:
    fieldnames = [
        "size", "variant", "samples", "ok_samples",
        "min_elapsed_s", "avg_elapsed_s", "max_elapsed_s",
        "min_mflops", "avg_mflops", "max_mflops",
    ]
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()
    for (size, variant), rows in sorted(groups.items(), key=lambda item: (int(item[0][0]), item[0][1])):
        elapsed = [float(row["elapsed_s"]) for row in rows]
        mflops = [float(row["mflops"]) for row in rows]
        ok_samples = sum(1 for row in rows if row["status"] in {"OK", "SKIPPED"})
        writer.writerow({
            "size": size,
            "variant": variant,
            "samples": len(rows),
            "ok_samples": ok_samples,
            "min_elapsed_s": f"{min(elapsed):.9g}",
            "avg_elapsed_s": f"{statistics.fmean(elapsed):.9g}",
            "max_elapsed_s": f"{max(elapsed):.9g}",
            "min_mflops": f"{min(mflops):.9g}",
            "avg_mflops": f"{statistics.fmean(mflops):.9g}",
            "max_mflops": f"{max(mflops):.9g}",
        })
PY
}

run_smoke_command() {
    local mode="$1"
    local backend="$2"
    local target="$3"
    local env_name="$4"
    shift 4
    local command=("$@")

    if [[ -n "${benchmark_command_prefix}" ]]; then
        local prefix=()
        read -r -a prefix <<<"${benchmark_command_prefix}"
        command=("${prefix[@]}" "${command[@]}")
    fi
    command=(env "${env_name}=${precision}" "${command[@]}")

    echo "SMOKE_${mode} Rgemm ${backend} ${target} ${command[*]}"
    if "${command[@]}" >/dev/null; then
        echo "SMOKE_${mode}_OK Rgemm ${backend} ${target}"
    else
        echo "SMOKE_${mode}_FAILED Rgemm ${backend} ${target}" >&2
        return 1
    fi
}

run_smoke_backend() {
    local backend="$1"
    local subdir="$2"
    local env_name="$3"
    shift 3
    local targets=("$@")

    if [[ "${smoke_enabled}" == "0" ]]; then
        echo "SMOKE_SKIP Rgemm backend=${backend} BENCH_SMOKE=0"
        return
    fi

    echo "SMOKE_BEGIN Rgemm backend=${backend} n=${smoke_n} precision=${precision} targets=${#targets[@]}"
    for target in "${targets[@]}"; do
        local exe="${benchmark_dir}/${subdir}/${target}"
        if [[ ! -x "${exe}" ]]; then
            echo "Executable not found: ${exe}" >&2
            exit 1
        fi

        if [[ "${smoke_nocheck_enabled}" != "0" ]]; then
            run_smoke_command NOCHECK "${backend}" "${target}" "${env_name}" "${exe}" "${smoke_n}" "${smoke_n}" "${smoke_n}" "${precision}" -nocheck
        fi
        if [[ "${smoke_check_enabled}" != "0" ]]; then
            run_smoke_command CHECK "${backend}" "${target}" "${env_name}" "${exe}" "${smoke_n}" "${smoke_n}" "${smoke_n}" "${precision}"
        fi
    done
    echo "SMOKE_DONE Rgemm backend=${backend}"
}

run_backend() {
    local backend="$1"
    local subdir="$2"
    local env_name="$3"
    shift 3
    local targets=("$@")
    local run_id="rgemm_${backend}_all_pow2_${step_size}_p${precision}_repeat${large_repeat}_small${small_repeat}_${timestamp}"
    local out_dir="${script_dir}/${backend}/03_Rgemm/results_raw/${run_id}"
    local raw_csv="${out_dir}/raw_${run_id}.csv"
    local summary_csv="${out_dir}/summary_${run_id}.csv"
    local log_file="${out_dir}/benchmark_${run_id}.log"

    mkdir -p "${out_dir}"
    printf "size,variant,run,runs,elapsed_s,mflops,status\n" > "${raw_csv}"

    {
        uname -a
        if [[ -r /proc/cpuinfo ]]; then
            grep 'model name' /proc/cpuinfo | head -1
        else
            uname -m
        fi
        echo "BENCHMARK_PARAMS backend=${backend} precision=${precision} max_n=${max_n} step=${step_size} small_threshold=${small_threshold} small_repeat=${small_repeat} large_repeat=${large_repeat}"
        echo "SIZE_SET ${sizes[*]}"
        echo "TARGET_COUNT ${#targets[@]}"
        echo "OPENMP_AFFINITY OMP_NUM_THREADS=${OMP_NUM_THREADS} OMP_PLACES=${OMP_PLACES} OMP_PROC_BIND=${OMP_PROC_BIND}"
        echo "DEFAULT_PRECISION_ENV ${env_name}=${precision}"
        echo "TIMED_NOCHECK enabled=${benchmark_nocheck}"
        echo "SMOKE_TESTS enabled=${smoke_enabled} check=${smoke_check_enabled} nocheck=${smoke_nocheck_enabled} n=${smoke_n}"
        if [[ -n "${benchmark_command_prefix}" ]]; then
            echo "BENCH_COMMAND_PREFIX ${benchmark_command_prefix}"
        fi
        echo

        run_smoke_backend "${backend}" "${subdir}" "${env_name}" "${targets[@]}"
        echo

        for size in "${sizes[@]}"; do
            local repeats
            repeats="$(repeat_for_size "${size}")"
            echo "SIZE_BEGIN n=${size} repeats=${repeats}"
            for target in "${targets[@]}"; do
                local exe="${benchmark_dir}/${subdir}/${target}"
                if [[ ! -x "${exe}" ]]; then
                    echo "Executable not found: ${exe}" >&2
                    exit 1
                fi
                for ((run = 1; run <= repeats; ++run)); do
                    local command=("${exe}" "${size}" "${size}" "${size}" "${precision}")
                    if [[ "${benchmark_nocheck}" != "0" ]]; then
                        command+=("-nocheck")
                    fi
                    if [[ -n "${benchmark_command_prefix}" ]]; then
                        local prefix=()
                        read -r -a prefix <<<"${benchmark_command_prefix}"
                        command=("${prefix[@]}" "${command[@]}")
                    fi
                    command=(env "${env_name}=${precision}" "${command[@]}")

                    echo "COMMAND Rgemm ${target} n=${size} run=${run}/${repeats}"
                    local output
                    output="$("${command[@]}" 2>&1)"
                    printf "%s\n" "${output}"

                    local elapsed
                    local mflops
                    local status
                    elapsed="$(printf "%s\n" "${output}" | awk '/^Elapsed time:/ {print $3; exit}')"
                    mflops="$(printf "%s\n" "${output}" | awk '/^MFLOPS:/ {print $2; exit}')"
                    status="NG"
                    if printf "%s\n" "${output}" | grep -q 'Result OK\|Check passed'; then
                        status="OK"
                    elif printf "%s\n" "${output}" | grep -q 'Check skipped\.'; then
                        status="SKIPPED"
                    fi
                    printf "%s,%s,%s,%s,%s,%s,%s\n" \
                        "${size}" "${target}" "${run}" "${repeats}" \
                        "${elapsed}" "${mflops}" "${status}" >> "${raw_csv}"
                    echo
                    if [[ "${status}" == "NG" ]]; then
                        echo "Benchmark failed: backend=${backend} n=${size} target=${target} run=${run}" >&2
                        exit 1
                    fi
                done
            done
        done
    } 2>&1 | tee "${log_file}"

    summarize_csv "${raw_csv}" "${summary_csv}"
    echo "RAW_CSV ${raw_csv}"
    echo "SUMMARY_CSV ${summary_csv}"
    echo "LOG ${log_file}"
}

targets_to_build=()
if want_backend gmp; then
    targets_to_build+=("${gmp_targets[@]}")
fi
if want_backend mpfr; then
    targets_to_build+=("${mpfr_targets[@]}")
fi

build_targets "${targets_to_build[@]}"

if want_backend gmp; then
    run_backend gmp gmp/03_Rgemm GMPXX_DEFAULT_MPF_PRECISION_BITS "${gmp_targets[@]}"
fi
if want_backend mpfr; then
    run_backend mpfr mpfr/03_Rgemm MPFRXX_DEFAULT_PRECISION_BITS "${mpfr_targets[@]}"
fi
