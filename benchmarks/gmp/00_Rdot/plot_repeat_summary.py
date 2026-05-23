#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause

import argparse
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Patch


def variant_name(raw):
    prefix = "Rdot_gmp_"
    return raw[len(prefix):] if raw.startswith(prefix) else raw


def variant_color(name):
    if name.startswith("C_native"):
        return "#8c8c8c"
    if "orig" in name:
        return "#2ca02c"
    if "FIXED_PRECISION_FASTPATH_FMA" in name:
        return "#2ca02c"
    if "FIXED_PRECISION_FASTPATH" in name:
        return "#d62728"
    if name.endswith("_FMA"):
        return "#9467bd"
    return "#4c78a8"


def read_summary(path):
    rows = []
    with path.open(newline="") as handle:
        for row in csv.DictReader(handle):
            name = variant_name(row["variant"])
            rows.append(
                {
                    "name": name,
                    "avg": float(row["avg_mflops"]),
                    "min": float(row["min_mflops"]),
                    "max": float(row["max_mflops"]),
                    "ok_runs": int(row["ok_runs"]),
                    "runs": int(row["runs"]),
                }
            )
    return rows


def natural_key(name):
    # Keep the CSV family ordering, but make the plot stable if rows are moved.
    family = 0 if name.startswith("C_native") else 1
    if "orig" in name:
        flavor = 0
    elif "mkII_FIXED_PRECISION_FASTPATH" in name:
        flavor = 2
    elif "mkII" in name:
        flavor = 1
    else:
        flavor = 0
    digits = "".join(ch if ch.isdigit() else " " for ch in name).split()
    number = int(digits[0]) if digits else 0
    return (family, number, flavor, name)


def plot_rows(rows, title, output):
    rows = sorted(rows, key=lambda row: natural_key(row["name"]))
    labels = [row["name"] for row in rows]
    averages = [row["avg"] for row in rows]
    minimums = [row["min"] for row in rows]
    maximums = [row["max"] for row in rows]
    colors = [variant_color(row["name"]) for row in rows]
    x_positions = list(range(len(rows)))
    max_value = max(maximums)
    label_pad = max_value * 0.012

    width = max(17, 0.58 * len(rows) + 4.0)
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
    ax.set_title(title, fontsize=14, fontweight="bold")
    ax.grid(axis="y", linestyle=":", alpha=0.45)
    ax.set_axisbelow(True)

    ax.set_ylim(0, max_value * 1.22)

    legend_items = [
        ("C native", "#8c8c8c"),
        ("orig", "#2ca02c"),
        ("mkII", "#4c78a8"),
        ("mkII + FMA", "#9467bd"),
        ("fixed precision", "#d62728"),
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


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("summary_csv", type=Path)
    parser.add_argument("--output-prefix", type=Path, required=True)
    parser.add_argument("--title-prefix", default="GMP Rdot repeat-10")
    args = parser.parse_args()

    rows = read_summary(args.summary_csv)
    if not rows:
        raise SystemExit("summary CSV contains no rows")

    serial = [row for row in rows if "openmp" not in row["name"]]
    openmp = [row for row in rows if "openmp" in row["name"]]

    plot_rows(
        serial,
        f"{args.title_prefix}: serial avg with min/max range",
        args.output_prefix.with_name(args.output_prefix.name + "_serial.png"),
    )
    plot_rows(
        openmp,
        f"{args.title_prefix}: OpenMP avg with min/max range",
        args.output_prefix.with_name(args.output_prefix.name + "_openmp.png"),
    )


if __name__ == "__main__":
    main()
