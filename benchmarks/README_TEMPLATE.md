<!--
Copyright (c) 2026
     Nakata, Maho
     All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
-->

# <BenchmarkName>

Describe the operation, backend, implementations being compared, and the
performance question answered by this benchmark.  Include the mathematical
operation when useful, for example `sum_i x_i * y_i` or
`y_i <- alpha * x_i + y_i`.

## Build

Show the repository-root build commands, build type, relevant options,
executable directory, command-line arguments, and one example invocation.

Document the repeat runner when the benchmark has one. New benchmark reports
should use a `run_repeat.sh` interface matching the existing 00_Rdot runners:

```text
benchmarks/<backend>/<benchmark>/run_repeat.sh <build-dir> <size-args...> <precision> <repeat-count> [output-dir]
```

The runner should set `OMP_NUM_THREADS`, `OMP_PLACES`, `OMP_PROC_BIND`, accept
`BENCH_COMMAND_PREFIX` or `BENCH_NUMACTL`, write raw data under
`results_raw/<run-id>/`, emit `BENCHMARK_PARAMS`, `OPENMP_AFFINITY`, and
backend default-precision environment lines, and invoke the plot script from
the committed raw log.

When documenting the repository-level runner for the 00_Rdot, 01_Raxpy, and
02_Rgemv benchmark families, use the current precision-list interface:

```bash
benchmarks/run_all.sh build_bench_release 512,1024 10 10000000 10000000 4000 4000
```

The default precision list is `512,1024`.  `both` and `all` are aliases for
that list, and a single value such as `512` still runs only that precision.
The repository-level runner writes per-precision output directories such as
`results_raw/run_all_p512_repeat10_<timestamp>/` and
`results_raw/run_all_p1024_repeat10_<timestamp>/` under each benchmark.


## Benchmark Parameters

Define every command-line parameter and environment-controlled parameter used
by the run script.

| Parameter | Meaning |
|-----------|---------|
| `N` or `m`, `n`, `k` | Problem size. |
| `precision` | Requested backend precision in bits. |
| `repeat` | Number of timed process executions per executable. |
| `OMP_NUM_THREADS` | OpenMP worker count for `openmp` executables. |
| `OMP_PLACES`, `OMP_PROC_BIND` | OpenMP affinity controls used by the runner. |

For GMP `mpf` benchmarks, the runner must export
`GMPXX_DEFAULT_MPF_PRECISION_BITS=<precision>` for every timed command. For
MPFR benchmarks, the runner must export
`MPFRXX_DEFAULT_PRECISION_BITS=<precision>` for every timed command. This keeps
explicit constructor precision and wrapper-owned default precision aligned.

## Variant Shapes

Define every numbered source-level variant.  The same number should mean the
same timed source shape across C native, upstream wrapper, mkII wrapper, serial,
and OpenMP variants.

| Variant | Transition from previous variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|----------------------------------|--------------------|---------------------------|---------|
| `01` | Baseline source shape. | ... | ... | ... |

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

## Source Transitions

Explain the transition from one source-level variant to the next. This section
must describe the actual source change, for example direct expression to
loop-local temporary, loop-local temporary to reusable temporary, reusable
temporary to copy-then-multiply, row traversal to column traversal, row
partitioning to blocking, or blocking to column partitioning with reduction. If
the variants branch from different baselines rather than forming a strictly
linear sequence, say so explicitly.

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

### 512-bit run

State the 512-bit run ID, benchmark command, raw log, raw CSV, summary CSV,
plots, and correctness result.

### 1024-bit run

State the 1024-bit run ID, benchmark command, raw log, raw CSV, summary CSV,
plots, and correctness result.

## Plot Regeneration

Document the exact command used to regenerate plots from the committed raw log
or summary CSV.

Use the backend-standard color palette so plots remain visually comparable
across benchmark families.

GMP plot palette:

| Class | Color |
|-------|-------|
| C native | `#8c8c8c` |
| upstream `orig` | `#2ca02c` |
| mkII baseline | `#4c78a8` |
| fixed precision fastpath | `#d62728` |

MPFR plot palette:

| Class | Color |
|-------|-------|
| C native | `#8c8c8c` |
| mkII baseline | `#4c78a8` |
| `STABLE_ROUNDING` or `ROUNDING` | `#ff7f0e` |
| `FMA` | `#9467bd` |
| `FIXED_PRECISION_FASTPATH` or `PRECISION` | `#d62728` |
| combined precision + FMA path | `#2ca02c` |

When a benchmark-specific plot script uses semantic classes instead of exact
target suffixes, keep those semantic classes mapped to the same colors above.
Do not introduce per-benchmark palettes unless the README explains why the
standard palette does not apply.

## Resource or Bandwidth Estimates

State whether values are model estimates or hardware-counter measurements.
Document object sizes, limb sizes, precision, byte-count formulas, and what the
model includes or excludes.

## Headline Results

Summarize the main performance conclusions from the committed run.

## Serial Results

### 512-bit serial interpretation

Include the main interpretation table for the 512-bit serial run, plus folded
tables sorted by max and average performance.

### 1024-bit serial interpretation

Include the main interpretation table for the 1024-bit serial run, plus folded
tables sorted by max and average performance.

## OpenMP Results

### 512-bit OpenMP interpretation

Include the main interpretation table for the 512-bit OpenMP run, plus folded
tables sorted by max and average performance.

### 1024-bit OpenMP interpretation

Include the main interpretation table for the 1024-bit OpenMP run, plus folded
tables sorted by max and average performance. If this benchmark has no OpenMP
variants, say so here rather than changing the section layout.

<!-- BEGIN MPFR GMP COMPARISON TEMPLATE NOTE -->

## Comparison with GMP version

For MPFR benchmark reports that have a GMP counterpart, include this section
between `Resource or Bandwidth Estimates` and `Hotpath Disassembly`. Compare the
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
