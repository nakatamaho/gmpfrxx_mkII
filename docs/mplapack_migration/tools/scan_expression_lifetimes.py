#!/usr/bin/env python3
"""Find likely saved gmpfrxx_mkII expression nodes in downstream sources."""

from __future__ import annotations

import argparse
import pathlib
import re
import sys


SOURCE_SUFFIXES = {".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp"}
AUTO_ASSIGNMENT = re.compile(r"\bauto\s+([A-Za-z_]\w*)\s*=\s*(.+);")
WRAPPER_TOKEN = re.compile(
    r"\b(?:mpfrxx|gmpxx)::|"
    r"\b(?:mpfr_class|mpc_class|mpf_class|mpfc_class)\b"
)
ARITHMETIC = re.compile(r"(?:\+|-|\*|/)")


def scan(root: pathlib.Path) -> list[str]:
    findings: list[str] = []
    for path in sorted(root.rglob("*")):
        if not path.is_file() or path.suffix not in SOURCE_SUFFIXES:
            continue
        relative = path.relative_to(root)
        if any(part in {".git", "build", "_build"} or part.startswith("build-")
               for part in relative.parts):
            continue
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except UnicodeDecodeError:
            continue
        for line_number, line in enumerate(lines, 1):
            match = AUTO_ASSIGNMENT.search(line)
            if not match:
                continue
            expression = match.group(2)
            if WRAPPER_TOKEN.search(expression) and ARITHMETIC.search(expression):
                findings.append(
                    f"{relative}:{line_number}: saved expression candidate: "
                    f"{line.strip()}"
                )
    return findings


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("root", type=pathlib.Path)
    args = parser.parse_args()
    root = args.root.resolve()
    if not root.is_dir():
        parser.error(f"not a directory: {root}")
    findings = scan(root)
    for finding in findings:
        print(finding)
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main())
