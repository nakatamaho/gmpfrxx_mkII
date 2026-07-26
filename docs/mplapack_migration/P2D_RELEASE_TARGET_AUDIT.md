# P2D release-target audit for gmpfrxx_mkII 1.1.0

## 1. Repository identity

- Repository: `/home/docker/gmpfrxx_mkII-p2a-clean.wbx83b`
- Remote: `git@github.com:nakatamaho/gmpfrxx_mkII.git`
- Branch: `topic/mplapack-release-hardening`
- Starting P2D tip: `9203f6636ae17052966879811f2a4f5fde4685e7`
- Approved P2C base and merge base:
  `6c7bce42494e2d6196ade9b90b89223436b2fe43`
- Starting ahead/behind relative to P2C: `0 5`
- Starting worktree: clean

The history is a straight, unrevised chain from the approved P2C commit.
The recorded archive source exists and is an ancestor of the starting tip.

## 2. Repository tag convention

Existing release tags are `v1.0.0-rc1`, `v1.0.0`, and `v1.0.1`. All three
are annotated tag objects. Release records and the manual use the same
`v<version>` form. The required future tag name is therefore `v1.1.0`.

No `v1.1.0` tag was created locally or remotely.

## 3. Complete P2D commit audit

### 94f49e61c359d65508e4c126049e70e4ed7bb681

Subject: `release: harden gmpfrxx_mkII 1.1.0 for MPLAPACK`

Purpose and changed files:

- release/public API: `.gitattributes`, `CMakeLists.txt`,
  `include/gmpfrxx_mkII/adapters/detail/binary_float.hpp`,
  `include/gmpfrxx_mkII/adapters/detail/real_components.hpp`,
  `include/gmpfrxx_mkII/detail/config.hpp`, and
  `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`;
- version and tests: `tests/test_version_info.cpp`;
- shipped release documentation: `CHANGES.1.1.0.md`, `README.md`,
  `SPECIFICATIONS.md`, `docs/mplapack_comparison_adapters.md`, and
  `manual/gmpfrxx_mkII_manual.tex`;
- release tests/evidence: `P2A_REPAIR_AUDIT.md`, `P2D_API_AUDIT.tsv`,
  `RELEASE_VERSION.txt`, `gate-P2D.sh`, the P2D consumer, and the expression
  lifetime scanner under `docs/mplapack_migration/`.

This commit is release-affecting and precedes the release target.

### 518934363d9b0f3fbab36f9c99c2fddb7d9ebf7c

Subject: `fix: harden P2D compiler and gate coverage`

Changed `binary80_complex.hpp`, `binary128_complex.hpp`, and `gate-P2D.sh`.
It fixes public-header feature guards for Clang and expands release
verification. It is release-affecting and precedes the release target.

### 6ea737d47c064a4231945fd2f18b95b297bfb046

Subject: `test: make P2D forbidden scans fail closed`

Changed only `gate-P2D.sh`. It corrects release-test behavior so scan errors
cannot be mistaken for clean results. It is required release verification
and precedes the release target.

### 429fd1b35e1927ebaccc9fda5aa2801300b45bf5

Subject: `fix: make P2D release checks portable`

Changed:

- `cmake/toolchains/mingw64-wine.cmake`;
- `scripts/test_mingw64_wine.sh`;
- `docs/mplapack_migration/gate-P2D.sh`.

The first two remove machine-specific dependency paths from package tooling.
The gate correction distinguishes generated `build_config.hpp.in` from build
directories. This commit is release-affecting and is the earliest complete
release target.

### 9203f6636ae17052966879811f2a4f5fde4685e7

Subject: `docs: record P2D release-hardening evidence`

Changed only:

- `docs/mplapack_migration/P2D_RELEASE_ARTIFACTS.txt`;
- `docs/mplapack_migration/REPORT-P2D-gate.log`;
- `docs/mplapack_migration/REPORT-P2D.md`.

This commit follows the release target and is evidence-only.

## 4. Complete post-release-source path classification

The initial binary/full-index delta is preserved at
`/tmp/gmpfrxx_mkII-p2d-post-release-source.diff`.

| Path | Classification | Justification |
| --- | --- | --- |
| `docs/mplapack_migration/P2D_RELEASE_ARTIFACTS.txt` | `MIGRATION_EVIDENCE_ONLY` | Records artifact provenance; not installed or archived. |
| `docs/mplapack_migration/REPORT-P2D-gate.log` | `GENERATED_EVIDENCE_ONLY` | Generated command output; not installed or archived. |
| `docs/mplapack_migration/REPORT-P2D.md` | `MIGRATION_EVIDENCE_ONLY` | Migration report; not installed or archived. |

