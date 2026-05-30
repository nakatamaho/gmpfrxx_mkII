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

import argparse
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def is_openmp(variant):
    return "_openmp" in variant


def implementation(variant):
    if "C_native" in variant:
        return "C native"
    if "_orig" in variant:
        return "orig"
    if "_mkII" in variant:
        return "mkII"
    return "other"


def read_summary(path, backend):
    rows = []
    with path.open(newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(
                {
                    "backend": backend,
                    "size": int(row["size"]),
                    "variant": row["variant"],
                    "mode": "OpenMP" if is_openmp(row["variant"]) else "serial",
                    "implementation": implementation(row["variant"]),
                    "max_mflops": float(row["max_mflops"]),
                    "avg_mflops": float(row["avg_mflops"]),
                }
            )
    return rows


def best_series(rows, backend, mode, implementation_name):
    sizes = sorted({row["size"] for row in rows})
    points = []
    for size in sizes:
        candidates = [
            row
            for row in rows
            if row["backend"] == backend
            and row["mode"] == mode
            and row["implementation"] == implementation_name
            and row["size"] == size
        ]
        if not candidates:
            continue
        best = max(candidates, key=lambda row: row["max_mflops"])
        points.append((size, best["max_mflops"], best["variant"]))
    return points


def plot_panel(ax, rows, mode):
    series = [
        ("GMP C native", best_series(rows, "GMP", mode, "C native"), "#666666", "-", "o"),
        ("GMP orig", best_series(rows, "GMP", mode, "orig"), "#2ca02c", "-", "^"),
        ("GMP mkII", best_series(rows, "GMP", mode, "mkII"), "#4c78a8", "-", "s"),
        ("MPFR C native", best_series(rows, "MPFR", mode, "C native"), "#666666", "--", "o"),
        ("MPFR mkII", best_series(rows, "MPFR", mode, "mkII"), "#ff7f0e", "-", "D"),
    ]
    for label, points, color, linestyle, marker in series:
        if not points:
            continue
        xs = [point[0] for point in points]
        ys = [point[1] for point in points]
        ax.plot(
            xs,
            ys,
            label=label,
            color=color,
            linestyle=linestyle,
            marker=marker,
            markersize=4.0,
            linewidth=1.8,
            alpha=0.92,
        )

    for size in [128, 256, 512, 1024]:
        ax.axvline(size, color="#dddddd", linewidth=0.8, zorder=0)

    ax.set_title(f"Best {mode} Rgemm performance by implementation family", fontsize=13, fontweight="bold")
    ax.set_xlabel("Square matrix size N = M = K")
    ax.set_ylabel("Max MFLOPS")
    ax.grid(axis="y", linestyle=":", alpha=0.45)
    ax.set_xlim(0, 1040)
    ax.legend(ncol=3, fontsize=9, frameon=False, loc="upper left")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--gmp-summary", type=Path, required=True)
    parser.add_argument("--mpfr-summary", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--title", default="GMP vs MPFR Rgemm, 512-bit precision")
    args = parser.parse_args()

    rows = read_summary(args.gmp_summary, "GMP")
    rows.extend(read_summary(args.mpfr_summary, "MPFR"))

    args.output.parent.mkdir(parents=True, exist_ok=True)
    fig, axes = plt.subplots(2, 1, figsize=(13.5, 10.0), sharex=True)
    fig.suptitle(args.title, fontsize=15, fontweight="bold")
    plot_panel(axes[0], rows, "OpenMP")
    plot_panel(axes[1], rows, "serial")
    fig.tight_layout(rect=(0, 0, 1, 0.965))
    fig.savefig(args.output, dpi=180)
    plt.close(fig)


if __name__ == "__main__":
    main()
