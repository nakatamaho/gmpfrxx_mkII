
# AGENTS.md

## Project

This repository implements `gmpfrxx_mkII`, a from-scratch C++17 header-only expression-template wrapper library for GMP, MPFR, and MPC.

Public namespaces:

```cpp
namespace gmpxx;   // GMP-only public API
namespace mpfrxx;  // MPFR public API; MPC is enabled by mpcxx_mkII.h
```

Internal implementation namespace:

```cpp
namespace gmpfrxx_mkII::detail;
```

Do not use `namespace gmpfrxx` as a public API namespace.

The implementation is independent and must not include `gmpxx.h` or link `libgmpxx`.

## Header Roles

The header roles are strict.

```text
gmpxx_mkII.h
  GMP-only header.
  Exposes:
    gmpxx::mpz_class
    gmpxx::mpq_class
    gmpxx::mpf_class
    optionally later: gmpxx::mpfc_class
  Depends on:
    GMP only
  Must not include:
    <mpfr.h>
    <mpc.h>
  Must not reference:
    mpfr_*
    mpc_*
    mpc_t

mpfrxx_mkII.h
  MPFR real header.
  Exposes:
    mpfrxx::mpz_class  // alias to gmpxx::mpz_class
    mpfrxx::mpq_class  // alias to gmpxx::mpq_class
    mpfrxx::mpfr_class
  Depends on:
    GMP + MPFR
  Must not expose:
    gmpxx::mpf_class
    gmpxx::mpfc_class
    mpfrxx::mpc_class
  Must not include:
    gmpxx_mkII.h
    mpcxx_mkII.h
    gmpfrxx_mkII/detail/mpf_impl.hpp
    gmpfrxx_mkII/detail/mpfc_impl.hpp
    gmpfrxx_mkII/detail/mpc_environment.hpp
    gmpfrxx_mkII/detail/mpc_impl.hpp
    <mpc.h>
  May include:
    <gmp.h>
    <mpfr.h>
    gmpfrxx_mkII/detail/zq_impl.hpp
    gmpfrxx_mkII/detail/mpfr_impl.hpp

mpcxx_mkII.h
  MPC opt-in complex header.
  Includes:
    mpfrxx_mkII.h
  Exposes:
    mpfrxx::mpc_class
  Depends on:
    GMP + MPFR + MPC
  Must not expose:
    gmpxx::mpf_class
    gmpxx::mpfc_class
  Must not include:
    gmpxx_mkII.h
    gmpfrxx_mkII/detail/mpf_impl.hpp
    gmpfrxx_mkII/detail/mpfc_impl.hpp
  May include:
    <mpc.h>
    gmpfrxx_mkII/detail/mpc_environment.hpp
    gmpfrxx_mkII/detail/mpc_impl.hpp

gmpfrxx_mkII.h
  Full combined header / aggregator.
  Exposes:
    gmpxx::mpz_class
    gmpxx::mpq_class
    gmpxx::mpf_class
    optionally later: gmpxx::mpfc_class
    mpfrxx::mpfr_class
    mpfrxx::mpc_class
  Depends on:
    GMP + MPFR + MPC
  May include:
    gmpxx_mkII.h
    mpfrxx_mkII.h
    mpcxx_mkII.h
```

## Namespace Rules

`mpfrxx::mpz_class` and `mpfrxx::mpq_class` must be aliases, not separate types.

Correct:

```cpp
namespace gmpxx {
    class mpz_class;
    class mpq_class;
}

namespace mpfrxx {
    using ::gmpxx::mpz_class;
    using ::gmpxx::mpq_class;
    class mpfr_class;
    class mpc_class;
}
```

Wrong:

```cpp
namespace mpfrxx {
    class mpz_class; // wrong if distinct from gmpxx::mpz_class
    class mpq_class; // wrong if distinct from gmpxx::mpq_class
}
```

## Implementation Rules

