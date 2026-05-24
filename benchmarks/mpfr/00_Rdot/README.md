<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 00_Rdot

This directory benchmarks the MPFR real dot product

```text
sum_i x_i * y_i
```

with raw MPFR C kernels and `mpfrxx::mpfr_class` wrapper kernels.  The benchmark
is organized like `benchmarks/mpfr/02_Rgemv`: numbered variants describe the
source-level kernel shape, while suffixes describe source modifiers and build
modifiers.  The goal is to make temporary lifetime, rounding capture, FMA build options, fixed-precision assumptions, and OpenMP worker loops directly visible
from the executable name and the hotpath disassembly.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j
```

Executables are created under:

```text
build_bench_release/benchmarks/mpfr/00_Rdot/
```

Each executable takes `<vector size> <precision>`. Example:

```bash
build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05_ROUNDING_PRECISION_FMA 10000000 512
```

The repeat-10 runner uses the same source/build taxonomy:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread     benchmarks/mpfr/00_Rdot/run_repeat.sh build_bench_release 10000000 512 10
```

MPFR Rdot wrapper targets omit a separate `mkII` implementation suffix because
this directory has only the mkII wrapper implementation.  The target suffixes
separate source changes from build flags:

| Suffix | Kind | Meaning |
|--------|------|---------|
| none | source baseline | Ordinary wrapper source for the numbered algorithm. |
| `ROUNDING` | source modifier | Captures `mpfrxx::evaluation_context` before the loop and uses `with_context` in the timed body.  No compile-time flag is implied. |
| `PRECISION` | build modifier | Builds the same source with `GMPFRXX_MKII_FAST_FIXED_PREC`. |
| final `FMA` | build modifier | Builds the FMA-capturable source with `GMPFRXX_MKII_ENABLE_FMA`. |

The C native targets encode rounding and FMA directly in their source, so they
do not split into `ROUNDING` and non-`ROUNDING` forms.

## Benchmark Parameters

| Parameter | Meaning |
|-----------|---------|
| `N` | Number of vector elements. |
| `precision` | MPFR precision in bits for all input values and accumulators. |
| `repeat` | Number of timed process executions per executable. |
| `OMP_NUM_THREADS` | OpenMP worker count for `openmp` executables. |
| `OMP_PLACES`, `OMP_PROC_BIND` | OpenMP affinity controls used by the runner. |

The intended committed run is:

```text
N = 10000000
precision = 512 and 1024
repeat = 10
OMP_NUM_THREADS = 32
OMP_PLACES = cores
OMP_PROC_BIND = spread
```

## Variant Shapes

The timed body is `_Rdot()`.  The numbered variant is written as a one-step
transition: each row says what changed from the previous source shape and why
that change is measured.  `ROUNDING`, `PRECISION`, and final `FMA` suffixes
modify the same numbered shape without changing the variant number.

| Variant | Transition from previous variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|----------------------------------|--------------------|---------------------------|---------|
| `01` | Starting point. | `acc += dx[i] * dy[i]` | Expression product is formed in the compound assignment. | Test the ET spelling.  `FMA` builds can lower this source to one `mpfr_fma` call per element. |
| `02` | `01 -> 02`: force product materialization inside the loop. | `mpfr_class templ = dx[i] * dy[i]; acc += templ;` | Loop-local product object is constructed and destroyed inside every iteration. | Intentionally expensive control for temporary lifetime. |
| `03` | `02 -> 03`: move the product object outside the loop. | `templ = dx[i] * dy[i]; acc += templ;` | One product object is initialized before the loop and reused. | Main reusable-product split multiply/add wrapper shape. |
| `04` | `03 -> 04`: change product spelling to copy-then-multiply. | `templ = dx[i]; templ *= dy[i]; acc += templ;` | One product object is reused, but each iteration copies `dx[i]` before multiplication. | Separate product-object reuse from copy-then-multiply spelling. |
| `05` | `04 -> 05`: add accumulator unrolling and remove product materialization. | Four accumulators with direct `accN += dx[i+k] * dy[i+k]` updates. | No product object is materialized in the source. | FMA-capturable four-accumulator source. |
| `06` | `05 -> 06`: keep the direct-expression unrolled class for the second native FMA comparison point. | Four accumulators with direct `accN += dx[i+k] * dy[i+k]` updates. | No product object is materialized in the source. | Paired with `C_native_06_FMA`; expected to be in the same hot-loop class as `05`. |

Serial and OpenMP wrapper variants use the same numbering.  OpenMP variants use
per-thread partial accumulators and perform the final reduction outside the
per-worker hot loop.

