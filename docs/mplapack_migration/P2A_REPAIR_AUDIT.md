# P2A Reproducibility Repair Audit

## Scope

This audit covers the P2A-artifact changes in approved P2C commit
`6c7bce42494e2d6196ade9b90b89223436b2fe43`, relative to approved P2B commit
`e4e06547bdbb0080714696937810a45726dd1780`.

## Changed P2A artifacts

### `docs/mplapack_migration/REPORT-P2A.md`

- Original defect: the recorded SHA-256 of the preserved forensic snapshot's
  `SHA256SUMS` file was truncated to 50 hexadecimal characters.
- Correction: the value was replaced with the complete digest
  `6a9fc4d4ab1fbe4ad71a1c5f40657ff6efcb24519edcf09341729b5d4e2c8ad3`.
- Executable gate behavior: unchanged by this report-only correction.
- Acceptance criteria: unchanged.

### `docs/mplapack_migration/gate-P2A.sh`

- Original defect: the expected snapshot digest was truncated to 58
  hexadecimal characters and could never equal a SHA-256 digest.
- Correction: the expected value was replaced with the complete digest above.
- Original defect: the binary80 and binary128 native-import checks searched
  leaf adapter headers although the accepted implementation centralizes
  `mpfr_set_ld` and `mpfr_set_float128` in
  `include/gmpfrxx_mkII/adapters/detail/binary_float.hpp`.
- Correction: only the two search paths were changed to the accepted common
  implementation header.
- Reproducibility correction: `P2A_ROOT` may select a detached worktree at the
  approved P2A SHA while preserving the repository root as the default.
- Executable gate behavior: corrected to inspect the intended accepted files
  and to accept the actual preserved snapshot digest.
- Acceptance criteria: unchanged. No required check was deleted, skipped, or
  weakened.

## Integrity and semantic conclusion

- The forensic snapshot content was not modified.
- `sha256sum --check SHA256SUMS` passed for all 16 files in
  `/home/docker/gmpfrxx_mkII-prototype-snapshot-p2a-retry.F58Geq`.
- The `SHA256SUMS` file itself has SHA-256
  `6a9fc4d4ab1fbe4ad71a1c5f40657ff6efcb24519edcf09341729b5d4e2c8ad3`.
- No immutable requirement hash changed.
- No P2A implementation header or numerical test changed in the repair.
- No required test was removed and no numerical threshold was loosened.
- No failure was converted to a pass by weakening a condition.
- The repaired gate passed independently against approved P2A commit
  `0bd061c0b62d87256a1d0c18a9aea453524c79c5`.
- The accepted P2A semantic result remains 189/189 tests passed.

Conclusion: the P2A changes in P2C are reproducibility-only corrections and
do not weaken P2A acceptance.
