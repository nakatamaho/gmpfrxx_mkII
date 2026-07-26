# MPLAPACK migration report: upstream P2D release hardening

## 1. Repository and candidate identity

- Repository: `/home/docker/gmpfrxx_mkII-p2a-clean.wbx83b`
- Remote: `origin = git@github.com:nakatamaho/gmpfrxx_mkII.git`
- Branch: `topic/mplapack-release-hardening`
- Approved P2C base: `6c7bce42494e2d6196ade9b90b89223436b2fe43`
- P2D release-source commit:
  `429fd1b35e1927ebaccc9fda5aa2801300b45bf5`
- Final evidence commit: the commit containing this report and the complete
  gate log; its exact SHA is recorded by `git rev-parse HEAD` in the phase
  acceptance response because a Git commit cannot contain its own SHA.
- Initial merge base: the exact approved P2C SHA.
- Initial ahead/behind relative to P2C: `0 0`.

## 2. Accepted P2C publication

The accepted P2C tree was clean at
`6c7bce42494e2d6196ade9b90b89223436b2fe43`. The command

```sh
P2C_JOBS=32 bash docs/mplapack_migration/gate-P2C.sh
```

passed before publication. The branch was pushed without force and
`origin/topic/mplapack-wrapper-compat` was verified at the exact approved
SHA.

## 3. P2A repair audit

`P2A_REPAIR_AUDIT.md` records the complete audit. The P2C repair:

- corrected a shortened snapshot digest to the actual immutable snapshot
  digest
  `6a9fc4d4ab1fbe4ad71a1c5f40657ff6efcb24519edcf09341729b5d4e2c8ad3`;
- corrected binary80 and binary128 implementation search paths to the
  accepted `adapters/detail/binary_float.hpp`;
- made the accepted P2A source root reproducibly selectable;
- did not modify the forensic snapshot, implementation, production headers,
  tests, thresholds, REQUIRED rows, or acceptance criteria;
- did not remove a test or convert a failure to a pass by weakening a
  condition.

All 16 repair-audit assertions passed. The repaired P2A gate independently
retained the accepted semantic result: 189/189 tests passed.

## 4. Release version

- Previous release: `1.0.1`
- Release-candidate version: `1.1.0`
- Version policy: no repository-controlled policy required another successor.
  The accepted changes add backward-compatible, opt-in public comparison
  adapters, so the next minor version was selected.

## 5. Version-bearing files

The authoritative version was changed consistently in:

- `CMakeLists.txt`
- `include/gmpfrxx_mkII/detail/config.hpp`
- `tests/test_version_info.cpp`
- `manual/gmpfrxx_mkII_manual.tex`
- `docs/mplapack_migration/RELEASE_VERSION.txt`

Release documentation was added or updated in:

- `CHANGES.1.1.0.md`
- `README.md`
- `SPECIFICATIONS.md`
- `docs/mplapack_comparison_adapters.md`

The CMake package metadata, generated version header, runtime version API,
manual release macro, archive name, and exact-version consumer all report
`1.1.0`.

## 6. Immutable inputs

All imported requirements remained byte-for-byte unchanged:

| File | SHA-256 |
| --- | --- |
| `LOCK.json` | `9fb757df92a36e996e0ea28c9dcf9351cf5063ff95646d750a188e3f7f32b075` |
| `SPIKE.md` | `29b4bf3c831c0a892773139be3e8283153bfb4db4d74726b0893a807cae0928d` |
| `interop_requirements.tsv` | `2bb9700d76cd50a6c4328f497b35e3794065fecc915eac40408f01c0188db00f` |
| `INTEROP_POLICY.md` | `859fe8ebc18100c1225d04633ceaa38140dd5ac48f6203538950bfc40886a9ed` |
| `PROVENANCE.md` | `d268d97b08c58d3dd5f4c2132b756cd22b5668313046496f62d12f1daecdacef` |

## 7. API audit

`P2D_API_AUDIT.tsv` contains 35 unique requirements:

- 24 `PASS`;
- 11 `NOT_APPLICABLE`, all MPLAPACK-owned P3/P4 utilities;
- 0 `FAIL`.

Every P2A real REQUIRED row and every P2B complex REQUIRED row appears
exactly once. Installed and source-tree evidence is recorded for all 24
upstream requirements. The audit covers include layering, owning
materialization, namespace boundaries, installed exports, and defaults.

## 8. Supported contract

The release supports explicit, one-way owning materialization:

- `mpfrxx::mpfr_class` from `double`, dd, qd, binary80, binary128, and
  `gmpxx::mpf_class`;
- `mpfrxx::mpc_class` from double complex, dd complex, qd complex,
  binary80 complex, binary128 complex, and `gmpxx::mpfc_class`.

dd and qd are reconstructed directly from stored components. binary80 uses
`mpfr_set_ld`; binary128 uses `mpfr_set_float128`; GMP values use direct
`mpfr_set_f`. Complex components are imported independently. No extended
source passes through binary64. Ordinary MPFR and GMP real defaults are
exactly 512 bits.

