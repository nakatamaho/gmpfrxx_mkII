#!/usr/bin/env python3
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

"""Check representative benchmark hotpath backend-call equivalence.

This is intentionally not a byte-for-byte disassembly comparison. The benchmark
READMEs define equivalence by source shape and backend call class: for example,
an MPFR FMA wrapper hot loop should contain the same `mpfr_fma` class as the raw
C FMA kernel, while a reusable-object split path should contain `mpfr_mul`
followed by `mpfr_add`. Address values and surrounding prologue/control-flow
are build-specific and are not stable regression-test material. The parser
is intentionally limited to Linux amd64 objdump output and recognizes x86-64
`call` and `callq` direct calls to PLT symbols. Selected no-temporary
cases also assert that backend init/clear calls are absent from the selected
function.
"""

from __future__ import annotations

import argparse
import dataclasses
import pathlib
import re
import subprocess
import sys
from typing import Iterable


FUNCTION_HEADER_RE = re.compile(r"^[0-9a-f]+ <(.+)>:$")
CALL_TARGET_RE = re.compile(r"\bcall\w*\s+(?:[0-9a-f]+ )?<([^>]+)>")


def normalize_call_target(target: str) -> str | None:
    target = target.split("@", 1)[0]
    if target.startswith("__gmpf_"):
        return "mpf_" + target[len("__gmpf_") :]
    if target.startswith("__gmp_"):
        return "gmp_" + target[len("__gmp_") :]
    if target.startswith("mpfr_"):
        return target
    return None


@dataclasses.dataclass(frozen=True)
class BinarySpec:
    relpath: str
    symbol: str
    forbidden_calls: tuple[str, ...] = ()


@dataclasses.dataclass(frozen=True)
class EquivalenceCase:
    name: str
    binaries: tuple[BinarySpec, ...]
    required_sequence: tuple[str, ...]
    forbidden_calls: tuple[str, ...] = ()


