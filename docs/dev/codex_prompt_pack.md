# gmpfrxx_mkII Codex Prompt Pack v3

This pack contains repository-local guidance and staged implementation prompts for `gmpfrxx_mkII`.

Place these files at the root of the target repository:

```text
gmpfrxx_mkII/
├─ AGENTS.md
├─ STATUS.md
├─ PROMPTS.md
├─ README.md
├─ CMakeLists.txt
├─ include/
├─ tests/
├─ cmake/
├─ docs/
└─ reference/
```

If this library is developed inside a larger monorepo, place this pack at the root of the `gmpfrxx_mkII` subproject and start Codex CLI from that subproject directory.

Recommended workflow:

```sh
cd /path/to/gmpfrxx_mkII
codex
```

Then paste the relevant phase from `PROMPTS.md`.

## Files

- `AGENTS.md`: persistent repository-local rules for Codex.
- `PROMPTS.md`: staged implementation prompts.
- `STATUS.md`: current implementation status and test log.
- `docs/design.md`: type, dependency, and header-boundary design.
- `docs/precision_policy.md`: precision and rounding rules.
- `docs/compatibility.md`: compatibility and intentional incompatibility notes.
- `reference/upstream/*/curated/`: curated notes extracted from upstream GMP, MPFR, and MPC manuals.

## Main policies in this pack

- strict GMP-only boundary for `gmpxx_mkII.h`
- MPFR/MPC boundary for `mpfrxx_mkII.h`
- `mpfr_class` is real-valued and wraps `mpfr_t`
- `mpc_class` is the MPFR/MPC complex type and wraps `mpc_t`
- `mpfc_class` is GMP-only complex and must not use MPC
- portable LP64/LLP64 integer conversion policy
- explicit MPFR and MPC rounding policy
- expression-template precision propagation policy
- initial 512-bit default precision for GMP `mpf_class` and MPFR/MPC defaults
- fixed-precision fast path controlled by `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`
