#!/usr/bin/env bash
set -euo pipefail

root=$(cd "$(dirname "$0")/../.." && pwd)
log="$root/docs/mplapack_migration/REPORT-P2D-gate.log"

if test "${P2D_GATE_INNER:-0}" != 1; then
    set +e
    P2D_GATE_INNER=1 bash "$0" "$@" 2>&1 | tee "$log"
    exit "${PIPESTATUS[0]}"
fi

cd "$root"

base=6c7bce42494e2d6196ade9b90b89223436b2fe43
p2a=0bd061c0b62d87256a1d0c18a9aea453524c79c5
p2c=6c7bce42494e2d6196ade9b90b89223436b2fe43
version=1.1.0
jobs=${P2D_JOBS:-32}
requirements=docs/mplapack_migration/requirements
api_audit=docs/mplapack_migration/P2D_API_AUDIT.tsv
artifact_record=docs/mplapack_migration/P2D_RELEASE_ARTIFACTS.txt
qd_include=${P2D_QD_INCLUDE_DIR:-/home/docker/MPLAPACK/include}
work=${P2D_WORK_ROOT:-/tmp/gmpfrxx-mkII-p2d-gate}

artifact_value()
{
    awk -F= -v key="$1" '$1 == key {sub(/^[^=]*=/, ""); print; found=1}
        END {if (!found) exit 1}' "$artifact_record"
}

reject_matches()
{
    local status
    if rg "$@"; then
        echo "Forbidden match found: rg $*" >&2
        return 1
    else
        status=$?
    fi
    if test "$status" -ne 1; then
        echo "Forbidden-match scan failed: rg $*" >&2
        return "$status"
    fi
}

cleanup_worktrees()
{
    for path in "${p2a_work:-}" "${p2c_work:-}"; do
        if test -n "$path" && test -d "$path"; then
            git worktree remove --force "$path" >/dev/null 2>&1 || true
        fi
    done
}
trap cleanup_worktrees EXIT

echo "== repository and ancestry =="
test "$(git merge-base HEAD "$base")" = "$base"
test "$(git rev-list --left-right --count "$base"...HEAD | awk '{print $1}')" = 0
test "$(git rev-parse "origin/topic/mplapack-wrapper-compat")" = "$p2c"
remote_p2c=$(git ls-remote origin refs/heads/topic/mplapack-wrapper-compat |
    awk '{print $1}')
test "$remote_p2c" = "$p2c"

echo "== immutable requirements =="
test "$(sha256sum "$requirements/LOCK.json" | cut -d' ' -f1)" = \
    9fb757df92a36e996e0ea28c9dcf9351cf5063ff95646d750a188e3f7f32b075
test "$(sha256sum "$requirements/interop_requirements.tsv" | cut -d' ' -f1)" = \
    2bb9700d76cd50a6c4328f497b35e3794065fecc915eac40408f01c0188db00f
test "$(sha256sum "$requirements/SPIKE.md" | cut -d' ' -f1)" = \
    29b4bf3c831c0a892773139be3e8283153bfb4db4d74726b0893a807cae0928d
test "$(sha256sum "$requirements/INTEROP_POLICY.md" | cut -d' ' -f1)" = \
    859fe8ebc18100c1225d04633ceaa38140dd5ac48f6203538950bfc40886a9ed

echo "== P2A repair audit =="
snapshot=/home/docker/gmpfrxx_mkII-prototype-snapshot-p2a-retry.F58Geq
snapshot_digest=6a9fc4d4ab1fbe4ad71a1c5f40657ff6efcb24519edcf09341729b5d4e2c8ad3
test "$(sha256sum "$snapshot/SHA256SUMS" | cut -d' ' -f1)" = "$snapshot_digest"
(cd "$snapshot" && sha256sum --check SHA256SUMS)
for statement in \
    "Acceptance criteria: unchanged" \
    "No required test was removed" \
    "No failure was converted to a pass by weakening a condition" \
    "189/189 tests passed"; do
    rg -q "$statement" docs/mplapack_migration/P2A_REPAIR_AUDIT.md
done

