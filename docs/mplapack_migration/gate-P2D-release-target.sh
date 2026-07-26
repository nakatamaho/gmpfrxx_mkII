#!/usr/bin/env bash
set -euo pipefail

root=$(cd "$(dirname "$0")/../.." && pwd)
cd "$root"

start_head=$(git rev-parse HEAD)
start_status=$(git status --porcelain=v1 --untracked-files=all)
start_tree=$(git rev-parse HEAD^{tree})
start_index_tree=$(git write-tree)
index_file=$(git rev-parse --git-path index)
start_index_sha=$(sha256sum "$index_file" | cut -d' ' -f1)
test -z "$start_status"
test "$start_index_tree" = "$start_tree"

runtime_dir=$(mktemp -d \
    "${TMPDIR:-/tmp}/gmpfrxx-p2d-release-gate.XXXXXX")
nested_p2d=

cleanup()
{
    local status=$?
    trap - EXIT INT TERM
    if test -n "$nested_p2d" && test -d "$nested_p2d"; then
        git -C "$root" worktree remove --force "$nested_p2d" \
            >/dev/null 2>&1 || true
    fi
    rm -rf "$runtime_dir"
    exit "$status"
}
trap cleanup EXIT INT TERM

base=6c7bce42494e2d6196ade9b90b89223436b2fe43
previous=9203f6636ae17052966879811f2a4f5fde4685e7
target=429fd1b35e1927ebaccc9fda5aa2801300b45bf5
version=1.1.0
tag_name=v1.1.0
expected_sha=e0f3b813463b7a45dd493a818c60a17530075e0e647ea02227b75501c1984c73
expected_size=15169540
jobs=${P2D_JOBS:-32}
qd_include=${P2D_QD_INCLUDE_DIR:-/home/docker/MPLAPACK/include}
work="$runtime_dir/release-target"
record=docs/mplapack_migration/RELEASE_TARGET.json
classification=docs/mplapack_migration/P2D_RELEASE_TARGET_CLASSIFICATION.tsv
artifact=/home/docker/gmpfrxx_mkII-p2d-artifacts/gmpfrxx_mkII.1.1.0.tar.xz

