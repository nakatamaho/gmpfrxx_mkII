# P2D Release-Target Gate Self-Modification Audit

## Scope

This audit covers the failed command:

```bash
P2D_JOBS=32 bash docs/mplapack_migration/gate-P2D-release-target.sh
```

The approved release source remains
`429fd1b35e1927ebaccc9fda5aa2801300b45bf5`. The authoritative
`gmpfrxx_mkII.1.1.0.tar.xz` archive remains 15,169,540 bytes with SHA-256
`e0f3b813463b7a45dd493a818c60a17530075e0e647ea02227b75501c1984c73`.

## Exact Self-Modification

Before this repair, the outer release-target gate resolved the repository root,
selected the tracked path
`docs/mplapack_migration/REPORT-P2D-release-target-gate.log`, and ran:

```bash
P2D_RELEASE_TARGET_GATE_INNER=1 bash "$0" "$@" 2>&1 | tee "$log"
```

The call chain was:

1. The requested outer `gate-P2D-release-target.sh` invocation.
2. The outer wrapper's `tee` command, which opened the tracked log with
   truncation and replaced it with current runtime output.
3. The inner release-target checks.
4. `gate-P2D.sh` with `P2D_GATE_INNER=1`, so its own tracked log was not
   rewritten.
5. P2A in a detached worktree, P2B in inner mode with an external build
   directory, and P2C in a detached worktree.
6. The release-target gate's final `git diff --check`.

The only primary-worktree path changed by the failed command was the tracked
release-target log. No untracked repository path was created. Build trees,
consumer trees, reproduced archives, and nested worktrees were under `/tmp`.

The compiler emitted warnings from parallel jobs. Their diagnostic source
excerpts contained trailing horizontal whitespace, and concurrent output also
interleaved some warning fragments. `tee` preserved those bytes in the tracked
replacement log. The final `git diff --check` then reported the added
whitespace and returned failure.

This was an evidence-generation failure. The numerical tests, archive
reproduction, clean-room build and install, sanitizer checks, packaging checks,
API checks, and nested phase gates had passed. After the failed run,
`git restore --source=HEAD --worktree --
docs/mplapack_migration/REPORT-P2D-release-target-gate.log` restored the
approved bytes, and the worktree was clean.

## Other Tracked Gate Logs

The outer P2B, P2C, and P2D gates also contain historical `tee` wrappers:

| Gate | Tracked outer log | Write mode |
|---|---|---|
| P2B | `REPORT-P2B-gate.log` | truncate and replace |
| P2C | `REPORT-P2C-gate.log` | truncate and replace |
| P2D | `REPORT-P2D-gate.log` | truncate and replace |
| P2D release target | `REPORT-P2D-release-target-gate.log` | truncate and replace |

The exact historical write commands were:

```bash
P2B_GATE_INNER=1 bash "$0" "$@" 2>&1 | tee "$log"
P2C_GATE_INNER=1 bash "$0" "$@" 2>&1 | tee "$log"
P2D_GATE_INNER=1 bash "$0" "$@" 2>&1 | tee "$log"
P2D_RELEASE_TARGET_GATE_INNER=1 bash "$0" "$@" 2>&1 | tee "$log"
```

Each `tee` target was the tracked log shown in the corresponding table row.

The release-target gate already invoked P2D and P2B in inner mode. P2C's outer
wrapper ran only in a disposable detached worktree. Therefore those historical
wrappers did not modify the primary worktree during the failed release-target
run.

The committed historical logs pass `git diff --check HEAD -- <paths>`.
Their accepted SHA-256 values are:

```text
10ba70c0488a644acc452a28e097f234a39e299514c1808ae954ee049552ab8f  REPORT-P2B-gate.log
07aee31da0e810da9b880ea7a694265ffab56e7d5bf22aa5c0126f9fc45b046c  REPORT-P2C-gate.log
11c49904cf7dc957be6b8adc99e9790c28f048d1124a0c9d864474e73ff8c3d1  REPORT-P2D-gate.log
b98488a689ad8a0fb798315b38c1446cab906c055c07a4bbd251066ad0f2b4ae  REPORT-P2D-release-target-gate.log
```

No committed historical log required normalization. The whitespace defect
existed only in the failed runtime replacement that was discarded.

## Repair

The repaired release-target gate:

- removes the tracked-log wrapper and writes no runtime output in the
  repository;
- allocates a unique runtime directory with `mktemp -d`;
- removes that directory through an `EXIT`, `INT`, and `TERM` cleanup trap;
- runs the complete nested P2D gate in a disposable detached worktree;
- keeps nested build, install, consumer, and archive-reproduction paths under
  the unique runtime directory;
- verifies accepted historical log hashes and PASS markers without rewriting
  them;
- hashes every tracked migration gate and report before and after execution;
- requires an initially clean worktree;
- requires unchanged HEAD, index tree, HEAD tree, tracked evidence manifest,
  and final worktree status;
- classifies commits after the recorded P2D evidence commit through their
  changed paths, avoiding a self-referential evidence-commit SHA while still
  requiring every changed path to be evidence-only;
- retains the final `git diff --check`;
- verifies that the future local and remote tag, remote evidence branch, and
  GitHub Release are absent.

No acceptance check was removed or weakened. No production source, public
header, version field, package metadata, release target, or release artifact
was changed.
