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
repo_dir="$(cd "${script_dir}/../.." && pwd)"

n="${1:-100000000}"
repeats="${2:-20}"
run_repeats="${3:-3}"
output_dir="${4:-${repo_dir}/benchmarks/results_raw/stream_like}"
binary="${5:-/tmp/gmpfrxx_stream_like_omp}"

mkdir -p "${output_dir}"
log_file="${output_dir}/stream_like_$(date +%Y%m%d_%H%M%S).log"

cxx="${CXX:-g++}"
"${cxx}" -O3 -march=native -fopenmp "${script_dir}/stream_like_omp.cpp" -o "${binary}"

{
    uname -a
    if command -v lscpu >/dev/null 2>&1; then
        lscpu | grep -E 'CPU\(s\)|Thread|Core|Socket|NUMA|Model name|L[123] cache'
    fi
    echo "STREAM_PARAMS n=${n} repeats=${repeats} run_repeats=${run_repeats} binary=${binary}"
    echo

    for bind in spread close; do
        for ((run = 1; run <= run_repeats; ++run)); do
            echo "=== bind=${bind} run=${run}/${run_repeats} ==="
            OMP_NUM_THREADS="${OMP_NUM_THREADS:-32}" \
                OMP_PLACES="${OMP_PLACES:-cores}" \
                OMP_PROC_BIND="${bind}" \
                "${binary}" "${n}" "${repeats}"
            echo
        done
    done

    if [[ "${RUN_STREAM_SMT:-1}" != "0" ]]; then
        echo "=== bind=spread threads=64 smt ==="
        OMP_NUM_THREADS=64 \
            OMP_PLACES=threads \
            OMP_PROC_BIND=spread \
            "${binary}" "${n}" "${repeats}"
        echo
    fi
} 2>&1 | tee "${log_file}"

echo "Wrote ${log_file}"