json_value()
{
    python3 -c \
        'import json,sys; print(json.load(open(sys.argv[1], encoding="utf-8"))[sys.argv[2]])' \
        "$record" "$1"
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

run_consumer()
{
    local name=$1
    local prefix=$2
    local build_dir="$work/consumer-$name"
    rm -rf "$build_dir"
    cmake -S "$root/docs/mplapack_migration/tools/p2d_consumer" \
        -B "$build_dir" \
        -DCMAKE_PREFIX_PATH="$prefix" \
        -DCMAKE_CXX_COMPILER=g++ \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DP2D_QD_INCLUDE_DIR="$qd_include"
    cmake --build "$build_dir" -j"$jobs" --verbose
    "$build_dir/p2d_consumer"
    rg -q "$prefix/include" "$build_dir/compile_commands.json"
    reject_matches -q "$root/include|/generated" \
        "$build_dir/compile_commands.json"
    local depfile
    depfile=$(find "$build_dir" -type f -name '*.d' -print -quit)
    test -n "$depfile"
    rg -q "$prefix/include/gmpfrxx_mkII" "$depfile"
    reject_matches -q "$root/include|/generated" "$depfile"
}

write_gate_report_manifest()
{
    local destination=$1
    git ls-files \
        'docs/mplapack_migration/gate-*.sh' \
        'docs/mplapack_migration/REPORT-*' |
        LC_ALL=C sort |
        while IFS= read -r path; do
            sha256sum "$path"
        done > "$destination"
}

verify_historical_gate_logs()
{
    while IFS=$'\t' read -r expected path marker; do
        test "$(sha256sum "$path" | cut -d' ' -f1)" = "$expected"
        git ls-files --error-unmatch "$path" >/dev/null
        rg -q "$marker" "$path"
    done <<'EOF'
10ba70c0488a644acc452a28e097f234a39e299514c1808ae954ee049552ab8f	docs/mplapack_migration/REPORT-P2B-gate.log	P2B gate: PASS
07aee31da0e810da9b880ea7a694265ffab56e7d5bf22aa5c0126f9fc45b046c	docs/mplapack_migration/REPORT-P2C-gate.log	P2C gate: PASS
11c49904cf7dc957be6b8adc99e9790c28f048d1124a0c9d864474e73ff8c3d1	docs/mplapack_migration/REPORT-P2D-gate.log	P2D gate: PASS
b98488a689ad8a0fb798315b38c1446cab906c055c07a4bbd251066ad0f2b4ae	docs/mplapack_migration/REPORT-P2D-release-target-gate.log	P2D release-target gate: PASS
EOF
    git diff --check HEAD -- \
        docs/mplapack_migration/REPORT-P2B-gate.log \
        docs/mplapack_migration/REPORT-P2C-gate.log \
        docs/mplapack_migration/REPORT-P2D-gate.log \
        docs/mplapack_migration/REPORT-P2D-release-target-gate.log
}

manifest_before="$runtime_dir/gate-report-before.sha256"
manifest_after="$runtime_dir/gate-report-after.sha256"
write_gate_report_manifest "$manifest_before"
verify_historical_gate_logs

echo "== repository and publication baseline =="
test "$(git branch --show-current)" = topic/mplapack-release-hardening
test "$(git merge-base HEAD "$base")" = "$base"
test "$(git rev-list --left-right --count "$base"...HEAD | awk '{print $1}')" = 0
git merge-base --is-ancestor "$previous" HEAD
git merge-base --is-ancestor "$target" HEAD
test "$(git cat-file -t "$target")" = commit
remote_branch_before=$(git ls-remote --heads origin \
    topic/mplapack-release-hardening)
test -z "$remote_branch_before"

echo "== immutable requirements =="
requirements=docs/mplapack_migration/requirements
test "$(sha256sum "$requirements/LOCK.json" | cut -d' ' -f1)" = \
    9fb757df92a36e996e0ea28c9dcf9351cf5063ff95646d750a188e3f7f32b075
test "$(sha256sum "$requirements/interop_requirements.tsv" | cut -d' ' -f1)" = \
    2bb9700d76cd50a6c4328f497b35e3794065fecc915eac40408f01c0188db00f
test "$(sha256sum "$requirements/SPIKE.md" | cut -d' ' -f1)" = \
    29b4bf3c831c0a892773139be3e8283153bfb4db4d74726b0893a807cae0928d
test "$(sha256sum "$requirements/INTEROP_POLICY.md" | cut -d' ' -f1)" = \
    859fe8ebc18100c1225d04633ceaa38140dd5ac48f6203538950bfc40886a9ed

echo "== commit and path classification =="
for sha in \
    94f49e61c359d65508e4c126049e70e4ed7bb681 \
    518934363d9b0f3fbab36f9c99c2fddb7d9ebf7c \
    6ea737d47c064a4231945fd2f18b95b297bfb046 \
    429fd1b35e1927ebaccc9fda5aa2801300b45bf5 \
    9203f6636ae17052966879811f2a4f5fde4685e7; do
    test "$(awk -F '\t' -v sha="$sha" \
        'NR > 1 && $1 == "COMMIT" && $2 == sha {n++} END {print n+0}' \
        "$classification")" = 1
done
for sha in $(git rev-list --reverse "$previous"..HEAD); do
    while IFS= read -r path; do
        test -n "$path"
        count=$(awk -F '\t' -v path="$path" \
            'NR > 1 && $1 == "PATH" && $3 == path {n++} END {print n+0}' \
            "$classification")
        test "$count" -ge 1
        class=$(awk -F '\t' -v path="$path" \
            'NR > 1 && $1 == "PATH" && $3 == path {print $4; exit}' \
            "$classification")
        case "$class" in
            MIGRATION_EVIDENCE_ONLY|GENERATED_EVIDENCE_ONLY) ;;
            *)
                echo "Post-P2D evidence commit has an unclassified path: $sha $path" >&2
                exit 1
                ;;
        esac
    done < <(git diff-tree --no-commit-id --name-only -r "$sha")