## Source Transitions

The transition table above is intentionally one-dimensional.  A variant number
changes the source shape; suffixes then ask separate questions about rounding
capture, FMA enablement, and fixed precision.

For every numbered wrapper variant `01` through `06`, including the matching
OpenMP variant, the generated wrapper target family is:

```text
<base>
<base>_PRECISION
<base>_ROUNDING
<base>_ROUNDING_PRECISION
```

`ROUNDING` is a source modifier for the same numbered shape: it captures the
MPFR precision and rounding mode before the timed loop, then routes the timed
updates through `with_context`.

`FMA` is a build modifier, not a separate source file.  It is generated only for
FMA-capturable source variants `01`, `05`, and `06`, always paired with fixed precision:

```text
<base>_PRECISION_FMA
<base>_ROUNDING_PRECISION_FMA
```

Variants `02` through `04` intentionally materialize product temporaries, so an
FMA target for those source files would not measure the same source-level shape.

## C Native Equivalent Kernels

The mapping is based on the timed `_Rdot()` source shape and generated hot loop,
not just on matching numeric suffixes.  Raw C kernels encode rounding and FMA
directly; wrapper kernels use suffixes to isolate those effects.

| C native kernel | Equivalent C++ wrapper kernel(s) | Equivalence basis |
|-----------------|----------------------------------|-------------------|
| `C_native_01` | closest to `kernel_02` | Legacy raw C loop-local product control.  It is not the exact equivalent of wrapper `01` expression syntax. |
| `C_native_01_FMA` | `kernel_01_PRECISION_FMA`, `kernel_01_ROUNDING_PRECISION_FMA` | One `mpfr_fma` call per element when ET FMA capture succeeds. |
| `C_native_02` | `kernel_02`, `kernel_02_PRECISION`, `kernel_02_ROUNDING`, `kernel_02_ROUNDING_PRECISION` | Loop-local product object. |
| `C_native_03` | `kernel_03`, `kernel_03_PRECISION`, `kernel_03_ROUNDING`, `kernel_03_ROUNDING_PRECISION` | One reusable product object with split multiply/add. |
| `C_native_04` | `kernel_04`, `kernel_04_PRECISION`, `kernel_04_ROUNDING`, `kernel_04_ROUNDING_PRECISION` | Copy-then-multiply reusable product. |
| `C_native_05` | `kernel_03`, `kernel_04` are the closest non-FMA temporary baselines | Raw C 05 used a four-accumulator reusable temporary; wrapper 05 now deliberately uses direct expression to expose FMA. |
| `C_native_05_FMA` | `kernel_05_PRECISION_FMA`, `kernel_05_ROUNDING_PRECISION_FMA` | Four accumulators with one direct `mpfr_fma`-class update per lane. |
| `C_native_06` | `kernel_03`, `kernel_04` are the closest non-FMA temporary baselines | Raw C 06 used four reusable product temporaries; wrapper 06 now deliberately uses direct expression to expose FMA. |
| `C_native_06_FMA` | `kernel_06_PRECISION_FMA`, `kernel_06_ROUNDING_PRECISION_FMA` | Four accumulators with one direct `mpfr_fma`-class update per lane. |
| `C_native_openmp_01_FMA` | `kernel_openmp_01_PRECISION_FMA`, `kernel_openmp_01_ROUNDING_PRECISION_FMA` | OpenMP one-call FMA worker loop. |
| `C_native_openmp_02` | `kernel_openmp_02`, `kernel_openmp_02_PRECISION`, `kernel_openmp_02_ROUNDING`, `kernel_openmp_02_ROUNDING_PRECISION` | OpenMP loop-local product object. |
| `C_native_openmp_03` | `kernel_openmp_03`, `kernel_openmp_03_PRECISION`, `kernel_openmp_03_ROUNDING`, `kernel_openmp_03_ROUNDING_PRECISION` | OpenMP reusable-product split multiply/add shape. |
| `C_native_openmp_04` | `kernel_openmp_04`, `kernel_openmp_04_PRECISION`, `kernel_openmp_04_ROUNDING`, `kernel_openmp_04_ROUNDING_PRECISION` | OpenMP copy-then-multiply shape. |
| `C_native_openmp_05_FMA` | `kernel_openmp_05_PRECISION_FMA`, `kernel_openmp_05_ROUNDING_PRECISION_FMA` | OpenMP four-accumulator direct FMA worker loop. |
| `C_native_openmp_06_FMA` | `kernel_openmp_06_PRECISION_FMA`, `kernel_openmp_06_ROUNDING_PRECISION_FMA` | OpenMP four-accumulator direct FMA worker loop. |

