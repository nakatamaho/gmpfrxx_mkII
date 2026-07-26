# P2C Ordinary Wrapper Compatibility for MPLAPACK

## 1. Repository, base, and candidate

- Repository: `git@github.com:nakatamaho/gmpfrxx_mkII.git`
- Clean repository path: `/home/docker/gmpfrxx_mkII-p2a-clean.wbx83b`
- Branch: `topic/mplapack-wrapper-compat`
- Approved P2B base: `e4e06547bdbb0080714696937810a45726dd1780`
- Merge base with the approved P2B SHA: `e4e06547bdbb0080714696937810a45726dd1780`
- Starting ahead/behind relative to P2B: `0 0`
- Final candidate: the commit containing this report; resolve with `git rev-parse HEAD` after commit (a commit cannot contain its own SHA).

## 2. Accepted P2B publication

The accepted P2B branch was verified clean at
`e4e06547bdbb0080714696937810a45726dd1780`, its gate passed, and a normal
fast-forward push created/updated
`origin/topic/mplapack-compare-embed-complex` at that exact SHA. No force push
was used.

## 3. Accepted P2B commits

| Commit | Purpose |
|---|---|
| `60221e1efa38bfe435e8ea378504cd9e41e5cc18` | P2B complex comparison embedding implementation and mapped tests |
| `e4e06547bdbb0080714696937810a45726dd1780` | P2B gate, installed consumer, and acceptance report |

The P2B delta was inspected against P2A. It contains no reverse conversion,
general mixed-backend arithmetic, precision metadata negotiation, EDD/TD, or
P2C wrapper-compatibility implementation.

## 4. Immutable requirements

| File | SHA-256 |
|---|---|
| `LOCK.json` | `9fb757df92a36e996e0ea28c9dcf9351cf5063ff95646d750a188e3f7f32b075` |
| `interop_requirements.tsv` | `2bb9700d76cd50a6c4328f497b35e3794065fecc915eac40408f01c0188db00f` |
| `SPIKE.md` | `29b4bf3c831c0a892773139be3e8283153bfb4db4d74726b0893a807cae0928d` |
| `INTEROP_POLICY.md` | `859fe8ebc18100c1225d04633ceaa38140dd5ac48f6203538950bfc40886a9ed` |

All imported hashes match the P2A provenance. `LOCK.json`,
`interop_requirements.tsv`, and `SPIKE.md` are byte-identical to the accepted
MPLAPACK P1 tree at `2ebf798fd0081ecdc5c1b53fc117431c406bf884`.

## 5. SPIKE disposition summary

`P2C_SPIKE_DISPOSITION.tsv` contains 50 unique rows and exactly one valid
disposition per row.

| Category | Count |
|---|---:|
| `UPSTREAM_GENERIC_OPERATOR` | 6 |
| `UPSTREAM_GENERIC_MATH` | 1 |
| `UPSTREAM_GENERIC_WRAPPER_API` | 4 |
| `ALREADY_SATISFIED` | 1 |
| `DOWNSTREAM_MPLAPACK_UTILITY` | 11 |
| `NOT_REQUIRED` | 27 |

| Disposition | Count |
|---|---:|
| `IMPLEMENT_P2C` | 0 |
| `VERIFY_ONLY` | 12 |
| `DEFER_TO_P3` | 6 |
| `DEFER_TO_P4` | 5 |
| `OUT_OF_SCOPE` | 27 |

The accepted SPIKE exposed no missing generic upstream primitive. Therefore no
production header or public API was changed merely for convenience.

## 6. Implemented P2C APIs

There are no `IMPLEMENT_P2C` rows and no newly implemented public wrapper APIs.
The exact required generic surface was already available and was verified:

- MPFR: same-family expression `a * b + sqrt(a)`, `abs`, `floor`, `const_pi`,
  decimal stream output, and hexadecimal stream output.
- MPC: same-family expression `z * z + z` and owning materialization.
- GMP MPF: same-family expression `f * g + f`, multiplication, `abs`, `floor`,
  `const_pi`, and stream output.