done

rm -rf "$work"
mkdir -p "$work"
post_paths="$work/post-target-paths.txt"
{
    git diff --name-only "$target"..HEAD
    git status --porcelain=v1 --untracked-files=all | cut -c4-
} | LC_ALL=C sort -u > "$post_paths"
test -s "$post_paths"
while IFS= read -r path; do
    test -n "$path"
    count=$(awk -F '\t' -v path="$path" \
        'NR > 1 && $1 == "PATH" && $3 == path {n++} END {print n+0}' \
        "$classification")
    test "$count" -ge 1
    class=$(awk -F '\t' -v path="$path" \
        'NR > 1 && $1 == "PATH" && $3 == path {print $4; exit}' \
        "$classification")
    case "$class" in
        MIGRATION_EVIDENCE_ONLY|GENERATED_EVIDENCE_ONLY) ;;
        *)
            echo "Post-target release-affecting or unclassified path: $path" >&2
            exit 1
            ;;
    esac
done < "$post_paths"
reject_matches -n $'\tUNCLASSIFIED\t' "$classification"
git show "$target:.gitattributes" |
    rg -q '^docs/mplapack_migration export-ignore$'
git show "$target:.gitattributes" | rg -q '^\*\.md export-ignore$'

echo "== release target JSON and tag convention =="
python3 -m json.tool "$record" >/dev/null
test "$(json_value version)" = "$version"
test "$(json_value tag_name)" = "$tag_name"
test "$(json_value tag_target_sha)" = "$target"
test "$(json_value archive_source_sha)" = "$target"
test "$(json_value archive_filename)" = "gmpfrxx_mkII.$version.tar.xz"
test "$(json_value archive_sha256)" = "$expected_sha"
test "$(json_value archive_size_bytes)" = "$expected_size"
test "$(json_value p2c_base_sha)" = "$base"
test "$(json_value previous_p2d_candidate_sha)" = "$previous"
test "$(json_value release_target_decision)" = EXISTING_SOURCE_VALID
test "$(json_value tag_created)" = False
test "$(json_value release_published)" = False
[[ "$target" =~ ^[0-9a-f]{40}$ ]]
[[ "$expected_sha" =~ ^[0-9a-f]{64}$ ]]
test "$(git cat-file -t "$target")" = commit
git merge-base --is-ancestor "$target" HEAD
for existing in v1.0.0-rc1 v1.0.0 v1.0.1; do
    test "$(git cat-file -t "refs/tags/$existing")" = tag
done
test "$tag_name" = v1.1.0
test ! -e ".git/refs/tags/$tag_name"
if git rev-parse -q --verify "refs/tags/$tag_name" >/dev/null; then
    echo "Unexpected local tag: $tag_name" >&2
    exit 1
fi
test -z "$(git ls-remote --tags origin "refs/tags/$tag_name" \
    "refs/tags/$tag_name^{}")"

echo "== authoritative archive =="
test -f "$artifact"
test "$(basename "$artifact")" = "gmpfrxx_mkII.$version.tar.xz"
test "$(sha256sum "$artifact" | cut -d' ' -f1)" = "$expected_sha"
test "$(stat -c %s "$artifact")" = "$expected_size"

echo "== two fresh-clone archive generations =="
for pass in one two; do
    clone="$work/clone-$pass"
    git clone --quiet --no-hardlinks --no-checkout "$root" "$clone"
    git -C "$clone" checkout --quiet --detach "$target"
    test -z "$(git -C "$clone" status --short)"
    cmake -S "$clone" -B "$clone/build-dist" \
        -DCMAKE_BUILD_TYPE=Release \
        -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF
    rg -q '^GMPFRXX_MKII_DEPS_AUTO_FETCH:BOOL=OFF$' \
        "$clone/build-dist/CMakeCache.txt"
    cmake --build "$clone/build-dist" --target dist -j"$jobs"
    reproduced="$clone/build-dist/gmpfrxx_mkII.$version.tar.xz"
    test -f "$reproduced"
    test "$(sha256sum "$reproduced" | cut -d' ' -f1)" = "$expected_sha"
    test "$(stat -c %s "$reproduced")" = "$expected_size"
