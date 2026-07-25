#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/../.." && pwd)
cd "$root"
req=docs/mplapack_migration/requirements
test "$(sha256sum "$req/LOCK.json" | cut -d' ' -f1)" = "9fb757df92a36e996e0ea28c9dcf9351cf5063ff95646d750a188e3f7f32b075"
test "$(sha256sum "$req/interop_requirements.tsv" | cut -d' ' -f1)" = "2bb9700d76cd50a6c4328f497b35e3794065fecc915eac40408f01c0188db00f"
test "$(sha256sum "$req/SPIKE.md" | cut -d' ' -f1)" = "29b4bf3c831c0a892773139be3e8283153bfb4db4d74726b0893a807cae0928d"
test "$(sha256sum "$req/INTEROP_POLICY.md" | cut -d' ' -f1)" = "859fe8ebc18100c1225d04633ceaa38140dd5ac48f6203538950bfc40886a9ed"
snapshot=/home/docker/gmpfrxx_mkII-prototype-snapshot-p2a-retry.F58Geq
snapshot=/home/docker/gmpfrxx_mkII-prototype-snapshot-p2a-retry.F58Geq
test "$(sha256sum "$snapshot/SHA256SUMS" | cut -d' ' -f1)" = "6a9fc4d4ab1fbe4ad71a1c5f40657ff6efcb24519edcf093bf4e2c8ad3"
rg -q 'mpfr_set_ld' include/gmpfrxx_mkII/adapters/binary80_real.hpp
rg -q 'mpfr_set_float128' include/gmpfrxx_mkII/adapters/binary128_real.hpp
rg -q 'mpfr_set_f' include/gmpfrxx_mkII/adapters/mpf_real.hpp
! rg -n 'cast2|external_mpc|mpfc|complex|edd|td|get_d\(' include/gmpfrxx_mkII/adapters tests/test_*adapter* tests/test_mpf_binary_real_adapters.cpp
printf 'int main(){return 0;}\n' | c++ -x c++ -std=c++17 -Iinclude -fsyntax-only -
ctest --test-dir build-p2a --output-on-failure
git diff --check
