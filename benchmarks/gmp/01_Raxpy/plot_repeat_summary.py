#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause

from __future__ import annotations

import argparse
import csv
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Plot repeat-summary GMP Raxpy MFLOPS bars.")
    parser.add_argument("summary_csv", type=Path)
    parser.add_argument("--output-prefix", required=True, type=Path)
    parser.add_argument("--title-prefix", default="GMP Raxpy")
    return parser.parse_args()


def plot_mode(rows: list[dict[str, str]], mode: str, output: Path, title_prefix: str) -> None:
    data = [row for row in rows if row["execution_mode"] == mode]
    labels = [row["variant"].replace("Raxpy_gmp_", "") for row in data]
    avg = [float(row["avg_mflops"]) for row in data]
    minv = [float(row["min_mflops"]) for row in data]
    maxv = [float(row["max_mflops"]) for row in data]
    yerr = [[a - mn for a, mn in zip(avg, minv)], [mx - a for a, mx in zip(avg, maxv)]]

    fig_width = max(10.0, len(data) * 0.72)
    fig, ax = plt.subplots(figsize=(fig_width, 5.6))
    bars = ax.bar(
        range(len(data)),
        avg,
        yerr=yerr,
        capsize=3,
        color="#3f6f93",
        edgecolor="#263238",
        linewidth=0.5,
    )
    for index, bar in enumerate(bars):
        ax.text(
            bar.get_x() + bar.get_width() / 2.0,
            bar.get_height(),
            f"{avg[index]:.1f}",
            ha="center",
            va="bottom",
            fontsize=7,
            rotation=90,
        )
    ax.set_xticks(range(len(data)))
    ax.set_xticklabels(labels, rotation=55, ha="right", fontsize=8)
    ax.set_ylabel("MFLOPS")
    ax.set_title(f"{title_prefix} {mode}: mean with min/max error bars")
    ax.grid(axis="y", linestyle=":", alpha=0.45)
    fig.tight_layout()
    fig.savefig(output, dpi=160)
    plt.close(fig)


def main() -> int:
    args = parse_args()
    rows = list(csv.DictReader(args.summary_csv.open(encoding="utf-8")))
    plot_mode(rows, "Serial", args.output_prefix.with_name(args.output_prefix.name + "_serial.png"), args.title_prefix)
    plot_mode(rows, "OpenMP", args.output_prefix.with_name(args.output_prefix.name + "_openmp.png"), args.title_prefix)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
