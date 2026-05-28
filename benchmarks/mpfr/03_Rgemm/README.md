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

# 03_Rgemm

## Purpose

This directory benchmarks the MPFR real dense matrix-matrix product:

```text
C = alpha * A * B + beta * C
```

It compares raw `mpfr_t` C native kernels against `mpfrxx::mpfr_class`
wrapper kernels. The main question is whether each wrapper source shape emits
the same MPFR hot-loop class as the corresponding C native kernel, and which
wrapper suffixes are needed to remove repeated rounding/default-state work.

There is no upstream `gmpxx.h`-style `orig` implementation for MPFR in this
benchmark family; MPFR results compare raw C native and mkII wrapper paths.

## Build

From the repository root:

```bash
cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_bench_release -j --target Rgemm_mpfr_C_native_03
cmake --build build_bench_release -j --target Rgemm_mpfr_kernel_03_mkII_ROUNDING_FIXED_PRECISION_FASTPATH_FMA
```

Executables are written under:

```text
build_bench_release/benchmarks/mpfr/03_Rgemm/
```

Individual executables take:

```text
<rows m> <cols k> <cols n> <precision>
```

Example:

```bash
build_bench_release/benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_kernel_03_mkII_ROUNDING_FIXED_PRECISION_FASTPATH_FMA 128 128 128 512
```

## Benchmark Parameters

The current executable arguments define:

| Parameter | Meaning |
|-----------|---------|
| `m` | Number of rows of `A` and `C`. |
| `k` | Shared dimension of `A` and `B`. |
| `n` | Number of columns of `B` and `C`. |
| `precision` | MPFR precision in bits for raw and wrapper objects. |

The benchmark initializes random MPFR values, times only the selected `_Rgemm`
kernel, computes a wrapper reference result, and reports the L1 norm of the
difference.

## Variant Shapes

The MPFR Rgemm source matrix mirrors the GMP Rgemm numbering.

| Variant | Transition from previous variant | Timed source shape | Temporary/resource policy | Purpose |
|---------|----------------------------------|--------------------|---------------------------|---------|
| `01` | Baseline | Row-dot form, `i -> j -> l`; each `C(i,j)` builds a dot product and then applies `alpha`. | A dot-product temporary is local to each output element. | Stress expression materialization in the direct mathematical spelling. |
| `02` | Change traversal | Rank-1 / column-major update, `j -> l -> i`; `alpha * B(l,j)` is reused across all rows. | One scaled-`B` temporary per `(l,j)` panel step. | Test the BLAS-like streaming update that reuses `alpha * B`. |
| `03` | Reuse temporaries explicitly | `temp = alpha; temp *= B(l,j); templ = temp; templ *= A(i,l); C += templ`. | `temp` and `templ` are created outside the timed inner loop and reused. | Compare wrapper code against a raw reusable-object C native baseline. |
| `04` | Add 4x4 output blocking | 4x4 `C` tile accumulator with edge redirection. | A 4x4 set of managed `mpfr_class` accumulators plus scratch objects. | Test whether register-blocking ideas transfer to managed MPFR objects. |
| `05` | Replace 4x4 accumulators with a 4-column panel | Precompute four `scaled_b` values and stream rows with one product object. | Four scaled-`B` scratch values and one reusable product object. | Reduce accumulator object pressure while preserving `B` reuse. |
| `06` | Add row blocking to `05` | Variant `05` with a fixed 256-row block. | Same panel scratch as `05`, applied per row block. | Test OpenMP granularity and cache behavior for the panel kernel. |

Wrapper optimization suffixes are generated for every numbered source shape:

| Suffix | Kind | Meaning |
|--------|------|---------|
| `mkII` | baseline | Ordinary wrapper source with default wrapper rounding lookup behavior. |
| `mkII_ROUNDING` | source modifier | Uses `mpfrxx::with_rounding(..., rounding)` after capturing rounding outside the loop. |
| `mkII_ROUNDING_FIXED_PRECISION_FASTPATH` | build modifier on `ROUNDING` source | Adds `GMPFRXX_MKII_FAST_FIXED_PREC` to the cached-rounding source. |
| `mkII_ROUNDING_FIXED_PRECISION_FASTPATH_FMA` | build modifier on `ROUNDING` source | Adds fixed precision and `GMPFRXX_MKII_ENABLE_FMA`; this only emits `mpfr_fma` when the source expression shape is FMA-capturable. |

## C Native Equivalent Kernels

The raw C native and wrapper variants use the same numbering. Equivalence is
based on source shape and emitted MPFR calls.

| C native kernel | Wrapper equivalent | Notes |
|-----------------|--------------------|-------|
| `Rgemm_mpfr_C_native_01` | `Rgemm_mpfr_kernel_01_mkII*` | Same row-dot source shape; wrapper expression handling can still affect temporary construction and rounding lookup. |
| `Rgemm_mpfr_C_native_02` | `Rgemm_mpfr_kernel_02_mkII*` | Same rank-1 traversal. |
| `Rgemm_mpfr_C_native_03` | `Rgemm_mpfr_kernel_03_mkII_ROUNDING_FIXED_PRECISION_FASTPATH*` | Closest exact hot-loop match: reusable `temp`/`templ`, cached rounding, and split `mpfr_mul` + `mpfr_add`. |
| `Rgemm_mpfr_C_native_04` | `Rgemm_mpfr_kernel_04_mkII*` | Same 4x4 accumulator shape. |
| `Rgemm_mpfr_C_native_05` | `Rgemm_mpfr_kernel_05_mkII*` | Same 4-column panel shape. |
| `Rgemm_mpfr_C_native_06` | `Rgemm_mpfr_kernel_06_mkII*` | Same row-blocked 4-column panel shape. |

The `FMA` suffix is not enough by itself to guarantee `mpfr_fma`. Variant `03`
spells the update as reusable `templ *= A; C += templ`, so the FMA-enabled
target remains a split multiply/add hot loop. A source shape must expose
`a += b * c` for the expression-template FMA path to capture it.

## Recorded Run

No committed full MPFR Rgemm timing sweep is recorded in this README yet. The
disassembly below was taken from the current `build_bench_release` binaries
after building the representative targets listed in the `Build` section.

## Resource or Bandwidth Estimates

No bandwidth estimate is reported here yet. Rgemm has much higher arithmetic
intensity than Rdot/Raxpy, and the useful estimate should be derived from a
fresh committed timing sweep rather than from disassembly alone.

## Hotpath Disassembly

Representative disassembly was collected with:

```bash
objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/03_Rgemm/<binary>
```

Addresses and TLS offsets are build-specific. The relevant information is the
call sequence inside the loops.

| Kernel | Representative hot-loop calls | Rounding/default-state work in arithmetic loop | Interpretation |
|--------|-------------------------------|-----------------------------------------------|----------------|
| `Rgemm_mpfr_C_native_03` | `mpfr_set4`, `mpfr_mul`, `mpfr_add` | No; `mpfr_get_default_rounding_mode` is outside the loop. | Raw reusable-object baseline. |
| `Rgemm_mpfr_kernel_03_mkII` | `mpfr_get_default_rounding_mode`, TLS guard, `mpfr_set4`, `mpfr_mul`, `mpfr_add` | Yes | Baseline wrapper does not match the cached-rounding C native hot loop. |
| `Rgemm_mpfr_kernel_03_mkII_ROUNDING_FIXED_PRECISION_FASTPATH_FMA` | `mpfr_set4`, `mpfr_mul`, `mpfr_add` | No inner-loop rounding lookup | Closest wrapper match for C native `03`; `FMA` suffix does not change this reusable-temp source shape into `mpfr_fma`. |
| `Rgemm_mpfr_kernel_openmp_06_mkII_ROUNDING_FIXED_PRECISION_FASTPATH_FMA` | panel `mpfr_set4` / `mpfr_mul`, row update `mpfr_set4` / `mpfr_mul` / `mpfr_add` | No inner-loop rounding lookup | OpenMP outlined panel loop carries cached rounding through the worker function. |