1. Use C++17.
2. Do not require C++20 concepts.
3. Do not include `gmpxx.h`.
4. Do not link `libgmpxx`.
5. Do not bridge to an existing `gmpxx_mkII.h` implementation.
6. Generate `gmpxx_mkII.h`, `mpfrxx_mkII.h`, `mpcxx_mkII.h`, and `gmpfrxx_mkII.h` from scratch.
7. Use GMP C API from `<gmp.h>`.
8. Use MPFR C API from `<mpfr.h>` only in MPFR/MPC implementation paths.
9. Use MPC C API from `<mpc.h>` only in MPC implementation paths.
10. Do not use MPC for `gmpxx::mpf_class` or `gmpxx::mpfc_class`.
11. Keep code, comments, identifiers, tests, and documentation in English.
12. Update `STATUS.md` after each phase.
13. Run CMake/CTest after each phase.

## Private Headers

Implementation headers must live under:

```text
include/gmpfrxx_mkII/detail/
```

Do not create `include/detail/`.

Recommended private headers:

```text
config.hpp
type_traits.hpp
integer_conversion.hpp
environment.hpp
expr.hpp
eval_context.hpp
zq_impl.hpp
mpfr_impl.hpp
mpc_impl.hpp
mpf_impl.hpp
mpfc_impl.hpp
math_mpfr.hpp
math_mpc.hpp
math_mpf.hpp
math_mpfc.hpp
```

## Expression Template Rule

Public arithmetic operators must return expression nodes, not eager public numeric objects.

Example:

```cpp
auto e = a + b;
static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(e)>);
```

The following must be false for expression-forming operators:

```cpp
std::is_same_v<decltype(e), gmpxx::mpf_class>
std::is_same_v<decltype(e), mpfrxx::mpfr_class>
std::is_same_v<decltype(e), mpfrxx::mpc_class>
```

Materialization is allowed only in:

```text
construction from expression
assignment from expression
explicit evaluation helpers
compound assignment operators
documented eager math functions
stream output of expressions
comparison of expressions
```

Expression node lifetime rule:

Do NOT bind expression templates to `auto` and let them outlive their operands.
Expression nodes may hold raw references to lvalue operands.

```cpp
// BAD: e holds raw references to a and b.
auto e = a + b;
mutate_or_destroy(a);
mpf_class r = e;  // dangling if a was destroyed; changed value if mutated

// GOOD: evaluate immediately.
mpf_class r = a + b;
```

`[[nodiscard]]` on expression node types is only a diagnostic aid for discarded
expressions such as `a + b;`. It is not a lifetime-safety mechanism for
`auto e = a + b;`.

Do not implement eager arithmetic as the primary API:

```cpp
friend mpfrxx::mpfr_class operator+(const mpfrxx::mpfr_class&, const mpfrxx::mpfr_class&);
friend mpfrxx::mpc_class operator+(const mpfrxx::mpc_class&, const mpfrxx::mpc_class&);
friend gmpxx::mpf_class operator+(const gmpxx::mpf_class&, const gmpxx::mpf_class&);
```

Every arithmetic phase must include static-assert tests proving expression-node return types.

## Integer Policy

Do not expose GMP's `long` limitation.

Rules:

1. Do not blindly cast public integral inputs to `long`.
2. `std::uint64_t{UINT64_MAX}` must convert exactly.
3. `std::int64_t{INT64_MIN}` must convert exactly without evaluating `-value`.
4. For `gmpxx::mpz_class`, use `mpz_import` or an equivalent chunk path when needed.
5. For `gmpxx::mpq_class`, convert numerator and denominator through `gmpxx::mpz_class`.
6. For `gmpxx::mpf_class`, convert integral values through `gmpxx::mpz_class` and `mpf_set_z`.
7. For `mpfrxx::mpfr_class`, convert integral values through `gmpxx::mpz_class` and `mpfr_set_z`.
8. For `mpfrxx::mpc_class`, convert real scalar operands through the MPFR conversion path.

Reject as expression scalar leaves:

```text
bool
long double
__int128
```

`__int128` may be supported for direct `gmpxx::mpz_class` construction and comparison, but not as an expression scalar leaf.

## Precision and Environment

### MPFR

`mpfrxx::mpfr_class` uses MPFR precision, exponent range, and rounding.