done
archive_one="$work/clone-one/build-dist/gmpfrxx_mkII.$version.tar.xz"
archive_two="$work/clone-two/build-dist/gmpfrxx_mkII.$version.tar.xz"
cmp -s "$archive_one" "$archive_two"
cmp -s "$archive_one" "$artifact"

echo "== archive content and version =="
file_list="$work/archive-files.txt"
tar -tf "$artifact" > "$file_list"
rg -q "^gmpfrxx_mkII\\.$version/CMakeLists.txt$" "$file_list"
reject_matches -n \
    '(^|/)(\.git|CMakeCache\.txt|CMakeFiles|build(-[^/]*)?|docs/mplapack_migration)(/|$)|\.(o|a|so|dylib|dll)$' \
    "$file_list"
tar -xOf "$artifact" "gmpfrxx_mkII.$version/CMakeLists.txt" |
    rg -q "project\\(gmpfrxx_mkII VERSION $version LANGUAGES CXX\\)"

echo "== clean-room archive build and complete suite =="
clean_src="$work/archive-source"
clean_build="$work/archive-build"
clean_install="$work/archive-install"
mkdir -p "$clean_src"
tar -xf "$artifact" -C "$clean_src" --strip-components=1
test ! -e "$clean_src/.git"
reject_matches -a -n \
    '/home/docker|gmpfrxx_mkII-p2a-clean|prototype-snapshot|forensic' \
    "$clean_src" --glob '!reference/upstream/**'
cmake -S "$clean_src" -B "$clean_build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-I$qd_include" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_TESTING=ON \
    -DGMPFRXX_MKII_BUILD_EXAMPLES=ON \
    -DGMPFRXX_MKII_BUILD_BENCHMARKS=ON \
    -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF \
    -DGMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC
rg -q '^GMPFRXX_MKII_DEPS_AUTO_FETCH:BOOL=OFF$' \
    "$clean_build/CMakeCache.txt"
cmake --build "$clean_build" -j"$jobs"
test "$(ctest --test-dir "$clean_build" -N |
    awk '/Total Tests:/ {print $3}')" = 192
ctest --test-dir "$clean_build" --output-on-failure

echo "== directed release-contract tests =="
ctest --test-dir "$clean_build" --output-on-failure -R \
    'test_(dd_real_adapter|qd_real_adapter|mpf_binary_real_adapters|complex_compare_adapters|mplapack_default_512|mplapack_wrapper_compat|temporary_expression_lifetime)|compile_fail_(test_long_double_scalar|test_mpf_plus_mpfr|test_mpfr_plus_mpf|test_mpf_plus_mpc|test_mpc_plus_mpf|test_mpfc_plus_mpfr|test_mpfr_plus_mpfc|test_mpfc_plus_mpc|test_mpc_plus_mpfc)'

echo "== clean install and installed consumer =="
cmake --install "$clean_build" --prefix "$clean_install"
run_consumer archive "$clean_install"
rg -q "set\\(PACKAGE_VERSION \"$version\"\\)" \
    "$clean_install/lib/cmake/gmpfrxx_mkII/gmpfrxx_mkIIConfigVersion.cmake"
reject_matches -n "$clean_src|$clean_build|$root|/home/docker" \
    "$clean_install/lib/cmake"

echo "== relocated install and consumer =="
relocated="$work/relocated-install"
cp -a "$clean_install" "$relocated"
run_consumer relocated "$relocated"
reject_matches -n "$clean_src|$clean_build|$clean_install|$root|/home/docker" \
    "$relocated/lib/cmake"

echo "== production forbidden API scans =="
rg -q 'mpfr_set_ld' \
    "$clean_src/include/gmpfrxx_mkII/adapters/detail/binary_float.hpp"