- GMP MPFC: same-family expression `c * c + c` and owning materialization.
- Header boundaries: fully qualified legacy and replacement complex names
  remain non-colliding.

All are `VERIFY_ONLY`; existing expression-template architecture and public
headers were left unchanged.

## 7. Verify-only evidence

`test_mplapack_default_512` verifies `DEFAULT-MPFR` and `DEFAULT-GMP` in one
fresh process. `test_mplapack_wrapper_compat` verifies `SAME-MPFR`, `SAME-MPC`,
`SAME-MPF`, `SAME-MPFC`, the three MPFR utility rows, and MPFR/MPC expression
lifetime rows. Existing `test_header_boundaries` verifies
`LEGACY-NAME-COLLISION`.

The installed-tree consumer compiles and runs the same generic API subset from
installed public headers and the `gmpfrxx_mkII::gmpfrxx_mkII` package target.

## 8. Downstream MPLAPACK utilities

The following names are not added to upstream public headers:

- P3 MPFR: `pow2`, `sign`, `nint`, `pi`, `sprintnum`, and
  `sprinthex_mpfr_fixed`.
- P4 GMP: `pow2`, `sign`, `nint`, `pi`, and `sprintnum`.

The disposition matrix records the generic upstream primitive that already
exists for each later MPLAPACK shim. No MPLAPACK header, `mplapackint`,
`castREAL_*`, `castINTEGER_*`, `iceil`, `cabs1`, or MPLAPACK compatibility
header was added upstream.

## 9. Changed files by purpose

- Generic operators, constructors, math, and wrapper API: none.
- Tests: `tests/test_mplapack_default_512.cpp`,
  `tests/test_mplapack_wrapper_compat.cpp`, and two registrations in
  `tests/CMakeLists.txt`.
- Installed public API validation:
  `docs/mplapack_migration/tools/p2c_consumer/CMakeLists.txt` and `main.cpp`.
- Documentation and classification: `P2C_SPIKE_DISPOSITION.tsv` and this
  report.
- Gate and full log: `gate-P2C.sh` and `REPORT-P2C-gate.log`.
- Accepted P2A gate reproducibility repair: `gate-P2A.sh` and
  `REPORT-P2A.md`; details are in section 19.

No generated production source, public header, version, release archive, or
package metadata changed.

## 10. Expression-template lifetime

`test_mplapack_wrapper_compat` statically verifies that all four same-family
expressions produce expression nodes. It then materializes owning results.
Directed nested-temporary cases materialize
`(mpfr_class(1)+mpfr_class(2))*(mpfr_class(3)+mpfr_class(4))` as 21 and an MPC
nested expression as `(8,12)`. Both passed. No borrowing node escapes the full
expression under test.

## 11. Fixed 512-bit default

P2C preserves the existing mechanisms: MPFR default construction uses
`mpfr_class::default_precision()`, and GMP default construction uses the
existing `default_mpf_precision_bits()` whose built-in default is 512. No new
precision state, provider, negotiation, assignment check, sweep, mutation, or
cross-DSO machinery was added.

`test_mplapack_default_512` clears both precision environment variables before
first wrapper use, default-constructs both real wrapper families in one fresh
process, and verifies exactly 512 bits. Result: PASS.

## 12. GMP no-binary64 regression selection

The selected actual MPLAPACK-side operation is the GMP `pow2` utility need from
SPIKE ID `MPLAPACK-GMP-POW2`. P2C verifies its already available generic
primitive, same-family `gmpxx::mpf_class` multiplication.

At ordinary 512-bit default precision, the test forms `x = 1 + 2^-200`,
computes `x*x`, and verifies the residual is strictly between `2^-200` and
`2^-198`. This exercises genuine GMP same-family expression evaluation.

## 13. GMP regression result and route

The production path evaluates through the existing GMP expression-template
multiplication and `mpf_mul`; it does not convert to binary64. Observed output:

```text
P2C GMP pow2 residual: 2^-200 < residual < 2^-198; emulated binary64 residual: 0
```