The initial wrapper-owned MPFR default precision is 512 bits.

Environment variables:

```text
MPFRXX_DEFAULT_PRECISION_BITS
MPFRXX_DEFAULT_EMIN
MPFRXX_DEFAULT_EMAX
MPFRXX_DEFAULT_ROUNDING_MODE
```

Allowed rounding strings:

```text
RNDN RNDZ RNDU RNDD RNDA RNDF
MPFR_RNDN MPFR_RNDZ MPFR_RNDU MPFR_RNDD MPFR_RNDA MPFR_RNDF
```

Existing-object assignment preserves destination precision. New expression materialization uses max leaf precision.

### MPC

`mpfrxx::mpc_class` is MPC-backed and exposed by `mpcxx_mkII.h` and `gmpfrxx_mkII.h`.

Environment variables:

```text
MPCXX_DEFAULT_PRECISION_BITS
MPCXX_DEFAULT_REAL_PRECISION_BITS
MPCXX_DEFAULT_IMAG_PRECISION_BITS
MPCXX_DEFAULT_ROUNDING_MODE
MPCXX_DEFAULT_REAL_ROUNDING_MODE
MPCXX_DEFAULT_IMAG_ROUNDING_MODE
```

MPC defaults inherit MPFR defaults when no MPC-specific environment variables are set.
If any `MPCXX_*` variable is present, first use of the MPC default API
or default `mpc_class` construction must read those variables once for the
current thread and install MPC-specific precision or rounding overrides.
MPC-specific environment overrides affect only `mpfrxx::mpc_class` defaults
and do not change `mpfrxx::mpfr_class` defaults.

MPC environment variables must not affect `gmpxx::mpfc_class`.

### MPF

`gmpxx::mpf_class` uses wrapper-owned default precision, not GMP's process-global default precision.

The initial wrapper-owned GMP `mpf_class` default precision is 512 bits.

Do not call `mpf_set_default_prec()` from this library.

Environment variable:

```text
GMPXX_DEFAULT_MPF_PRECISION_BITS
```

Existing-object assignment preserves destination precision. New expression materialization uses max leaf precision.

## Compile-Time Fast Path Options

Canonical options:

```text
GMPFRXX_MKII_FAST_FIXED_PREC
GMPFRXX_MKII_FAST_STABLE_RND
GMPFRXX_MKII_ENABLE_FMA
```

Use preprocessor checks only in `detail/config.hpp`, then expose constexpr flags such as:

```cpp
gmpfrxx_mkII::detail::build_options::fast_fixed_precision
gmpfrxx_mkII::detail::build_options::fast_stable_rounding
gmpfrxx_mkII::detail::build_options::enable_fma
```

The rest of the implementation must use `if constexpr` or ordinary runtime
branches on these constexpr flags. Do not scatter `#ifdef` checks for these
options outside `detail/config.hpp`.

## Type Promotion

Required result types:

```text
gmpxx::mpz + gmpxx::mpz       -> gmpxx::mpz_class
gmpxx::mpz + gmpxx::mpq       -> gmpxx::mpq_class
gmpxx::mpq + gmpxx::mpq       -> gmpxx::mpq_class

mpfrxx::mpz/mpq + mpfrxx::mpfr -> mpfrxx::mpfr_class
mpfrxx::mpfr + mpfrxx::mpfr    -> mpfrxx::mpfr_class

mpfrxx::mpc + mpfrxx::mpc      -> mpfrxx::mpc_class
mpfrxx::mpc + mpfrxx::mpfr     -> mpfrxx::mpc_class
mpfrxx::mpc + mpfrxx::mpz/mpq  -> mpfrxx::mpc_class

gmpxx::mpz/mpq + gmpxx::mpf   -> gmpxx::mpf_class
gmpxx::mpf + gmpxx::mpf       -> gmpxx::mpf_class
```

Forbidden:

```text
gmpxx::mpf + mpfrxx::mpfr
mpfrxx::mpfr + gmpxx::mpf
gmpxx::mpf + mpfrxx::mpc
mpfrxx::mpc + gmpxx::mpf
```