### `Rgemm_mpfr_C_native_03`

The raw C native reusable-temporary kernel reads the default rounding mode once
before the matrix loops and passes the cached `mpfr_rnd_t` to each MPFR call.

```asm
2d66: call   mpfr_get_default_rounding_mode@plt
...
2ea0: mov    0x30(%rsp),%rsi
2ea5: mov    %r13d,%edx
2ea8: mov    %rbx,%rdi
2eab: mov    0x8(%rsi),%ecx
2eae: call   mpfr_set4@plt
2eb3: mov    0x18(%rsp),%rdx
2eb8: mov    %r13d,%ecx
2ebb: mov    %rbx,%rsi
2ebe: mov    %rbx,%rdi
2ec1: call   mpfr_mul@plt
...
2ef0: mov    0x98(%rsp),%ecx
2ef7: mov    %r13d,%edx
2efa: mov    %rbx,%rsi
2efd: mov    %r12,%rdi
2f00: add    $0x1,%r15
2f04: call   mpfr_set4@plt
2f09: mov    %r14,%rdx
2f0c: mov    %r13d,%ecx
2f0f: mov    %r12,%rsi
2f12: mov    %r12,%rdi
2f15: add    $0x20,%r14
2f19: call   mpfr_mul@plt
2f1e: mov    %rbp,%rsi
2f21: mov    %rbp,%rdi
2f24: mov    %r13d,%ecx
2f27: mov    %r12,%rdx
2f2a: add    $0x20,%rbp
2f2e: call   mpfr_add@plt
2f33: cmp    %r15,0x8(%rsp)
2f38: jne    2ef0 <_Rgemm+0x210>
```

This is the C native target that wrapper `03` should be compared against:
cached rounding plus reusable `temp`/`templ` objects.

### `Rgemm_mpfr_kernel_03_mkII`

The vanilla wrapper source has the same high-level reusable temporary spelling,
but without the `ROUNDING` suffix it still enters the default-state and
rounding lookup path from the loop.

```asm
33a0: cmpb   $0x0,%fs:0xfffffffffffffff8
...
33c3: call   mpfr_get_default_rounding_mode@plt
33c8: mov    %eax,%fs:0xfffffffffffffffc
33d0: call   mpfr_get_default_rounding_mode@plt
33d5: add    $0x1,%r14
33d9: mov    %rbx,%rsi
33dc: mov    %rbx,%rdi
33df: mov    %eax,%ecx
33e1: mov    %r12,%rdx
33e4: add    $0x20,%rbx
33e8: call   mpfr_mul@plt
33ed: cmp    %r14,%r13
33f0: jne    33a0 <_Rgemm+0xc0>
```

A later update loop shows the same pattern combined with the reusable object
operations:

```asm
3570: cmpb   $0x0,%fs:0xfffffffffffffff8
...
3593: call   mpfr_get_default_rounding_mode@plt
3598: mov    %eax,%fs:0xfffffffffffffffc
35a0: call   mpfr_get_default_rounding_mode@plt
...
35b4: call   mpfr_set4@plt
...
35e9: call   mpfr_get_default_rounding_mode@plt
...
35fd: call   mpfr_mul@plt
```

The `cmpb $0x0,%fs:...` instruction is the Linux x86-64 TLS guard check for
the wrapper per-thread MPFR default-state initialization flag. The offset is
DSO/build specific. Its presence in this excerpt means the vanilla target is
not the same hot-loop class as the C native cached-rounding baseline.

### `Rgemm_mpfr_kernel_03_mkII_ROUNDING_FIXED_PRECISION_FASTPATH_FMA`

The rounded/fixed/FMA build captures rounding before the matrix loops.

