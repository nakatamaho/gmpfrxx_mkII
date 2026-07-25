#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/../.." && pwd); cd "$root"
for f in LOCK.json interop_requirements.tsv SPIKE.md; do test -f docs/mplapack_migration/requirements/$f; done
! rg -n 'cast2|external_mpc|mpfc|complex|edd|td' include/gmpfrxx_mkII/adapters tests/test_*adapter* tests/test_mpf_binary_real_adapters.cpp
ctest --test-dir build-p2a --output-on-failure