The release does not support reverse conversion, general mixed-backend
arithmetic, adapter arithmetic in either operand order, adapter compound
assignment, precision metadata preservation or negotiation, arbitrary
correct-rounding claims, exact round trips, edd/td MPLAPACK adapters, or
MPLAPACK-specific public utilities.

## 9. Files changed

Release contract and adapter hardening:

- `include/gmpfrxx_mkII/adapters/detail/binary_float.hpp`
- `include/gmpfrxx_mkII/adapters/detail/real_components.hpp`
- `include/gmpfrxx_mkII/adapters/binary80_complex.hpp`
- `include/gmpfrxx_mkII/adapters/binary128_complex.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`

The hardening removed unused reverse binary helpers, removed destination
precision inspection from the accepted forward routes, and retained only
ordinary-default explicit construction and assignment. It also corrected
Clang feature guards for binary80 and binary128 complex adapters.

Version, package portability, and distribution:

- `.gitattributes`
- `CMakeLists.txt`
- `include/gmpfrxx_mkII/detail/config.hpp`
- `tests/test_version_info.cpp`
- `cmake/toolchains/mingw64-wine.cmake`
- `scripts/test_mingw64_wine.sh`

The MinGW helper defaults are now build-local instead of containing a
machine-specific `/home/docker/mplapack/external/i` path.

Tests and gates:

- `docs/mplapack_migration/gate-P2D.sh`
- `docs/mplapack_migration/tools/p2d_consumer/CMakeLists.txt`
- `docs/mplapack_migration/tools/p2d_consumer/main.cpp`
- `docs/mplapack_migration/tools/scan_expression_lifetimes.py`

Documentation and evidence:

- `CHANGES.1.1.0.md`
- `README.md`
- `SPECIFICATIONS.md`
- `manual/gmpfrxx_mkII_manual.tex`
- `docs/mplapack_comparison_adapters.md`
- `docs/mplapack_migration/P2A_REPAIR_AUDIT.md`
- `docs/mplapack_migration/P2D_API_AUDIT.tsv`
- `docs/mplapack_migration/P2D_RELEASE_ARTIFACTS.txt`
- `docs/mplapack_migration/RELEASE_VERSION.txt`
- `docs/mplapack_migration/REPORT-P2D-gate.log`
- `docs/mplapack_migration/REPORT-P2D.md`

## 10. Build and test environment

- OS: Ubuntu Linux x86_64, kernel `6.8.0-124-generic`
- GCC: `g++ 15.2.0`
- Clang: `clang++ 21.1.8`
- CMake: `4.2.3`
- Build tool: GNU Make
- GMP: `6.3.0`
- MPFR: `4.2.2`
- MPC: `1.3.1`
- QD: staged MPLAPACK prefix under `/home/docker/MPLAPACK`; no system
  pkg-config metadata was installed
- Jobs: 32
- Dependency auto-fetch: OFF in every acceptance configuration
- Correctness options: FMA OFF, fast fixed precision OFF, fast stable
  rounding OFF

## 11. GCC Debug

A clean GCC Debug configure/build completed. 191/191 applicable tests passed.
The release-only optimized disassembly equivalence test is intentionally
excluded in Debug because it verifies optimized machine code. Compile-fail
tests, install, and the installed-tree P2D consumer passed.

## 12. GCC Release

A clean GCC Release configure/build completed. 192/192 tests passed,
including optimized disassembly equivalence. Compile-fail tests, install,
exact package-version discovery, and the installed-tree P2D consumer passed.

## 13. Clang and sanitizers

Clang Debug with ASan and UBSan configured, built, and ran 191/191 applicable
tests without sanitizer findings. The sanitizer install and installed-tree
consumer also compiled, linked, and ran. The optimized GCC disassembly test is
not applicable to this Clang Debug cell.

## 14. Feature configurations

- GMP-only: 63/63 tests passed.
- GMP+MPFR: 126/126 tests passed.
- Full GMP+MPFR+MPC: covered by GCC Debug, GCC Release, and Clang sanitizer.
- QD, dd, binary80, and binary128 adapter executables compiled and ran in the
  full configurations.
- Native binary80 and binary128 routes were available; no binary64 fallback
  configuration was used.

## 15. Accepted phase gates

From the P2D candidate tree:

- P2A: PASS, accepted 189/189 semantic result retained.
- P2B: PASS, current complete 192/192 suite and installed consumer passed.
- P2C: PASS, current 192/192 suite, GMP high-precision regression,
  compile-fail subset, nested P2A/P2B checks, and installed consumer passed.

## 16. Directed numerical and integrity tests

The directed 16-test subset passed. It covers:

- dd, qd, binary80, binary128, and GMP MPF real anti-binary64 sentinels;
- the corresponding complex real- and imaginary-component sentinels;
- component order/copy detection and a negative-component sign check;
- raw `long double` and raw `__float128` rejection;
- forbidden cross-family arithmetic compile failures;
- expression lifetime;
- the single fresh-process MPFR/GMP default test.