The release-source commit's `.gitattributes` contains
`docs/mplapack_migration export-ignore`; current attributes are identical.
Consequently none of these paths changes release code, public headers,
version metadata, build/install/package metadata, shipped documentation,
tests in the source archive, or archive contents.

Finalization adds only the evidence paths listed in
`P2D_RELEASE_TARGET_CLASSIFICATION.tsv`. `STATUS.md` is excluded by the
repository-wide `*.md export-ignore` rule. Every other finalization path is
under the migration directory and therefore excluded. There are no
`UNCLASSIFIED` paths.

## 5. Binding decision

Decision: **Case A - existing archive source is valid**.

Every change after `429fd1...` is either `MIGRATION_EVIDENCE_ONLY` or
`GENERATED_EVIDENCE_ONLY`. No post-target change is required in released
source, installed packages, public APIs, version metadata, release notes,
release tests, packaging, or clean-room archive behavior.

The branch tip remains the complete migration evidence chain. It is not the
future release tag target.

## 6. Selected release target

- Version: `1.1.0`
- Future annotated tag name: `v1.1.0`
- Future tag target:
  `429fd1b35e1927ebaccc9fda5aa2801300b45bf5`
- Archive source:
  `429fd1b35e1927ebaccc9fda5aa2801300b45bf5`

The tag target and archive source are byte-for-byte identical 40-character
commit identifiers. The target is an ancestor of the P2D evidence branch.

## 7. Archive reproduction

Two independent local clones were created from the clean repository and
detached at the exact release target:

- `/tmp/gmpfrxx-p2d-release-target-one.bVPSzA`
- `/tmp/gmpfrxx-p2d-release-target-two.fAPjYC`

Each clone was clean, configured with dependency auto-fetch OFF, and ran the
established CMake `dist` target.

Both generations and the recorded stable artifact are identical:

- filename: `gmpfrxx_mkII.1.1.0.tar.xz`;
- size: 15,169,540 bytes;
- SHA-256:
  `e0f3b813463b7a45dd493a818c60a17530075e0e647ea02227b75501c1984c73`;
- byte-for-byte comparison: PASS;
- embedded version: `1.1.0`.

The previous archive remains authoritative and is not superseded.

## 8. Release-target verification

The release-target gate performs, from the reproduced archive:

1. empty-directory extraction;
2. Release configure with dependency auto-fetch OFF;
3. complete build and applicable 192-test suite;
4. clean-prefix install;
5. installed-tree consumer covering every real and complex source family,
   same-family wrappers, both 512-bit defaults, and GMP high precision;
6. copied-prefix relocation and a second installed consumer;
7. exact CMake package version and exported-target checks;
8. directed comparison sentinels, component integrity, GMP high-precision,
   default, expression-lifetime, and compile-fail checks;
9. production scans for binary64 fallback, reverse conversion, mixed adapter
   arithmetic, edd, td, and MPLAPACK-owned public APIs;
10. accepted P2A, P2B, P2C, and P2D gates.

Result: PASS. The clean-room complete suite passed 192/192. Installed and
relocated consumers passed. Package relocation and forbidden-path scans
passed.

## 9. Version and package metadata

The release target reports `1.1.0` in CMake project metadata, generated
version headers, runtime version tests, and manual metadata. The archive
prefix and filename encode `1.1.0`.

Installed CMake metadata is relocatable and contains no source/build absolute
paths. The archive contains no `.git`, migration evidence, forensic
snapshots, `/home/docker` path, generated test output, editor file, compiler
cache, or unexpected binary.

## 10. Gate

Acceptance command:

```sh
P2D_JOBS=32 bash docs/mplapack_migration/gate-P2D-release-target.sh
```

Result: PASS. Complete output is recorded in
`REPORT-P2D-release-target-gate.log`:

- size: 3,715,044 bytes;
- SHA-256: `b98488a689ad8a0fb798315b38c1446cab906c055c07a4bbd251066ad0f2b4ae`.

## 11. Publication state

No push occurred during this audit. The P2D remote branch remained absent.
No `v1.1.0` tag exists locally or remotely. No tag, GitHub Release, source
publication, MPLAPACK change, or P1R work was created.

## 12. Deviations and blockers

Deviations: none. Blockers: none.
