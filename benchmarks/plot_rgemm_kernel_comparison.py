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
    matrix = [[values[(variant, size)] for size in sizes] for variant in variants]

    fig_height = max(8.0, 0.27 * len(variants) + 2.7)
    fig_width = max(15.0, 0.34 * len(sizes) + 6.5)
    fig, ax = plt.subplots(figsize=(fig_width, fig_height))
    image = ax.imshow(matrix, aspect="auto", interpolation="nearest", cmap="viridis")

    ax.set_title(title, fontsize=14, fontweight="bold")
    ax.set_xlabel("Square matrix size N = M = K")
    ax.set_ylabel(f"{backend.upper()} {mode} target")
    ax.set_xticks(range(len(sizes)))
    ax.set_xticklabels([str(size) for size in sizes], rotation=60, ha="right", fontsize=8)
    ax.set_yticks(range(len(variants)))
    ax.set_yticklabels([display_name(variant, backend) for variant in variants], fontsize=8)

    for size in [128, 256, 512, 1024]:
        if size in sizes:
            x = sizes.index(size)
            ax.axvline(x, color="white", linewidth=0.8, alpha=0.8)

    colorbar = fig.colorbar(image, ax=ax, pad=0.01)
    colorbar.set_label("Max MFLOPS", rotation=90)

    fig.tight_layout()
    fig.savefig(output, dpi=180)
    plt.close(fig)


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