The GMP same-family regression retained a residual between `2^-200` and
`2^-198`; the deliberately emulated `mpf_get_d` reference residual was zero.
This demonstrates the exercised path does not plateau at binary64 without
claiming correctly rounded transcendental results.

## 17. Default precision

`test_mplapack_default_512` and the installed consumer both reported exactly
512 bits for fresh default-constructed `mpfrxx::mpfr_class` and
`gmpxx::mpf_class`. No precision sweep, rounding sweep, mutation test,
assignment-time precision check, or second default state was added.

## 18. Installed-tree consumer matrix

The P2D consumer uses:

```cmake
find_package(gmpfrxx_mkII 1.1.0 EXACT CONFIG REQUIRED)
target_link_libraries(p2d_consumer PRIVATE gmpfrxx_mkII::gmpfrxx_mkII)
```

It compiled, linked, and ran from GCC Debug, GCC Release, Clang sanitizer,
a copied relocation prefix, and the clean-room archive install. It covers
all six real source families, all six complex source families, same-family
MPFR/MPC and MPF/MPFC owning materialization, both 512-bit defaults, and the
GMP below-binary64 operation. Verbose compiler output proved that
gmpfrxx_mkII headers came from the tested install prefixes. The installed
provider has no `libgmpxx` dynamic dependency.

## 19. Release artifact

- Filename: `gmpfrxx_mkII.1.1.0.tar.xz`
- Stable path:
  `/home/docker/gmpfrxx_mkII-p2d-artifacts/gmpfrxx_mkII.1.1.0.tar.xz`
- Size: 15,169,540 bytes
- SHA-256:
  `e0f3b813463b7a45dd493a818c60a17530075e0e647ea02227b75501c1984c73`
- Entries: 806
- Source SHA: `429fd1b35e1927ebaccc9fda5aa2801300b45bf5`
- Embedded version: `1.1.0`
- Generation:
  `cmake --build /tmp/gmpfrxx-p2d-dist-portable-one --target dist -j32`

The tarball is an external release-candidate artifact and is not committed.
No tag or release was created.

## 20. Clean-room archive verification

The archive was unpacked into an empty directory. It contained no `.git`
directory or untracked development artifacts. With dependency auto-fetch
OFF it configured, built, passed 192/192 tests, installed to a fresh prefix,
and passed the complete installed P2D consumer.

## 21. Relocation and absolute-path audit

A copied installation prefix was discovered with
`find_package(gmpfrxx_mkII 1.1.0 EXACT CONFIG REQUIRED)` and its consumer
passed. Installed CMake files contain no source/build-tree absolute paths.
The archive contains no source-tree, build-tree, temporary, dirty-prototype,
forensic-snapshot, `/home/docker`, cache, editor, or generated test-output
artifacts.

## 22. Reproducibility

Two clean distribution builds from source commit
`429fd1b35e1927ebaccc9fda5aa2801300b45bf5` produced byte-for-byte identical
archives. Both SHA-256 values are
`e0f3b813463b7a45dd493a818c60a17530075e0e647ea02227b75501c1984c73`.

## 23. P2D gate

The single acceptance command was:

```sh
P2D_JOBS=32 bash docs/mplapack_migration/gate-P2D.sh
```

Result: PASS. The complete output is committed as
`docs/mplapack_migration/REPORT-P2D-gate.log` with SHA-256
`11c49904cf7dc957be6b8adc99e9790c28f048d1124a0c9d864474e73ff8c3d1`.

## 24. Explicit non-goals

No reverse conversion, mixed adapter arithmetic, precision metadata
machinery, assignment-time precision checking, MPLAPACK-specific public API,
edd support, or td support was added. No tag, GitHub Release, or P1R work was
started.

## 25. Deviations and unavailable cells

Resolved release-hardening findings:

- accepted adapter internals still contained unused reverse binary helpers
  and destination-precision inspection; P2D removed these forbidden remnants
  without changing REQUIRED forward API coverage;
- Clang exposed missing binary80/binary128 complex feature guards; the guards
  now use the accepted adapter capability macros;
- gate scans initially failed open under shell negation; they now fail closed;
- the original archive hygiene expression falsely matched
  `build_config.hpp.in`; it was narrowed to build-directory names;
- two MinGW helper defaults contained a machine-specific MPLAPACK path; they
  were made build-local before archive creation.

Unavailable environment cells:

- a system `libqd-dev` pkg-config package was not installed; adapter tests
  used the established staged QD installation;
- Ninja was not installed, so all matrix cells used GNU Make;
- MinGW/Wine was not rerun because it is not a required P2D matrix cell; its
  helper was audited for archive portability.

There are no remaining blockers.

## 26. Worktree status

At gate completion the only untracked paths were this phase's artifact record
and complete gate log. They are committed together with this report. The
expected final `git status --short` is empty.

## 27. Publication status

The accepted P2C branch was pushed. The P2D candidate is local only. No P2D
push, tag, GitHub Release, publication, MPLAPACK repository modification, or
P1R work was performed.