echo "== version consistency =="
test "$(cat docs/mplapack_migration/RELEASE_VERSION.txt)" = "$version"
rg -q "project\\(gmpfrxx_mkII VERSION $version LANGUAGES CXX\\)" CMakeLists.txt
rg -q "#define GMPFRXX_MKII_VERSION \"$version\"" \
    include/gmpfrxx_mkII/detail/config.hpp
rg -q "strcmp\\(gmp_version, \"$version\"\\)" tests/test_version_info.cpp
rg -Fq "\\newcommand{\\manualversion}{$version}" manual/gmpfrxx_mkII_manual.tex
rg -Fq "\\newcommand{\\manualreleasetag}{v$version}" manual/gmpfrxx_mkII_manual.tex
rg -q "^# Changes for $version$" "CHANGES.$version.md"

echo "== API audit =="
test "$(awk -F '\t' 'NR > 1 {n++} END {print n+0}' "$api_audit")" = 35
test "$(awk -F '\t' 'NR > 1 {seen[$1]++}
    END {for (id in seen) if (seen[id] != 1) exit 1}' "$api_audit"; echo $?)" = 0
while IFS=$'\t' read -r id _ _ _ requirement _ _; do
    if test "$requirement" = REQUIRED; then
        test "$(awk -F '\t' -v id="$id" \
            'NR > 1 && $1 == id && $7 == "PASS" {n++} END {print n+0}' \
            "$api_audit")" = 1
    fi
done < "$requirements/interop_requirements.tsv"
test "$(awk -F '\t' 'NR > 1 && $2 == "P2A" && $7 == "PASS" {n++}
    END {print n+0}' "$api_audit")" = 6
test "$(awk -F '\t' 'NR > 1 && $2 == "P2B" && $7 == "PASS" {n++}
    END {print n+0}' "$api_audit")" = 6
test "$(awk -F '\t' 'NR > 1 && $2 == "P2C" && $7 == "PASS" {n++}
    END {print n+0}' "$api_audit")" = 12
test "$(awk -F '\t' 'NR > 1 && $2 == "P2C" &&
    $7 == "NOT_APPLICABLE" {n++} END {print n+0}' "$api_audit")" = 11

echo "== public contract static checks =="
rg -q 'mpfr_set_ld' include/gmpfrxx_mkII/adapters/detail/binary_float.hpp
rg -q 'mpfr_set_float128' include/gmpfrxx_mkII/adapters/detail/binary_float.hpp
rg -q 'mpfr_set_f' include/gmpfrxx_mkII/adapters/mpf_real.hpp
rg -q 'set_mpfr_from_real_components<dd_real, 2>' \
    include/gmpfrxx_mkII/adapters/dd_real.hpp
rg -q 'set_mpfr_from_real_components<qd_real, 4>' \
    include/gmpfrxx_mkII/adapters/qd_real.hpp
reject_matches -n 'get_d\(|mpf_get_d\(|mpfr_get_d\(' \
    include/gmpfrxx_mkII/adapters
reject_matches -n \
    'cast_(mpfr|mpc|mpf)_to_|set_mpf_from_binary|get_mpfr_component|mpfr_get_float128' \
    include/gmpfrxx_mkII/adapters
reject_matches -n 'mpfr_get_prec\(dest\)|adapter_accumulator_precision' \
    include/gmpfrxx_mkII/adapters
reject_matches -n 'mpfr_class\(const External& value, mpfr_prec_t precision\)' \
    include/gmpfrxx_mkII/detail/mpfr_impl.hpp
reject_matches -n '#include .*mplapack|\bmplapackint\b' include
reject_matches -n \
    '#include[[:space:]]*[<"]gmpxx\.h[>"]|(^|[[:space:]])-lgmpxx([[:space:]]|$)|\blibgmpxx\b' \
    CMakeLists.txt cmake include
git diff --unified=0 "$base" -- include |
    reject_matches '^\+.*(castREAL_|castINTEGER_|\bsign\(|\bnint\(|\biceil\(|\bcabs1\(|\bpow2\(|\bpow4\(|sprintnum|sprinthex)'
git diff --name-only "$base" -- include tests |
    reject_matches '(^|/)(edd|td)(_|/|\.)'