CASES: tuple[EquivalenceCase, ...] = (
    EquivalenceCase(
        "gmp/00_Rdot reusable split multiply/add",
        (
            BinarySpec("gmp/00_Rdot/Rdot_gmp_C_native_03", "_Rdot("),
            BinarySpec("gmp/00_Rdot/Rdot_gmp_kernel_03_mkII", "_Rdot("),
            BinarySpec(
                "gmp/00_Rdot/Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH",
                "_Rdot(",
            ),
        ),
        ("mpf_mul", "mpf_add"),
    ),
    EquivalenceCase(
        "mpfr/00_Rdot FMA expression",
        (
            BinarySpec("mpfr/00_Rdot/Rdot_mpfr_C_native_01_FMA", "_Rdot("),
            BinarySpec(
                "mpfr/00_Rdot/Rdot_mpfr_kernel_01_ROUNDING_PRECISION_FMA",
                "_Rdot(",
            ),
        ),
        ("mpfr_fma",),
    ),
    EquivalenceCase(
        "mpfr/00_Rdot reusable split multiply/add",
        (
            BinarySpec("mpfr/00_Rdot/Rdot_mpfr_C_native_03", "_Rdot("),
            BinarySpec("mpfr/00_Rdot/Rdot_mpfr_kernel_03_ROUNDING_PRECISION", "_Rdot("),
        ),
        ("mpfr_mul", "mpfr_add"),
    ),
    EquivalenceCase(
        "gmp/01_Raxpy reusable split multiply/add",
        (
            BinarySpec("gmp/01_Raxpy/Raxpy_gmp_C_native_03", "_Raxpy("),
            BinarySpec("gmp/01_Raxpy/Raxpy_gmp_kernel_03_mkII", "_Raxpy("),
            BinarySpec(
                "gmp/01_Raxpy/Raxpy_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH",
                "_Raxpy(",
            ),
        ),
        ("mpf_mul", "mpf_add"),
    ),
    EquivalenceCase(
        "mpfr/01_Raxpy FMA expression",
        (
            BinarySpec("mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA", "_Raxpy("),
            BinarySpec(
                "mpfr/01_Raxpy/Raxpy_mpfr_kernel_01_ROUNDING_PRECISION_FMA",
                "_Raxpy(",
                ("mpfr_init2", "mpfr_clear"),
            ),
        ),
        ("mpfr_fma",),
    ),
    EquivalenceCase(
        "mpfr/01_Raxpy reusable split multiply/add",
        (
            BinarySpec("mpfr/01_Raxpy/Raxpy_mpfr_C_native_03", "_Raxpy("),
            BinarySpec("mpfr/01_Raxpy/Raxpy_mpfr_kernel_03_ROUNDING_PRECISION", "_Raxpy("),
        ),
        ("mpfr_mul", "mpfr_add"),
    ),
    EquivalenceCase(
        "gmp/02_Rgemv reusable split multiply/add",
        (
            BinarySpec("gmp/02_Rgemv/Rgemv_gmp_C_native_03", "_Rgemv("),
            BinarySpec("gmp/02_Rgemv/Rgemv_gmp_kernel_03_mkII", "_Rgemv("),
            BinarySpec(
                "gmp/02_Rgemv/Rgemv_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH",
                "_Rgemv(",
            ),
        ),
        ("mpf_mul", "mpf_add"),
    ),
    EquivalenceCase(
        "gmp/02_Rgemv OpenMP column-partition split multiply/add",
        (
            BinarySpec("gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_07", "_omp_fn"),
            BinarySpec("gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_07_mkII", "_omp_fn"),
            BinarySpec(
                "gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH",
                "_omp_fn",
            ),
        ),
        ("mpf_mul", "mpf_add"),
    ),
    EquivalenceCase(
        "mpfr/02_Rgemv column-major FMA",
        (
            BinarySpec("mpfr/02_Rgemv/Rgemv_mpfr_C_native_02_FMA", "_Rgemv("),
            BinarySpec(
                "mpfr/02_Rgemv/Rgemv_mpfr_kernel_02_ROUNDING_FMA_CAPTURE_PRECISION_FMA",
                "_Rgemv(",
            ),
        ),
        ("mpfr_mul", "mpfr_fma"),
    ),
    EquivalenceCase(
        "mpfr/02_Rgemv OpenMP column-partition FMA",
        (
            BinarySpec("mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_07_FMA", "_omp_fn"),
            BinarySpec(
                "mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07_ROUNDING_FMA_CAPTURE_PRECISION_FMA",
                "_omp_fn",
            ),
        ),
        ("mpfr_mul", "mpfr_fma"),
    ),
    EquivalenceCase(
        "gmp/03_Rgemm reusable split multiply/add",
        (
            BinarySpec("gmp/03_Rgemm/Rgemm_gmp_C_native_03", "_Rgemm("),
            BinarySpec("gmp/03_Rgemm/Rgemm_gmp_kernel_03_mkII", "_Rgemm("),
            BinarySpec(
                "gmp/03_Rgemm/Rgemm_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH",
                "_Rgemm(",
            ),
        ),
        ("mpf_set", "mpf_mul", "mpf_add"),
    ),
    EquivalenceCase(
        "gmp/03_Rgemm panel split multiply/add",
        (
            BinarySpec("gmp/03_Rgemm/Rgemm_gmp_C_native_05", "_Rgemm("),
            BinarySpec("gmp/03_Rgemm/Rgemm_gmp_kernel_05_mkII", "_Rgemm("),
            BinarySpec(
                "gmp/03_Rgemm/Rgemm_gmp_kernel_05_mkII_FIXED_PRECISION_FASTPATH",
                "_Rgemm(",
            ),
        ),
        ("mpf_mul", "mpf_add"),
    ),
    EquivalenceCase(
        "mpfr/03_Rgemm reusable split multiply/add",
        (
            BinarySpec("mpfr/03_Rgemm/Rgemm_mpfr_C_native_03", "_Rgemm("),
            BinarySpec(
                "mpfr/03_Rgemm/Rgemm_mpfr_kernel_03_mkII_ROUNDING_PRECISION",
                "_Rgemm(",
            ),
        ),
        ("mpfr_mul", "mpfr_add"),
    ),
    EquivalenceCase(
        "mpfr/03_Rgemm reusable FMA",
        (
            BinarySpec("mpfr/03_Rgemm/Rgemm_mpfr_C_native_03_FMA", "_Rgemm("),
            BinarySpec(
                "mpfr/03_Rgemm/Rgemm_mpfr_kernel_03_mkII_ROUNDING_PRECISION_FMA",
                "_Rgemm(",
            ),
        ),
        ("mpfr_mul", "mpfr_fma"),
    ),
)


