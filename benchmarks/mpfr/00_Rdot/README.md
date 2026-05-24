<!-- SPDX-License-Identifier: BSD-2-Clause -->

# 00_Rdot

This directory benchmarks the MPFR real dot product

```text
sum_i x_i * y_i
```

with raw MPFR C kernels and `mpfrxx::mpfr_class` wrapper kernels.  The current
layout follows the `benchmarks/mpfr/02_Rgemv` reporting model: numbered variants
describe source-level kernel shape, while suffixes describe build or context
modifiers.  This keeps temporary lifetime, rounding capture, FMA capture, and
fixed-precision assumptions as separate questions.

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
build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_PRECISION_FMA 10000000 512
```

The repeat runner uses the same taxonomy:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread     benchmarks/mpfr/00_Rdot/run_repeat.sh build_bench_release 10000000 512 10
```

Wrapper targets omit a separate `mkII` suffix because this directory has only
the mkII MPFR wrapper implementation.  Target suffixes separate source shape
from build flags:

| Suffix | Kind | Meaning |
|--------|------|---------|
| none | source baseline | Ordinary wrapper source for the numbered variant. |
| `PRECISION` | build modifier | Builds the same source with `GMPFRXX_MKII_FAST_FIXED_PREC`. |
| final `FMA` | build modifier | Builds an FMA-capturable source with `GMPFRXX_MKII_ENABLE_FMA`. |
| `PRECISION_FMA` | build modifier combination | Fixed-precision fastpath plus FMA-enabled expression lowering. |

