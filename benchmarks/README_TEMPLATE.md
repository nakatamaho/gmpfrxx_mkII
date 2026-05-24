<!-- SPDX-License-Identifier: BSD-2-Clause -->

# <BenchmarkName>

Describe the operation, backend, implementations being compared, and the
performance question answered by this benchmark.  Include the mathematical
operation when useful, for example `sum_i x_i * y_i` or
`y_i <- alpha * x_i + y_i`.

## Build

Show the repository-root build commands, build type, relevant options,
executable directory, command-line arguments, and one example invocation.

## Kernel Shapes

Define every numbered source-level variant.  The same number should mean the
same timed source shape across C native, upstream wrapper, mkII wrapper, serial,
and OpenMP variants.

| Variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|--------------------|---------------------------|---------|
| `01` | ... | ... | ... |

Keep numbered variants and optimization suffixes separate.  The variant number
defines the source-level kernel shape and must not change meaning when a
compile-time fastpath is enabled.  Optimization suffixes such as
`FIXED_PRECISION_FASTPATH`, `STABLE_ROUNDING`, and `FMA` describe how that same
source shape is compiled.

For MPFR expression-template kernels, `FMA` should normally be a build suffix,
not a separate source variant: if the timed source already has an FMA-capturable
shape such as `lhs += a * b`, the FMA-enabled build may lower it to
`mpfr_fma`.  Source-level differences that change whether an operation is
FMA-capturable must remain separate numbered variants.  In particular, a
non-FMA variant may intentionally use a reusable local temporary, such as
`temp = a * b; lhs += temp;`, to match a raw C kernel that avoids local
allocation without using FMA.  Do not force that variant into FMA form; it
answers a different performance question.

Raw C kernels do not split into wrapper-context variants; raw C has no wrapper
context layer.  If a wrapper benchmark needs to measure default-rounding lookup
overhead, split the C++ wrapper source instead.  Use `NN` for the ordinary
wrapper path, `NN_ROUNDING` for the same algorithm with loop-external rounding
capture via `with_context`, and `NN_ROUNDING_FMA_CAPTURE` for the captured
source shape that is also FMA-capturable.  Build suffixes remain separate:
`PRECISION` means `GMPFRXX_MKII_FAST_FIXED_PREC`, and a final `FMA` suffix means
`GMPFRXX_MKII_ENABLE_FMA`.  Map these wrapper source variants back to the same
raw C algorithmic kernel in the C native equivalence table, and state which
wrapper variant is the closest hot-loop comparison.

## C Native Equivalent Kernels

Map C++ wrapper kernels to raw C native kernels by timed hot-loop source shape
and generated code.  If no exact C native equivalent exists, state the closest
comparison point.

| C native kernel | Equivalent C++ wrapper kernel(s) | Equivalence notes |
|-----------------|----------------------------------|-------------------|
| `C_native_01` | ... | ... |

## Recorded Run

State the exact committed run parameters, run environment, raw-data directory,
log/CSV/plot paths, and correctness result.

## Plot Style

Use the MPFR Rgemv plot palette consistently across benchmark reports:

| Class | Color | Notes |
|-------|-------|-------|
| `C native` | `#8c8c8c` | Raw C baseline. |
| `mkII` | `#4c78a8` | Default mkII wrapper path. |
| `mkII + FMA` | `#9467bd` | FMA-enabled mkII or C-native FMA class. |
| `fixed precision` | `#d62728` | Fixed-precision fastpath, or MPFR stable-rounding/fixed-precision class when no separate color is useful. |
| `fixed precision + FMA` | `#2ca02c` | Combined fixed-precision/FMA class. |

For GMP reports that include upstream `orig` / `gmpxx.h` wrapper variants, color
`orig` as green (`#2ca02c`) so it is visually distinct from mkII blue and C
native gray.  Keep this mapping explicit in the plot legend.

## Headline Results

Summarize the main performance conclusions from the committed run.

## Serial Results

Include the main interpretation table, plus folded tables sorted by max and
average performance.

## OpenMP Results

Include the main interpretation table, plus folded tables sorted by max and
average performance.  If this benchmark has no OpenMP variants, say so here
rather than changing the section layout.

## Memory Bandwidth Estimates

State whether values are model estimates or hardware-counter measurements.
Document object sizes, limb sizes, precision, byte-count formulas, and what the
model includes or excludes.

<!-- BEGIN MPFR GMP COMPARISON TEMPLATE NOTE -->

## Comparison with GMP version

For MPFR benchmark reports that have a GMP counterpart, include this section
between `Memory Bandwidth Estimates` and `Hotpath Disassembly`. Compare the
same problem size, precision, repeat count, and execution mode against the GMP
report. State clearly that MPFR and GMP have different numerical semantics:
MPFR has explicit rounding and range behavior, while GMP `mpf` uses GMP's
floating-point model. GMP-only reports should omit this section.

<!-- END MPFR GMP COMPARISON TEMPLATE NOTE -->

## Hotpath Disassembly

State the snippet selection policy before showing assembly.  The snippets are
representative, not exhaustive: choose enough kernels to explain the performance
classes.  Cover the raw/reference slow path when relevant, the raw reusable
resource baseline, the main wrapper or mkII path, a representative parallel
worker loop, the best-performing class, and any surprising or suspicious class.

For GMP reports, whenever an mkII snippet is selected to argue equivalence or a
performance class, include the corresponding upstream `gmpxx.h` `orig` snippet
or explicitly state why no direct `orig` counterpart exists.  The report should
make raw C, upstream `orig`, and mkII differences visible from the hot loop, not
only from the result table.

Explain temporary lifetime, backend calls, rounding/precision lookups,
allocation-like behavior, and how the hot loop matches the intended source
shape.

## Lessons Learned

Summarize the actual performance boundary found in the run.  Explain which
source shapes change the hot loop, which variants are in the same performance
class, which results are likely variance, and what should guide the next change.