If `gmpxx::mpfc_class` is implemented, it remains separate from `mpfrxx::mpc_class`.

## CMake Rules

Required interface targets:

```text
gmpxx_mkII       -> GMP only
mpfrxx_mkII      -> GMP + MPFR
mpcxx_mkII       -> GMP + MPFR + MPC
gmpfrxx_mkII     -> GMP + MPFR + MPC
```

Provide:

```text
cmake/FindGMP.cmake
cmake/FindMPFR.cmake
cmake/FindMPC.cmake
```

Required commands:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Required Compile-Fail Tests

At minimum:

```text
mpfrxx_mkII.h must not expose gmpxx::mpf_class
mpfrxx_mkII.h must not expose gmpxx::mpfc_class
mpfrxx_mkII.h must not expose mpfrxx::mpc_class
gmpxx_mkII.h must not expose mpfrxx::mpfr_class
gmpxx_mkII.h must not expose mpfrxx::mpc_class

gmpxx::mpf + mpfrxx::mpfr must not compile
mpfrxx::mpfr + gmpxx::mpf must not compile
gmpxx::mpf + mpfrxx::mpc must not compile
mpfrxx::mpc + gmpxx::mpf must not compile

bool scalar rejected
long double scalar rejected
__int128 scalar leaf rejected
scalar/scalar ET operators not provided
```

## Source Scan Tests

Production code for `gmpxx_mkII.h` must not contain:

```cpp
#include <mpfr.h>
#include <mpc.h>
```

Production code for `mpfrxx_mkII.h` must not contain:

```cpp
#include <mpc.h>
```

Production code must not contain:

```cpp
#include <gmpxx.h>
```

`mpc_*` symbols are allowed only in MPC-specific implementation files such as:

```text
include/gmpfrxx_mkII/detail/mpc_impl.hpp
include/gmpfrxx_mkII/detail/math_mpc.hpp
```

## Status Discipline

Update `STATUS.md` after each phase with:

```text
phase name
implemented features
missing features
tests added
exact commands run
pass/fail result
known issues
```

Do not claim completion without tests.

## Benchmark README/reporting conventions

These conventions apply to benchmark reports under:

```text
benchmarks/
````

A benchmark README must be a reproducible technical report, not just a list of
numbers.  It should describe the benchmark purpose, source-level variant shapes,
build procedure, recorded run conditions, raw-data location, static GitHub
tables, generated plots, representative disassembly, and lessons learned.

When the same benchmark exists for multiple backends, implementations, or
optimization modes, keep the directory layout, executable naming, README
sections, result tables, raw-data layout, and disassembly analysis as parallel
as possible.  The goal is to make corresponding variants directly comparable.

### Backend and implementation parallelism

For related benchmark families, use the same benchmark structure across
backends whenever possible.

Examples of related backends include:

```text
GMP
MPFR
MPC
binary64
binary80
binary128
DD
QD
other multiprecision or extended-precision backends
```

If the same benchmark is implemented for both GMP and MPFR, the two benchmark
directories should use parallel:

* source layout
* kernel numbering
* executable naming
* README section order
* result-table columns
* raw-data directory structure
* plot-generation workflow
* disassembly-analysis format
* lessons-learned format

When comparing GMP wrapper variants, use this optimization order unless the
specific benchmark documents a reason to differ:

1. baseline, with no special options
2. `FIXED_PRECISION_FASTPATH`

When comparing MPFR wrapper variants, use this optimization order unless the
specific benchmark documents a reason to differ:

1. baseline, with no special options
2. `STABLE_ROUNDING`
3. `FMA`
4. `FIXED_PRECISION_FASTPATH`

Do not reorder optimization variants in the main README tables unless the table
is explicitly a sorted result view.

### Numbered benchmark variants

Use numbered benchmark variants such as `01`, `02`, `03`, ... when a benchmark
compares multiple source-level kernel shapes.

The same numeric suffix must mean the same source-level shape across:

* raw C kernels
* upstream C++ wrapper kernels
* mkII C++ wrapper kernels
* serial variants
* OpenMP variants
* other parallel variants, when applicable
* related backends, when applicable

Every benchmark README that uses numbered variants must include a variant table.

Use a table shape like:

```markdown
| Variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|--------------------|---------------------------|---------|
| `01` | ... | ... | ... |
| `02` | ... | ... | ... |
| `03` | ... | ... | ... |
```

The table must define what each variant means.  Do not assume that the reader
can infer the meaning from filenames.

The `Timed source shape` column should describe the actual code pattern being
timed.

The `Temporary/resource policy` column should describe whether temporaries,
scratch buffers, precision objects, accumulators, work arrays, or other
resources are created inside or outside the timed loop.

The `Purpose` column should explain why this variant exists, for example:

```text
stress per-iteration construction
test reusable temporary object
test expression-template materialization
test fixed-precision scratch fastpath
test loop unrolling
test accumulator dependency
test OpenMP per-thread temporary reuse
test stable rounding path
test FMA path
```

When adding a new source shape, append a new number such as `07`.  Do not reuse
an existing number for a different kernel shape.

If a variant is meaningful for one backend but not another, document the
difference explicitly.  Do not silently change the numbering scheme.

### Kernel source separation

Do not mix the timed kernel body directly into the benchmark driver when it can
be separated cleanly.

Put representative kernel implementations in separate kernel source or header
files.  The benchmark driver should be responsible for:

* parsing benchmark arguments
* allocating and initializing input data
* configuring precision or backend state
* running repeats
* checking correctness
* timing
* printing raw results

The kernel file should contain the actual timed hot loop or the closest
backend-specific equivalent.

This separation is required so that the source kernel can be compared directly
against emitted assembly, compiler IR, GPU code, or other generated code.

### Executable naming

Keep executable names systematic and parallel.

Use a naming scheme of the form:

```text
<BenchmarkName>_<backend>_<implementation>_<variant>
<BenchmarkName>_<backend>_<implementation>_<parallel-mode>_<variant>
<BenchmarkName>_<backend>_<implementation>_<variant>_<optimization>
<BenchmarkName>_<backend>_<implementation>_<parallel-mode>_<variant>_<optimization>
```

Recommended components:

```text
<BenchmarkName>    Rdot, Rgemm, Raxpy, matrix_mul, norm, axpy, etc.
<backend>          gmp, mpfr, mpc, binary64, binary128, dd, qd, etc.
<implementation>   C_native, kernel, orig, mkII, reference, etc.
<parallel-mode>    openmp, serial, gpu, cuda, hip, threads, etc.
<variant>          01, 02, 03, ...
<optimization>     FIXED_PRECISION_FASTPATH, STABLE_ROUNDING, FMA, etc.
```

For raw C kernels, use names like:

```text
<BenchmarkName>_<backend>_C_native_NN
<BenchmarkName>_<backend>_C_native_openmp_NN
```

For wrapper kernels, use names like:

```text
<BenchmarkName>_<backend>_kernel_NN_orig
<BenchmarkName>_<backend>_kernel_NN_mkII
<BenchmarkName>_<backend>_kernel_NN_mkII_<OPTIMIZATION>

<BenchmarkName>_<backend>_kernel_openmp_NN_orig
<BenchmarkName>_<backend>_kernel_openmp_NN_mkII
<BenchmarkName>_<backend>_kernel_openmp_NN_mkII_<OPTIMIZATION>
```

For MPFR-style optimization variants, use names like:

```text
<BenchmarkName>_mpfr_kernel_NN_mkII_STABLE_ROUNDING
<BenchmarkName>_mpfr_kernel_NN_mkII_FMA
<BenchmarkName>_mpfr_kernel_NN_mkII_FIXED_PRECISION_FASTPATH
```

If a variant is not meaningful for a backend, document that explicitly rather
than silently changing the naming scheme.

### Raw result storage

Store raw benchmark data under `results_raw/`.

Use run-specific subdirectories:

```text
benchmarks/<backend>/<benchmark>/results_raw/<run-id>/
```

The run ID should include enough information to identify the run without
opening the files.  Use a format like:

```text
<benchmark>_<backend>_<main-parameters>_repeat<repeat-count>_<YYYYMMDD_HHMMSS>
```

Examples:

```text
rdot_gmp_n10000000_p512_repeat10_YYYYMMDD_HHMMSS
rgemm_mpfr_m512_n512_k512_p512_repeat10_YYYYMMDD_HHMMSS
raxpy_gmp_n10000000_p512_repeat10_YYYYMMDD_HHMMSS
```

Each committed benchmark run should include:

* raw benchmark log
* raw CSV
* summary CSV
* regenerated plot images, when applicable
* generated Markdown table fragments, if they are part of the workflow
* disassembly snippets or commands, when applicable

Before collecting a new committed run, remove the old raw-data directory for
that benchmark unless the old run is intentionally being preserved for an
explicit comparison.

Do not mix stale and fresh raw results in the same README update.

### README structure

Each benchmark README should follow this structure unless the benchmark has a
documented reason to differ:

```text
SPDX header
# <BenchmarkName>

