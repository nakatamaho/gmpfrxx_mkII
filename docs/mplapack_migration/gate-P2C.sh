#!/usr/bin/env bash
set -euo pipefail

root=$(cd "$(dirname "$0")/../.." && pwd)
log="$root/docs/mplapack_migration/REPORT-P2C-gate.log"

if test "${P2C_GATE_INNER:-0}" != 1; then
    set +e
    P2C_GATE_INNER=1 bash "$0" "$@" 2>&1 | tee "$log"
    exit "${PIPESTATUS[0]}"
fi

cd "$root"
base=e4e06547bdbb0080714696937810a45726dd1780
p2a=0bd061c0b62d87256a1d0c18a9aea453524c79c5
requirements=docs/mplapack_migration/requirements
matrix=docs/mplapack_migration/P2C_SPIKE_DISPOSITION.tsv
build_dir=${P2C_BUILD_DIR:-build-p2c}
jobs=${P2C_JOBS:-32}

test "$(git merge-base HEAD "$base")" = "$base"
test "$(git rev-list --left-right --count "$base"...HEAD | awk '{print $1}')" = 0
test "$(sha256sum "$requirements/LOCK.json" | cut -d' ' -f1)" = \
    9fb757df92a36e996e0ea28c9dcf9351cf5063ff95646d750a188e3f7f32b075
test "$(sha256sum "$requirements/interop_requirements.tsv" | cut -d' ' -f1)" = \
    2bb9700d76cd50a6c4328f497b35e3794065fecc915eac40408f01c0188db00f
test "$(sha256sum "$requirements/SPIKE.md" | cut -d' ' -f1)" = \
    29b4bf3c831c0a892773139be3e8283153bfb4db4d74726b0893a807cae0928d
test "$(sha256sum "$requirements/INTEROP_POLICY.md" | cut -d' ' -f1)" = \
    859fe8ebc18100c1225d04633ceaa38140dd5ac48f6203538950bfc40886a9ed

cmp -s "$requirements/LOCK.json" \
    <(git -C /home/docker/mplapack show \
      2ebf798fd0081ecdc5c1b53fc117431c406bf884:docs/migration/gmpfrxx_mkII/LOCK.json)
cmp -s "$requirements/interop_requirements.tsv" \
    <(git -C /home/docker/mplapack show \
      2ebf798fd0081ecdc5c1b53fc117431c406bf884:docs/migration/gmpfrxx_mkII/interop_requirements.tsv)
cmp -s "$requirements/SPIKE.md" \
    <(git -C /home/docker/mplapack show \
      2ebf798fd0081ecdc5c1b53fc117431c406bf884:docs/migration/gmpfrxx_mkII/SPIKE.md)

awk -F '\t' '
BEGIN {
    valid_category["UPSTREAM_GENERIC_OPERATOR"] = 1
    valid_category["UPSTREAM_GENERIC_CONSTRUCTOR"] = 1
    valid_category["UPSTREAM_GENERIC_MATH"] = 1
    valid_category["UPSTREAM_GENERIC_WRAPPER_API"] = 1
    valid_category["DOWNSTREAM_MPLAPACK_UTILITY"] = 1
    valid_category["ALREADY_SATISFIED"] = 1
    valid_category["NOT_REQUIRED"] = 1
    valid_category["BLOCKED"] = 1
    valid_disposition["IMPLEMENT_P2C"] = 1
    valid_disposition["VERIFY_ONLY"] = 1
    valid_disposition["DEFER_TO_P3"] = 1
    valid_disposition["DEFER_TO_P4"] = 1
    valid_disposition["OUT_OF_SCOPE"] = 1
    valid_disposition["STOP_FOR_MAINTAINER"] = 1
}
NR == 1 {
    expected = "spike_id\twrapper_family\trequired_signature_or_expression\tcategory\tcurrent_status\tdisposition\tevidence\timplementation_file\ttest_id"
    if ($0 != expected) exit 1
    next
}
{
    if (NF != 9 || seen[$1]++ || !valid_category[$4] || !valid_disposition[$6]) exit 1
    if ($6 == "IMPLEMENT_P2C" && ($8 == "" || $9 == "")) exit 1
    if ($6 == "VERIFY_ONLY" && $9 == "") exit 1
    if ($4 == "DOWNSTREAM_MPLAPACK_UTILITY" &&
        $6 != "DEFER_TO_P3" && $6 != "DEFER_TO_P4") exit 1
    rows++
}
END {
    if (rows != 50) exit 1
}
' "$matrix"