rg -q 'mpfr_set_float128' \
    "$clean_src/include/gmpfrxx_mkII/adapters/detail/binary_float.hpp"
rg -q 'mpfr_set_f' "$clean_src/include/gmpfrxx_mkII/adapters/mpf_real.hpp"
reject_matches -n 'get_d\(|mpf_get_d\(|mpfr_get_d\(' \
    "$clean_src/include/gmpfrxx_mkII/adapters"
reject_matches -n \
    'cast_(mpfr|mpc|mpf)_to_|set_mpf_from_binary|get_mpfr_component|mpfr_get_float128' \
    "$clean_src/include/gmpfrxx_mkII/adapters"
reject_matches -n 'mpfr_get_prec\(dest\)|adapter_accumulator_precision' \
    "$clean_src/include/gmpfrxx_mkII/adapters"
reject_matches -n '#include .*mplapack|\bmplapackint\b|castREAL_|castINTEGER_|\bnint\(|\biceil\(|\bcabs1\(' \
    "$clean_src/include"
reject_matches -n \
    '#include[[:space:]]*[<"]gmpxx\.h[>"]|\blibgmpxx\b' \
    "$clean_src/CMakeLists.txt" "$clean_src/cmake" "$clean_src/include"
find "$clean_src/include" "$clean_src/tests" -type f -print |
    reject_matches '(^|/)(edd|td)(_|/|\.)'

echo "== accepted P2A/P2B/P2C and P2D gates =="
nested_root="$runtime_dir/worktrees"
mkdir -p "$nested_root"
nested_p2d="$nested_root/p2d"
git worktree add --detach "$nested_p2d" "$start_head"
(
    cd "$nested_p2d"
    P2D_GATE_INNER=1 P2D_JOBS="$jobs" \
        P2D_WORK_ROOT="$runtime_dir/p2d-gate" \
        bash docs/mplapack_migration/gate-P2D.sh
)
git worktree remove --force "$nested_p2d"
nested_p2d=

echo "== no publication and classified worktree =="
test "$(json_value tag_created)" = False
test "$(json_value release_published)" = False
test -z "$(git ls-remote --heads origin \
    topic/mplapack-release-hardening)"
test -z "$(git ls-remote --tags origin "refs/tags/$tag_name" \
    "refs/tags/$tag_name^{}")"
remote_branch_after=$(git ls-remote --heads origin \
    topic/mplapack-release-hardening)
test "$remote_branch_after" = "$remote_branch_before"

github_release_probe="$runtime_dir/github-release.txt"
set +e
gh api -i repos/nakatamaho/gmpfrxx_mkII/releases/tags/"$tag_name" \
    >"$github_release_probe" 2>&1
github_release_status=$?
set -e
test "$github_release_status" -ne 0
rg -q '404|Not Found' "$github_release_probe"

final_paths="$work/final-status-paths.txt"
git status --porcelain=v1 --untracked-files=all | cut -c4- |
    LC_ALL=C sort -u > "$final_paths"
while IFS= read -r path; do
    test -n "$path"
    count=$(awk -F '\t' -v path="$path" \
        'NR > 1 && $1 == "PATH" && $3 == path {n++} END {print n+0}' \
        "$classification")
    test "$count" -ge 1
done < "$final_paths"

write_gate_report_manifest "$manifest_after"
cmp -s "$manifest_before" "$manifest_after"
test "$(git rev-parse HEAD)" = "$start_head"
test "$(git write-tree)" = "$start_index_tree"
test "$(git write-tree)" = "$start_tree"
test -z "$(git status --porcelain=v1 --untracked-files=all)"
test "$(sha256sum "$index_file" | cut -d' ' -f1)" = \
    "$start_index_sha"
git diff --check

echo "release_target_sha=$target"
echo "archive_filename=gmpfrxx_mkII.$version.tar.xz"
echo "archive_size_bytes=$expected_size"
echo "archive_sha256=$expected_sha"
echo "P2D release-target gate: PASS"