echo "== expression lifetime scanner =="
scanner=docs/mplapack_migration/tools/scan_expression_lifetimes.py
test -x "$scanner"
scanner_fixture="$work/scanner-fixture"
rm -rf "$scanner_fixture"
mkdir -p "$scanner_fixture"
printf '%s\n' \
    'void f(){ auto x = mpfrxx::mpfr_class(1) + mpfrxx::mpfr_class(2); }' \
    > "$scanner_fixture/bad.cpp"
if "$scanner" "$scanner_fixture" > "$scanner_fixture/out"; then
    echo "scanner failed to reject saved expression" >&2
    exit 1
fi
rg -q 'saved expression candidate' "$scanner_fixture/out"
rm "$scanner_fixture/bad.cpp"
printf '%s\n' \
    'void f(){ mpfrxx::mpfr_class x = mpfrxx::mpfr_class(1) + mpfrxx::mpfr_class(2); }' \
    > "$scanner_fixture/good.cpp"
"$scanner" "$scanner_fixture"
"$scanner" /home/docker/mplapack

configure_build_test()
{
    local name=$1
    local build_type=$2
    local compiler=$3
    local components=$4
    local flags=$5
    local build_dir="$work/build-$name"
    local install_dir="$work/install-$name"
    rm -rf "$build_dir" "$install_dir"
    local benchmarks=ON
    case "$name" in
        component-*) benchmarks=OFF ;;
    esac
    cmake -S "$root" -B "$build_dir" \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_CXX_COMPILER="$compiler" \
        -DCMAKE_CXX_FLAGS="$flags -I$qd_include" \
        -DCMAKE_EXE_LINKER_FLAGS="$flags" \
        -DCMAKE_SHARED_LINKER_FLAGS="$flags" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DBUILD_TESTING=ON \
        -DGMPFRXX_MKII_BUILD_EXAMPLES=ON \
        -DGMPFRXX_MKII_BUILD_BENCHMARKS="$benchmarks" \
        -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF \
        -DGMPFRXX_MKII_COMPONENTS="$components"
    rg -q '^GMPFRXX_MKII_DEPS_AUTO_FETCH:BOOL=OFF$' "$build_dir/CMakeCache.txt"
    cmake --build "$build_dir" -j"$jobs"
    if test "$name" = clang-sanitize; then
        ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1 \
            ctest --test-dir "$build_dir" --output-on-failure
    else
        ctest --test-dir "$build_dir" --output-on-failure
    fi
    cmake --install "$build_dir" --prefix "$install_dir"
}

run_consumer()
{
    local name=$1
    local prefix=$2
    local compiler=$3
    local flags=$4
    local consumer_dir="$work/consumer-$name"
    rm -rf "$consumer_dir"
    cmake -S "$root/docs/mplapack_migration/tools/p2d_consumer" \
        -B "$consumer_dir" \
        -DCMAKE_PREFIX_PATH="$prefix" \
        -DCMAKE_CXX_COMPILER="$compiler" \
        -DCMAKE_CXX_FLAGS="$flags" \
        -DCMAKE_EXE_LINKER_FLAGS="$flags" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DP2D_QD_INCLUDE_DIR="$qd_include"
    cmake --build "$consumer_dir" -j"$jobs" --verbose
    if test "$name" = clang-sanitize; then
        ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1 \
            "$consumer_dir/p2d_consumer"
    else
        "$consumer_dir/p2d_consumer"
    fi
    rg -q "$prefix/include" "$consumer_dir/compile_commands.json"
    reject_matches -q "$root/include|/generated" "$consumer_dir/compile_commands.json"
    depfile=$(find "$consumer_dir" -type f -name '*.d' -print -quit)
    test -n "$depfile"
    rg -q "$prefix/include/gmpfrxx_mkII" "$depfile"
    reject_matches -q "$root/include|/generated" "$depfile"
}

echo "== GCC Debug matrix =="
rm -rf "$work"
mkdir -p "$work"
configure_build_test gcc-debug Debug g++ GMP,MPFR,MPC ""
run_consumer gcc-debug "$work/install-gcc-debug" g++ ""

