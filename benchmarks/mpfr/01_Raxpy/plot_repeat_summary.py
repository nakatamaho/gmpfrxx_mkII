#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause

from __future__ import annotations

import argparse
import csv
import re
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Patch


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Plot repeat-summary MPFR Raxpy MFLOPS bars.")
    parser.add_argument("summary_csv", type=Path)
    parser.add_argument("--output-prefix", required=True, type=Path)
    parser.add_argument("--title-prefix", default="MPFR Raxpy")
    return parser.parse_args()


def natural_key(name: str) -> tuple[int, int, int, str]:
    family = 0 if name.startswith("C_native") else 1
    openmp = 1 if "openmp" in name else 0
    if "STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH" in name:
        flavor = 4
    elif "STABLE_ROUNDING_FMA" in name:
        flavor = 3
    elif "STABLE_ROUNDING" in name:
        flavor = 2
    elif name.endswith("_FMA"):
        flavor = 1
    else:
        flavor = 0
    numbers = [int(value) for value in re.findall(r"\d+", name)]
    number = numbers[0] if numbers else 0
    return (openmp, family, number, flavor, name)


def variant_color(name: str) -> str:
    if name.startswith("C_native"):
        return "#8c8c8c"
    if "STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH" in name:
        return "#2ca02c"
    if "STABLE_ROUNDING_FMA" in name:
        return "#9467bd"
    if "STABLE_ROUNDING" in name:
        return "#73bf69"
    if name.endswith("_FMA"):
        return "#9467bd"
    return "#4c78a8"


def plot_mode(rows: list[dict[str, str]], mode: str, output: Path, title_prefix: str) -> None:
    data = [row for row in rows if row["execution_mode"] == mode]
    data = sorted(data, key=lambda row: natural_key(row["variant"]))
    labels = [row["variant"] for row in data]
    averages = [float(row["avg_mflops"]) for row in data]
    minimums = [float(row["min_mflops"]) for row in data]
    maximums = [float(row["max_mflops"]) for row in data]
    colors = [variant_color(label) for label in labels]
    x_positions = list(range(len(data)))
    max_value = max(maximums)
    label_pad = max_value * 0.012

    width = max(17, 0.58 * len(data) + 4.0)
    fig, ax = plt.subplots(figsize=(width, 8.5))
    ax.bar(x_positions, averages, color=colors, width=0.72, alpha=0.9)

    for x, avg, low, high in zip(x_positions, averages, minimums, maximums):
        ax.vlines(x, low, high, color="black", linewidth=1.6, zorder=3)
        ax.hlines([low, high], x - 0.18, x + 0.18, color="black", linewidth=1.2, zorder=3)
        ax.text(
            x,
            avg + label_pad,
            f"{avg:.1f}",
            va="bottom",
            ha="center",
            fontsize=9,
            fontweight="bold",
            color="#111111",
            rotation=90,
        )
        ax.text(x - 0.22, low, f"{low:.1f}", va="center", ha="right", fontsize=7, color="#333333", rotation=90)
        ax.text(x + 0.22, high, f"{high:.1f}", va="center", ha="left", fontsize=7, color="#333333", rotation=90)

    ax.set_xticks(x_positions)
    ax.set_xticklabels(labels, rotation=63, ha="right", fontsize=8)
    ax.set_ylabel("MFLOPS", fontsize=12, fontweight="bold")
    ax.set_title(f"{title_prefix}: {mode} avg with min/max range", fontsize=14, fontweight="bold")
    ax.grid(axis="y", linestyle=":", alpha=0.45)
    ax.set_axisbelow(True)
    ax.set_ylim(0, max_value * 1.22)

    legend_items = [
        ("C native", "#8c8c8c"),
        ("mkII", "#4c78a8"),
        ("stable rounding", "#73bf69"),
        ("stable rounding + FMA", "#9467bd"),
        ("stable rounding + FMA + fixed precision", "#2ca02c"),
    ]
    handles = [Patch(facecolor=color, label=label) for label, color in legend_items]
    ax.legend(
        handles=handles,
        loc="lower right",
        bbox_to_anchor=(1.0, 1.01),
        ncol=3,
        frameon=False,
        fontsize=9,
    )

    fig.tight_layout()
    fig.savefig(output, dpi=180)
    plt.close(fig)


def main() -> int:
    args = parse_args()
    rows = list(csv.DictReader(args.summary_csv.open(encoding="utf-8")))
    plot_mode(rows, "Serial", args.output_prefix.with_name(args.output_prefix.name + "_serial.png"), args.title_prefix)
    plot_mode(rows, "OpenMP", args.output_prefix.with_name(args.output_prefix.name + "_openmp.png"), args.title_prefix)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