def resolve_binary(build_dir: pathlib.Path, relpath: str) -> pathlib.Path:
    candidates = (
        build_dir / "benchmarks" / relpath,
        build_dir / relpath,
    )
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    raise FileNotFoundError(
        f"missing benchmark binary {relpath}; tried "
        + ", ".join(str(path) for path in candidates)
    )


def run_objdump(objdump: str, binary: pathlib.Path) -> str:
    result = subprocess.run(
        [objdump, "-Cd", "--no-show-raw-insn", str(binary)],
        check=True,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    return result.stdout


def split_functions(disassembly: str) -> list[tuple[str, list[str]]]:
    functions: list[tuple[str, list[str]]] = []
    current_name: str | None = None
    current_body: list[str] = []
    for line in disassembly.splitlines():
        match = FUNCTION_HEADER_RE.match(line)
        if match:
            if current_name is not None:
                functions.append((current_name, current_body))
            current_name = match.group(1)
            current_body = []
            continue
        if current_name is not None:
            current_body.append(line)
    if current_name is not None:
        functions.append((current_name, current_body))
    return functions


def select_function(functions: Iterable[tuple[str, list[str]]], symbol: str) -> tuple[str, list[str]]:
    matches: list[tuple[str, list[str]]] = []
    for name, body in functions:
        if symbol not in name:
            continue
        if "[clone .cold]" in name or "[clone .part" in name:
            continue
        if "run_" in name:
            continue
        matches.append((name, body))
    if not matches:
        raise LookupError(f"could not find non-cold function containing {symbol!r}")
    matches.sort(key=lambda item: (len(item[0]), item[0]))
    return matches[0]


def backend_calls(function_body: Iterable[str]) -> list[str]:
    calls: list[str] = []
    for line in function_body:
        match = CALL_TARGET_RE.search(line)
        if not match:
            continue
        target = normalize_call_target(match.group(1))
        if target is not None:
            calls.append(target)
    return calls


def contains_subsequence(calls: list[str], expected: tuple[str, ...]) -> bool:
    if not expected:
        return True
    index = 0
    for call in calls:
        if call == expected[index]:
            index += 1
            if index == len(expected):
                return True
    return False


def check_case(case: EquivalenceCase, build_dir: pathlib.Path, objdump: str) -> list[str]:
    reports: list[str] = []
    for binary_spec in case.binaries:
        binary = resolve_binary(build_dir, binary_spec.relpath)
        disassembly = run_objdump(objdump, binary)
        function_name, function_body = select_function(
            split_functions(disassembly), binary_spec.symbol
        )
        calls = backend_calls(function_body)
        if not contains_subsequence(calls, case.required_sequence):
            sequence = " -> ".join(case.required_sequence)
            observed = ", ".join(calls) if calls else "<none>"
            raise AssertionError(
                f"{case.name}: {binary_spec.relpath}::{function_name} does not "
                f"contain backend call sequence {sequence}; observed {observed}"
            )
        forbidden_calls = case.forbidden_calls + binary_spec.forbidden_calls
        forbidden_hits = [call for call in forbidden_calls if call in calls]
        if forbidden_hits:
            observed = ", ".join(calls) if calls else "<none>"
            forbidden = ", ".join(forbidden_hits)
            raise AssertionError(
                f"{case.name}: {binary_spec.relpath}::{function_name} contains "
                f"forbidden backend calls {forbidden}; observed {observed}"
            )
        forbidden_note = ""
        if forbidden_calls:
            forbidden_note = (
                f"; forbidden absent: {', '.join(forbidden_calls)}"
            )
        reports.append(
            f"  {binary_spec.relpath}::{function_name}: "
            f"{' -> '.join(case.required_sequence)}{forbidden_note}"
        )
    return reports


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--build-dir",
        default="build_bench_release",
        type=pathlib.Path,
        help="CMake build directory or its benchmarks subdirectory",
    )
    parser.add_argument("--objdump", default="objdump", help="objdump executable")
    args = parser.parse_args()

    failures: list[str] = []
    for case in CASES:
        try:
            reports = check_case(case, args.build_dir, args.objdump)
        except Exception as exc:  # noqa: BLE001 - test runner prints all failures.
            failures.append(f"FAIL {case.name}: {exc}")
            continue
        print(f"PASS {case.name}")
        for report in reports:
            print(report)

    if failures:
        for failure in failures:
            print(failure, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