echo "== GCC Release matrix =="
configure_build_test gcc-release Release g++ GMP,MPFR,MPC ""
run_consumer gcc-release "$work/install-gcc-release" g++ ""

echo "== directed tests and compile-fail tests =="
ctest --test-dir "$work/build-gcc-release" --output-on-failure -R \
    'test_(dd_real_adapter|qd_real_adapter|mpf_binary_real_adapters|complex_compare_adapters|mplapack_default_512|mplapack_wrapper_compat|temporary_expression_lifetime)|compile_fail_(test_long_double_scalar|test_mpf_plus_mpfr|test_mpfr_plus_mpf|test_mpf_plus_mpc|test_mpc_plus_mpf|test_mpfc_plus_mpfr|test_mpfr_plus_mpfc|test_mpfc_plus_mpc|test_mpc_plus_mpfc)'

echo "== Clang Debug ASan/UBSan matrix =="
clang_flags='-fsanitize=address,undefined -fno-omit-frame-pointer'
configure_build_test clang-sanitize Debug clang++ GMP,MPFR,MPC "$clang_flags"
run_consumer clang-sanitize "$work/install-clang-sanitize" clang++ "$clang_flags"

echo "== component and feature matrices =="
configure_build_test component-gmp Debug g++ GMP ""
configure_build_test component-gmp-mpfr Debug g++ GMP,MPFR ""
for feature_test in test_dd_real_adapter test_qd_real_adapter \
        test_mpf_binary_real_adapters test_complex_compare_adapters; do
    "$work/build-gcc-release/tests/$feature_test"
done

echo "== accepted P2A gate =="
p2a_work="$work/accepted-p2a"
git worktree add --detach "$p2a_work" "$p2a"
cmake -S "$p2a_work" -B "$p2a_work/build-p2a" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-I$qd_include" \
    -DBUILD_TESTING=ON \
    -DGMPFRXX_MKII_BUILD_BENCHMARKS=ON \
    -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF \
    -DGMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC
cmake --build "$p2a_work/build-p2a" -j"$jobs"
P2A_ROOT="$p2a_work" bash "$root/docs/mplapack_migration/gate-P2A.sh"
test "$(ctest --test-dir "$p2a_work/build-p2a" -N |
    awk '/Total Tests:/ {print $3}')" = 189
git worktree remove --force "$p2a_work"
p2a_work=

echo "== current P2B compatibility gate =="
P2B_GATE_INNER=1 P2B_JOBS="$jobs" \
    P2B_BUILD_DIR="$work/build-p2b-current" \
    bash docs/mplapack_migration/gate-P2B.sh

echo "== accepted P2C gate =="
p2c_work="$work/accepted-p2c"
git worktree add --detach "$p2c_work" "$p2c"
P2C_JOBS="$jobs" P2C_BUILD_DIR="$p2c_work/build-p2c" \
    bash "$p2c_work/docs/mplapack_migration/gate-P2C.sh"
git worktree remove --force "$p2c_work"
p2c_work=

echo "== installed package relocation =="
relocated="$work/relocated-install"
rm -rf "$relocated"
cp -a "$work/install-gcc-release" "$relocated"
run_consumer relocated "$relocated" g++ ""
reject_matches -n \
    "$root|$work/build-gcc-release|/home/docker/gmpfrxx_mkII-p2a-clean" \
    "$relocated/lib/cmake"

echo "== release archive =="
test -f "$artifact_record"
archive_filename=$(artifact_value archive_filename)
archive_sha256=$(artifact_value archive_sha256)
archive_size=$(artifact_value archive_size)
source_sha=$(artifact_value source_sha)
artifact_version=$(artifact_value version)
archive_path=$(artifact_value archive_path)
test "$archive_filename" = "gmpfrxx_mkII.$version.tar.xz"
test "$artifact_version" = "$version"
test "$(git cat-file -t "$source_sha")" = commit
test -f "$archive_path"
test "$(basename "$archive_path")" = "$archive_filename"
test "$(sha256sum "$archive_path" | cut -d' ' -f1)" = "$archive_sha256"
test "$(stat -c %s "$archive_path")" = "$archive_size"