Rdot does not use a `ROUNDING` suffix.  Explicit rounding capture is a source
shape and is therefore represented by variants `07` and `08`.

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
precision = 512
repeat = 10
OMP_NUM_THREADS = 32
OMP_PLACES = cores
OMP_PROC_BIND = spread
```

## Variant Shapes

The timed body is `_Rdot()`.  Variants `01`-`06` keep the same source-level
meaning as the GMP Rdot benchmark.  Variants `07` and `08` are MPFR-specific
explicit-context forms that capture rounding before the loop.

| Variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|--------------------|---------------------------|---------|
| `01` | `acc += dx[i] * dy[i]` | Expression product is formed in the compound assignment. | Test the ET spelling.  FMA builds can lower this source to one `mpfr_fma` call per element. |
| `02` | `mpfr_class templ = dx[i] * dy[i]; acc += templ;` | Loop-local product object is constructed and destroyed inside every iteration. | Intentionally expensive control for temporary lifetime. |
| `03` | `templ = dx[i] * dy[i]; acc += templ;` | One product object is initialized before the loop and reused. | Main non-FMA reusable-product wrapper shape. |
| `04` | `templ = dx[i]; templ *= dy[i]; acc += templ;` | One product object is reused, but each iteration copies `dx[i]` before multiplication. | Separate product-object reuse from copy-then-multiply spelling. |
| `05` | Four accumulators with one reused product object. | Four accumulators share one product temporary. | Test accumulator unrolling while keeping one product temporary. |
| `06` | Four accumulators with four reused product objects. | Four accumulators and four independent product temporaries are reused. | Test accumulator unrolling plus independent product temporaries. |
| `07` | `with_context(acc, precision, rnd) += dx[i] * dy[i]` | Rounding is captured before the loop; `PRECISION` builds compile out fixed-precision context checks. | Context-bound `01`; FMA builds can lower this source to `mpfr_fma`. |
| `08` | `with_context(templ, precision, rnd) = dx[i] * dy[i]; with_context(acc, precision, rnd) += templ;` | One product object is reused and both product and accumulator updates use cached context rounding. | Context-bound `03`; raw-C-like non-FMA reusable-product control. |

Serial and OpenMP wrapper variants use the same numbering.  OpenMP variants use
per-thread partial accumulators and perform the final reduction outside the
per-worker hot loop.

## C Native Equivalent Kernels

The mapping is based on the timed `_Rdot()` source shape and generated hot loop,
not just on matching numeric suffixes.  Raw C kernels encode rounding and FMA
directly.  Wrapper kernels isolate fixed precision and FMA through suffixes.

| C native kernel | Equivalent C++ wrapper kernel(s) | Equivalence basis |
|-----------------|----------------------------------|-------------------|
| `C_native_01` | closest to `kernel_02` | Legacy raw C loop-local product control.  It is not the exact equivalent of wrapper `01` expression syntax. |
| `C_native_01_FMA` | `kernel_01_FMA`, `kernel_01_PRECISION_FMA`, `kernel_07_FMA`, `kernel_07_PRECISION_FMA` | One `mpfr_fma` call per element.  `07` additionally proves cached wrapper context can reach the same call shape. |
| `C_native_02` | `kernel_02`, `kernel_02_PRECISION` | Loop-local wrapper product materialization. |
| `C_native_03` | `kernel_03`, `kernel_03_PRECISION`, `kernel_08`, `kernel_08_PRECISION` | One reusable product object with split multiply/add. |
| `C_native_04` | `kernel_04`, `kernel_04_PRECISION` | Copy-then-multiply reusable product. |
| `C_native_05` | `kernel_05`, `kernel_05_PRECISION` | Four accumulators sharing one reusable product temporary. |
| `C_native_06` | `kernel_06`, `kernel_06_PRECISION` | Four accumulators with four reusable product temporaries. |
| `C_native_openmp_01_FMA` | `kernel_openmp_01_FMA`, `kernel_openmp_01_PRECISION_FMA`, `kernel_openmp_07_FMA`, `kernel_openmp_07_PRECISION_FMA` | OpenMP one-call FMA worker loop. |
| `C_native_openmp_02` | `kernel_openmp_02`, `kernel_openmp_02_PRECISION` | OpenMP loop-local product object. |
| `C_native_openmp_03` | `kernel_openmp_03`, `kernel_openmp_03_PRECISION`, `kernel_openmp_08`, `kernel_openmp_08_PRECISION` | OpenMP reusable-product split multiply/add shape. |
| `C_native_openmp_04` | `kernel_openmp_04`, `kernel_openmp_04_PRECISION` | OpenMP copy-then-multiply shape. |
| `C_native_openmp_05` | `kernel_openmp_05`, `kernel_openmp_05_PRECISION` | OpenMP four-accumulator, one-product control. |
| `C_native_openmp_06` | `kernel_openmp_06`, `kernel_openmp_06_PRECISION` | OpenMP four-accumulator, four-product control. |

There is no exact raw C source equivalent for the non-FMA wrapper expression
spelling `acc += dx[i] * dy[i]`; raw C must choose either split `mpfr_mul` plus
`mpfr_add`, or fused `mpfr_fma`.

## Recorded Run

No fresh repeat-10 result is committed for this target matrix yet.  The older
MPFR Rdot raw result directories were removed so this README does not reference
stale data from the previous `mkII_STABLE_ROUNDING_*` naming matrix.

The next committed run should use:

```bash
OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread     benchmarks/mpfr/00_Rdot/run_repeat.sh build_bench_release 10000000 512 10
```

The runner writes:

```text
benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_<timestamp>/
```

and generates the raw log, raw CSV, summary CSV, and serial/OpenMP plots.

## Resource or Bandwidth Estimates

The values in this section are static model estimates, not hardware-counter
measurements.

For 512-bit MPFR data, each input object has a 32-byte `mpfr_t` header and an
8-limb significand allocation, assuming 64-bit limbs.  The active limb payload
is therefore 64 bytes per element per vector, and the header-inclusive input
footprint is approximately 96 bytes per element per vector.

For Rdot, each iteration reads `x[i]` and `y[i]` and updates a small set of
accumulators.  A simple lower-bound traffic model is:

```text
active-limb bytes/iteration = 2 * 64 = 128 bytes
header-inclusive bytes/iteration = 2 * (32 + 64) = 192 bytes
active-limb GB/s ~= MFLOPS / 2 * 128 / 1e3
header-inclusive GB/s ~= MFLOPS / 2 * 192 / 1e3
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
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_PRECISION_FMA
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07_PRECISION_FMA
```

The expected checks are:

| Kernel class | Hot-loop check |
|--------------|----------------|
| `C_native_01_FMA` | One `mpfr_fma` call per element with rounding captured outside the loop. |
| `kernel_02` | `mpfr_init2` / `mpfr_clear` from loop-local product construction may appear in the hot loop. |
| `kernel_03` | Reusable product object should move product construction outside the loop and leave split multiply/add inside. |
| `kernel_07_PRECISION_FMA` | Context-bound expression should avoid per-iteration default rounding lookup and lower to `mpfr_fma`. |
| `kernel_openmp_07_PRECISION_FMA` | Worker loop should match the serial context/FMA class; final reduction should be outside the worker hot loop. |

## Lessons Learned

The current change is a taxonomy cleanup rather than a completed performance
report.  The important boundary is now explicit in the target names:

- Numbered variants define source shape and temporary lifetime.
- `07` and `08` are source-level context variants because rounding capture
  changes the C++ source, not just build flags.
- `PRECISION` is a build modifier and does not create a new source variant.
- `FMA` is a build modifier for FMA-capturable source shapes, primarily `01`
  and `07`.
- Wrapper target names no longer carry an unnecessary `mkII` suffix in this
  directory.
- Future results should be interpreted by hot-loop class: loop-local product,
  reusable split multiply/add, expression FMA, and context-bound expression FMA.