There is no exact raw C source equivalent for the non-FMA wrapper expression
spelling `acc += dx[i] * dy[i]`; raw C must choose either split `mpfr_mul` plus
`mpfr_add`, or fused `mpfr_fma`.

## Recorded Run

No fresh repeat-10 result is committed for this target matrix yet.  Older MPFR
Rdot raw result directories were removed so this README does not reference
stale data from the previous target matrix.

The next committed 512-bit run should use:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread     benchmarks/mpfr/00_Rdot/run_repeat.sh build_bench_release 10000000 512 10
```

The matching 1024-bit run should use:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread     benchmarks/mpfr/00_Rdot/run_repeat.sh build_bench_release 10000000 1024 10
```

The runner writes raw logs, raw CSV, summary CSV, and plots under:

```text
benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p<precision>_repeat10_<timestamp>/
```

## Resource or Bandwidth Estimates

The values in this section are static model estimates, not hardware-counter
measurements.

For 512-bit MPFR data, each input object has a 32-byte `mpfr_t` header and an
8-limb significand allocation, assuming 64-bit limbs.  The active limb payload
is therefore 64 bytes per element per vector, and the header-inclusive input
footprint is approximately 96 bytes per element per vector.

For 1024-bit MPFR data, the active limb payload doubles to 128 bytes per object,
and the header-inclusive input footprint is approximately 160 bytes per element
per vector.

For Rdot, each iteration reads `x[i]` and `y[i]` and updates a small set of
accumulators.  A simple lower-bound traffic model is:

```text
active-limb GB/s ~= MFLOPS / 2 * active_limb_bytes_per_iteration / 1e3
header-inclusive GB/s ~= MFLOPS / 2 * header_inclusive_bytes_per_iteration / 1e3
```

This model excludes allocator metadata, MPFR internal temporaries, instruction
fetch, cache-line overfetch, writeback traffic for accumulators, OpenMP
reduction traffic, and pointer-chase latency.

## Serial Results

Fresh serial results are pending for the new target matrix.

<details>
<summary>Serial results sorted by Max MFLOPS</summary>

No committed fresh data yet.

</details>

<details>
<summary>Serial results sorted by Avg MFLOPS</summary>

No committed fresh data yet.

</details>

## OpenMP Results

Fresh OpenMP results are pending for the new target matrix.

<details>
<summary>OpenMP results sorted by Max MFLOPS</summary>

No committed fresh data yet.

</details>

<details>
<summary>OpenMP results sorted by Avg MFLOPS</summary>

No committed fresh data yet.

</details>

## Hotpath Disassembly

Representative disassembly should be regenerated after the fresh repeat-10 run:

```bash
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01_FMA
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_02
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05_ROUNDING_PRECISION_FMA
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_05_ROUNDING_PRECISION_FMA
```

The expected checks are:

| Kernel class | Hot-loop check |
|--------------|----------------|
| `C_native_01_FMA` | One `mpfr_fma` call per element with rounding captured outside the loop. |
| `kernel_02` | `mpfr_init2` / `mpfr_clear` from loop-local product construction may appear in the hot loop. |
| `kernel_03` | Reusable product object should move product construction outside the loop and leave split multiply/add inside. |
| `kernel_03_ROUNDING_PRECISION` | Context-bound reusable product path should avoid per-iteration default rounding lookup. |
| `kernel_05_ROUNDING_PRECISION_FMA` | Context-bound four-accumulator direct expression should avoid per-iteration default rounding lookup and lower each update to `mpfr_fma`. |
| `kernel_openmp_05_ROUNDING_PRECISION_FMA` | Worker loop should match the serial unrolled context/FMA class; final reduction should be outside the worker hot loop. |

## Lessons Learned

The current change is a taxonomy cleanup rather than a completed performance
report.  The important boundary is now explicit in the target names:

- Numbered variants define source shape and temporary lifetime.
- `ROUNDING` is a source modifier because cached MPFR rounding/context changes
  the C++ wrapper source path.
- `PRECISION` is a build modifier and does not create a new source variant.
- final `FMA` is a build modifier for FMA-capturable source shapes; those shapes are `01`, `05`, and `06`.
- Wrapper target names do not carry an unnecessary `mkII` suffix in this
  directory.
- Future results should be interpreted by hot-loop class: loop-local product,
  reusable split multiply/add, context-bound split multiply/add, and
  context-bound expression FMA.