Purpose
Build
Benchmark Parameters
Variant Shapes
C Native Equivalent Kernels
Recorded Run
Resource or Bandwidth Estimates
Serial Results
Parallel Results
Hotpath Disassembly or Generated Code Analysis
Lessons Learned
```

Use the benchmark-specific title:

```markdown
# 00_Rdot
# 01_Raxpy
# 02_Rgemm
# matrix_mul
# norm
```

The `Purpose` section must state:

* what operation is being benchmarked
* what backend or implementation family is being compared
* what performance question the benchmark is intended to answer

When useful, include the mathematical operation, for example:

```text
sum_i x_i * y_i
y_i <- alpha * x_i + y_i
C <- alpha * A * B + beta * C
```

The `Build` section must show:

* build commands from the repository root
* build type
* relevant CMake/configure options
* executable output directory
* expected command-line arguments
* at least one example invocation

The `Benchmark Parameters` section must define the meaning of command-line
parameters such as:

* vector size
* matrix dimensions
* precision
* repeat count
* block size
* number of threads
* backend-specific precision or rounding mode
* input distribution
* correctness tolerance

The `Variant Shapes` section must define the numbered variants.

The `C Native Equivalent Kernels` section must explain which C++ wrapper
kernels are equivalent to each raw C native kernel.  Base the mapping on the
timed hot-loop source shape and generated code, not just on matching numeric
suffixes.  If there is no exact C native equivalent for a C++ expression
template spelling, say so explicitly and name the closest C native comparison
point.

The `Recorded Run` section must state the exact committed run parameters,
including at least:

* problem size
* precision or backend mode
* repeat count
* compiler and compiler version, when known
* build type and optimization flags
* CPU model, when known
* GPU model, when applicable
* operating system, when useful
* OpenMP thread count, when applicable
* `OMP_PLACES`, when applicable
* `OMP_PROC_BIND`, when applicable
* other relevant environment variables
* raw result directory
* raw log path
* raw CSV path
* summary CSV path
* whether all variants passed correctness checks

### Static GitHub Markdown tables

GitHub README files cannot run JavaScript sorting.  Do not rely on JavaScript,
HTML widgets, or dynamic sorting in benchmark READMEs.

Generate static Markdown tables from CSV data.

For each execution mode, include:

1. a main interpretation table
2. a folded table sorted by the primary maximum-performance metric
3. a folded table sorted by the primary average-performance metric

For serial benchmarks, use:

```text
Serial results sorted by Max <metric>
Serial results sorted by Avg <metric>
```

For OpenMP benchmarks, use:

```text
OpenMP results sorted by Max <metric>
OpenMP results sorted by Avg <metric>
```

For other parallel modes, use the relevant name:

```text
CUDA results sorted by Max <metric>
CUDA results sorted by Avg <metric>

GPU results sorted by Max <metric>
GPU results sorted by Avg <metric>

Threaded results sorted by Max <metric>
Threaded results sorted by Avg <metric>
```

Use `<details>` blocks for sorted views:

```markdown
<details>
<summary>Serial results sorted by Max MFLOPS</summary>