The test deliberately calls `mpf_get_d` only to construct the negative
historical-fallback reference, which plateaus at zero. The exercised production
path and `math_mpf.hpp` contain no `get_d()`, `mpf_get_d()`, binary64 temporary,
or `std` math fallback. This is a no-binary64 smoke, not a correct-rounding
claim.

## 14. P2A/P2B compatibility

- Repaired P2A gate against the approved P2A tree: PASS, 189/189 tests.
- Accepted P2B gate against the current tree: PASS, 192/192 tests.
- P2A real embedding public API and tests are unchanged.
- P2B complex embedding public API and tests are unchanged.
- Raw `long double` rejection and eight forbidden cross-family arithmetic
  compile-fail tests passed, 9/9.

## 15. Installed-tree consumer

The gate installs the Release build to a fresh `/tmp` prefix, then runs:

```text
cmake -S docs/mplapack_migration/tools/p2c_consumer \
  -B <fresh-consumer-build> -DCMAKE_PREFIX_PATH=<fresh-install-prefix>
cmake --build <fresh-consumer-build> -j32
<fresh-consumer-build>/p2c_consumer
```

Result: configure, compile, link, and execution PASS. The consumer exercises
all four wrapper families, owning expression materialization, both 512-bit real
defaults, GMP information below binary64, stream output, and installed CMake
target discovery.

## 16. Build and test environment

- OS/architecture: Linux x86_64.
- Compiler: GNU C++ 15.2.0.
- CMake/CTest: 4.2.3.
- Build type: Release; jobs: 32; C++17.
- Components: GMP, MPFR, MPC.
- GMP 6.3.0, MPFR 4.2.2, MPC 1.3.1 from system include/library paths.
- Dependency auto-fetch: `GMPFRXX_MKII_DEPS_AUTO_FETCH=OFF`.
- Correctness semantic fast-path macros `GMPFRXX_MKII_ENABLE_FMA`,
  `GMPFRXX_MKII_FAST_FIXED_PREC`, and `GMPFRXX_MKII_FAST_STABLE_RND` are absent
  from the P2C test compile commands, which is this tree's OFF representation.

Complete P2C suite result: 192/192 PASS. The gate additionally ran approved
P2A 189/189 and current P2B 192/192 suites.

## 17. Gate command and result

```text
P2C_JOBS=32 bash docs/mplapack_migration/gate-P2C.sh
```

Result: PASS. The complete output is in `REPORT-P2C-gate.log`; its final lines
include `P2B gate: PASS` and `P2C gate: PASS`.

## 18. Explicit non-goals confirmed

No MPLAPACK-specific public utility API, reverse conversion, MPFR-to-GMP
conversion, mixed adapter arithmetic, adapter compound assignment, precision
metadata handling, default-context machinery, EDD, or TD support was added.
P2A/P2B adapters were not broadened. No release version, archive, or tag was
created.

## 19. Deviations and blockers

A reproducibility defect was discovered in the accepted P2A gate/report. The
forensic snapshot's internally verified `SHA256SUMS` digest is
`6a9fc4d4ab1fbe4ad71a1c5f40657ff6efcb24519edcf09341729b5d4e2c8ad3`, but
the accepted gate stored only 58 hexadecimal characters and the report stored
only 50. Both were corrected to the complete 64-character digest. The snapshot
itself was not modified, and `sha256sum --check SHA256SUMS` passed for every
listed file.

The accepted gate also searched leaf binary80/binary128 adapter headers for
native MPFR calls that are intentionally centralized in
`adapters/detail/binary_float.hpp`. The search targets were corrected to that
accepted implementation file. A `P2A_ROOT` override permits the repaired gate
to validate a detached worktree at the approved P2A SHA. These changes affect
gate reproducibility only; no P2A implementation, test, or public contract was
changed.

Remaining blockers: none.

## 20. Final worktree status

The committed candidate is expected to leave `git status --short` empty. The
exact final status and candidate SHA are recorded in the final phase response.
