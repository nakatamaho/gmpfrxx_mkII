#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause

import argparse
import csv
import re
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Patch


COMMAND_RE = re.compile(r"^COMMAND\s+Rdot\s+(\S+)\s+(\S+)\s+(.+)$")
RUN_RE = re.compile(r"^RUN\s+(\d+)/(\d+)")
ELAPSED_RE = re.compile(r"^Elapsed time:\s+([0-9.eE+-]+)\s+s")
MFLOPS_RE = re.compile(r"^MFLOPS:\s+([0-9.eE+-]+)")
DIFF_RE = re.compile(r"^DIFF:\s+(.+)\s+(OK|NG)$")


def display_name(raw):
    return raw.removeprefix("Rdot_mpfr_")


def natural_key(name):
    if name.startswith("C_native"):
        family = 0
    else:
        family = 1
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


def variant_color(name):
    if name.startswith("C_native"):
        return "#8c8c8c"
    if "STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH" in name:
        return "#2ca02c"
    if "STABLE_ROUNDING_FMA" in name:
        return "#9467bd"
    if "STABLE_ROUNDING" in name:
        return "#73bf69"
    return "#4c78a8"


def parse_log(path):
    rows = []
    current = None
    run_index = None
    run_count = None
    elapsed = None
    mflops = None

    for line in path.read_text(encoding="utf-8").splitlines():
        match = COMMAND_RE.match(line)
        if match:
            current = {
                "variant": display_name(match.group(1)),
                "command": match.group(2),
                "args": match.group(3),
            }
            run_index = None
            run_count = None
            elapsed = None
            mflops = None
            continue

        match = RUN_RE.match(line)
        if match:
            run_index = int(match.group(1))
            run_count = int(match.group(2))
            elapsed = None
            mflops = None
            continue

        match = ELAPSED_RE.match(line)
        if match:
            elapsed = float(match.group(1))
            continue

        match = MFLOPS_RE.match(line)
        if match:
            mflops = float(match.group(1))
            continue

        match = DIFF_RE.match(line)
        if match and current is not None and run_index is not None:
            rows.append(
                {
                    "variant": current["variant"],
                    "run": run_index,
                    "runs": run_count,
                    "elapsed_s": elapsed,
                    "mflops": mflops,
                    "diff": match.group(1),
                    "status": match.group(2),
                }
            )
    return rows


def summarize(rows):
    grouped = {}
    for row in rows:
        grouped.setdefault(row["variant"], []).append(row)

    summary = []
    for variant, values in grouped.items():
        mflops = [row["mflops"] for row in values if row["mflops"] is not None]
        elapsed = [row["elapsed_s"] for row in values if row["elapsed_s"] is not None]
        ok_runs = sum(1 for row in values if row["status"] == "OK")
        summary.append(
            {
                "variant": variant,
                "runs": len(values),
                "ok_runs": ok_runs,
                "max_mflops": max(mflops),
                "avg_mflops": sum(mflops) / len(mflops),
                "min_mflops": min(mflops),
                "max_elapsed_s": max(elapsed),
                "avg_elapsed_s": sum(elapsed) / len(elapsed),
                "min_elapsed_s": min(elapsed),
            }
        )
    return sorted(summary, key=lambda row: natural_key(row["variant"]))


def write_raw_csv(path, rows):
    fieldnames = ["variant", "run", "runs", "elapsed_s", "mflops", "diff", "status"]
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def write_summary_csv(path, rows):
    fieldnames = [
        "variant",
        "runs",
        "ok_runs",
        "max_mflops",
        "avg_mflops",
        "min_mflops",
        "max_elapsed_s",
        "avg_elapsed_s",
        "min_elapsed_s",
    ]
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def plot_rows(rows, title, output):
    rows = sorted(rows, key=lambda row: natural_key(row["variant"]))
    labels = [row["variant"] for row in rows]
    averages = [row["avg_mflops"] for row in rows]
    minimums = [row["min_mflops"] for row in rows]
    maximums = [row["max_mflops"] for row in rows]
    colors = [variant_color(row["variant"]) for row in rows]
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


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("log", type=Path)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--output-prefix", required=True)
    parser.add_argument("--title-prefix", default="MPFR Rdot repeat benchmark")
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    raw_rows = parse_log(args.log)
    if not raw_rows:
        raise SystemExit("no benchmark rows found")
    summary_rows = summarize(raw_rows)

    raw_csv = args.output_dir / f"raw_{args.output_prefix}.csv"
    summary_csv = args.output_dir / f"summary_{args.output_prefix}.csv"
    write_raw_csv(raw_csv, raw_rows)
    write_summary_csv(summary_csv, summary_rows)

    serial = [row for row in summary_rows if "openmp" not in row["variant"]]
    openmp = [row for row in summary_rows if "openmp" in row["variant"]]
    plot_rows(
        serial,
        f"{args.title_prefix}: serial avg with min/max range",
        args.output_dir / f"{args.output_prefix}_serial.png",
    )
    plot_rows(
        openmp,
        f"{args.title_prefix}: OpenMP avg with min/max range",
        args.output_dir / f"{args.output_prefix}_openmp.png",
    )

    print(raw_csv)
    print(summary_csv)


if __name__ == "__main__":
    main()
