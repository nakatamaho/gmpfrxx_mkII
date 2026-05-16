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
    if "mkII_FIXED_PRECISION_FASTPATH" in name:
        return "#2ca02c"
    if "mkII" in name:
        return "#73bf69"
    if "orig" in name:
        return "#4c78a8"
    return "#8c8c8c"


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
    y_positions = list(range(len(rows)))

    height = max(7.5, 0.39 * len(rows) + 2.0)
    fig, ax = plt.subplots(figsize=(14, height))
    bars = ax.barh(y_positions, averages, color=colors, height=0.7, alpha=0.9)

    label_pad = max_value = max(maximums)
    label_pad *= 0.01

    for y, avg, low, high in zip(y_positions, averages, minimums, maximums):
        ax.hlines(y, low, high, color="black", linewidth=1.6, zorder=3)
        ax.vlines([low, high], y - 0.18, y + 0.18, color="black", linewidth=1.2, zorder=3)
        ax.text(
            max(avg - label_pad, 0.0),
            y,
            f"{avg:.1f}",
            va="center",
            ha="right",
            fontsize=9,
            fontweight="bold",
            color="white",
        )
        ax.text(low, y - 0.28, f"{low:.1f}", va="top", ha="center", fontsize=7, color="#333333")
        ax.text(high, y - 0.28, f"{high:.1f}", va="top", ha="center", fontsize=7, color="#333333")

    ax.set_yticks(y_positions)
    ax.set_yticklabels(labels, fontsize=9)
    ax.invert_yaxis()
    ax.set_xlabel("MFLOPS", fontsize=12, fontweight="bold")
    ax.set_title(title, fontsize=14, fontweight="bold")
    ax.grid(axis="x", linestyle=":", alpha=0.45)
    ax.set_axisbelow(True)

    ax.set_xlim(0, max_value * 1.16)

    legend_items = [
        ("native C", "#8c8c8c"),
        ("orig(gmpxx.h)", "#4c78a8"),
        ("mkII", "#73bf69"),
        ("mkII fixed precision fastpath", "#2ca02c"),
    ]
    handles = [Patch(facecolor=color, label=label) for label, color in legend_items]
    ax.legend(
        handles=handles,
        loc="lower right",
        bbox_to_anchor=(1.0, 1.01),
        ncol=4,
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