```asm
3126: cmpb   $0x0,%fs:0xfffffffffffffff8
312f: je     340b <_Rgemm+0x34b>
3135: call   mpfr_get_default_rounding_mode@plt
313a: mov    %eax,%r12d
```

The arithmetic loop then uses the cached `%r12d` rounding value and no longer
calls `mpfr_get_default_rounding_mode` inside the element update.

```asm
32e0: mov    0x30(%rsp),%rsi
32e5: mov    0x8(%rsp),%rbx
32ea: mov    %r12d,%edx
32ed: mov    0x8(%rsi),%ecx
32f0: mov    %rbx,%rdi
32f3: call   mpfr_set4@plt
32f8: mov    0x18(%rsp),%rdx
32fd: mov    %r12d,%ecx
3300: mov    %rbx,%rsi
3303: mov    %rbx,%rdi
3306: call   mpfr_mul@plt
...
3330: mov    0xa8(%rsp),%ecx
3337: mov    0x8(%rsp),%rsi
333c: mov    %r12d,%edx
333f: mov    %rbp,%rdi
3342: call   mpfr_set4@plt
3347: mov    %r12d,%ecx
334a: mov    %r13,%rdx
334d: mov    %rbp,%rsi
3350: mov    %rbp,%rdi
3353: call   mpfr_mul@plt
3358: mov    %r12d,%ecx
335b: mov    %rbp,%rdx
335e: mov    %rbx,%rsi
3361: mov    %rbx,%rdi
3364: call   mpfr_add@plt
3369: add    $0x1,%r14
336d: add    $0x20,%rbx
3371: add    $0x20,%r13
3375: cmp    %r14,%r15
3378: jne    3330 <_Rgemm+0x270>
```

This is the closest wrapper hot-loop match to `Rgemm_mpfr_C_native_03`.
Although the target has an `FMA` suffix, variant `03` is a split reusable-temp
source shape, so the emitted arithmetic remains `mpfr_mul` plus `mpfr_add`.

### `Rgemm_mpfr_kernel_openmp_06_mkII_ROUNDING_FIXED_PRECISION_FASTPATH_FMA`

The OpenMP row-blocked panel variant passes cached rounding into the worker
function. The panel scaling loop below uses that value directly.

```asm
3abf: mov    %r12,%rdi
3ac2: mov    0x8(%r15),%ecx
3ac6: mov    %r13d,%edx
3ac9: mov    %r15,%rsi
3acc: shl    $0x5,%rdi
3ad0: add    $0x1,%r12
3ad4: lea    (%r14,%rdi,1),%rbp
3ad8: mov    %rbp,%rdi
3adb: call   mpfr_set4@plt
3ae0: mov    %rbx,%rdx
3ae3: mov    %r13d,%ecx
3ae6: mov    %rbp,%rsi
3ae9: mov    %rbp,%rdi
3aec: call   mpfr_mul@plt
3af1: add    0x8(%rsp),%rbx
3af6: cmp    %r12,(%rsp)
3afa: jg     3abf <...+0x59f>
```

The representative OpenMP hot loop has no `mpfr_get_default_rounding_mode`
call. As with serial variant `03`, the `FMA` suffix does not force FMA for this
panel source shape.

## Lessons Learned

For MPFR Rgemm, C native equivalence requires both source-shape alignment and
rounding-policy alignment. Variant `03` demonstrates the distinction clearly:
the vanilla wrapper has the same reusable temporary source shape, but its
disassembly still contains loop-visible rounding/default-state work. The
`ROUNDING_FIXED_PRECISION_FASTPATH` target removes that work and reaches the
same split `mpfr_mul` / `mpfr_add` call class as the raw C native baseline.

The `FMA` suffix is a build capability, not a promise that every source shape
will emit `mpfr_fma`. Rgemm variants `03` and `06` use explicit reusable
temporaries, so they remain split multiply/add loops even in the FMA-enabled
build. FMA needs a source expression that exposes the multiply-add as one
capturable expression.