archive_dir="$work/archive-repro"
rm -rf "$archive_dir"
mkdir -p "$archive_dir"
for pass in one two; do
    git archive --worktree-attributes --format=tar \
        "--prefix=gmpfrxx_mkII.$version/" "$source_sha" \
        -o "$archive_dir/$pass.tar"
    xz -z -f -9 "$archive_dir/$pass.tar"
done
cmp -s "$archive_dir/one.tar.xz" "$archive_dir/two.tar.xz"
cmp -s "$archive_path" "$archive_dir/one.tar.xz"

tar -tf "$archive_path" > "$archive_dir/file-list.txt"
rg -q "^gmpfrxx_mkII\\.$version/CMakeLists.txt$" "$archive_dir/file-list.txt"
reject_matches -n \
    '(^|/)(\.git|CMakeCache\.txt|CMakeFiles|build[^/]*|docs/mplapack_migration)(/|$)|\.(o|a|so|dylib|dll)$' \
    "$archive_dir/file-list.txt"

clean_src="$work/archive-source"
clean_build="$work/archive-build"
clean_install="$work/archive-install"
rm -rf "$clean_src" "$clean_build" "$clean_install"
mkdir -p "$clean_src"
tar -xf "$archive_path" -C "$clean_src" --strip-components=1
test ! -e "$clean_src/.git"
reject_matches -a -n '/home/docker|gmpfrxx_mkII-p2a-clean|prototype-snapshot' \
    "$clean_src" \
    --glob '!reference/upstream/**'
cmake -S "$clean_src" -B "$clean_build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-I$qd_include" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_TESTING=ON \
    -DGMPFRXX_MKII_BUILD_EXAMPLES=ON \
    -DGMPFRXX_MKII_BUILD_BENCHMARKS=ON \
    -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF \
    -DGMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC
rg -q '^GMPFRXX_MKII_DEPS_AUTO_FETCH:BOOL=OFF$' "$clean_build/CMakeCache.txt"
cmake --build "$clean_build" -j"$jobs"
ctest --test-dir "$clean_build" --output-on-failure
cmake --install "$clean_build" --prefix "$clean_install"
run_consumer archive "$clean_install" g++ ""
reject_matches -n "$clean_src|$clean_build|$root|/home/docker" \
    "$clean_install/lib/cmake"
rg -q "set\\(PACKAGE_VERSION \"$version\"\\)" \
    "$clean_install/lib/cmake/gmpfrxx_mkII/gmpfrxx_mkIIConfigVersion.cmake"

echo "== source hygiene =="
git diff --check
while IFS= read -r status_line; do
    path=${status_line:3}
    case "$path" in
        .gitattributes|CMakeLists.txt|README.md|SPECIFICATIONS.md|STATUS.md|\
        CHANGES.1.1.0.md|manual/gmpfrxx_mkII_manual.tex|tests/test_version_info.cpp|\
        include/gmpfrxx_mkII/adapters/detail/binary_float.hpp|\
        include/gmpfrxx_mkII/adapters/detail/real_components.hpp|\
        include/gmpfrxx_mkII/detail/config.hpp|\
        include/gmpfrxx_mkII/detail/mpfr_impl.hpp|\
        docs/mplapack_comparison_adapters.md|\
        docs/mplapack_migration/P2A_REPAIR_AUDIT.md|\
        docs/mplapack_migration/P2D_API_AUDIT.tsv|\
        docs/mplapack_migration/P2D_RELEASE_ARTIFACTS.txt|\
        docs/mplapack_migration/RELEASE_VERSION.txt|\
        docs/mplapack_migration/REPORT-P2D.md|\
        docs/mplapack_migration/REPORT-P2D-gate.log|\
        docs/mplapack_migration/gate-P2D.sh|\
        docs/mplapack_migration/tools/p2d_consumer/CMakeLists.txt|\
        docs/mplapack_migration/tools/p2d_consumer/main.cpp|\
        docs/mplapack_migration/tools/scan_expression_lifetimes.py)
            ;;
        *)
            echo "Unclassified worktree path: $status_line" >&2
            exit 1
            ;;
    esac
done < <(git status --porcelain=v1 --untracked-files=all)

echo "P2D gate: PASS"
