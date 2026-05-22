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

## Hotpath Disassembly

Include representative disassembly snippets and explain temporary lifetime,
backend calls, rounding/precision lookups, allocation-like behavior, and how the
hot loop matches the intended source shape.

## Lessons Learned

Summarize the actual performance boundary found in the run.  Explain which
source shapes change the hot loop, which variants are in the same performance
class, which results are likely variance, and what should guide the next change.