| Rank | Variant | Max MFLOPS | Avg MFLOPS | Min MFLOPS |
|------|---------|------------|------------|------------|
| 1 | `...` | ... | ... | ... |

</details>
```

If the benchmark uses a metric other than MFLOPS, replace the metric name
consistently:

```text
ns/op
GB/s
GFLOPS
items/s
calls/s
speedup
relative time
```

Do not mix metric names without defining them.

### Result interpretation

Do not report numbers alone.

The main result tables must include an `Interpretation` column.  This column
should explain the observed performance class in terms of source shape,
temporary lifetime, precision policy, allocation behavior, compiler behavior,
parallel scaling, memory traffic, or run-to-run variance.

Use a table shape like:

```markdown
| Variant | Max MFLOPS | Avg MFLOPS | Interpretation |
|---------|------------|------------|----------------|
| `kernel_03_mkII` | ... | ... | Reused product object; same hot loop class as the raw reusable-object baseline. |
```

For non-MFLOPS benchmarks, use the appropriate metric:

```markdown
| Variant | Min time | Avg time | Interpretation |
|---------|----------|----------|----------------|
| `kernel_03_mkII` | ... | ... | ... |
```

The interpretation must be technical and specific.  Avoid vague phrases such
as "faster", "slower", "optimized", or "better" unless the reason is also
stated.

Good interpretation examples:

```text
Expression product materializes inside the loop.
Loop-local construction remains expensive.
Reusable temporary object matches the raw reusable-object class.
Scratch fastpath removes repeated temporary allocation.
Stable rounding changes the call path but not the dominant loop structure.
FMA reduces one rounding step but does not remove the backend call overhead.
Four accumulators do not create a new performance class.
The lower average is likely OpenMP run-to-run variance.
The hot loop is memory-traffic limited under this problem size.
The wrapper work is outside the timed loop.
```

### Resource and bandwidth estimates

When useful, include a `Resource or Bandwidth Estimates` section.

Clearly state whether the values are:

* direct hardware-counter measurements
* model estimates derived from benchmark metrics
* static object-size estimates
* cache-footprint estimates
* theoretical upper/lower bounds

Do not present model estimates as measured hardware data.

For fixed-precision multiprecision data, document assumptions such as:

* object/header size
* limb size
* effective precision
* used limbs
* allocated limbs
* input/output bytes per element
* temporary-object footprint
* work-array size
* formula converting the benchmark metric to bandwidth or footprint

Useful estimate classes include:

```text
active-data GB/s
header-inclusive GB/s
allocated-footprint GB/s
temporary-footprint estimate
work-array footprint estimate
```

The section must explain what each model includes and what it excludes.

### Plot regeneration

If plots are committed, include the exact command used to regenerate them from
the committed summary CSV.

The command must use repository-relative paths where possible.

Use a command shape like:

```bash
python3 benchmarks/<backend>/<benchmark>/plot_repeat_summary.py \
    benchmarks/<backend>/<benchmark>/results_raw/<run-id>/summary_<run>.csv \
	    --output-prefix benchmarks/<backend>/<benchmark>/results_raw/<run-id>/<plot-prefix> \
		    --title-prefix "<Backend> <Benchmark> <parameters>"
