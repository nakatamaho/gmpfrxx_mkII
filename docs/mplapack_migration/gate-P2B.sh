#!/usr/bin/env bash
set -euo pipefail

root=$(cd "$(dirname "$0")/../.." && pwd)
log="$root/docs/mplapack_migration/REPORT-P2B-gate.log"

if test "${P2B_GATE_INNER:-0}" != 1; then
    set +e
    P2B_GATE_INNER=1 bash "$0" "$@" 2>&1 | tee "$log"
    exit "${PIPESTATUS[0]}"
fi

cd "$root"
base=0bd061c0b62d87256a1d0c18a9aea453524c79c5
requirements=docs/mplapack_migration/requirements
build_dir=${P2B_BUILD_DIR:-build-p2b}
jobs=${P2B_JOBS:-32}

test "$(git merge-base HEAD "$base")" = "$base"
test "$(sha256sum "$requirements/LOCK.json" | cut -d' ' -f1)" = \
    9fb757df92a36e996e0ea28c9dcf9351cf5063ff95646d750a188e3f7f32b075
test "$(sha256sum "$requirements/interop_requirements.tsv" | cut -d' ' -f1)" = \
    2bb9700d76cd50a6c4328f497b35e3794065fecc915eac40408f01c0188db00f
test "$(sha256sum "$requirements/SPIKE.md" | cut -d' ' -f1)" = \
    29b4bf3c831c0a892773139be3e8283153bfb4db4d74726b0893a807cae0928d
test "$(sha256sum "$requirements/INTEROP_POLICY.md" | cut -d' ' -f1)" = \
    859fe8ebc18100c1225d04633ceaa38140dd5ac48f6203538950bfc40886a9ed

for id in COMPLEX-DOUBLE COMPLEX-DD COMPLEX-QD COMPLEX-BINARY80 \
          COMPLEX-BINARY128 COMPLEX-GMP; do
    rg -q "^${id}" "$requirements/interop_requirements.tsv"
    rg -q "$id" tests/test_complex_compare_adapters.cpp
done

for file in dd_real qd_real binary80_real binary128_real mpf_real; do
    test -f "include/gmpfrxx_mkII/adapters/${file}.hpp"
done
for test_name in test_dd_real_adapter test_qd_real_adapter \
                 test_mpf_binary_real_adapters; do
    rg -q "$test_name" tests/CMakeLists.txt
done

test "$(rg -c 'require_below_binary64_components\(target' \
    tests/test_complex_compare_adapters.cpp)" = 5
rg -q 'source.imag\(dd_real\(-2\.0' tests/test_complex_compare_adapters.cpp
rg -q 'source.real\(qd_real\(3\.0' tests/test_complex_compare_adapters.cpp
rg -q 'source.imag\(qd_real\(-4\.0' tests/test_complex_compare_adapters.cpp

test -z "$(git diff --unified=0 "$base" -- \
    include/gmpfrxx_mkII/adapters \
    include/gmpfrxx_mkII/detail/mpc_impl.hpp |
    rg '^\+.*(cast2|operator[+*/-]|get_d\(|mpfr_get_d\(|\bedd\b|\btd_complex\b)' || true)"
test -z "$(git diff --unified=0 "$base" -- \
    include/gmpfrxx_mkII/adapters/*complex.hpp \
    include/gmpfrxx_mkII/adapters/detail/complex_components.hpp |
    rg '^\+.*(precision\(|mpfr_get_prec|mpc_get_prec)' || true)"
test -z "$(rg -n '#include <mplapack/' \
    include/gmpfrxx_mkII/adapters include/gmpfrxx_mkII/detail/mpc_impl.hpp || true)"

git diff --check
cmake -S . -B "$build_dir" \
    -DBUILD_TESTING=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS=-I/home/docker/MPLAPACK/include \
    -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF \
    -DGMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC
rg -q '^GMPFRXX_MKII_DEPS_AUTO_FETCH:BOOL=OFF$' "$build_dir/CMakeCache.txt"
cmake --build "$build_dir" -j"$jobs"
ctest --test-dir "$build_dir" --output-on-failure

install_prefix=$(mktemp -d /tmp/gmpfrxx-mkII-p2b-gate-install.XXXXXX)
consumer_build=$(mktemp -d /tmp/gmpfrxx-mkII-p2b-consumer-build.XXXXXX)
cmake --install "$build_dir" --prefix "$install_prefix"
test -f "$install_prefix/include/gmpfrxx_mkII/adapters/mpfc_complex.hpp"
test -f "$install_prefix/include/gmpfrxx_mkII/adapters/dd_complex.hpp"
cmake -S docs/mplapack_migration/tools/p2b_consumer \
    -B "$consumer_build" \
    -DCMAKE_PREFIX_PATH="$install_prefix"
cmake --build "$consumer_build" -j"$jobs"
"$consumer_build/p2b_consumer"

echo "P2B gate: PASS"