while IFS=$'\t' read -r id _; do
    test "$(awk -F '\t' -v id="$id" 'NR > 1 && $1 == id {n++} END {print n+0}' "$matrix")" = 1
done < <(tail -n +2 "$requirements/interop_requirements.tsv")

for id in DEFAULT-MPFR DEFAULT-GMP SAME-MPFR SAME-MPC SAME-MPF SAME-MPFC \
          UTILITY-MPFR-MATH UTILITY-MPFR-DECIMAL UTILITY-MPFR-HEX \
          MPLAPACK-MPFR-POW2 MPLAPACK-MPFR-SIGN MPLAPACK-MPFR-NINT \
          MPLAPACK-MPFR-PI MPLAPACK-MPFR-SPRINTNUM MPLAPACK-MPFR-SPRINTHEX \
          MPLAPACK-GMP-POW2 MPLAPACK-GMP-SIGN MPLAPACK-GMP-NINT \
          MPLAPACK-GMP-PI MPLAPACK-GMP-SPRINTNUM LEGACY-NAME-COLLISION \
          LIFETIME-MPFR LIFETIME-MPC; do
    test "$(awk -F '\t' -v id="$id" 'NR > 1 && $1 == id {n++} END {print n+0}' "$matrix")" = 1
done

test "$(awk -F '\t' 'NR > 1 && $6 == "IMPLEMENT_P2C" {n++} END {print n+0}' "$matrix")" = 0
test "$(awk -F '\t' 'NR > 1 && $6 == "VERIFY_ONLY" {n++} END {print n+0}' "$matrix")" = 12
test "$(awk -F '\t' 'NR > 1 && $6 == "DEFER_TO_P3" {n++} END {print n+0}' "$matrix")" = 6
test "$(awk -F '\t' 'NR > 1 && $6 == "DEFER_TO_P4" {n++} END {print n+0}' "$matrix")" = 5
test "$(awk -F '\t' 'NR > 1 && $6 == "OUT_OF_SCOPE" {n++} END {print n+0}' "$matrix")" = 27

test -z "$(git diff --unified=0 "$base" -- include |
    rg '^\+.*(mplapackint|castREAL_|castINTEGER_|\bsign\(|\bnint\(|\biceil\(|\bcabs1\(|\bpow2\(|\bpow4\(|sprintnum|sprinthex)' || true)"
test -z "$(git diff --unified=0 "$base" -- . ':!docs/mplapack_migration/*' |
    rg '^\+.*(#include .*mplapack|mplapackint)' || true)"
git diff --quiet "$base" -- include/gmpfrxx_mkII/adapters
git diff --quiet "$base" -- \
    include/gmpfrxx_mkII/detail/mpfr_impl.hpp \
    include/gmpfrxx_mkII/detail/mpc_impl.hpp \
    include/gmpfrxx_mkII/detail/mpf_impl.hpp \
    include/gmpfrxx_mkII/detail/mpfc_impl.hpp
test -z "$(git diff --unified=0 "$base" -- include tests |
    rg '^\+.*(cast2|external.*operator[+*/-]|precision negotiation|mpfr_get_prec|mpc_get_prec|\bedd\b|\btd\b)' || true)"

! rg -n 'get_d\(|mpf_get_d\(|std::(sqrt|exp|log|sin|cos|tan|pow)\(' \
    include/gmpfrxx_mkII/detail/math_mpf.hpp
rg -q 'mpf_mul\(' include/gmpfrxx_mkII/detail/mpf_impl.hpp
rg -q 'mpf_abs\(' include/gmpfrxx_mkII/detail/math_mpf.hpp
rg -q 'mpf_floor\(' include/gmpfrxx_mkII/detail/math_mpf.hpp
rg -q 'inline mpf_class pi\(mp_bitcnt_t' include/gmpfrxx_mkII/detail/math_mpf.hpp

git diff --check
cmake -S . -B "$build_dir" \
    -DBUILD_TESTING=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS=-I/home/docker/MPLAPACK/include \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF \
    -DGMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC
rg -q '^GMPFRXX_MKII_DEPS_AUTO_FETCH:BOOL=OFF$' "$build_dir/CMakeCache.txt"
test -z "$(rg 'tests/test_mplapack_(default_512|wrapper_compat)\.cpp' \
    "$build_dir/compile_commands.json" |
    rg -- '-DGMPFRXX_MKII_(ENABLE_FMA|FAST_FIXED_PREC|FAST_STABLE_RND)' || true)"
cmake --build "$build_dir" -j"$jobs"

test_list=$(mktemp /tmp/gmpfrxx-mkII-p2c-test-list.XXXXXX)
ctest --test-dir "$build_dir" -N > "$test_list"
for test_id in $(awk -F '\t' 'NR > 1 && $6 == "VERIFY_ONLY" {print $9}' "$matrix" |
                 LC_ALL=C sort -u); do
    rg -q "Test +#[0-9]+: ${test_id}$" "$test_list"
done
ctest --test-dir "$build_dir" --output-on-failure
"$build_dir/tests/test_mplapack_default_512"
"$build_dir/tests/test_mplapack_wrapper_compat"

ctest --test-dir "$build_dir" --output-on-failure -R \
    'compile_fail_(test_long_double_scalar|test_mpf_plus_mpfr|test_mpfr_plus_mpf|test_mpf_plus_mpc|test_mpc_plus_mpf|test_mpfc_plus_mpfr|test_mpfr_plus_mpfc|test_mpfc_plus_mpc|test_mpc_plus_mpfc)'

install_prefix=$(mktemp -d /tmp/gmpfrxx-mkII-p2c-gate-install.XXXXXX)
consumer_build=$(mktemp -d /tmp/gmpfrxx-mkII-p2c-consumer-build.XXXXXX)
cmake --install "$build_dir" --prefix "$install_prefix"
cmake -S docs/mplapack_migration/tools/p2c_consumer \
    -B "$consumer_build" \
    -DCMAKE_PREFIX_PATH="$install_prefix"
cmake --build "$consumer_build" -j"$jobs"
"$consumer_build/p2c_consumer"

p2a_work=$(mktemp -d /tmp/gmpfrxx-mkII-p2a-recheck.XXXXXX)
rmdir "$p2a_work"
cleanup_p2a()
{
    if test -n "${p2a_work:-}" && test -d "$p2a_work"; then
        git worktree remove --force "$p2a_work"
    fi
}
trap cleanup_p2a EXIT
git worktree add --detach "$p2a_work" "$p2a"
cmake -S "$p2a_work" -B "$p2a_work/build-p2a" \
    -DBUILD_TESTING=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS=-I/home/docker/MPLAPACK/include \
    -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF \
    -DGMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC
cmake --build "$p2a_work/build-p2a" -j"$jobs"
P2A_ROOT="$p2a_work" bash docs/mplapack_migration/gate-P2A.sh
git worktree remove --force "$p2a_work"
p2a_work=
trap - EXIT

P2B_GATE_INNER=1 P2B_JOBS="$jobs" \
    bash docs/mplapack_migration/gate-P2B.sh

while IFS= read -r status_line; do
    path=${status_line:3}
    case "$path" in
        STATUS.md|\
        docs/mplapack_migration/REPORT-P2A.md|\
        docs/mplapack_migration/gate-P2A.sh|\
        tests/CMakeLists.txt|\
        tests/test_mplapack_default_512.cpp|\
        tests/test_mplapack_wrapper_compat.cpp|\
        docs/mplapack_migration/P2C_SPIKE_DISPOSITION.tsv|\
        docs/mplapack_migration/REPORT-P2C.md|\
        docs/mplapack_migration/REPORT-P2C-gate.log|\
        docs/mplapack_migration/gate-P2C.sh|\
        docs/mplapack_migration/tools/p2c_consumer/CMakeLists.txt|\
        docs/mplapack_migration/tools/p2c_consumer/main.cpp)
            ;;
        *)
            echo "Unclassified worktree path: $status_line" >&2
            exit 1
            ;;
    esac
done < <(git status --porcelain=v1 --untracked-files=all)

git diff --check
echo "P2C gate: PASS"
