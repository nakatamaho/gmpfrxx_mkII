# P2A Real Comparison Embeddings

- MPLAPACK P1: `2ebf798fd0081ecdc5c1b53fc117431c406bf884`
- Locked upstream base: `2f06785c3f1b62f92e1e2026c2c975df73d1e426`
- Clean clone: `/home/docker/gmpfrxx_mkII-p2a-clean.wbx83b`
- Branch: `topic/mplapack-compare-embed-real`
- Dirty evidence snapshot: `/home/docker/gmpfrxx_mkII-prototype-snapshot-p2a-retry.F58Geq`
- Snapshot SHA256SUMS: `6a9fc4d4ab1fbe4ad71a1c5f40657ff6efcb24519edcf09341729b5d4e2c8ad3`

The failed tar capture was caused by placing `-C` after file operands. The old partial snapshot was preserved untouched. The corrected retry snapshot is read-only. No dirty-clone source file or patch was transplanted; the externally added `tests/test_mpf_mpfc_adapter.cpp`, MPFC, complex, EDD, and TD work were excluded.

## Rows and API

The selected API is explicit construction/assignment to an owning `mpfrxx::mpfr_class`.

| REQUIRED row | Status | Route |
|---|---|---|
| double -> mpfr_class | implemented | existing MPFR import constructor |
| dd real -> mpfr_class | implemented | direct MPFR sum of stored components |
| qd real -> mpfr_class | implemented | direct MPFR sum of stored components |
| binary80 real -> mpfr_class | implemented | native `mpfr_set_ld` |
| binary128 real -> mpfr_class | implemented | native `mpfr_set_float128` |
| GMP MPF -> mpfr_class | implemented | direct `mpfr_set_f` |

No source or destination precision metadata is inspected, compared, negotiated, or preserved. No reverse conversion or mixed arithmetic was added.

## Tests and gate

Automatic dependency fetching was disabled. The complete relevant commands were:

```text
cmake -S . -B build-p2a -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF
cmake --build build-p2a -j32
ctest --test-dir build-p2a --output-on-failure
```

Result: **189/189 tests passed**. New tests cover dd, qd, GMP MPF, binary80, and binary128. The dd sentinel retains a component around `2^-100`; qd, binary80, binary128, and GMP MPF retain information beyond binary64. Existing fresh-process tests cover the accepted 512-bit defaults.

The gate is `docs/mplapack_migration/gate-P2A.sh`; it checks imported hashes, the immutable forensic snapshot hash, native routes, forbidden APIs, an installed-header syntax consumer, and the full suite.

No MPFC, complex interop, reverse conversion, mixed arithmetic, precision or rounding sweeps, EDD, or TD work was added. The implementation was independently written in the clean clone; read-only prototype inspection only informed component and native-import choices. No dirty file was copied wholesale.

Deviations/blockers: none. The live dirty worktree is informational only after the frozen snapshot and was not used subsequently.