```

If the plotting script is shared across benchmarks, document the shared script
path.

The README should not contain plots that cannot be regenerated from committed
data.

### Hotpath disassembly or generated code analysis

Each benchmark README must include a `Hotpath Disassembly` or
`Generated Code Analysis` section when the benchmark is intended to compare
implementation strategies.

The goal is to compare the emitted hot loop against the source-level C/C++
kernel implementation.

Use:

```bash
objdump -Cd --no-show-raw-insn <binary>
```

or the closest platform-equivalent disassembler.

For macOS, an appropriate alternative may be:

```bash
otool -tvV <binary>
```

For compiler IR or generated code investigations, use the relevant tool and
document the exact command.

Addresses are build-specific.  Do not treat absolute addresses as meaningful.
The relevant information is the call sequence, loop structure, object lifetime,
temporary lifetime, arithmetic calls, loads/stores, branches, vectorization,
and reduction structure.

For each representative kernel, include:

* the source kernel file being analyzed
* the executable or binary used for disassembly
* the build options used to generate the binary, if relevant
* the disassembly or generated-code command
* the relevant hot-loop excerpt
* a short explanation comparing the emitted instructions with the C/C++ source
* the observed performance implication

At minimum, compare:

* a raw or reference slow path
* a raw or reference reusable-resource baseline
* the main wrapper or mkII kernel
* a representative parallel kernel, when applicable
* the best-performing kernel from the run
* any surprising or suspicious kernel from the run

The disassembly discussion should explicitly check for:

* temporary object lifetime
* scratch/work-buffer lifetime
* repeated initialization or cleanup inside the loop
* repeated allocation-like behavior
* number of backend arithmetic calls per element or operation
* whether the loop body matches the intended source-level shape
* loop unrolling
* branch structure
* vectorization or lack of vectorization
* FMA generation, when relevant
* precision or rounding mode calls inside the loop
* accumulator dependency
* memory load/store pattern
* OpenMP or threaded per-worker loop shape
* whether final reduction is outside the hot loop

For GMP/MPFR/MPC-style code, explicitly check for calls such as:

```text
mpf_init2
mpf_clear
mpf_mul
mpf_add
mpfr_init2
mpfr_clear
mpfr_mul
mpfr_add
mpc_init2
mpc_clear
```

Good disassembly conclusions are specific:

```text
The hot loop has one backend multiply and one backend add per element.
The wrapper work is outside the timed loop.
The loop still calls init and clear per element.
The OpenMP outlined loop has the same arithmetic sequence as the serial reusable-object baseline.
The final reduction is outside the per-thread hot loop.
The FMA variant changes the arithmetic call sequence but does not remove temporary construction.
The fixed-precision fastpath removes repeated temporary initialization from the hot loop.
```

### Lessons Learned

End each benchmark README with a `Lessons Learned` section.

This section must summarize the actual performance boundary found in the run.
Do not merely restate the fastest variant.

The section should answer:

* What source-level shape controls performance?
* Which optimization changes the hot loop?
* Which optimization only changes syntax but not the emitted hot loop?
* Which variants are in the same performance class?
* Which result is probably run-to-run variance?
* Which result needs further verification?
* What should guide the next implementation step?

Good examples of lessons:

```text
The main performance boundary is temporary lifetime, not wrapper syntax.
The reusable-temporary kernel is the practical serial baseline.
The fixed-precision fastpath matters for expression-form kernels.
The fixed-precision fastpath does not help explicitly constructed loop-local objects.
Four-way unrolling does not create a new serial performance class.
OpenMP results should be interpreted by performance class, not by a single fastest run.
The generated hot loop, not the source expression alone, determines the performance class.
```

### No standard allocation counters

Do not add or keep allocation-counter instrumentation as part of the standard
benchmark reports.

Allocation counters are not part of the normal benchmark-reporting workflow.
Allocation-related behavior should usually be inferred from:

* source-level kernel shape
* whether temporary objects are constructed inside the timed loop
* hotpath disassembly
* backend init/clear calls inside the loop
* observed performance class

If allocation behavior must be investigated, do it as a separate focused
diagnostic task.  Do not mix diagnostic allocation-counter code into the normal
benchmark kernels or standard README report unless the benchmark is explicitly
about allocation behavior.

### Updating benchmark reports

When updating a benchmark README:

1. Remove stale raw data unless it is intentionally preserved for comparison.
2. Run the benchmark with the documented parameters.
3. Store raw logs, raw CSV, summary CSV, and plots under a new `results_raw/<run-id>/`.
4. Regenerate static Markdown tables from the committed CSV data.
5. Update the recorded-run section.
6. Update serial and parallel result tables.
7. Update plots and plot-regeneration commands.
8. Refresh representative disassembly snippets when the kernel source or build flags changed.
9. Update `Lessons Learned` based on the new data.
10. Check that all paths in the README are repository-relative and valid.

Do not update only the headline numbers.  A benchmark README must remain
self-contained and reproducible.
