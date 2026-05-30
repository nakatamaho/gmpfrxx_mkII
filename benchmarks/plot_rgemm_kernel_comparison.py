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
import re
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def is_openmp(variant):
    return "_openmp" in variant


def display_name(variant, backend):
    prefix = f"Rgemm_{backend}_"
    if variant.startswith(prefix):
        return variant[len(prefix):]
    return variant


def variant_sort_key(variant):
    display = display_name(variant, "gmp")
    display = display_name(display, "mpfr")
    match = re.search(r"_(\d{2})(?:_|$)", display)
    number = int(match.group(1)) if match else 0
    if display.startswith("C_native"):
        family = 0
    elif "_orig" in display:
        family = 1
    else:
        family = 2
    if "ROUNDING_PRECISION_FMA" in display:
        suffix = 6
    elif "ROUNDING_FMA" in display:
        suffix = 5
    elif "ROUNDING_PRECISION" in display:
        suffix = 4
    elif "ROUNDING" in display:
        suffix = 3
    elif "FIXED_PRECISION_FASTPATH" in display or "PRECISION" in display:
        suffix = 2
    elif "_FMA" in display:
        suffix = 1
    else:
        suffix = 0
    return (number, family, suffix, display)


def read_summary(path):
    rows = []
    with path.open(newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(
                {
                    "size": int(row["size"]),
                    "variant": row["variant"],
                    "mode": "openmp" if is_openmp(row["variant"]) else "serial",
                    "max_mflops": float(row["max_mflops"]),
                }
            )
    return rows


def plot_mode(rows, backend, mode, output, title):
    selected = [row for row in rows if row["mode"] == mode]
    if not selected:
        raise SystemExit(f"no rows for mode {mode}")

    sizes = sorted({row["size"] for row in selected})
    variants = sorted({row["variant"] for row in selected}, key=variant_sort_key)
    values = {(row["variant"], row["size"]): row["max_mflops"] for row in selected}

    fig_height = 10.0 if len(variants) <= 28 else 12.0
    fig_width = 18.0
    fig, ax = plt.subplots(figsize=(fig_width, fig_height))
    markers = ["o", "s", "^", "D", "v", "P", "X", "*"]

    for variant in variants:
        points = [(size, values[(variant, size)]) for size in sizes]
        xs = [point[0] for point in points]
        ys = [point[1] for point in points]
        ax.plot(
            xs,
            ys,
            label=display_name(variant, backend),
            color=variant_color(variant),
            linestyle=variant_linestyle(variant),
            marker=markers[variant_sort_key(variant)[0] % len(markers)],
            markersize=3.0,
            markevery=3,
            linewidth=1.25,
            alpha=0.84,
        )

    ax.set_title(title, fontsize=14, fontweight="bold")
    ax.set_xlabel("Square matrix size N = M = K")
    ax.set_ylabel("Max MFLOPS")
    ax.set_xlim(0, max(sizes) * 1.02)
    ax.set_ylim(0, max(values.values()) * 1.08)
    ax.grid(axis="y", linestyle=":", alpha=0.45)
    ax.grid(axis="x", linestyle=":", alpha=0.18)

    for size in [128, 256, 512, 1024]:
        if size in sizes:
            ax.axvline(size, color="#bbbbbb", linewidth=0.8, alpha=0.85, zorder=0)

    legend_fontsize = 6.4 if len(variants) > 30 else 7.2
    ax.legend(
        loc="upper left",
        bbox_to_anchor=(1.01, 1.0),
        borderaxespad=0.0,
        frameon=False,
        fontsize=legend_fontsize,
        ncol=1,
    )

    fig.tight_layout(rect=(0, 0, 0.72, 1.0))
    fig.savefig(output, dpi=180)
    plt.close(fig)


def variant_color(variant):
    if "C_native" in variant:
        return "#555555" if "_FMA" not in variant else "#111111"
    if "_orig" in variant:
        return "#2ca02c"
    if "ROUNDING_PRECISION_FMA" in variant:
        return "#17becf"
    if "ROUNDING_FMA" in variant:
        return "#9467bd"
    if "ROUNDING_PRECISION" in variant:
        return "#8c564b"
    if "ROUNDING" in variant:
        return "#ff7f0e"
    if "FIXED_PRECISION_FASTPATH" in variant or "PRECISION" in variant:
        return "#d62728"
    return "#4c78a8"


def variant_linestyle(variant):
    if "C_native" in variant and "_FMA" in variant:
        return "--"
    if "ROUNDING" in variant:
        return "-"
    if "FIXED_PRECISION_FASTPATH" in variant or "PRECISION" in variant:
        return "-."
    return "-"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--summary", type=Path, required=True)
    parser.add_argument("--backend", choices=("gmp", "mpfr"), required=True)
    parser.add_argument("--output-prefix", type=Path, required=True)
    parser.add_argument("--title-prefix", required=True)
    args = parser.parse_args()

    rows = read_summary(args.summary)
    args.output_prefix.parent.mkdir(parents=True, exist_ok=True)
    for mode in ("serial", "openmp"):
        output = args.output_prefix.with_name(f"{args.output_prefix.name}_{mode}.png")
        title = f"{args.title_prefix}: {mode} kernel comparison"
        plot_mode(rows, args.backend, mode, output, title)


if __name__ == "__main__":
    main()
