Post-phase MPFR Rgemv strict native numbering:
DONE

Implemented features:
- Tightened MPFR Rgemv raw C native numbering so numeric suffixes map to the
  same timed source shape as the wrapper kernels.
- Reworked `Rgemv_mpfr_C_native_01.cpp` and
  `Rgemv_mpfr_C_native_01_FMA.cpp` to use row-dot source shapes matching
  wrapper variant `01`.
- Added `Rgemv_mpfr_C_native_04.cpp` as the serial raw C counterpart for
  wrapper variant `04`.
- Reworked `Rgemv_mpfr_C_native_openmp_02.cpp` and
  `Rgemv_mpfr_C_native_openmp_02_FMA.cpp` to match wrapper OpenMP variant
  `02`, which is currently the row-owned row-dot placeholder.
- Added `Rgemv_mpfr_C_native_openmp_04.cpp` and
  `Rgemv_mpfr_C_native_openmp_04_FMA.cpp` as raw C counterparts for wrapper
  OpenMP variant `04`.
- Registered the new MPFR Rgemv raw C native `04` targets in
  `benchmarks/CMakeLists.txt`.
- Updated `benchmarks/mpfr/02_Rgemv/README.md` to remove ambiguous
  `closest` mappings and document exact C native equivalent kernels.

Tests added:
- No unit tests were added; this phase updates benchmark kernels.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/mpfr/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,120p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_02.cpp`
- `sed -n '1,120p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_02_FMA.cpp`
- `sed -n '1,120p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_04.cpp`
- `sed -n '1,120p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_02.cpp`
- `sed -n '1,140p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_04.cpp && sed -n '1,140p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_04.cpp`
- `sed -n '1,120p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_04.cpp && sed -n '1,140p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_04.cpp`
- `sed -n '32,70p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01.cpp`
- `sed -n '32,70p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01_FMA.cpp`
- `git diff --stat`
- `sed -n '86,128p' benchmarks/mpfr/02_Rgemv/README.md`
- `sed -n '142,158p' benchmarks/mpfr/02_Rgemv/README.md`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_01 Rgemv_mpfr_C_native_01_FMA Rgemv_mpfr_C_native_04 Rgemv_mpfr_C_native_openmp_02 Rgemv_mpfr_C_native_openmp_02_FMA Rgemv_mpfr_C_native_openmp_04 Rgemv_mpfr_C_native_openmp_04_FMA Rgemv_mpfr_C_native_openmp_05 Rgemv_mpfr_C_native_openmp_06 Rgemv_mpfr_C_native_openmp_07 -j`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01_FMA 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_04 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_02 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_02_FMA 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_04 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_04_FMA 13 17 128`
- `rg -n "Closest|closest|none yet|not been added|C_native_openmp_02.*kernel_openmp_04|C_native_02.*kernel_04" benchmarks/mpfr/02_Rgemv/README.md`
- `rg -n "Rgemv_mpfr_C_native_0[1-4]|Rgemv_mpfr_C_native_openmp_0[1-7]" benchmarks/CMakeLists.txt`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Build of changed/new raw C native targets: PASS.
- Serial C native 01 smoke run at `m=13`, `n=17`, `precision=128`: PASS,
  `Result OK`.
- Serial C native 01_FMA smoke run at `m=13`, `n=17`, `precision=128`:
  PASS, `Result OK`.
- Serial C native 04 smoke run at `m=13`, `n=17`, `precision=128`: PASS,
  `Result OK`.
- OpenMP C native 02 smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- OpenMP C native 02_FMA smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- OpenMP C native 04 smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- OpenMP C native 04_FMA smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- README ambiguous mapping scan: PASS, no matches.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- The checked-in MPFR Rgemv repeat-10 result set predates this numbering
  cleanup.  It should be refreshed before using the README tables as current
  performance data.

Post-phase MPFR Rgemv C native OpenMP 05/06/07 counterparts:
DONE

Implemented features:
- Added `Rgemv_mpfr_C_native_openmp_05.cpp` as the raw `mpfr_t`
  counterpart of `Rgemv_mpfr_kernel_openmp_05.cpp`, using precomputed
  `scaled_x[j] = alpha * x[j]` and one reusable `templ` per thread.
- Added `Rgemv_mpfr_C_native_openmp_06.cpp` as the raw `mpfr_t`
  counterpart of `Rgemv_mpfr_kernel_openmp_06.cpp`, using 256-row blocks,
  a column loop, and contiguous row updates inside each block.
- Added `Rgemv_mpfr_C_native_openmp_07.cpp` as the raw `mpfr_t`
  counterpart of `Rgemv_mpfr_kernel_openmp_07.cpp`, using column
  partitioning, `num_threads * m` partial accumulators, and a final
  reduction into `y`.
- Registered `Rgemv_mpfr_C_native_openmp_05`,
  `Rgemv_mpfr_C_native_openmp_06`, and
  `Rgemv_mpfr_C_native_openmp_07` in `benchmarks/CMakeLists.txt`.
- Updated `benchmarks/mpfr/02_Rgemv/README.md` so the C native equivalent
  table no longer says that OpenMP 05/06/07 raw C counterparts are missing.

Tests added:
- No unit tests were added; this phase adds benchmark executables.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/mpfr/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `rg --files benchmarks/mpfr/02_Rgemv | sort`
- `sed -n '1,260p' benchmarks/CMakeLists.txt`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_common.hpp`
- `sed -n '220,520p' benchmarks/mpfr/02_Rgemv/Rgemv_common.hpp`
- `sed -n '300,460p' benchmarks/CMakeLists.txt`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_01.cpp && sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_02.cpp && sed -n '1,240p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_03.cpp`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_05.cpp`
- `sed -n '1,240p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_06.cpp`
- `sed -n '1,280p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07.cpp`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_openmp_05 Rgemv_mpfr_C_native_openmp_06 Rgemv_mpfr_C_native_openmp_07 -j`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_openmp_05 Rgemv_mpfr_C_native_openmp_06 Rgemv_mpfr_C_native_openmp_07 -j`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_05 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_06 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_07 13 17 128`
- `rg -n "none yet|openmp_05|openmp_06|openmp_07|C_native_openmp_03" benchmarks/mpfr/02_Rgemv/README.md`
- `sed -n '80,150p' benchmarks/mpfr/02_Rgemv/README.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`
- `git diff --check`
- `git status --short --untracked-files=all`
- `git diff --stat`

Pass/fail result:
- Initial build attempt before reconfiguring CMake: FAIL, build tree did not
  yet know the new targets.
- CMake configure refresh: PASS.
- Build of `Rgemv_mpfr_C_native_openmp_05`: PASS.
- Build of `Rgemv_mpfr_C_native_openmp_06`: PASS.
- Build of `Rgemv_mpfr_C_native_openmp_07`: PASS.
- OpenMP 05 smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- OpenMP 06 smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- OpenMP 07 smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- The current checked-in MPFR Rgemv repeat-10 result set predates these new
  raw C native OpenMP 05/06/07 targets, so they are documented as
  source-equivalence targets for the next benchmark refresh but are not in the
  current result tables.

Post-phase MPFR Rgemv README full rewrite:
DONE

Implemented features:
- Rewrote `benchmarks/mpfr/02_Rgemv/README.md` from scratch instead of
  incrementally appending to the previous report.
- Removed stale wording about the older 28-variant result set and made the
  current 31-variant repeat-10 result set the only recorded result.
- Rebuilt the report structure around purpose, build commands, benchmark
  parameters, variant shapes, C native equivalent kernels, recorded run data,
  bandwidth estimates, serial/OpenMP result tables, disassembly notes, and
  lessons learned.
- Documented OpenMP `05`, `06`, and `07` as first-class recorded variants.

Tests added:
- No unit tests were added; this phase rewrites benchmark documentation.

Tests updated:
- `benchmarks/mpfr/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `wc -l benchmarks/mpfr/02_Rgemv/README.md`
- `sed -n '1,260p' benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_232612/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `sed -n '430,560p' AGENTS.md`
- `apply_patch` attempted full delete/add replacement of
  `benchmarks/mpfr/02_Rgemv/README.md`, but file deletion failed.
- `ls -l benchmarks/mpfr/02_Rgemv/README.md && git status --short benchmarks/mpfr/02_Rgemv/README.md`
- `python3` whole-file README replacement script after `apply_patch` delete
  failed.
- `rg -n '20260517_222713|All 28|280 successful|not included|later wrapper|maximum-MFLOPS|best OpenMP average is' benchmarks/mpfr/02_Rgemv/README.md`
- `sed -n '1,80p' benchmarks/mpfr/02_Rgemv/README.md`
- `sed -n '300,430p' benchmarks/mpfr/02_Rgemv/README.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- README full rewrite: PASS.
- Stale wording scan: PASS, no matches.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- Raw C native equivalents for wrapper OpenMP `05`, `06`, and `07` still have
  not been added.

Post-phase MPFR Rgemv OpenMP 05/06/07 kernels:
DONE

Implemented features:
- Added `Rgemv_mpfr_kernel_openmp_05.cpp`, which precomputes
  `scaled_x[j] = alpha * x[j]` and then performs a row-owned update with a
  per-thread reusable product object.
- Added `Rgemv_mpfr_kernel_openmp_06.cpp`, which uses 256-row blocks, a
  column loop, and a contiguous row loop inside each block.
- Added `Rgemv_mpfr_kernel_openmp_07.cpp`, which partitions columns across
  threads, accumulates into thread-local partial `y` vectors, and reduces the
  partials into `y`.
- Registered `Rgemv_mpfr_kernel_openmp_05_mkII`,
  `Rgemv_mpfr_kernel_openmp_06_mkII`, and
  `Rgemv_mpfr_kernel_openmp_07_mkII` in `benchmarks/CMakeLists.txt`.
- Updated the MPFR Rgemv README variant table to document OpenMP variants
  `05`, `06`, and `07`.

Tests added:
- No unit tests were added; this phase adds benchmark executables.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/mpfr/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_05.cpp`
- `sed -n '1,240p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_06.cpp`
- `sed -n '1,280p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_07.cpp`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_04.cpp`
- `sed -n '1,140p' benchmarks/CMakeLists.txt`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_03.cpp`
- `sed -n '80,125p' benchmarks/mpfr/02_Rgemv/README.md`
- `rg -n "set_prec|precision\\(\\)|mpfr_class\\(0\\.0, precision\\)|default_precision_bits" include/gmpfrxx_mkII/detail/mpfr_impl.hpp benchmarks/mpfr -g '*.hpp' -g '*.cpp'`
- `sed -n '180,280p' benchmarks/mpfr/02_Rgemv/Rgemv_common.hpp`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rgemv_mpfr_kernel_openmp_05_mkII Rgemv_mpfr_kernel_openmp_06_mkII Rgemv_mpfr_kernel_openmp_07_mkII -j`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_05_mkII 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_06_mkII 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07_mkII 13 17 128`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Build of `Rgemv_mpfr_kernel_openmp_05_mkII`: PASS.
- Build of `Rgemv_mpfr_kernel_openmp_06_mkII`: PASS.
- Build of `Rgemv_mpfr_kernel_openmp_07_mkII`: PASS.
- OpenMP 05 smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- OpenMP 06 smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- OpenMP 07 smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- The checked-in MPFR Rgemv repeat-10 result set predates these new wrapper
  OpenMP targets, so the README documents them as available variants but does
  not include them in the recorded result tables.
- Raw C native equivalent kernels for OpenMP `05`, `06`, and `07` have not
  been added yet.

Post-phase MPFR Rgemv 28-variant repeat benchmark refresh:
DONE

Implemented features:
- Deleted the old tracked MPFR Rgemv repeat-10 result directory
  `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_212035`.
- Rebuilt and reran the complete current MPFR Rgemv benchmark matrix,
  including the new raw C `03` and `openmp_03` FMMA targets.
- Wrote the fresh repeat-10 result set under
  `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_222713`.
- Generated raw CSV, summary CSV, and serial/OpenMP plots with average bars
  and min/max ranges.
- Updated `benchmarks/mpfr/02_Rgemv/README.md` with the new 28-variant result
  set, bandwidth estimates, sorted tables, and revised interpretation.

Tests added:
- No unit tests were added; this phase refreshes benchmark results.

Tests updated:
- `benchmarks/mpfr/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `find benchmarks/mpfr/02_Rgemv -maxdepth 3 -type f -o -type d | sort`
- `rg -n "rgemv_mpfr|Rgemv_mpfr|repeat10|summary_rgemv|openmp_mflops|serial" benchmarks/mpfr benchmarks/common benchmarks/CMakeLists.txt -g '*.sh' -g '*.py' -g '*.md' -g 'CMakeLists.txt'`
- `git ls-files benchmarks/mpfr/02_Rgemv/results_raw`
- `git rm -r benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_212035`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_01 Rgemv_mpfr_C_native_01_FMA Rgemv_mpfr_C_native_02 Rgemv_mpfr_C_native_02_FMA Rgemv_mpfr_C_native_03 Rgemv_mpfr_C_native_openmp_01 Rgemv_mpfr_C_native_openmp_01_FMA Rgemv_mpfr_C_native_openmp_02 Rgemv_mpfr_C_native_openmp_02_FMA Rgemv_mpfr_C_native_openmp_03 Rgemv_mpfr_kernel_01_mkII Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_02_mkII Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_03_mkII Rgemv_mpfr_kernel_03_mkII_FMA Rgemv_mpfr_kernel_04_mkII Rgemv_mpfr_kernel_openmp_01_mkII Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_openmp_02_mkII Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_openmp_03_mkII Rgemv_mpfr_kernel_openmp_03_mkII_FMA Rgemv_mpfr_kernel_openmp_04_mkII -j`
- `bash -lc` repeat-10 MPFR Rgemv benchmark runner for 28 variants at
  `m=4000`, `n=4000`, `precision=512`, with
  `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, and `OMP_PROC_BIND=spread`.
- `python3` log parser and plot generator for
  `raw_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`,
  `summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`,
  `rgemv_mpfr_m4000_n4000_p512_repeat10_serial.png`, and
  `rgemv_mpfr_m4000_n4000_p512_repeat10_openmp.png`.
- `python3` summary helper for README tables and bandwidth estimates.
- `sed -n '120,430p' benchmarks/mpfr/02_Rgemv/README.md`
- `rg -n '20260517_212035|All 26|260 successful|277\.394|23\.594|best OpenMP wrapper' benchmarks/mpfr/02_Rgemv/README.md`
- `git diff --check`
- `ls benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_222713`
- `sed -n '1,40p' benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_222713/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Build of the full MPFR Rgemv target matrix: PASS.
- Fresh repeat-10 benchmark run: PASS, 280/280 runs reported `Result OK`.
- Best serial average: `Rgemv_mpfr_C_native_02_FMA`, 23.526 MFLOPS.
- Best wrapper serial average: `Rgemv_mpfr_kernel_04_mkII`, 20.498 MFLOPS.
- Best OpenMP average: `Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA`,
  275.267 MFLOPS.
- Best raw C OpenMP average: `Rgemv_mpfr_C_native_openmp_01`,
  266.518 MFLOPS.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- `kernel_openmp_02_*` still has the same row-dot source shape as
  `kernel_openmp_01_*`; the README documents this explicitly.
- The benchmark harness still runs a reference Rgemv after each timed kernel,
  so wall-clock time is much larger than the timed `Elapsed time` for fast
  OpenMP variants.

Post-phase MPFR Rgemv C native 03 FMMA kernels:
DONE

Implemented features:
- Added `Rgemv_mpfr_C_native_03.cpp` as the serial raw C row-dot counterpart
  to `Rgemv_mpfr_kernel_03_mkII_FMA`.
- Added `Rgemv_mpfr_C_native_openmp_03.cpp` as the OpenMP raw C row-dot
  counterpart to `Rgemv_mpfr_kernel_openmp_03_mkII_FMA`.
- Both new kernels accumulate `temp += A[i + j*lda] * x[j]` with `mpfr_fma`
  and finish each row with `mpfr_fmma(y[i], alpha, temp, beta, y[i], rnd)`.
- Registered the new serial and OpenMP targets in `benchmarks/CMakeLists.txt`.
- Updated the MPFR Rgemv README to document the new C native equivalent
  kernels and FMMA hotpath.

Tests added:
- No unit tests were added; this phase adds benchmark executables.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/mpfr/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '280,360p' benchmarks/CMakeLists.txt`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_common.hpp`
- `sed -n '1,200p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_01_FMA.cpp`
- `sed -n '1,200p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01_FMA.cpp`
- `sed -n '80,180p' benchmarks/mpfr/02_Rgemv/README.md`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_03 Rgemv_mpfr_C_native_openmp_03 -j`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_03 13 17 128`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_03 13 17 128`
- `objdump -d build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_03 | grep -E 'mpfr_(fma|fmma|set_d|get_default_rounding_mode|init2|clear)@plt' | head -n 40`
- `objdump -d build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_03 | grep -E 'mpfr_(fma|fmma|set_d|get_default_rounding_mode|init2|clear)@plt|GOMP_barrier@plt' | head -n 60`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Build of `Rgemv_mpfr_C_native_03`: PASS.
- Build of `Rgemv_mpfr_C_native_openmp_03`: PASS.
- Serial smoke run at `m=13`, `n=17`, `precision=128`: PASS,
  `Result OK`.
- OpenMP smoke run at `m=13`, `n=17`, `precision=128`,
  `OMP_NUM_THREADS=2`: PASS, `Result OK`.
- Disassembly check: PASS, both new targets contain `mpfr_fma` and
  `mpfr_fmma` calls in the relevant executable.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- The committed repeat-10 MPFR Rgemv result set predates these new C native
  `03` targets, so the README records them as available targets but does not
  include them in the existing 26-variant summary tables.

Post-phase MPFR Rgemv README report:
DONE

Implemented features:
- Added `benchmarks/mpfr/02_Rgemv/README.md` as a reproducible benchmark
  report for the current MPFR Rgemv repeat-10 result set.
- Documented build commands, benchmark parameters, variant shapes, C native
  equivalent kernels, recorded run files, resource/bandwidth estimates,
  serial/OpenMP result tables, representative hotpath disassembly, and lessons
  learned.
- Included folded serial/OpenMP tables sorted by maximum and average MFLOPS.

Tests added:
- No unit tests were added; this phase documents benchmark results.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '430,560p' AGENTS.md`
- `sed -n '1,260p' benchmarks/gmp/02_Rgemv/README.md`
- `ls benchmarks/mpfr/02_Rgemv`
- `sed -n '1,80p' benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_212035/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `sed -n '1,180p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_01.cpp`
- `sed -n '1,180p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_02.cpp`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_03.cpp`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_04.cpp`
- `sed -n '1,180p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01.cpp`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_02.cpp`
- `sed -n '1,200p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_02_FMA.cpp`
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_04.cpp`
- `sed -n '1,180p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_01.cpp`
- `sed -n '1,180p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_02.cpp`
- `sed -n '1,200p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_03.cpp`
- `ls benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_212035`
- `python3` table generator for sorted Markdown result tables and selected
  bandwidth estimates from the committed summary CSV.
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Documentation update: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- `kernel_openmp_02_*` currently has the same row-dot source shape as
  `kernel_openmp_01_*`; the README documents this explicitly.
- No new benchmark execution was performed in this phase.

Post-phase MPFR Rgemv repeat benchmark refresh:
DONE

Implemented features:
- Removed stale tracked legacy MPFR benchmark result directories under
  `benchmarks/mpfr/results_raw`, leaving the `.gitkeep` anchor.
- Reran the MPFR Rgemv benchmark matrix with the current raw C native,
  FMA, wrapper, fixed-precision, explicit-context, and OpenMP targets.
- Wrote the fresh repeat-10 Rgemv result set under
  `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_212035`.
- Generated raw CSV, summary CSV, and serial/OpenMP bar plots with min/max
  error bars from the fresh log.

Tests added:
- No unit tests were added; this phase refreshes benchmark results.

Tests updated:
- `STATUS.md`

Exact commands run:
- `find benchmarks/mpfr/02_Rgemv -maxdepth 3 -type f -o -type d | sort`
- `rg -n "Rgemv_mpfr|02_Rgemv|repeat|summary|mflops|results_raw" benchmarks/mpfr benchmarks/common benchmarks/CMakeLists.txt -g '*.sh' -g '*.py' -g 'README.md' -g 'CMakeLists.txt'`
- `git ls-files benchmarks/mpfr/results_raw benchmarks/mpfr/02_Rgemv/results_raw`
- `git rm -r benchmarks/mpfr/results_raw/clean_release_20260509_smoke benchmarks/mpfr/results_raw/raxpy_port_smoke_20260509 benchmarks/mpfr/results_raw/rdot_n1e7_20260509 benchmarks/mpfr/results_raw/rgemm_port_smoke_20260510 benchmarks/mpfr/results_raw/rgemv_port_smoke_20260510 benchmarks/mpfr/results_raw/scaled_full_20260510`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_01 Rgemv_mpfr_C_native_01_FMA Rgemv_mpfr_C_native_02 Rgemv_mpfr_C_native_02_FMA Rgemv_mpfr_C_native_openmp_01 Rgemv_mpfr_C_native_openmp_01_FMA Rgemv_mpfr_C_native_openmp_02 Rgemv_mpfr_C_native_openmp_02_FMA Rgemv_mpfr_kernel_01_mkII Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_02_mkII Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_03_mkII Rgemv_mpfr_kernel_03_mkII_FMA Rgemv_mpfr_kernel_04_mkII Rgemv_mpfr_kernel_openmp_01_mkII Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_openmp_02_mkII Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_openmp_03_mkII Rgemv_mpfr_kernel_openmp_03_mkII_FMA Rgemv_mpfr_kernel_openmp_04_mkII -j`
- `bash -lc` repeat-10 MPFR Rgemv benchmark runner for 26 variants at
  `m=4000`, `n=4000`, `precision=512`.
- `python3` log parser to generate
  `raw_rgemv_mpfr_m4000_n4000_p512_repeat10.csv` and
  `summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`.
- `python3` plot generator to create
  `rgemv_mpfr_m4000_n4000_p512_repeat10_serial.png` and
  `rgemv_mpfr_m4000_n4000_p512_repeat10_openmp.png`.
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Build of the full MPFR Rgemv target matrix: PASS.
- Fresh repeat-10 benchmark run: PASS, 260/260 runs reported `Result OK`.
- Best serial average: `Rgemv_mpfr_C_native_02_FMA`, 23.594 MFLOPS.
- Best wrapper serial average: `Rgemv_mpfr_kernel_04_mkII`, 20.447 MFLOPS.
- Best OpenMP average: `Rgemv_mpfr_kernel_openmp_03_mkII_FMA`,
  277.394 MFLOPS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- The benchmark harness still runs a reference Rgemv after each timed kernel,
  so `WALL_SECONDS` is much larger than the timed `Elapsed time` for fast
  OpenMP variants.
- This phase refreshed data only; README analysis is not yet updated for the
  new result set.

Post-phase MPFR Rgemv explicit-context kernels:
DONE

Implemented features:
- Added `Rgemv_mpfr_kernel_03.cpp` as the explicit-context counterpart to the
  existing row-dot expression kernel.
- Added `Rgemv_mpfr_kernel_04.cpp` as the explicit-context counterpart to the
  reusable `temp`/`templ` kernel.
- Added OpenMP counterparts `Rgemv_mpfr_kernel_openmp_03.cpp` and
  `Rgemv_mpfr_kernel_openmp_04.cpp`.
- Registered `03` and `openmp_03` as `mkII` and `mkII_FMA` targets, and
  registered `04` and `openmp_04` as `mkII` targets.

Tests added:
- No unit tests were added; this phase adds benchmark executables.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `git status --short --untracked-files=all`
- `find benchmarks/mpfr/02_Rgemv -maxdepth 1 -type f | sort`
- `sed -n '303,330p' benchmarks/CMakeLists.txt`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rgemv_mpfr_kernel_03_mkII Rgemv_mpfr_kernel_03_mkII_FMA Rgemv_mpfr_kernel_04_mkII Rgemv_mpfr_kernel_openmp_03_mkII Rgemv_mpfr_kernel_openmp_03_mkII_FMA Rgemv_mpfr_kernel_openmp_04_mkII -j`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_03_mkII 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_03_mkII_FMA 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_04_mkII 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_03_mkII 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_03_mkII_FMA 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_04_mkII 13 17 128`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- New explicit-context target build: PASS.
- Small MPFR Rgemv smoke runs: PASS, all six new wrapper executables reported
  `Result OK`.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- `kernel_openmp_04` keeps the row-owned OpenMP update pattern to avoid races
  on `y[i]`, so it recomputes the column-scale temporary inside each row
  worker.  This is intentionally comparable to `C_native_openmp_02`, not to
  the serial column-hoisted `kernel_04` source shape.

Post-phase MPFR Rgemv C native kernel matrix:
DONE

Implemented features:
- Added the missing MPFR Rgemv raw C native reusable-temporary kernel
  `Rgemv_mpfr_C_native_02`.
- Added FMA raw C native variants for the existing `01` shape and the new
  `02` shape.
- Added OpenMP raw C native variants for `02`, `01_FMA`, and `02_FMA`.
- Registered the new MPFR Rgemv native benchmark targets in
  `benchmarks/CMakeLists.txt`.

Tests added:
- No unit tests were added; this phase adds benchmark executables.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01.cpp`
- `sed -n '1,260p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_01.cpp`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_02.cpp`
- `sed -n '1,260p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_02.cpp`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_01_FMA Rgemv_mpfr_C_native_02 Rgemv_mpfr_C_native_02_FMA Rgemv_mpfr_C_native_openmp_01_FMA Rgemv_mpfr_C_native_openmp_02 Rgemv_mpfr_C_native_openmp_02_FMA -j`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01_FMA 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_02 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_02_FMA 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_01_FMA 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_02 13 17 128`
- `build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_02_FMA 13 17 128`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- New target build: PASS.
- Small MPFR Rgemv smoke runs: PASS, all six new executables reported
  `Result OK`.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- The OpenMP raw C `02` shape computes the column-scale temporary inside each
  row-owned worker loop, matching the existing GMP OpenMP `02` ownership
  pattern but not the serial `02` column-hoisted reuse pattern.

Post-phase GMP Rgemv README refresh:
DONE

Implemented features:
- Re-read `AGENTS.md` benchmark README/reporting conventions before updating
  the GMP Rgemv report.
- Rewrote `benchmarks/gmp/02_Rgemv/README.md` around the current repeat-10
  `20260516_184101` result set only.
- Added benchmark parameters, variant-shape tables, `C Native Equivalent
  Kernels`, recorded-run details, logical bandwidth estimates, serial/OpenMP
  interpretation tables, folded tables sorted by max and average MFLOPS,
  hotpath disassembly notes, and lessons learned.
- Removed the stale tracked single-run result directory
  `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209`
  because the refreshed README no longer references it.

Tests added:
- No test source files were added; this is documentation and benchmark-result
  cleanup.

Tests updated:
- `benchmarks/gmp/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '444,1120p' AGENTS.md`
- `find benchmarks/gmp/02_Rgemv -maxdepth 3 -type f -o -type d | sort`
- `sed -n '1,360p' benchmarks/gmp/02_Rgemv/README.md`
- `git rm -r benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209`
- `python3` summary-table generator over
  `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/summary_rgemv_gmp_m4000_n4000_p512_repeat10.csv`
- `sed -n` over representative Rgemv C native and wrapper kernel sources to
  verify timed source shapes.
- `rg -n "20260516_130209|20260430|Linux_Ryzen_3970X_32-Core|Superseded|\\.\\./results_raw" benchmarks/gmp/02_Rgemv/README.md benchmarks/gmp/02_Rgemv/results_raw --glob '!*.png' --glob '!*.pdf'`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Stale-reference scan: PASS, no stale references remain in the refreshed
  README or retained non-image result files.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- The README notes that the committed repeat plots were generated during the
  recorded benchmark workflow; a standalone Rgemv repeat-summary plotting
  script is not currently checked in.

Post-phase benchmark README equivalence rule:
DONE

Implemented features:
- Updated `AGENTS.md` benchmark README conventions to require a
  `C Native Equivalent Kernels` section.
- Specified that the mapping must be based on timed hot-loop source shape and
  generated code, not only on matching numeric suffixes.
- Required benchmark READMEs to explicitly document when a C++ expression
  template kernel has no exact raw C native equivalent.

Tests added:
- No test source files were added; this is documentation-only.

Tests updated:
- `AGENTS.md`
- `STATUS.md`

Exact commands run:
- `rg -n "README|benchmark|Bench|documentation|Status Discipline|Implementation Rules" AGENTS.md`
- `sed -n '1,260p' AGENTS.md`
- `sed -n '444,760p' AGENTS.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- None for this documentation update.

Post-phase Rdot C native equivalence documentation:
DONE

Implemented features:
- Added `C Native Equivalent Kernels` sections to both GMP and MPFR Rdot
  READMEs.
- Documented which raw C kernels are source-shape equivalents for each C++
  wrapper kernel family.
- Clarified that GMP/MPFR `C_native_01` and `C_native_02` are both closest to
  loop-local product materialization, while optimized reusable-product paths
  should be compared with `C_native_03`.
- Clarified that MPFR FMA and explicit-context kernels have separate C native
  comparison points because rounding delivery and `mpfr_fma` change the hot
  loop shape.
- Reverted the partial GMP Rgemv README/result deletion left by the
  interrupted previous task before making the Rdot documentation changes.

Tests added:
- No test source files were added; this is documentation-only.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `git restore --staged --worktree benchmarks/gmp/02_Rgemv/README.md benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209`
- `sed -n '1,220p' benchmarks/gmp/00_Rdot/README.md`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/README.md`
- `rg -n` over the GMP and MPFR Rdot C native source files to verify timed
  hot-loop shapes.
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.

Known issues:
- None for this documentation update.

Post-phase MPFR Raxpy repeat-10 regeneration:
DONE

Implemented features:
- Re-read the project instructions and used the MPFR Rdot README structure as
  the documentation template for MPFR Raxpy.
- Regenerated the MPFR Raxpy repeat-10 benchmark data with the new target
  matrix after aligning the C native, wrapper, OpenMP, FMA, stable-rounding,
  fixed-precision fastpath, and explicit-context targets.
- Removed the tracked stale MPFR Raxpy `20260515` result directories from the
  current result set.
- Rewrote `benchmarks/mpfr/01_Raxpy/README.md` from the current
  `20260517_144507` data only, including inline plots, headline results,
  serial/OpenMP interpretation tables, folded tables sorted by max and average
  MFLOPS, memory-bandwidth estimates, hotpath disassembly, GMP comparison, and
  lessons learned.

Tests added:
- No CTest test source files were added; this phase updates benchmarks,
  generated benchmark data, and documentation.

Tests updated:
- `benchmarks/mpfr/01_Raxpy/README.md`
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260517_144507/`
- `STATUS.md`

Exact commands run:
- `git rm -r benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_20260515_153432 benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821`
- Custom Raxpy-only repeat loop over all 43 MPFR Raxpy variants with
  `N=10000000`, `precision=512`, `repeat=10`, `OMP_NUM_THREADS=32`,
  `OMP_PLACES=cores`, and `OMP_PROC_BIND=spread`, writing
  `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_mpfr_n10000000_p512_repeat10_20260517_144507/benchmark_raxpy_mpfr_n10000000_p512_repeat10.log`.
- `python3` parser and plot generator writing
  `raw_raxpy_mpfr_n10000000_p512_repeat10.csv`,
  `summary_raxpy_mpfr_n10000000_p512_repeat10.csv`,
  `raxpy_mpfr_n10000000_p512_repeat10_serial.png`, and
  `raxpy_mpfr_n10000000_p512_repeat10_openmp.png`.
- `objdump -d -C` and `nm -C` on representative C native, wrapper FMA,
  context, and OpenMP MPFR Raxpy targets to capture hotpath disassembly.
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Repeat-10 benchmark: PASS, all 430 timed runs reported `OK`.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS, 158/158
  tests passed.
- Best serial wrapper average: `kernel_01_mkII_STABLE_ROUNDING_FMA`,
  `22.860 MFLOPS`.
- Best OpenMP wrapper average and maximum overall:
  `kernel_openmp_01_mkII_STABLE_ROUNDING_FMA`, `416.851 average`,
  `423.582 max MFLOPS`.

Known issues:
- One unrelated untracked old MPFR Raxpy result directory remains in the
  worktree and was not modified:
  `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_200214/`.
- The documented bandwidth numbers are logical estimates, not hardware-counter
  measurements.

Post-phase MPFR Rdot README cleanup:
DONE

Implemented features:
- Re-read `AGENTS.md` before updating the benchmark documentation.
- Rewrote `benchmarks/mpfr/00_Rdot/README.md` so it only documents the
  current MPFR Rdot `01..06` benchmark layout and the repeat-10
  `20260517_090826` result set.
- Removed stale README references to the old 20260514 runs, the removed
  `kernel_07` experiments, and allocation-counter based analysis.
- Deleted stale MPFR Rdot result directories, preserving only the current
  `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_090826`
  recorded run.
- Kept the analysis focused on hotpath disassembly, current repeat-10
  MFLOPS, memory-bandwidth estimates, and the MPFR-vs-GMP rounding-delivery
  difference.

Tests added:
- No test source files were added; this is documentation and benchmark-result
  cleanup.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,220p' AGENTS.md`
- `find benchmarks/mpfr/00_Rdot -maxdepth 3 -type d -o -type f | sort`
- `find benchmarks/mpfr/results_raw benchmarks/mpfr/00_Rdot/results_raw -maxdepth 1 -type d | sort`
- `rm -rf benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260516_230953 benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_081907 benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_20260514 benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514 benchmarks/mpfr/results_raw/rdot_n1e7_512_repeat10_20260514 benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514`
- `rg -n "20260514|20260516_230953|20260517_081907|kernel_07|01-07|Timed allocs|BENCH_ALLOC|allocation count|rdot_n1e" benchmarks/mpfr/00_Rdot/README.md benchmarks/mpfr/00_Rdot/results_raw benchmarks/mpfr/results_raw --glob '!*.png'`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Stale-reference scan: PASS, no matches remained in the MPFR Rdot README or
  retained MPFR Rdot result directories.
- `git diff --check`: PASS after removing trailing whitespace from a blank
  line in `AGENTS.md`.
- `ctest --test-dir build --output-on-failure`: PASS, 157/157 tests passed.

Known issues:
- The README rewrite is documentation-only and does not change benchmark
  executables.

Post-phase MPFR Rdot GMP comparison documentation:
DONE

Implemented features:
- Added a `Comparison with GMP` section to the MPFR Rdot README.
- Documented that GMP `mpf` does not carry an explicit rounding-mode argument
  in hot arithmetic calls, while MPFR requires `mpfr_rnd_t` for every
  arithmetic operation.
- Clarified that C native MPFR can cache `rnd` before the loop, while generic
  wrapper expressions may still pay rounding context or TLS-load cost even
  after per-element product allocation has been removed.
- Recorded the practical distinction: GMP wrapper optimization mostly reduces
  to avoiding temporary materialization, while MPFR also needs rounding
  delivery to be hoisted, stabilized, or specialized.

Tests added:
- No tests were added; this is documentation-only.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `git diff --check`

Pass/fail result:
- `git diff --check`: PASS.

Known issues:
- No code changes were made in this phase.

Post-phase MPFR Rdot repeat-10 benchmark:
DONE

Implemented features:
- Ran the split MPFR Rdot benchmark set with `N=10000000`, 512-bit precision,
  repeat count 10, and `OMP_NUM_THREADS=32`.
- Generated raw log, serial/OpenMP plots, and a summary CSV under
  `benchmarks/mpfr/results_raw/rdot_n1e7_512_repeat10_20260514/`.
- Updated `benchmarks/mpfr/00_Rdot/README.md` with inline plots, key serial
  and OpenMP tables, and interpretation of stable rounding, FMA, and
  per-iteration temporary allocation.
- Added a `Lessons Learned` section documenting the practical interpretation
  of MPFR rounding lookup, FMA fusion, reusable temporaries, OpenMP variance,
  and the role of the `05`/`06` raw MPFR control kernels.

Tests added:
- No CTest test files were added.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `OMP_NUM_THREADS=32` repeat-10 MPFR Rdot loop over all 52 native, serial,
  and OpenMP variants, writing
  `benchmarks/mpfr/results_raw/rdot_n1e7_512_repeat10_20260514/benchmark_rdot_n1e7_512_repeat10.log`
- `python3 benchmarks/common/plot.py benchmarks/mpfr/results_raw/rdot_n1e7_512_repeat10_20260514/benchmark_rdot_n1e7_512_repeat10.log --output-dir benchmarks/mpfr/results_raw/rdot_n1e7_512_repeat10_20260514 --backend MPFR --kernels Rdot`
- `python3` log summarizer writing
  `benchmarks/mpfr/results_raw/rdot_n1e7_512_repeat10_20260514/summary_rdot_n1e7_512_repeat10.csv`
- `git diff --check`

Pass/fail result:
- Repeat-10 benchmark: PASS, all 52 variants reported `DIFF OK` in all 10
  runs.
- Serial best by max MFLOPS: `kernel_06_mkII_STABLE_ROUNDING_FMA`,
  `23.815 MFLOPS`; serial best wrapper by average:
  `kernel_03_mkII_STABLE_ROUNDING`, `23.308 MFLOPS`.
- OpenMP best by max and average MFLOPS: `C_native_openmp_01_FMA`,
  `593.061 max`, `538.348 average`.
- OpenMP best wrapper by average MFLOPS:
  `kernel_openmp_03_mkII_STABLE_ROUNDING`, `509.774 average`.

Known issues:
- OpenMP timed-loop MFLOPS has high run-to-run variance; the README reports
  both max and average.
- Full executable wall time is dominated by vector initialization and the
  serial correctness reference; this run compares timed `_Rdot()` loops.

Post-phase split MPFR Rdot benchmark kernels:
DONE

Implemented features:
- Reworked MPFR Rdot benchmark coverage to mirror the GMP Rdot layout with
  standalone kernel translation units instead of a shared kernel-common header.
- Added MPFR Rdot serial `kernel_06` and OpenMP `kernel_openmp_03` through
  `kernel_openmp_06`.
- Registered MPFR Rdot serial and OpenMP `01..06` variants in CMake with
  `mkII`, `mkII_STABLE_ROUNDING`, `mkII_FMA`, and
  `mkII_STABLE_ROUNDING_FMA` builds.
- Extended the MPFR benchmark runner so the Rdot sweep includes C native,
  C native FMA, C native OpenMP, serial wrapper `01..06`, and OpenMP wrapper
  `01..06`.
- Updated existing MPFR Rdot reference and wrapper kernels so accumulators and
  reusable temporaries use the input precision instead of relying on default
  construction.
- Added `benchmarks/mpfr/00_Rdot/README.md` documenting the split kernel
  shapes, MPFR rounding/FMA interpretation, and smoke-test status.

Tests added:
- No CTest test files were added.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `benchmarks/mpfr/00_Rdot/Rdot.hpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_02.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_04.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_06.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_02.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_03.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_04.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_05.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_06.cpp`
- `benchmarks/mpfr/00_Rdot/README.md`
- `benchmarks/mpfr/README.md`
- `STATUS.md`

Exact commands run:
- `cmake --build build_bench_release -j --target Rdot_mpfr_C_native_01 Rdot_mpfr_C_native_01_FMA Rdot_mpfr_C_native_openmp_01 Rdot_mpfr_C_native_openmp_01_FMA Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_01_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_01_mkII_FMA Rdot_mpfr_kernel_01_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_02_mkII Rdot_mpfr_kernel_02_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_02_mkII_FMA Rdot_mpfr_kernel_02_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_03_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_03_mkII_FMA Rdot_mpfr_kernel_03_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_04_mkII Rdot_mpfr_kernel_04_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_04_mkII_FMA Rdot_mpfr_kernel_04_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_05_mkII Rdot_mpfr_kernel_05_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_05_mkII_FMA Rdot_mpfr_kernel_05_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_06_mkII Rdot_mpfr_kernel_06_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_06_mkII_FMA Rdot_mpfr_kernel_06_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_openmp_01_mkII Rdot_mpfr_kernel_openmp_01_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_openmp_01_mkII_FMA Rdot_mpfr_kernel_openmp_01_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_openmp_02_mkII Rdot_mpfr_kernel_openmp_02_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_openmp_02_mkII_FMA Rdot_mpfr_kernel_openmp_02_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_openmp_03_mkII Rdot_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_openmp_03_mkII_FMA Rdot_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_openmp_04_mkII Rdot_mpfr_kernel_openmp_04_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_openmp_04_mkII_FMA Rdot_mpfr_kernel_openmp_04_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_openmp_05_mkII Rdot_mpfr_kernel_openmp_05_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_openmp_05_mkII_FMA Rdot_mpfr_kernel_openmp_05_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_openmp_06_mkII Rdot_mpfr_kernel_openmp_06_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_openmp_06_mkII_FMA Rdot_mpfr_kernel_openmp_06_mkII_STABLE_ROUNDING_FMA`
- `build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01 1000 512`
- `build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03_mkII 1000 512`
- `build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_06_mkII_FMA 1000 512`
- `OMP_NUM_THREADS=4 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_06_mkII_STABLE_ROUNDING_FMA 1000 512`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `objdump -d -C build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FMA | rg -n "mpfr_fma|mpfr_mul|mpfr_add"`
- `objdump -d -C build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII | rg -n "mpfr_fma|mpfr_mul|mpfr_add"`
- `nm -C build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FMA | rg " _Rdot| mpfr_fma| mpfr_mul| mpfr_add"`
- `nm -C build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII | rg " _Rdot| mpfr_fma| mpfr_mul| mpfr_add"`
- `objdump -d -C --start-address=0x3990 --stop-address=0x3b20 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FMA`
- `objdump -d -C --start-address=0x3970 --stop-address=0x3b20 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII`

Pass/fail result:
- MPFR Rdot target build: PASS, all native, serial `01..06`, and OpenMP
  `01..06` variants built.
- Representative Rdot smoke runs: PASS, all listed executions reported
  `DIFF OK`.
- `git diff --check`: PASS.
- Full CTest: PASS, 156/156.

Known issues:
- A repeat-10 long MPFR Rdot run has not been recorded yet for the new split
  kernel set, so the README documents expected hotpath interpretation rather
  than a stable performance ranking.
- MPFR FMA variants are not numerically identical to `mpfr_mul` plus
  `mpfr_add`, because `mpfr_fma` performs one fused operation with one final
  rounding.

Post-phase MPFR stable rounding fastpath:
DONE

Implemented features:
- Added `GMPFRXX_MKII_ASSUME_STABLE_MPFR_ROUNDING_MODE` and
  `build_options::assume_stable_mpfr_rounding_mode`.
- In stable-rounding builds, MPFR wrapper arithmetic reads a thread-local
  cached rounding value instead of calling `mpfr_get_default_rounding_mode()`
  for every `current_eval_context()` request.
- The cached value is constant-initialized to `MPFR_RNDN` and refreshed when
  wrapper environment defaults are applied or
  `mpfrxx::set_default_rounding_mode(...)` is called.
- Added MPFR Rdot benchmark variants
  `*_mkII_STABLE_ROUNDING` and `*_mkII_STABLE_ROUNDING_FMA`.
- Added `mpfrxx::rounding_mode_scope` for scoped per-thread MPFR default
  rounding. The scope also updates/restores the stable-rounding cache so a
  numeric kernel can pin rounding at function entry.
- Documented the fastpath contract in `SPECIFICATIONS.md` and
  `benchmarks/README.md`.

Tests added:
- `tests/test_mpfr_rounding_scope.cpp`

Tests updated:
- `include/gmpfrxx_mkII/detail/config.hpp`
- `include/gmpfrxx_mkII/detail/environment.hpp`
- `include/gmpfrxx_mkII/detail/eval_context.hpp`
- `tests/CMakeLists.txt`
- `benchmarks/CMakeLists.txt`
- `SPECIFICATIONS.md`
- `benchmarks/README.md`

Exact commands run:
- `cmake -S . -B build-release-nocount -DCMAKE_BUILD_TYPE=Release -DGMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS=OFF -DGMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS=OFF`
- `cmake --build build-release-nocount -j --target Rdot_mpfr_kernel_01_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_02_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_03_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_04_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_01_mkII_STABLE_ROUNDING_FMA test_mpfr_compound_assign test_mpfr_numeric_equivalence`
- `cmake --build build-release-nocount -j --target Rdot_mpfr_kernel_03_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_04_mkII_STABLE_ROUNDING test_mpfr_compound_assign test_mpfr_numeric_equivalence test_mpfr_rounding_scope test_mpfr_rounding_scope_stable`
- `ctest --test-dir build-release-nocount -R "test_mpfr_compound_assign|test_mpfr_numeric_equivalence|test_mpfr_rounding_scope" --output-on-failure`
- `for k in 01 02 03 04; do for s in mkII mkII_STABLE_ROUNDING; do exe=build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_${k}_${s}; echo "== kernel_${k}_${s}"; "$exe" 1000000 512 | rg "MFLOPS|DIFF"; done; done`
- `nm -C build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03_mkII_STABLE_ROUNDING | rg ' T _Rdot'`
- `objdump -Cd build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03_mkII_STABLE_ROUNDING --start-address=0x3940 --stop-address=0x3990`

Pass/fail result:
- CMake regeneration: PASS.
- Focused stable-rounding Rdot target build: PASS.
- Focused MPFR tests: PASS, `4/4`.
- Rdot `1000000 512` smoke comparison: PASS, all variants reported `DIFF`
  `OK`.
- Disassembly check: PASS. `Rdot_mpfr_kernel_03_mkII_STABLE_ROUNDING` hot loop
  contains TLS rounding loads plus `mpfr_mul`/`mpfr_add`, with no
  `mpfr_get_default_rounding_mode()` call inside the loop.

Known issues:
- Stable-rounding mode still loads the cached thread-local rounding value for
  each wrapper operation. It removes function calls, but it does not make
  generic wrapper expressions identical to raw kernels that keep `rnd` in a
  register for the whole loop.
- Code that changes MPFR rounding directly with `mpfr_set_default_rounding_mode`
  while stable-rounding mode is enabled must refresh through
  `mpfrxx::set_default_rounding_mode(...)` before continuing wrapper
  arithmetic.

Post-phase remove floating wrapper valid flag:
DONE

Implemented features:
- Removed wrapper-side `valid_` ownership flags from `gmpxx::mpf_class`,
  `mpfrxx::mpfr_class`, and `mpfrxx::mpc_class`.
- Restored dense backend-compatible layout for floating wrappers. The ABI
  fingerprint test now checks that `mpf_class`, `mpfr_class`, and `mpc_class`
  have the same size and alignment as `mpf_t`, `mpfr_t`, and `mpc_t`.
- Changed move constructors to transfer backend object storage to the
  destination and immediately reinitialize the moved-from source as a valid
  backend object. This may allocate, but avoids adding per-object state that
  widens array stride.
- Simplified destructors, copy operations, scalar assignment, expression
  assignment, and swap/move-assignment paths because wrappers always own a
  valid backend object after construction.
- Updated the move allocation-count tests to document the new tradeoff:
  move construction can allocate to reinitialize the source, while array layout
  remains backend-compatible.
- Updated `SPECIFICATIONS.md` to record the no-wrapper-valid-flag layout
  policy.

Tests added:
- No new test files were added.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `tests/test_abi_fingerprint.cpp`
- `tests/test_mpf_alloc_count.cpp`
- `tests/test_mpfr_alloc_count.cpp`
- `tests/test_mpc_alloc_count.cpp`
- `tests/test_mpfc_basic.cpp`
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `ctest --test-dir build -R 'test_abi_fingerprint|test_construction_copy|test_mpf_basic|test_mpfrxx_mkII|test_mpc_basic|test_mpfc_basic|test_mpf_alloc_count|test_mpfr_alloc_count|test_mpfr_fixed_precision_fma_alloc_count|test_mpc_alloc_count' --output-on-failure`
- `cmake --build build-release-nocount -j --target Rdot_mpfr_kernel_05_mkII Rdot_mpfr_kernel_05_mkII_FMA`
- `objdump -dr -C --no-show-raw-insn build-release-nocount/benchmarks/CMakeFiles/Rdot_mpfr_kernel_05_mkII.dir/mpfr/00_Rdot/Rdot_mpfr_kernel_05.cpp.o --start-address=0x2f0 --stop-address=0x338`
- `objdump -dr -C --no-show-raw-insn build-release-nocount/benchmarks/CMakeFiles/Rdot_mpfr_kernel_05_mkII_FMA.dir/mpfr/00_Rdot/Rdot_mpfr_kernel_05.cpp.o --start-address=0x2a0 --stop-address=0x2e8`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused CTest: PASS, 10/10.
- Release/no-allocator-count MPFR Rdot kernel05 rebuild: PASS.
- Assembly check: PASS, MPFR wrapper array increments are `add $0x20` in both
  non-FMA and FMA kernel05 hot loops.
- Full debug build: PASS.
- Full CTest: PASS, 153/153.

Known issues:
- Move construction is no longer zero-allocation for MPF/MPFR/MPC wrappers,
  because the moved-from source is reinitialized to keep every wrapper object
  backend-valid without a layout-widening ownership flag.

Post-phase split MPF borrowed expression leaves:
DONE

Implemented features:
- Added `borrowed_object_leaf<T>` for expression operands that only reference
  existing lvalue objects.
- Routed GMP MPF lvalue operands through borrowed leaves while keeping rvalue
  operands on owning `object_leaf<T>`, preserving temporary lifetime safety.
- Updated GMP MPF expression precision, evaluation, alias checks, direct
  leaf-leaf assignment, and direct multiply compound-assignment detection to
  accept both borrowed and owning MPF leaves.
- Updated the ABI fingerprint test to reflect the new borrowed MPF lvalue leaf
  type.

Tests added:
- No new tests were added.

Tests updated:
- `include/gmpfrxx_mkII/detail/expr.hpp`
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `tests/test_abi_fingerprint.cpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target test_abi_fingerprint test_et_contract_mpf test_mpf_numeric_equivalence test_compound_assign test_mpf_aliasing`
- `cmake --build build-release-nocount -j --target Raxpy_gmp_kernel_01_mkII`
- `nm -C build-release-nocount/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII | rg "mpf_compound_assign|_Raxpy"`
- `objdump -Cd build-release-nocount/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII | rg -n "<_Raxpy|__gmpf_get_prec|__gmpf_init2|__gmpf_mul|__gmpf_add|__gmpf_clear|cmpb"`
- `ctest --test-dir build -R "test_abi_fingerprint|test_et_contract_mpf|test_mpf_numeric_equivalence|test_compound_assign|test_mpf_aliasing" --output-on-failure`
- `build-release-nocount/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII 1000 512`
- `git diff --check`
- Release/no-allocator-count Raxpy `5000000 512`, 5 runs each:
  `Raxpy_gmp_kernel_01_orig` and `Raxpy_gmp_kernel_01_mkII`.

Pass/fail result:
- Focused build: PASS.
- Focused CTest: PASS, 8/8.
- Raxpy smoke: PASS, `Result OK`.
- `git diff --check`: PASS.
- Best of 5 Release/no-allocator-count Raxpy `5000000 512`:
  - `Raxpy_gmp_kernel_01_orig`: `29.3731 MFLOPS`
  - `Raxpy_gmp_kernel_01_mkII`: `29.3350 MFLOPS`

Known issues:
- Borrowed leaves are currently used for the GMP MPF path. Other numeric
  families still use the existing owning-capable `object_leaf<T>` path.

Post-phase inline GMP MPF compound multiply assignment:
DONE

Implemented features:
- Added a portable `GMPFRXX_MKII_ALWAYS_INLINE` helper macro.
- Marked the GMP MPF direct multiply compound-assignment helpers as
  always-inline candidates so `y += alpha * x` can be flattened into the
  caller loop.
- Verified that the `Raxpy_gmp_kernel_01_mkII` executable no longer exports a
  separate `mpf_compound_assign<add_op, binary_expr<mul_op,...>>` symbol.

Tests added:
- No new tests were added.

Tests updated:
- `include/gmpfrxx_mkII/detail/config.hpp`
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build-release-nocount -j --target Raxpy_gmp_kernel_01_mkII`
- `nm -C build-release-nocount/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII | rg "mpf_compound_assign|mpf_compound_mul_apply|_Raxpy"`
- `objdump -Cd build-release-nocount/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII | rg -n "<_Raxpy|mpf_compound_assign|__gmpf_get_prec|__gmpf_init2|__gmpf_mul|__gmpf_add|__gmpf_clear"`
- `objdump -Cd build-release-nocount/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII --start-address=0x6330 --stop-address=0x6508`
- `ctest --test-dir build -R "test_mpf_numeric_equivalence|test_compound_assign|test_mpf_aliasing" --output-on-failure`
- `build-release-nocount/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII 1000 512`
- Release/no-allocator-count Raxpy `5000000 512`, 5 runs each:
  `Raxpy_gmp_kernel_01_orig` and `Raxpy_gmp_kernel_01_mkII`.

Pass/fail result:
- Focused MPF tests: PASS, 3/3.
- Raxpy smoke: PASS, `Result OK`.
- Disassembly check: PASS, no separate `mpf_compound_assign` symbol remained.
- Best of 5 Release/no-allocator-count Raxpy `5000000 512`:
  - `Raxpy_gmp_kernel_01_orig`: `29.1365 MFLOPS`
  - `Raxpy_gmp_kernel_01_mkII`: `28.3677 MFLOPS`

Known issues:
- The helper call is gone, but stack cleanup flag branches for expression-node
  ownership remain visible in the hot loop.

Post-phase MPFR Raxpy FMA benchmark variants:
DONE

Implemented features:
- Added serial and OpenMP MPFR C native Raxpy FMA baseline targets.
- The normal C native Raxpy targets keep the existing `mpfr_mul` plus
  `mpfr_add` path, while the FMA targets use
  `mpfr_fma(y[i], alpha, x[i], y[i], rnd)`.
- Switched MPFR Raxpy wrapper benchmark variants from the legacy
  `*_mkII_FIXED_PRECISION_FASTPATH*` names to the short `*_mkII` and
  `*_mkII_FMA` names.
- Updated the MPFR benchmark runner, common plot colors, and benchmark README
  for the new Raxpy target names.

Tests added:
- No new tests were added.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/README.md`
- `benchmarks/common/plot.py`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01_FMA.cpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build-release-nocount -j --target Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_01_FMA Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_C_native_openmp_01_FMA Raxpy_mpfr_kernel_01_mkII Raxpy_mpfr_kernel_01_mkII_FMA Raxpy_mpfr_kernel_02_mkII Raxpy_mpfr_kernel_02_mkII_FMA Raxpy_mpfr_kernel_openmp_01_mkII Raxpy_mpfr_kernel_openmp_01_mkII_FMA Raxpy_mpfr_kernel_openmp_02_mkII Raxpy_mpfr_kernel_openmp_02_mkII_FMA`
- `cmake --build build-release-nocount -j --target Raxpy_mpfr_C_native_01_FMA Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_C_native_openmp_01_FMA Raxpy_mpfr_kernel_01_mkII Raxpy_mpfr_kernel_01_mkII_FMA Raxpy_mpfr_kernel_02_mkII Raxpy_mpfr_kernel_02_mkII_FMA Raxpy_mpfr_kernel_openmp_01_mkII Raxpy_mpfr_kernel_openmp_01_mkII_FMA Raxpy_mpfr_kernel_openmp_02_mkII Raxpy_mpfr_kernel_openmp_02_mkII_FMA`
- `build-release-nocount/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA 1000 512`
- `build-release-nocount/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01_FMA 1000 512`
- `build-release-nocount/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_01_mkII_FMA 1000 512`
- `build-release-nocount/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_01_mkII_FMA 1000 512`
- `/bin/bash -lc 'cmake --build build-release-nocount --target help | rg "Raxpy_mpfr_.*FIXED_PRECISION_FASTPATH"'`

Pass/fail result:
- Initial multi-target build configured the new targets but stopped before the
  newly generated FMA target could be built by name.
- Follow-up Raxpy target build: PASS.
- Serial C native FMA smoke: PASS, `Result OK`.
- OpenMP C native FMA smoke: PASS, `Result OK`.
- Serial wrapper FMA smoke: PASS, `Result OK`.
- OpenMP wrapper FMA smoke: PASS, `Result OK`.
- Raxpy legacy fixed-precision-fastpath target scan: PASS, no matches.

Known issues:
- FMA and non-FMA Raxpy use different rounding paths, so bitwise equality is
  not expected between those target families. The benchmark uses the existing
  tolerance-based L1 norm check.

Post-phase MPFR Rdot C native FMA baselines:
DONE

Implemented features:
- Added serial and OpenMP MPFR C native Rdot FMA baseline targets.
- The normal C native targets keep the existing `mpfr_mul` plus `mpfr_add`
  loop, while the FMA targets use `mpfr_fma(acc, x, y, acc, rnd)` and avoid
  the extra temporary product.
- Documented the new MPFR Rdot C native FMA target names in the benchmark
  README.

Tests added:
- No new tests were added.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/README.md`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01_FMA.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_openmp_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_openmp_01_FMA.cpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build-release-nocount -j --target Rdot_mpfr_C_native_01 Rdot_mpfr_C_native_01_FMA Rdot_mpfr_C_native_openmp_01 Rdot_mpfr_C_native_openmp_01_FMA`
- `cmake --build build-release-nocount -j --target Rdot_mpfr_C_native_01_FMA Rdot_mpfr_C_native_openmp_01_FMA`
- `build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01_FMA 1000 512`
- `build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_openmp_01_FMA 1000 512`
- `git diff --check`

Pass/fail result:
- Initial multi-target build configured the new targets but stopped before the
  new FMA target could be built by name.
- Follow-up FMA target build: PASS.
- Serial FMA smoke: PASS, `DIFF` OK.
- OpenMP FMA smoke: PASS, `DIFF` OK.
- `git diff --check`: PASS.

Known issues:
- FMA and non-FMA Rdot use different rounding paths, so bitwise equality is
  not expected. The existing benchmark keeps the tolerance-based `DIFF` check.

Post-phase remove Rdot fastpath-name variants:
DONE

Implemented features:
- Split Rdot benchmark variant registration from the other benchmark families.
- GMP Rdot now builds only `*_orig` and `*_mkII` wrapper variants.
- MPFR Rdot now builds `*_mkII` and `*_mkII_FMA`, removing the legacy
  `*_mkII_FIXED_PRECISION_FASTPATH*` names from Rdot.
- Updated the GMP Rdot runner, GMP Rdot plot legend, and benchmark
  documentation to stop advertising Rdot fastpath-name variants.

Tests added:
- No new tests were added.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/README.md`
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/gmp/00_Rdot/go.sh`
- `benchmarks/gmp/00_Rdot/plot.py`
- `STATUS.md`

Exact commands run:
- `cmake --build build-release-nocount -j --target Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_03_mkII Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_01_mkII_FMA Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_03_mkII_FMA`
- `cmake --build build-release-nocount --target help`
- `/bin/bash -lc 'cmake --build build-release-nocount --target help | rg "Rdot_.*FIXED_PRECISION_FASTPATH"'`
- `git diff --check`

Pass/fail result:
- Focused Rdot benchmark target build: PASS.
- Rdot legacy fixed-precision-fastpath target scan: PASS, no matches.
- `git diff --check`: PASS.

Known issues:
- Non-Rdot benchmark families still keep the legacy
  `*_mkII_FIXED_PRECISION_FASTPATH*` target names for continuity.

Post-phase direct leaf-leaf expression assignment:
DONE

Implemented features:
- Let GMP MPF existing-object assignment from direct leaf-leaf binary
  expressions call the backend operation directly even when the destination is
  one of the operands. GMP MPF arithmetic permits destination overlap for these
  operations.
- Added the same direct assignment path for MPFR leaf-leaf binary expressions,
  using the current destination precision and rounding mode.
- This targets loops such as `templ = x[i] * y[i]; acc += templ;`, where the
  first assignment can now bypass the generic recursive expression evaluator.

Tests added:
- No new tests were added.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target test_mpf_numeric_equivalence test_mpfr_numeric_equivalence test_alias_safety test_compound_assign test_mpfr_compound_assign`
- `ctest --test-dir build -R "test_mpf_numeric_equivalence|test_mpfr_numeric_equivalence|test_alias_safety|test_compound_assign|test_mpfr_compound_assign" --output-on-failure`
- `cmake --build build-release-nocount -j --target Rdot_gmp_kernel_03_mkII Rdot_mpfr_kernel_03_mkII`
- Release/no-allocator-count Rdot kernel03 `100000 1024` benchmarks, 3 runs
  per listed variant.

Pass/fail result:
- Focused build: PASS.
- Focused tests: PASS, 5/5.
- Best of 3 Release/no-allocator-count Rdot kernel03 `100000 1024`:
  - `GMP_kernel03_mkII` with `GMPXX_MKII_DEFAULT_MPF_PREC_BITS=1024`:
    `11.8464 MFLOPS`
  - `MPFR_kernel03_mkII`: `8.22549 MFLOPS`

Known issues:
- New expression materialization still follows the max-precision policy and is
  intentionally not routed through this existing-object assignment fast path.

Post-phase disable fixed-precision fastpath macro:
DONE

Implemented features:
- Disabled `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` for MPFR and MPC
  move assignment by removing the swap-only branch.
- GMP MPF, MPFR, and MPC move assignment now all preserve left-hand-side
  precision for valid destinations.
- Stopped benchmark targets from defining
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`; FMA variants now only add
  `MPFRXX_ENABLE_FMA`.
- Updated README and SPECIFICATIONS to document that the fixed-precision
  fastpath macro is currently disabled/reserved.

Tests added:
- No new tests were added.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `benchmarks/CMakeLists.txt`
- `benchmarks/README.md`
- `README.md`
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target test_mpfr_numeric_equivalence test_mpc_basic test_mpc_precision_policy test_mpf_numeric_equivalence test_compound_assign test_mpfr_compound_assign`
- `ctest --test-dir build -R "test_mpfr_numeric_equivalence|test_mpc_basic|test_mpc_precision_policy|test_mpf_numeric_equivalence|test_compound_assign|test_mpfr_compound_assign" --output-on-failure`

Pass/fail result:
- Focused build: PASS.
- Focused tests: PASS, 6/6.

Known issues:
- Legacy benchmark target names containing `FIXED_PRECISION_FASTPATH` remain
  for continuity, but they no longer define the disabled macro.

Post-phase direct multiply compound assignment:
DONE

Implemented features:
- Added a GMP MPF direct compound-assignment specialization for direct
  leaf-leaf multiply expressions:
  `a += b * c` and `a -= b * c`.
- The GMP path uses a scoped local temporary and calls `mpf_mul` followed by
  `mpf_add` or `mpf_sub`, avoiding the generic recursive expression evaluator
  while keeping header-owned TLS scratch pools removed.
- Added the analogous MPFR direct multiply compound path for non-FMA builds:
  `mpfr_mul` followed by `mpfr_add` or `mpfr_sub`.
- Kept the existing MPFR `MPFRXX_ENABLE_FMA` path using native `mpfr_fma` /
  adjusted `mpfr_fms` for the same expression shape.

Tests added:
- No new unit tests were added; existing compound-assignment, allocation-count,
  and numeric-equivalence tests cover the affected behavior.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target test_mpf_alloc_count test_mpfr_alloc_count test_compound_assign test_mpfr_compound_assign test_mpf_numeric_equivalence test_mpfr_numeric_equivalence`
- `git diff --check`
- `ctest --test-dir build -R "test_mpf_alloc_count|test_mpfr_alloc_count|test_compound_assign|test_mpfr_compound_assign|test_mpf_numeric_equivalence|test_mpfr_numeric_equivalence" --output-on-failure`
- `cmake --build build-release-nocount -j --target Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_03_mkII Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH_FMA`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- Release/no-allocator-count Rdot `100000 512` benchmarks, 3 runs per listed
  variant.

Pass/fail result:
- Focused tests: PASS, 6/6.
- Full build: PASS.
- Full CTest: PASS, 153/153.
- Whitespace check: PASS.
- Best of 3 Release/no-allocator-count Rdot `100000 512`:
  - `GMP_kernel01_orig`: `27.6801 MFLOPS`
  - `GMP_kernel01_mkII`: `27.3689 MFLOPS`
  - `GMP_kernel01_mkII_assume`: `23.969 MFLOPS`
  - `GMP_kernel03_orig`: `32.5501 MFLOPS`
  - `GMP_kernel03_mkII`: `32.3836 MFLOPS`
  - `GMP_kernel03_mkII_assume`: `28.7573 MFLOPS`
  - `MPFR_C_native`: `15.4542 MFLOPS`
  - `MPFR_kernel01_mkII`: `14.061 MFLOPS`
  - `MPFR_kernel01_mkII_assume`: `14.6144 MFLOPS`
  - `MPFR_kernel01_mkII_assume_FMA`: `17.6206 MFLOPS`
  - `MPFR_kernel03_mkII`: `15.3644 MFLOPS`
  - `MPFR_kernel03_mkII_assume`: `15.5625 MFLOPS`
  - `MPFR_kernel03_mkII_assume_FMA`: `15.6834 MFLOPS`

Known issues:
- The direct GMP path closes most of the `kernel_01` gap in the normal build.
  The fixed-precision assume build is slower in these measurements and needs a
  separate investigation before treating that macro as a performance win for
  GMP MPF Rdot.

Post-phase remove MPF/MPFR scratch pools:
DONE

Implemented features:
- Removed the MPF header-owned TLS scratch pool and the GMP MPF scratch
  compound-assignment path.
- Removed `GMPXX_ENABLE_FMA` / `build_options::enable_gmp_fma`; GMP MPF has
  no wrapper fused multiply-add option now.
- Removed the unused MPFR header-owned TLS scratch pool definitions. MPFR FMA
  support continues to use native MPFR fused operations under
  `MPFRXX_ENABLE_FMA`.
- Renamed the MPFR fixed-precision allocation test from the old scratch name
  to `test_mpfr_fixed_precision_fma_alloc_count`.
- Deleted the obsolete MPF fixed-precision TLS scratch allocation test.
- Updated `SPECIFICATIONS.md` to state that MPF compound assignment must not
  keep a header-owned TLS scratch pool.

Tests added:
- No new behavior tests were added; the MPFR allocation test was renamed to
  reflect that it tests native FMA allocation behavior, not scratch reuse.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `include/gmpfrxx_mkII/detail/config.hpp`
- `tests/CMakeLists.txt`
- `tests/test_mpfr_fixed_precision_fma_alloc_count.cpp`
- `tests/test_mpfr_fixed_precision_tls_scratch.cpp` removed by rename
- `tests/test_mpf_fixed_precision_tls_scratch.cpp` removed
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpf_alloc_count test_mpf_fixed_precision_materialization test_mpfr_fixed_precision_fma_alloc_count test_mpfr_compound_assign Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH`
- `git diff --check`
- `ctest --test-dir build -R "test_mpf_(alloc_count|fixed_precision_materialization)|test_mpfr_fixed_precision_fma_alloc_count|test_mpfr_compound_assign" --output-on-failure`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH 100000 512`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH 100000 512`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused tests: PASS, 4/4.
- Full build: PASS.
- Full CTest: PASS, 153/153.
- Scratch/GMP FMA source scan: no live references in `include`, `tests`,
  `CMakeLists.txt`, `SPECIFICATIONS.md`, or `benchmarks`.
- `Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH 100000 512`:
  alloc/free `100001/100001`, elapsed `0.0353801 s`, `5.65287 MFLOPS`,
  `DIFF` OK.
- `Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH 100000 512`:
  alloc/free `2/2`, elapsed `0.0290525 s`, `6.88406 MFLOPS`, `DIFF` OK.

Known issues:
- Removing the MPF scratch pool intentionally gives up the previous
  allocation-free `a += b * c` scratch reuse path. The simpler implementation
  avoids header-owned TLS state and leaves future GMP MPF optimization to
  direct expression-assignment or benchmark-specific kernel changes.

Post-phase MPF direct leaf binary assignment:
DONE

Implemented features:
- Added an MPF expression-assignment fast path for direct leaf-leaf binary
  expressions such as `out = a + b`, `out = a - b`, `out = a * b`, and
  `out = a / b`.
- The fast path calls the corresponding GMP `mpf_*` operation directly from
  `mpf_class::operator=(Expr)` before entering the generic recursive
  expression evaluator.
- Self-aliasing assignments such as `x = x * y` continue to fall back to the
  existing temporary-based evaluator, preserving the previous alias-safety
  policy and destination-precision-preserving assignment semantics.

Tests added:
- No unit tests were added; this is an internal fast path for existing tested
  expression-assignment behavior.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target test_mpf_alloc_count test_mpf_aliasing test_mpf_numeric_equivalence Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_03_mkII`
- `git diff --check`
- `ctest --test-dir build -R "test_mpf_(alloc_count|aliasing|numeric_equivalence)" --output-on-failure`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII 1000 512`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH 1000 512`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII 100000 512`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH 100000 512`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_orig 100000 512`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused MPF tests: PASS, 3/3.
- Full build: PASS.
- Full CTest: PASS, 154/154.
- `Rdot_gmp_kernel_03_mkII 100000 512`: alloc/free `2/2`,
  elapsed `0.0300275 s`, `6.66053 MFLOPS`, `DIFF` OK.
- `Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH 100000 512`:
  alloc/free `2/2`, elapsed `0.0289908 s`, `6.8987 MFLOPS`, `DIFF` OK.
- `Rdot_gmp_kernel_03_orig 100000 512`: alloc/free `2/2`,
  elapsed `0.00942289 s`, `21.2248 MFLOPS`, `DIFF` OK.
- Whitespace check: PASS.

Known issues:
- This fast path removes generic evaluator dispatch from the simplest MPF
  binary assignment case, but it does not close the remaining gap to the
  original GMP wrapper kernel. The current benchmark still points to residual
  wrapper overhead outside allocation traffic.

Post-phase MPFR benchmark FMA variants:
DONE

Implemented features:
- Added a third MPFR wrapper benchmark configuration for every MPFR kernel:
  `*_mkII_FIXED_PRECISION_FASTPATH_FMA`.
- The new variant enables both
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` and `MPFRXX_ENABLE_FMA`.
- Updated the MPFR benchmark runner to execute the normal, fixed-precision
  fast path, and fixed-precision fast path plus FMA variants.
- Updated the shared plotter color map and benchmark documentation for the new
  suffix.

Tests added:
- No unit tests were added; this phase adds benchmark targets and runner
  entries.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `benchmarks/common/plot.py`
- `benchmarks/README.md`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH_FMA`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII 1000 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH 1000 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA 1000 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH_FMA 1000 512`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Representative MPFR Rdot variants: PASS.
- `Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA 1000 512`
  printed `BENCH_ALLOC_COUNTS ... alloc=1 ... free=1` and `DIFF` OK.
- Full build: PASS.
- Full CTest: PASS, 154/154.
- Whitespace check: PASS.

Known issues:
- The new FMA benchmark variant is only generated for MPFR wrapper kernel
  targets. Native C targets and GMP targets keep their existing variant sets.

Post-phase Rdot allocator profiling:
DONE

Implemented features:
- Added `benchmarks/common/benchmark_allocator_counter.hpp`, a benchmark-only
  GMP allocator counter based on `mp_set_memory_functions()`.
- Rdot benchmark executables now install the allocator counter before the
  first GMP allocation and print `BENCH_ALLOC_COUNTS` for the timed kernel.
- Integrated allocator baseline/printing with the existing Rdot operation
  counter calls while keeping init/clear macro counters opt-in.
- Added allocator profiling to GMP and MPFR Rdot serial and OpenMP variants.

Tests added:
- No compiled unit tests were added; this phase instruments benchmark
  executables.

Tests updated:
- `benchmarks/common/benchmark_allocator_counter.hpp`
- `benchmarks/common/mpf_operation_counter.hpp`
- `benchmarks/common/mpfr_operation_counter.hpp`
- `benchmarks/gmp/00_Rdot/*.cpp`
- `benchmarks/mpfr/00_Rdot/*.cpp`
- `benchmarks/README.md`
- `benchmarks/gmp/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_C_native_01 Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII 1000 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII 1000 512`
- `cmake --build build -j`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_01 1000 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01 1000 512`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Representative Rdot GMP/MPFR benchmark builds: PASS.
- Representative Rdot runs print `BENCH_ALLOC_COUNTS`: PASS.
- Full build: PASS.
- Full CTest: PASS, 154/154.
- Whitespace check: PASS.

Known issues:
- `BENCH_ALLOC_COUNTS` measures actual GMP allocator traffic. Direct
  `mpf_init2`/`mpfr_init2` and clear operation counts remain available only
  through the existing opt-in macro counters, because those counters rewrite C
  API calls in benchmark translation units.

Post-phase moved-from assignment guards:
DONE

Implemented features:
- Reinitialized `mpf_class`, `mpfr_class`, and `mpc_class` when scalar,
  string, expression, or selected value assignments target a moved-from object.
- Made copy construction and copy assignment from moved-from `mpf_class`,
  `mpfr_class`, and `mpc_class` deterministic by treating the source as zero
  with wrapper default precision instead of passing precision 0 to GMP/MPFR/MPC.
- Routed `mpfc_class` assignments through component assignment so moved-from
  `mpf_class` components are restored before use.

Tests added:
- Extended allocation-count tests for `mpf_class`, `mpfr_class`, and
  `mpc_class` to cover moved-from reassignment and copy-from-moved sources.
- Extended `test_mpfc_basic` to cover assignment into moved-from `mpfc_class`
  objects.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `tests/test_mpf_alloc_count.cpp`
- `tests/test_mpfr_alloc_count.cpp`
- `tests/test_mpc_alloc_count.cpp`
- `tests/test_mpfc_basic.cpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target test_mpf_alloc_count test_mpfr_alloc_count test_mpc_alloc_count test_mpfc_basic test_mpfr_initialization_sentinel`
- `ctest --test-dir build -R 'test_mpf_alloc_count|test_mpfr_alloc_count|test_mpc_alloc_count|test_mpfc_basic|test_mpfr_initialization_sentinel' --output-on-failure`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Targeted moved-from assignment and MPFR sentinel tests: PASS.
- Whitespace check: PASS.
- Full build: PASS.
- Full CTest: PASS, 154/154.

Known issues:
- Copying from a moved-from numeric object yields a valid zero value with
  default precision. This is intentional defensive behavior for an otherwise
  unspecified moved-from value.

Post-phase MPFR default initialization sentinel:
DONE

Implemented features:
- Removed the inline function-local `thread_local` initialization flag from
  `initialize_mpfr_defaults_for_current_thread()`.
- Added a libmpfr-state sentinel: wrapper environment defaults are applied only
  when the calling thread's MPFR default precision, rounding mode, `emin`, and
  `emax` still match MPFR's library-initial values.
- Documented that libmpfr TLS default state is the source of truth across DSO
  boundaries; the wrapper must not own a DSO-local initialization flag.

Tests added:
- `tests/test_mpfr_initialization_sentinel.cpp`

Tests updated:
- `include/gmpfrxx_mkII/detail/environment.hpp`
- `tests/CMakeLists.txt`
- `README.md`
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `rg -n "initialize_mpfr_defaults_for_current_thread|apply_mpfr_environment_defaults|set_default_precision_bits|default_options|mpfr_get_default_prec|mpfr_set_default_prec" include/gmpfrxx_mkII/detail include`
- `sed -n '1,280p' include/gmpfrxx_mkII/detail/environment.hpp`
- `sed -n '278,340p' include/gmpfrxx_mkII/detail/environment.hpp`
- `rg -n "MPFR default|initialize_mpfr_defaults_for_current_thread|reload_mpfr_defaults_from_environment|set_default_precision_bits|default_options|MPFRXX_DEFAULT_PRECISION_BITS" tests include README.md SPECIFICATIONS.md STATUS.md`
- `sed -n '1,220p' tests/test_mpfr_defaults.cpp && sed -n '1,220p' tests/test_mpfr_environment.cpp`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpfr_initialization_sentinel test_mpfr_defaults test_mpfr_environment test_mpfr_thread_safety`
- `ctest --test-dir build -R 'test_mpfr_initialization_sentinel|test_mpfr_defaults|test_mpfr_environment|test_mpfr_thread_safety' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Targeted MPFR default tests: PASS.
- Full build: PASS.
- Full CTest: PASS, 154/154.

Known issues:
- The sentinel intentionally treats any pre-existing change to MPFR default
  state as application-owned state and leaves it alone. Applications that reset
  all MPFR defaults exactly to library-initial values before wrapper access can
  still trigger wrapper environment initialization.

Post-phase benchmark raw result directories split by backend:
DONE

Implemented features:
- Moved the remaining top-level `benchmarks/results_raw/Linux_Ryzen_3970X_32-Core`
  GMP benchmark result set into `benchmarks/gmp/results_raw/`.
- Removed the now-empty top-level `benchmarks/results_raw/` directory.
- Added `benchmarks/mpfr/results_raw/` as the MPFR/MPC raw-result location.
- Updated benchmark documentation to state that raw results are backend-scoped:
  `benchmarks/gmp/results_raw/` for GMP/MPF and
  `benchmarks/mpfr/results_raw/` for MPFR/MPC.

Missing features:
- The current shared runner still runs the GMP benchmark families. MPFR/MPC
  benchmark runner support remains future work.

Tests added:
- No compiled tests were added; this phase changes benchmark artifact layout
  and documentation.

Exact commands run:
- `rg -n "results_raw|RESULT|benchmark|csv|json|\\.dat|\\.txt" benchmarks CMakeLists.txt cmake scripts .github`
- `find benchmarks -maxdepth 3 -type f | sort | sed -n '1,220p'`
- `find benchmarks -maxdepth 3 -type d | sort`
- `sed -n '1,260p' benchmarks/common/run_benchmarks.sh`
- `sed -n '1,220p' benchmarks/common/plot.py`
- `sed -n '1,120p' benchmarks/README.md`
- `find benchmarks/results_raw -maxdepth 2 -type f | sort | sed -n '1,80p'`
- `rg -n "benchmarks/results_raw|results_raw" benchmarks --glob '!**/results_raw/**'`
- `find benchmarks/gmp/results_raw -maxdepth 1 -mindepth 1 -type d -printf '%f\\n' | sort`
- `find benchmarks/results_raw -maxdepth 1 -mindepth 1 -type d -printf '%f\\n' | sort`
- `mkdir -p benchmarks/gmp/results_raw benchmarks/mpfr/results_raw && mv benchmarks/results_raw/Linux_Ryzen_3970X_32-Core benchmarks/gmp/results_raw/Linux_Ryzen_3970X_32-Core && rmdir benchmarks/results_raw`
- `find benchmarks -maxdepth 3 -type d | sort`
- `find benchmarks/gmp/results_raw -maxdepth 2 -type f | sort | sed -n '1,60p'`
- `rg -n "benchmarks/results_raw" benchmarks --glob '!**/results_raw/**'`
- `test ! -e benchmarks/results_raw && echo top_results_raw_absent`
- `git status --short | sed -n '1,180p'`

Pass/fail result:
- Layout check: PASS. `benchmarks/results_raw` no longer exists.
- GMP raw results check: PASS. Existing Linux Ryzen result set now lives under
  `benchmarks/gmp/results_raw/Linux_Ryzen_3970X_32-Core/`.
- MPFR raw directory check: PASS. `benchmarks/mpfr/results_raw/` exists.

Known issues:
- Existing GMP result files are large and remain untracked until intentionally
  staged.

Post-phase MPF defaults routed through GMP:
DONE

Implemented features:
- Routed MPF default precision APIs through GMP:
  `gmpxx::default_mpf_precision_bits()` now reads `mpf_get_default_prec()`,
  and `gmpxx::set_default_mpf_precision_bits()` calls
  `mpf_set_default_prec()`.
- Changed `gmpxx::reload_default_mpf_precision_bits_from_environment()` to
  parse `MPFXX_DEFAULT_PREC_BITS` and write the result to GMP with
  `mpf_set_default_prec()`, falling back to the project default of 512 bits
  when the variable is absent or invalid.
- Added one-time MPF default initialization so ordinary wrapper default
  construction starts from `MPFXX_DEFAULT_PREC_BITS` or 512 bits before reading
  GMP's default precision.
- Removed wrapper-owned MPF default precision storage. MPF default precision is
  now GMP process-global state, matching the user's policy choice.
- Updated MPF thread/default tests to verify routing to GMP's default precision
  and to synchronize runtime mutation because GMP's default is process-global.
- Updated precision and header-only state documentation to describe GMP as the
  MPF default source of truth.

Missing features:
- GMP MPF default precision is process-global, not thread-local. Applications
  that mutate it at runtime must synchronize those mutations or use explicit
  object factories such as `mpf_class::with_precision(...)`.
- GMP may round requested precision to an effective precision; tests now check
  that the effective precision is at least the requested value where needed.

Tests added:
- Updated `tests/test_mpf_precision_policy.cpp` for GMP effective precision.
- Updated `tests/test_mpf_thread_safety.cpp` to verify wrapper setters route to
  `mpf_set_default_prec()` / `mpf_get_default_prec()` and to avoid unsynchronized
  concurrent mutation of GMP process-global default state.
- Updated MPF compatibility expectations in `tests/test_gmpxx_mkII.cpp`.

Exact commands run:
- `sed -n '45,90p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1,120p' tests/test_mpf_thread_safety.cpp`
- `sed -n '55,110p' tests/test_mpf_precision_policy.cpp`
- `sed -n '90,130p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `rg -n "mpf_set_default_prec|mpf_get_default_prec|default_mpf_precision_bits_storage|process-global|must not call ..." docs README.md tests include STATUS.md`
- `cmake --build build -j --target test_mpf_precision_policy test_mpf_thread_safety test_mpf_fixed_precision_materialization test_gmpxx_mkII test_type_conversions test_construction_copy test_random`
- `ctest --test-dir build -R 'test_mpf_precision_policy|test_mpf_thread_safety|test_mpf_fixed_precision_materialization|test_gmpxx_mkII|test_type_conversions|test_construction_copy|test_random' --output-on-failure`
- `gdb -q -batch -ex run -ex bt --args ./build/tests/test_mpf_precision_policy`
- `gdb -q -batch -ex run -ex bt --args ./build/tests/test_mpf_thread_safety`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused MPF default tests: initially FAIL while tests still expected
  wrapper-owned/default-exact precision, then PASS after updating expectations
  for GMP process-global and effective precision.
- Full build: PASS.
- Full ctest: PASS, 146/146 tests passed.

Known issues:
- `mpf_set_default_prec()` is GMP process-global state. This intentionally
  resolves header-only DSO-local wrapper default divergence at the cost of
  thread-local MPF defaults.

Post-phase MPC defaults routed through MPFR/lib state and TLS detection:
DONE

Implemented features:
- Removed wrapper-owned `thread_local mpc_default_options` storage by deleting
  `mpfrxx::mutable_mpc_default_options_unlocked()`.
- Routed MPC default precision APIs through MPFR default precision:
  `default_mpc_*_precision_bits()` now read `mpfr_get_default_prec()` via the
  MPFR wrapper APIs, and setters write with `mpfr_set_default_prec()`.
- Routed MPC default rounding APIs through MPFR default rounding:
  `default_mpc_rounding_mode()` now builds `MPC_RND(r, r)` from
  `mpfr_get_default_rounding_mode()`, and single-rounding setters call
  `mpfr_set_default_rounding_mode()`.
- Preserved source-compatible two-argument MPC default setters without storing
  state: two precision arguments collapse to `max(real, imag)`, and two
  rounding arguments update MPFR only when both modes are equal.
- Added CMake probes for `mpfr_buildopt_tls_p()` and `mpc_buildopt_tls_p()`.
  The generated `build_config.hpp` exposes whether each probe API exists and
  whether the library reports TLS enabled.
- Added `gmpfrxx_mkII::detail::build_options` constants for the detected MPFR
  and MPC TLS probe results.
- Updated MPC default/environment/precision tests and documentation to reflect
  that MPC defaults no longer have independent wrapper-owned real/imag state.

Missing features:
- GNU MPC in this environment does not expose `mpc_buildopt_tls_p()`, so CMake
  records MPC TLS probe API availability as false. MPC arithmetic still uses
  explicit per-call `mpc_rnd_t`; only wrapper default state was removed.
- Unequal MPC default real/imag precision and mixed default real/imag rounding
  cannot be represented without wrapper-owned state. Use explicit object
  factories such as `mpc_class::with_precision(real, imag)` when unequal
  component precision is required.
- MPF/MPFC defaults still use wrapper-owned TLS by design; this phase only
  changed MPFR-backed MPFR/MPC default state.

Tests added:
- Added `tests/test_library_tls_detection.cpp` to verify generated CMake TLS
  probe results against MPFR/MPC buildopt APIs when those APIs exist.
- Updated `tests/test_mpc_defaults.cpp` for MPFR-derived MPC defaults.
- Updated `tests/test_mpc_environment.cpp` for max-collapsed precision,
  representable rounding, and MPFR TLS per-thread behavior.
- Updated `tests/test_mpc_precision_policy.cpp` for scalar promotion after
  two-argument default precision collapse.

Exact commands run:
- `git status --short`
- `sed -n '1,220p' include/gmpfrxx_mkII/detail/mpc_environment.hpp`
- `sed -n '1,180p' include/gmpfrxx_mkII/detail/config.hpp`
- `sed -n '1,220p' CMakeLists.txt`
- `sed -n '1,220p' tests/test_mpc_defaults.cpp`
- `sed -n '1,260p' tests/test_mpc_environment.cpp`
- `sed -n '1,220p' tests/test_mpc_precision_policy.cpp`
- `sed -n '1,220p' tests/CMakeLists.txt`
- `rg -n "mutable_mpc_default_options|default_mpc_|set_default_mpc|reload_mpc_defaults" include tests docs README.md STATUS.md`
- `sed -n '1,220p' include/gmpfrxx_mkII/detail/environment.hpp`
- `sed -n '220,340p' include/gmpfrxx_mkII/detail/environment.hpp`
- `rg -n "mutable_mpc_default_options|thread_local mpc_default|default_real_prec|default_imag_prec|mpc_buildopt_tls_p|mpfr_buildopt_tls_p" include tests CMakeLists.txt`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `sed -n '1,80p' build/generated/gmpfrxx_mkII/detail/build_config.hpp`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- CMake configure: PASS. MPFR probe API found and reported TLS enabled
  (`MPFR_HAS_BUILDOPT_TLS_P=1`, `MPFR_BUILDOPT_TLS=1`). MPC probe API not found
  (`MPC_HAS_BUILDOPT_TLS_P=0`, `MPC_BUILDOPT_TLS=0`).
- Full build: PASS.
- Full ctest: PASS, 146/146 tests passed.

Known issues:
- MPC has no independently stored wrapper default real/imag precision or mixed
  default rounding mode after this phase. This is intentional to avoid
  DSO-local header-only TLS divergence.
- Remaining header-only wrapper-owned state is documented in
  `docs/header_only_state.md`.

Post-phase MPFR defaults routed through libmpfr:
DONE

Implemented features:
- Removed wrapper-owned `thread_local mpfr_default_options` storage by deleting
  `mpfrxx::mutable_mpfr_default_options_unlocked()`.
- Routed MPFR default precision APIs through libmpfr:
  `mpfrxx::default_precision_bits()` / `default_prec()` read
  `mpfr_get_default_prec()`, and `set_default_precision_bits()` calls
  `mpfr_set_default_prec()`.
- Routed MPFR default rounding APIs through libmpfr:
  `default_rounding_mode()` reads `mpfr_get_default_rounding_mode()`, and
  `set_default_rounding_mode()` calls `mpfr_set_default_rounding_mode()`.
- Routed MPFR default exponent range APIs through libmpfr:
  `default_emin()` / `default_emax()` read `mpfr_get_emin()` /
  `mpfr_get_emax()`, and `set_default_exponent_range()` calls
  `mpfr_set_emin()` / `mpfr_set_emax()` with ordering that preserves valid
  intermediate ranges.
- Changed `reload_mpfr_defaults_from_environment()` so environment parsing
  writes directly to libmpfr default precision, rounding mode, and exponent
  range for the current MPFR thread context.
- Updated MPFR/MPC tests to assert routing to libmpfr defaults instead of
  wrapper-local MPFR default state.
- Added `docs/header_only_state.md` to document remaining header-only state and
  clarify that MPFR default precision, rounding, and exponent range now follow
  libmpfr state rather than wrapper-owned TLS.
- Updated `docs/precision_policy.md` to describe libmpfr as the source of truth
  for MPFR defaults.

Missing features:
- MPC defaults still use wrapper-owned `thread_local mpc_default_options`.
  Separate real/imag MPC defaults cannot be represented directly by MPFR's
  single default precision.
- MPF/MPFC defaults still use wrapper-owned TLS because GMP MPF's
  `mpf_set_default_prec()` is process-global and not an MPFR-backed API.
- MPFR thread-local behavior depends on libmpfr being built thread-safe. If
  libmpfr is not thread-safe, these defaults follow MPFR's global behavior.

Tests added:
- Extended `tests/test_mpfr_defaults.cpp` to prove wrapper default precision
  and rounding setters update `mpfr_get_default_prec()` and
  `mpfr_get_default_rounding_mode()`, and exponent-range setters update
  `mpfr_get_emin()` / `mpfr_get_emax()`.
- Updated `tests/test_mpfr_thread_safety.cpp` to prove wrapper default APIs
  route to MPFR TLS and remain isolated per thread in the tested MPFR build.
- Updated `tests/test_mpc_environment.cpp` to expect the MPFR TLS exponent
  range to be visible during MPC helper calls.
- Updated `tests/test_mpfr_environment_invalid.cpp` and
  `tests/test_mpfr_type_conversions.cpp` to preserve and restore libmpfr
  defaults around tests that intentionally mutate defaults.

Exact commands run:
- `rg -n "default_precision_bits\\(|default_prec\\(|set_default_precision_bits|default_rounding_mode\\(|set_default_rounding_mode|default_emin\\(|default_emax\\(|set_default_exponent_range|reload_mpfr_defaults_from_environment|default_options\\(\\)" include tests examples benchmarks docs README.md`
- `sed -n '1,290p' include/gmpfrxx_mkII/detail/environment.hpp`
- `git status --short`
- `sed -n '1,170p' tests/test_mpfr_defaults.cpp`
- `cmake --build build -j --target test_mpfr_defaults test_mpfr_environment test_mpfr_environment_invalid test_mpfr_thread_safety test_mpc_defaults test_mpc_environment test_mpc_environment_invalid test_mpfrxx_mkII`
- `ctest --test-dir build -R 'test_mpfr_defaults|test_mpfr_environment|test_mpfr_environment_invalid|test_mpfr_thread_safety|test_mpc_defaults|test_mpc_environment|test_mpc_environment_invalid|test_mpfrxx_mkII' --output-on-failure`
- `sed -n '1,150p' tests/test_mpfr_environment_invalid.cpp`
- `sed -n '1,170p' tests/test_mpc_environment.cpp`
- `sed -n '1,260p' tests/test_mpfr_thread_safety.cpp`
- `rg -n "mutable_mpfr_default_options_unlocked|thread_local mpfr_default_options|mpfrxx::default.*thread-local|wrapper records thread-local|MPFR.*wrapper-owned" include docs README.md tests`
- `sed -n '35,95p' docs/precision_policy.md`
- `git diff -- include/gmpfrxx_mkII/detail/environment.hpp tests/test_mpfr_defaults.cpp tests/test_mpfr_environment_invalid.cpp tests/test_mpfr_thread_safety.cpp tests/test_mpc_environment.cpp docs/precision_policy.md docs/header_only_state.md`
- `rg -n "thread_local" include/gmpfrxx_mkII/detail/environment.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_environment.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `sed -n '130,185p' tests/test_mpfr_type_conversions.cpp`
- `sed -n '1,130p' tests/test_mpfr_type_conversions.cpp`
- `sed -n '185,270p' tests/test_mpfr_type_conversions.cpp`
- `sed -n '270,380p' tests/test_mpfr_type_conversions.cpp`
- `cmake --build build -j --target test_mpfr_type_conversions`
- `ctest --test-dir build -R 'test_mpfr_type_conversions' --output-on-failure`
- `git status --short`
- `rg -n "mutable_mpfr_default_options_unlocked|thread_local mpfr_default_options|mpfr_exponent_range_guard|mpfr_exponent_range_mutex|set_mpfr_exponent_range_unlocked" include tests docs`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Removed-symbol scan: PASS, no remaining
  `mutable_mpfr_default_options_unlocked`, `thread_local mpfr_default_options`,
  or removed MPFR exponent guard symbols in active include/test/doc files.
- Focused MPFR/MPC default build: PASS.
- Focused MPFR/MPC default ctest: initially FAIL while tests still expected
  wrapper-local MPFR state, then PASS after test updates.
- Focused `test_mpfr_type_conversions`: PASS after preserving/restoring
  libmpfr defaults in that test.
- CMake configure: PASS.
- Full build: PASS.
- Full ctest: PASS, 145/145 tests passed.

Known issues:
- `docs/header_only_state.md` documents remaining wrapper-owned header-only
  state, including MPF/MPC defaults and MPF math caches. MPFR defaults are no
  longer listed as wrapper-owned state.
- `mpfrxx_mkII.h` still does not expose MPC directly; that pre-existing
  header-boundary issue was not part of this MPFR default-state fix.

Post-phase MPQ zero-denominator constructor guard:
DONE

Implemented features:
- Added explicit zero-denominator validation to
  `gmpxx::mpq_class(const mpz_class&, const mpz_class&)` before
  `mpq_set_den()` and `mpq_canonicalize()`.
- The integral numerator/denominator template constructor now inherits the
  same protection because it delegates through the `mpz_class` pair
  constructor.
- Added the same defensive denominator validation to the raw
  `mpq_class(mpq_srcptr)` constructor before canonicalization, so malformed
  external `mpq_t` values cannot reach `mpq_canonicalize()` through this API.
- Preserved the repository's current canonical-rational policy for constructed
  `mpq_class` values; this phase only rejects denominator zero before GMP
  canonicalization.

Missing features:
- Upstream `gmpxx` may expose some non-canonical construction paths, but this
  repository's existing tests require constructor normalization for numerator
  and denominator pairs. No compatibility switch was added.

Tests added:
- Extended `tests/test_mpq_canonicalization.cpp` to require
  `std::invalid_argument` for both `mpq_class(mpz_class(1), mpz_class(0))` and
  the integral template path `mpq_class(1, 0)`.

Exact commands run:
- `rg -n "mpq_class\\(|mpq_canonicalize|canonical" include/gmpfrxx_mkII/detail/zq_impl.hpp tests/test_mpq* tests/test_gmpxx_mkII.cpp`
- `git status --short`
- `sed -n '740,860p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1,180p' tests/test_mpq_basic.cpp`
- `sed -n '1,150p' tests/test_mpq_canonicalization.cpp`
- `sed -n '1,60p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `rg -n "invalid_argument|domain_error|zero denominator|denominator" include/gmpfrxx_mkII/detail/zq_impl.hpp tests/test_mpq*`
- `cmake --build build -j --target test_mpq_canonicalization test_mpq_basic test_mpq_arithmetic test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_mpq_canonicalization|test_mpq_basic|test_mpq_arithmetic|test_gmpxx_mkII' --output-on-failure`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused build: PASS.
- Focused ctest: PASS, 4/4 matching tests passed.
- CMake configure: PASS.
- Full build: PASS.
- Full ctest: PASS, 145/145 tests passed.

Known issues:
- No source-wide audit was performed for every possible way to manufacture an
  invalid raw `mpq_t` outside this wrapper. Public string and denominator-pair
  constructors now reject zero denominators before canonicalization.

Post-phase MPFR exponent-range global-state removal:
DONE

Implemented features:
- Removed `gmpfrxx_mkII::detail::mpfr_exponent_range_guard`, its global
  mutex, and the internal `mpfr_set_emin()`/`mpfr_set_emax()` mutate/restore
  path.
- Removed exponent-range guard construction from MPFR and MPC constructors,
  assignment, expression materialization, compound assignment, stream input,
  and math helper paths.
- Kept wrapper-owned thread-local MPFR default `emin`/`emax` parsing,
  validation, setters, and getters as configuration state, but ordinary
  wrapper operations no longer mutate MPFR process-global exponent range.
- Documented the process-global MPFR exponent-range boundary in
  `docs/precision_policy.md`.

Missing features:
- MPFR does not provide per-object or per-call `emin`/`emax`; this phase does
  not attempt to emulate wrapper-local exponent ranges. Applications that need
  a non-default MPFR exponent range must manage MPFR's process-global range at
  their own synchronization boundary.
- `eval_context` still carries `emin`/`emax` for default-state visibility and
  possible future policy work, but arithmetic no longer applies those fields to
  MPFR global state.

Tests added:
- Updated `tests/test_mpfr_thread_safety.cpp` so parallel operations prove
  wrapper default exponent-range changes do not mutate MPFR process-global
  `emin`/`emax`.
- Updated `tests/test_mpc_environment.cpp` so MPC math helper callbacks prove
  MPFR process-global `emin`/`emax` remain unmodified while wrapper default
  exponent-range state differs.

Exact commands run:
- `rg -n "mpfr_exponent_range_guard|emin|emax|mpfr_set_emin|mpfr_set_emax|get_emin|get_emax" include tests STATUS.md`
- `rg -n "mutex|lock_guard|scoped_lock|default_precision|thread_local|environment" include/gmpfrxx_mkII/detail include/*.h tests`
- `git status --short`
- `sed -n '1,380p' include/gmpfrxx_mkII/detail/environment.hpp`
- `sed -n '1,360p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `git diff -- include/gmpfrxx_mkII/detail/environment.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp tests/test_mpfr_thread_safety.cpp tests/test_mpfr_environment.cpp STATUS.md`
- `sed -n '1,280p' tests/test_mpfr_thread_safety.cpp`
- `sed -n '1,190p' tests/test_mpfr_environment.cpp`
- `sed -n '1,190p' tests/test_mpc_environment.cpp`
- `perl -0pi -e 's/^\\s*const (?:gmpfrxx_mkII::detail::)?mpfr_exponent_range_guard range_guard\\(context\\.emin, context\\.emax\\);\\n//mg' include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `rg -n "mpfr_exponent_range_guard|mpfr_exponent_range_mutex|set_mpfr_exponent_range_unlocked|std::mutex|#include <mutex>" include/gmpfrxx_mkII/detail/environment.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp tests`
- `rg -n "mpfr_set_emin|mpfr_set_emax|mpfr_get_emin\\(|mpfr_get_emax\\(" include tests`
- `git diff -- include/gmpfrxx_mkII/detail/environment.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp tests/test_mpfr_thread_safety.cpp tests/test_mpc_environment.cpp`
- `cmake --build build -j --target test_mpfr_thread_safety test_mpfr_environment test_mpc_environment test_mpfrxx_mkII test_mpc_basic`
- `ctest --test-dir build -R 'test_mpfr_thread_safety|test_mpfr_environment|test_mpc_environment|test_mpfrxx_mkII|test_mpc_basic' --output-on-failure`
- `rg -n "exponent-range|exponent range|emin|emax|MPFRXX_EMIN|MPFRXX_EMAX|global|guard|mutex" docs README.md STATUS.md include tests`
- `sed -n '1,220p' docs/precision_policy.md`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Guard-symbol source scan over active include/test files: PASS, no remaining
  `mpfr_exponent_range_guard`, `mpfr_exponent_range_mutex`, or
  `set_mpfr_exponent_range_unlocked` references.
- Focused build: PASS.
- Focused ctest: PASS, 7/7 matching tests passed.
- CMake configure: PASS.
- Full build: PASS.
- Full ctest: PASS, 145/145 tests passed.

Known issues:
- Historical `STATUS.md` entries still describe the removed guard because they
  record previous phases. This phase supersedes those entries.
- `mpfrxx_mkII.h` still does not expose MPC directly; that pre-existing
  header-boundary issue was not part of this fix.

Post-phase MPFR/MPC constructor RAII guards and real-leaf MPC precision:
DONE

Implemented features:
- Added `gmpfrxx_mkII::detail::scoped_mpfr_init` and used it in MPFR
  constructors that initialize `mpfr_t` before running throwing setup or
  conversion code.
- Added `gmpfrxx_mkII::detail::scoped_mpc_init` and used it in MPC
  constructors that initialize `mpc_t` before running throwing setup or
  assignment code.
- Changed `object_leaf<mpfrxx::mpfr_class>` precision inside MPC expressions
  from `{mpfr_precision, 0}` to `{mpfr_precision, mpfr_precision}`, matching
  the policy that real-valued leaves promoted into complex expressions
  contribute to both component precisions.
- Kept the prior scalar MPC expression precision fix, where scalar leaves use
  both default MPC real and imaginary precisions.

Missing features:
- The remaining header-boundary blocker where `mpfrxx_mkII.h` does not expose
  `mpfrxx::mpc_class` is still unresolved and tracked separately.

Tests added:
- Updated `tests/test_mpc_precision_policy.cpp` so `mpc + mpfr` expression
  materialization expects the MPFR leaf precision in both real and imaginary
  components.

Exact commands run:
- `cmake --build build -j --target test_mpc_precision_policy test_mpfr_precision_policy test_mpc_basic test_mpfrxx_mkII`
- `ctest --test-dir build -R 'test_mpc_precision_policy|test_mpfr_precision_policy|test_mpc_basic|test_mpfrxx_mkII' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused build: PASS.
- Focused ctest: PASS, 4/4 tests passed.
- Full build: PASS.
- Full ctest: PASS, 145/145 tests passed.

Known issues:
- `mpfrxx_mkII.h` still does not expose MPC directly; the existing
  compile-fail test still encodes that behavior.

Post-phase MPFR/MPC constructor cleanup, MPC scalar precision, and TLS documentation:
DONE

Implemented features:
- Wrapped MPFR constructors that call `mpfr_init2` before exponent-range setup
  in `try`/`catch` cleanup blocks so `mpfr_clear` runs if the range guard or
  conversion path throws.
- Wrapped MPC constructors that call `mpc_init3` before exponent-range setup
  in `try`/`catch` cleanup blocks so `mpc_clear` runs if setup or assignment
  throws.
- Changed MPC scalar expression precision from `{default_real, 0}` to
  `{default_real, default_imag}` so scalar promotion contributes both
  component precisions.
- Documented the header-only function-local `thread_local` default-state
  boundary across executable/shared-library instantiations.

Missing features:
- No compiled anchor was introduced for process-wide default state.  The
  header-only TLS behavior is now documented; applications crossing dynamic
  library boundaries must set defaults per boundary or use object-level
  precision factories.
- No synthetic exception-injection test was added for failing MPFR exponent
  range setup; the cleanup paths are structural and were covered by compiling
  the touched constructors through existing MPFR/MPC tests.

Tests added:
- Extended `tests/test_mpc_precision_policy.cpp` to verify that scalar
  promotion into an MPC expression contributes both default real and default
  imaginary component precisions.

Exact commands run:
- `cmake --build build -j --target test_mpc_precision_policy test_mpfr_precision_policy test_mpc_basic test_mpfrxx_mkII`
- `ctest --test-dir build -R 'test_mpc_precision_policy|test_mpfr_precision_policy|test_mpc_basic|test_mpfrxx_mkII' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused build: PASS.
- Focused ctest: PASS, 4/4 tests passed.
- Full build: PASS.
- Full ctest: PASS, 145/145 tests passed.

Known issues:
- `mpfrxx_mkII.h` still does not expose MPC directly; this was a separate
  previously identified header-boundary blocker and was not part of this
  requested fix set.

Post-phase MPF move and MPC API surface regression coverage:
DONE

Implemented features:
- Confirmed `gmpxx::mpf_class` move construction and move assignment are
  `noexcept`.
- Added regression coverage for `std::vector<gmpxx::mpf_class>` reallocation
  so the nothrow-move surface remains visible to standard containers and
  values/precision survive capacity growth.
- Confirmed `mpfrxx::mpc_class` already supports direct construction and
  assignment from `mpfr_class`, `mpz_class`, `mpq_class`, supported integral
  scalars, `float`/`double`, C strings, and `std::string`.
- Added compile-time regression checks for the direct MPC assignment API.

Missing features:
- No timing-based vector benchmark was added.  Runtime timing would be noisy
  in CTest; the stable contract is the `noexcept` move surface that lets
  `std::vector` choose move during reallocation.

Tests added:
- Extended `tests/test_construction_copy.cpp` with a small MPF vector
  reallocation test.
- Extended `tests/test_mpc_basic.cpp` with `std::is_assignable_v` checks for
  direct MPC scalar/exact/MPFR/string assignment and bool rejection.

Exact commands run:
- `cmake --build build -j --target test_construction_copy test_mpc_basic`
- `ctest --test-dir build -R 'test_construction_copy|test_mpc_basic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused build: PASS.
- Focused ctest: PASS, 2/2 tests passed.
- Full build: PASS.
- Full ctest: PASS, 144/144 tests passed.

Known issues:
- None.

## Phase: MPFR Rgemv OpenMP 05/06/07 FMA Variants

Implemented features:
- Added raw C native OpenMP FMA counterparts for MPFR Rgemv variants 05, 06,
  and 07.
- Added mkII OpenMP FMA source files for the same 05/06/07 source shapes.
- Registered the new CMake targets:
  `Rgemv_mpfr_C_native_openmp_05_FMA`,
  `Rgemv_mpfr_C_native_openmp_06_FMA`,
  `Rgemv_mpfr_C_native_openmp_07_FMA`,
  `Rgemv_mpfr_kernel_openmp_05_mkII_FMA`,
  `Rgemv_mpfr_kernel_openmp_06_mkII_FMA`, and
  `Rgemv_mpfr_kernel_openmp_07_mkII_FMA`.
- Added the new targets to `benchmarks/mpfr/02_Rgemv/run_repeat.sh`.
- Documented the FMA counterparts in the MPFR Rgemv README variant and
  C-native-equivalence sections.

Missing features:
- The committed repeat-10 data has not been regenerated with the new FMA
  variants yet.
- The 05/06/07 source shapes do not have a natural final `mpfr_fmma` step;
  they use `mpfr_fma` for the inner accumulation.  `mpfr_fmma` remains
  applicable to row-dot final-update variants such as 01/03.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/mpfr/02_Rgemv/README.md`
- `benchmarks/mpfr/02_Rgemv/run_repeat.sh`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_05_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_06_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_07_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_05_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_06_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07_FMA.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,220p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_05.cpp`
- `sed -n '1,240p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_06.cpp`
- `sed -n '1,260p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_07.cpp`
- `sed -n '1,240p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_05.cpp`
- `sed -n '1,260p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_06.cpp`
- `sed -n '1,300p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07.cpp`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_openmp_05_FMA Rgemv_mpfr_C_native_openmp_06_FMA Rgemv_mpfr_C_native_openmp_07_FMA Rgemv_mpfr_kernel_openmp_05_mkII_FMA Rgemv_mpfr_kernel_openmp_06_mkII_FMA Rgemv_mpfr_kernel_openmp_07_mkII_FMA -j`
- `OMP_NUM_THREADS=4 OMP_PLACES=cores OMP_PROC_BIND=spread bash -lc 'set -euo pipefail; for exe in Rgemv_mpfr_C_native_openmp_05_FMA Rgemv_mpfr_C_native_openmp_06_FMA Rgemv_mpfr_C_native_openmp_07_FMA Rgemv_mpfr_kernel_openmp_05_mkII_FMA Rgemv_mpfr_kernel_openmp_06_mkII_FMA Rgemv_mpfr_kernel_openmp_07_mkII_FMA; do ...; done'`
- `bash -lc 'set -euo pipefail; for exe in Rgemv_mpfr_C_native_openmp_05_FMA Rgemv_mpfr_C_native_openmp_06_FMA Rgemv_mpfr_C_native_openmp_07_FMA Rgemv_mpfr_kernel_openmp_05_mkII_FMA Rgemv_mpfr_kernel_openmp_06_mkII_FMA Rgemv_mpfr_kernel_openmp_07_mkII_FMA; do objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/02_Rgemv/$exe | rg "call.*mpfr_(fma|fmma|mul|add)@plt" | head -n 20; done'`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Release configure: PASS.
- New target build: PASS.
- Smoke correctness: PASS.  All six new FMA executables reported
  `Result OK` for `m=17`, `n=19`, `precision=128`.
- Disassembly check: PASS.  All six new FMA executables contain
  `mpfr_fma@plt` call paths.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- Repeat-10 benchmark data and plots still need a full regeneration before
  the new FMA variants appear in the committed result tables.

## Phase: MPFR Rgemv Repeat-10 Benchmark Refresh

Implemented features:
- Removed the old checked-in MPFR Rgemv repeat-10 result directory.
- Rebuilt the MPFR Rgemv release benchmark targets after tightening the raw C
  native numbering and adding the missing native counterparts.
- Re-ran the full MPFR Rgemv repeat-10 benchmark matrix with 37 variants.
- Generated raw log, raw CSV, summary CSV, and serial/OpenMP plots under a new
  run-specific `results_raw/` directory.
- Refreshed `benchmarks/mpfr/02_Rgemv/README.md` so it only references the new
  result set and includes updated interpretation tables, sorted folded tables,
  bandwidth estimates, plot links, and lessons learned.

Missing features:
- No hardware-counter validation was added.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/02_Rgemv/README.md`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/benchmark_rgemv_mpfr_m4000_n4000_p512_repeat10.log`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/raw_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/rgemv_mpfr_m4000_n4000_p512_repeat10_serial.png`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/rgemv_mpfr_m4000_n4000_p512_repeat10_openmp.png`
- `STATUS.md`

Exact commands run:
- `git rm -r benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260517_232612`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rgemv_mpfr_C_native_01 Rgemv_mpfr_C_native_01_FMA Rgemv_mpfr_C_native_02 Rgemv_mpfr_C_native_02_FMA Rgemv_mpfr_C_native_03 Rgemv_mpfr_C_native_04 Rgemv_mpfr_C_native_openmp_01 Rgemv_mpfr_C_native_openmp_01_FMA Rgemv_mpfr_C_native_openmp_02 Rgemv_mpfr_C_native_openmp_02_FMA Rgemv_mpfr_C_native_openmp_03 Rgemv_mpfr_C_native_openmp_04 Rgemv_mpfr_C_native_openmp_04_FMA Rgemv_mpfr_C_native_openmp_05 Rgemv_mpfr_C_native_openmp_06 Rgemv_mpfr_C_native_openmp_07 Rgemv_mpfr_kernel_01_mkII Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_02_mkII Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_03_mkII Rgemv_mpfr_kernel_03_mkII_FMA Rgemv_mpfr_kernel_04_mkII Rgemv_mpfr_kernel_openmp_01_mkII Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_openmp_02_mkII Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH_FMA Rgemv_mpfr_kernel_openmp_03_mkII Rgemv_mpfr_kernel_openmp_03_mkII_FMA Rgemv_mpfr_kernel_openmp_04_mkII Rgemv_mpfr_kernel_openmp_05_mkII Rgemv_mpfr_kernel_openmp_06_mkII Rgemv_mpfr_kernel_openmp_07_mkII -j`
- `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/mpfr/02_Rgemv/run_repeat.sh build_bench_release 4000 4000 512 10`
- `wc -l benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/raw_rgemv_mpfr_m4000_n4000_p512_repeat10.csv benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `find benchmarks/mpfr/02_Rgemv/results_raw -maxdepth 2 -type f | sort`
- `python3 - <<'PY' ... generate README tables from summary CSV ...`
- `rg -n "20260517_232612|All 31|310 successful|next refresh|444\\.009|425\\.405|24\\.238|rgemv_mpfr_m4000_n4000_p512_repeat10_20260517" benchmarks/mpfr/02_Rgemv/README.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Release configure: PASS.
- Release target build: PASS.
- Full MPFR Rgemv repeat-10 benchmark: PASS.  370/370 timed runs reported
  `Result OK`.
- Plot generation: PASS.
- Stale README reference scan: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- Bandwidth numbers in the README are model estimates, not hardware-counter
  measurements.

## Phase: MPFR Raxpy Kernel Matrix Alignment

Implemented features:
- Split MPFR Raxpy C native FMA and non-FMA sources into independent
  translation units.  The C native files no longer share implementation by
  defining `MPFR_C_NATIVE_USE_FMA` and including another `.cpp`.
- Added `Raxpy_mpfr_kernel_openmp_04.cpp` so the OpenMP kernel matrix has a
  direct counterpart for serial `kernel_04`, with a loop-local product object
  inside the parallel loop.
- Added explicit-context MPFR Raxpy wrappers:
  - `Raxpy_mpfr_kernel_05.cpp`: direct `y[i] += alpha * x[i]` through
    `mpfrxx::with_context`.
  - `Raxpy_mpfr_kernel_06.cpp`: reusable product object through
    `mpfrxx::with_context`.
  - `Raxpy_mpfr_kernel_openmp_05.cpp`: OpenMP direct explicit-context form.
  - `Raxpy_mpfr_kernel_openmp_06.cpp`: OpenMP reusable-product
    explicit-context form.
- Aligned `add_mpfr_raxpy_kernel_variants()` with the MPFR Rdot cumulative
  fastpath ladder:
  `mkII`, `mkII_STABLE_ROUNDING`, `mkII_STABLE_ROUNDING_FMA`, and
  `mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH`.
- Updated the MPFR benchmark runner's Raxpy executable list to include
  fixed-precision fastpath variants and the new context kernels.

Missing features:
- MPFR Raxpy repeat-10 benchmark data and README tables have not yet been
  regenerated for the new target matrix.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01_FMA.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_05.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_06.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_04.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_05.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_06.cpp`
- `STATUS.md`

Exact commands run:
- `git status --short`
- `rg -n "Raxpy_mpfr|add_mpfr_raxpy|MPFR_C_NATIVE_USE_FMA|kernel_07|with_context|evaluation_context|run_mpfr_benchmarks|01_Raxpy" benchmarks/mpfr/01_Raxpy benchmarks/mpfr/00_Rdot benchmarks/CMakeLists.txt benchmarks/common/run_mpfr_benchmarks.sh STATUS.md`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07.cpp`
- `sed -n '1,240p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_08.cpp`
- `sed -n '120,155p' benchmarks/CMakeLists.txt`
- `sed -n '1,220p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01.cpp`
- `sed -n '1,220p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA.cpp`
- `sed -n '1,240p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01.cpp`
- `sed -n '1,120p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01_FMA.cpp`
- `sed -n '1,220p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_01.cpp`
- `sed -n '1,210p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_02.cpp`
- `sed -n '1,210p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_03.cpp`
- `sed -n '1,230p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_01.cpp`
- `sed -n '1,230p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_03.cpp`
- `sed -n '230,295p' benchmarks/CMakeLists.txt`
- `sed -n '125,215p' benchmarks/common/run_mpfr_benchmarks.sh`
- `sed -n '1,220p' benchmarks/mpfr/01_Raxpy/Raxpy_common.hpp`
- `sed -n '1,130p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07.cpp`
- `sed -n '1,140p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_08.cpp`
- `tail -n 120 STATUS.md`
- `sed -n '1,220p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_04.cpp`
- `git diff -- benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01.cpp benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA.cpp benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01.cpp benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01_FMA.cpp`
- `ls benchmarks/mpfr/01_Raxpy`
- `rg -n "MPFR_C_NATIVE_USE_FMA|Raxpy_mpfr_kernel_0[5-6]|Raxpy_mpfr_kernel_openmp_0[4-6]|STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH" benchmarks/mpfr/01_Raxpy benchmarks/CMakeLists.txt benchmarks/common/run_mpfr_benchmarks.sh`
- `git diff --stat`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_01_FMA Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_C_native_openmp_01_FMA Raxpy_mpfr_kernel_01_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH Raxpy_mpfr_kernel_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH Raxpy_mpfr_kernel_openmp_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH Raxpy_mpfr_kernel_05_mkII Raxpy_mpfr_kernel_05_mkII_FMA Raxpy_mpfr_kernel_06_mkII Raxpy_mpfr_kernel_openmp_05_mkII Raxpy_mpfr_kernel_openmp_05_mkII_FMA Raxpy_mpfr_kernel_openmp_06_mkII`
- `/bin/bash -lc 'set -euo pipefail; exe_dir=build_bench_release/benchmarks/mpfr/01_Raxpy; for exe in Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_01_FMA Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_C_native_openmp_01_FMA Raxpy_mpfr_kernel_openmp_04_mkII_STABLE_ROUNDING_FMA_FIXED_PRECISION_FASTPATH Raxpy_mpfr_kernel_05_mkII Raxpy_mpfr_kernel_05_mkII_FMA Raxpy_mpfr_kernel_06_mkII Raxpy_mpfr_kernel_openmp_05_mkII Raxpy_mpfr_kernel_openmp_05_mkII_FMA Raxpy_mpfr_kernel_openmp_06_mkII; do echo "== $exe =="; OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close "$exe_dir/$exe" 128 512 | tail -n 4; done'`
- `sed -n '1,120p' benchmarks/common/run_mpfr_benchmarks.sh`
- `sed -n '250,285p' benchmarks/common/run_mpfr_benchmarks.sh`
- `sed -n '1,260p' benchmarks/mpfr/00_Rdot/plot_repeat_summary.py`
- `sed -n '260,360p' benchmarks/mpfr/00_Rdot/plot_repeat_summary.py`
- `cmake --build build_bench_release -j`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Release configure: PASS.
- Targeted MPFR Raxpy build: PASS.
- Smoke run for new/changed MPFR Raxpy native and wrapper kernels: PASS.
  All tested executables reported `Result OK`.
- Full release build: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- The MPFR Raxpy README and checked-in repeat-10 data still describe the
  earlier target matrix.  They need a fresh benchmark sweep before performance
  conclusions are updated.

## Phase: MPFR Rdot Pointer-Inclusive Bandwidth Documentation

Implemented features:
- Updated `benchmarks/mpfr/00_Rdot/README.md` memory-bandwidth estimates to
  include a model that counts `_mpfr_d` pointer reads in addition to limb
  payload bytes.
- Added a full `mpfr_t` header-inclusive reference model so the lower-bound,
  pointer-inclusive, and full-header estimates are explicitly separated.
- Updated the representative OpenMP bandwidth table with all three bandwidth
  estimates.

Missing features:
- No hardware-counter validation was added.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '330,390p' benchmarks/mpfr/00_Rdot/README.md`
- `python3 - <<'PY' ... compute MPFR Rdot bandwidth estimates ...`
- `sed -n '345,395p' benchmarks/mpfr/00_Rdot/README.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Bandwidth table update: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- The bandwidth estimates are logical traffic models; they do not include
  cache-line overfetch, allocator locality, write-allocate effects for
  temporaries, or hardware-counter validation.

## Phase: MPFR Rdot C Native 03 and Kernel 08 Hotpath Mapping

Implemented features:
- Updated `benchmarks/mpfr/00_Rdot/README.md` Hotpath Disassembly section to
  show that `Rdot_mpfr_kernel_08_mkII::_Rdot` corresponds to `C_native_03`.
- Added the `C_native_03` hotpath snippet showing one `mpfr_mul` plus one
  `mpfr_add` with cached rounding in a register.
- Clarified that `kernel_08_mkII` is the explicit-context wrapper counterpart
  to the raw C reusable-product non-FMA kernel, not an FMA path.

Missing features:
- None.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `objdump -Cd build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_03 | rg -n "<_Rdot|mpfr_get_default_rounding_mode|mpfr_mul@plt|mpfr_add@plt|jne|mov.*%e.*c|mov.*%e.*x"`
- `objdump -Cd build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_08_mkII | rg -n "<_Rdot|mpfr_get_default_rounding_mode|mpfr_mul@plt|mpfr_add@plt|jne|mov.*%e.*c|mov.*%e.*x"`
- `sed -n '1,120p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_03.cpp benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_08.cpp`
- `sed -n '372,480p' benchmarks/mpfr/00_Rdot/README.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Hotpath mapping check: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- None.

## Phase: MPFR Rdot OpenMP 03 Stable Hotpath Documentation

Implemented features:
- Rewrote the `benchmarks/mpfr/00_Rdot/README.md` Hotpath Disassembly section
  as a comparison table plus focused snippets.
- Added the `kernel_openmp_03_mkII_STABLE_ROUNDING` OpenMP hotpath disassembly.
- Documented that this path is a non-FMA reusable-product loop with
  `mpfr_mul` plus `mpfr_add`, and that rounding is still loaded from TLS before
  both MPFR calls.
- Clarified that this kernel has the highest max among non-FMA wrapper OpenMP
  paths in the recorded run, while the best average remains the raw C FMA path.

Missing features:
- None.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `objdump -Cd build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING | rg -n "<_Rdot|mpfr_mul@plt|mpfr_add@plt|fs:|GOMP|omp|\\.omp|Rdot"`
- `objdump -d -C --start-address=0x3970 --stop-address=0x39d8 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING`
- `sed -n '360,470p' benchmarks/mpfr/00_Rdot/README.md`
- `sed -n '1,90p' benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/summary_rdot_mpfr_n10000000_p512_repeat10.csv`
- `sed -n '372,485p' benchmarks/mpfr/00_Rdot/README.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Hotpath disassembly extraction: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- None.

## Phase: MPFR Rdot Context-Bound 07/08 Benchmark

Implemented features:
- Added `Rdot_mpfr_kernel_07_mkII`, `Rdot_mpfr_kernel_07_mkII_FMA`,
  `Rdot_mpfr_kernel_08_mkII`, `Rdot_mpfr_kernel_openmp_07_mkII`,
  `Rdot_mpfr_kernel_openmp_07_mkII_FMA`, and
  `Rdot_mpfr_kernel_openmp_08_mkII` to the common MPFR benchmark runner.
- Ran a focused repeat-10 benchmark for all serial and OpenMP `07`/`08`
  context-bound MPFR Rdot kernels at `N=10000000` and 512-bit precision.
- Generated raw CSV, summary CSV, and serial/OpenMP bar plots with min/max
  range lines for the focused benchmark.
- Updated `benchmarks/mpfr/00_Rdot/README.md` with the `07`/`08` kernel
  shapes, benchmark results, inline plots, memory-bandwidth estimates, hotpath
  disassembly, and lessons learned.
- Updated the MPFR Rdot plot helper so mkII `_FMA` variants use the FMA color
  even when they are explicit-context targets rather than stable-rounding
  suffix targets.

Missing features:
- No 1024-bit `07`/`08` context-bound MPFR Rdot benchmark has been collected
  yet.

Tests added:
- None.

Tests updated:
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `benchmarks/mpfr/00_Rdot/README.md`
- `benchmarks/mpfr/00_Rdot/plot_repeat_summary.py`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_20260517_115204/benchmark_rdot_mpfr_context_07_08_n10000000_p512_repeat10.log`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_20260517_115204/raw_rdot_mpfr_context_07_08_n10000000_p512_repeat10.csv`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_20260517_115204/summary_rdot_mpfr_context_07_08_n10000000_p512_repeat10.csv`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_20260517_115204/rdot_mpfr_context_07_08_n10000000_p512_repeat10_serial.png`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_20260517_115204/rdot_mpfr_context_07_08_n10000000_p512_repeat10_openmp.png`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Rdot_mpfr_kernel_07_mkII Rdot_mpfr_kernel_07_mkII_FMA Rdot_mpfr_kernel_08_mkII Rdot_mpfr_kernel_openmp_07_mkII Rdot_mpfr_kernel_openmp_07_mkII_FMA Rdot_mpfr_kernel_openmp_08_mkII -j`
- `/bin/bash -lc 'set -euo pipefail; stamp=$(date +%Y%m%d_%H%M%S); out="benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_${stamp}"; ...; for exe in "${executables[@]}"; do for run in $(seq 1 10); do OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread /usr/bin/time -f "WALL_SECONDS %e" "$path" 10000000 512; done; done'`
- `python3 benchmarks/mpfr/00_Rdot/plot_repeat_summary.py benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_20260517_115204/benchmark_rdot_mpfr_context_07_08_n10000000_p512_repeat10.log --output-dir benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_20260517_115204 --output-prefix rdot_mpfr_context_07_08_n10000000_p512_repeat10 --title-prefix "MPFR Rdot 07/08 context N=10000000 precision=512 repeat=10"`
- `sed -n '1,20p' benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_context_07_08_n10000000_p512_repeat10_20260517_115204/summary_rdot_mpfr_context_07_08_n10000000_p512_repeat10.csv`
- `objdump -d -C --start-address=0x38a0 --stop-address=0x39a0 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_mkII_FMA`
- `objdump -d -C --start-address=0x38e0 --stop-address=0x3a20 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_08_mkII`
- `objdump -d -C --start-address=0x3a10 --stop-address=0x3b40 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07_mkII_FMA`
- `objdump -d -C --start-address=0x3b70 --stop-address=0x3cd0 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_08_mkII`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Release reconfigure: PASS.
- New benchmark target build: PASS.
- Focused benchmark: PASS.  60/60 timed runs reported `OK`.
- Plot and CSV generation: PASS.
- Hotpath disassembly extraction: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- OpenMP `07`/`08` measurements still have normal run-to-run variance; compare
  average and min/max range, not max alone.

## Phase: MPFR Explicit Evaluation Context

Implemented features:
- Added `mpfrxx::evaluation_context` as an explicit precision and rounding
  carrier for MPFR evaluation.
- Added `mpfrxx::with_context(mpfr_class&, evaluation_context)`, returning a
  context-bound `mpfr_context_ref` proxy.
- Added context-aware `operator+=`, `operator-=`, `operator*=`, and
  `operator/=` on the proxy, so kernels can hoist rounding selection outside
  hot loops without changing the default `mpfr_class` operators.
- Added context-aware `operator=` on the proxy so reusable temporary kernels can
  materialize `templ = dx[i] * dy[i]` under the explicit rounding context.
- Added `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07.cpp` as an
  OpenMP Rdot kernel derived from `kernel_openmp_01`, using
  `mpfrxx::with_context` for the thread-local accumulator and final reduction.
- Added `Rdot_mpfr_kernel_openmp_07_mkII` and
  `Rdot_mpfr_kernel_openmp_07_mkII_FMA` benchmark targets.
- Added `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_08.cpp` as an
  OpenMP Rdot kernel derived from `kernel_openmp_03`, using
  `mpfrxx::with_context` for both reusable product materialization and
  accumulation.
- Added the `Rdot_mpfr_kernel_openmp_08_mkII` benchmark target.
- Added `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07.cpp` as the serial
  counterpart to `kernel_openmp_07`.
- Added `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_08.cpp` as the serial
  counterpart to `kernel_openmp_08`.
- Added `Rdot_mpfr_kernel_07_mkII`, `Rdot_mpfr_kernel_07_mkII_FMA`, and
  `Rdot_mpfr_kernel_08_mkII` benchmark targets.
- Refactored MPFR compound assignment through
  `mpfr_compound_assign_with_context`, while preserving the existing default
  rounding behavior for normal `mpfr_class& operator+=` and related operators.
- `with_context` rejects a precision mismatch between the context and target
  object to preserve the existing destination-precision policy.

Missing features:
- No repeat benchmark data has been collected for `kernel_openmp_07` or
  `kernel_openmp_08`, or their serial counterparts yet.
- No README benchmark table has been updated for `kernel_07`,
  `kernel_08`, `kernel_openmp_07`, or `kernel_openmp_08` yet.

Tests added:
- `tests/test_mpfr_evaluation_context.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_08.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_08.cpp`

Tests updated:
- `benchmarks/CMakeLists.txt`
- `tests/CMakeLists.txt`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `STATUS.md`

Exact commands run:
- `rg -n "struct eval_context|class eval_context|current_eval_context|mpfr_compound_assign|operator\\+=|class mpfr_class|namespace mpfrxx|rounding" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/eval_context.hpp include/gmpfrxx_mkII/detail/environment.hpp include/gmpfrxx_mkII/detail/config.hpp tests benchmarks/mpfr/00_Rdot -g '!**/results_raw/**'`
- `git status --short`
- `rg --files tests | rg 'mpfr|fixed|round|context'`
- `nl -ba include/gmpfrxx_mkII/detail/eval_context.hpp | sed -n '1,120p'`
- `nl -ba include/gmpfrxx_mkII/detail/mpfr_impl.hpp | sed -n '110,650p'`
- `nl -ba include/gmpfrxx_mkII/detail/mpfr_impl.hpp | sed -n '2110,2525p'`
- `nl -ba include/gmpfrxx_mkII/detail/mpfr_impl.hpp | sed -n '2720,2795p'`
- `nl -ba include/gmpfrxx_mkII/detail/mpfr_impl.hpp | sed -n '1,120p'`
- `nl -ba tests/CMakeLists.txt | sed -n '1,130p'`
- `nl -ba tests/test_mpfr_compound_assign.cpp | sed -n '1,130p'`
- `tail -n 80 STATUS.md`
- `cmake --build build -j`
- `git diff -- include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_evaluation_context.cpp tests/CMakeLists.txt`
- `ctest --test-dir build -R test_mpfr_evaluation_context --output-on-failure`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `ls benchmarks/mpfr/00_Rdot`
- `nl -ba benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_01.cpp | sed -n '1,220p'`
- `rg -n "Rdot_mpfr_kernel_openmp|add_mpfr|00_Rdot|kernel_openmp_0" benchmarks/CMakeLists.txt benchmarks/mpfr/00_Rdot -g '!**/results_raw/**'`
- `cmake -S . -B build`
- `cmake --build build --target Rdot_mpfr_kernel_openmp_07_mkII_FMA -j`
- `cmake --build build --target Rdot_mpfr_kernel_openmp_07_mkII -j`
- `env OMP_NUM_THREADS=2 build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07_mkII_FMA 128 512`
- `env OMP_NUM_THREADS=2 build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07_mkII 128 512`
- `nm -C build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07_mkII_FMA | rg "_Rdot|mpfr_fma|mpfr_get_default_rounding_mode"`
- `objdump -Cd build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07_mkII_FMA | rg -n "mpfr_fma@plt|mpfr_get_default_rounding_mode@plt|%fs:"`
- `objdump -Cd --start-address=0x6380 --stop-address=0x63e0 build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07_mkII_FMA`
- `nl -ba benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_03.cpp | sed -n '1,220p'`
- `nl -ba benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03.cpp | sed -n '1,140p'`
- `rg -n "void mpfr_evaluate|mpfr_try_assign_direct_leaf_binary|object_leaf|scalar_leaf" include/gmpfrxx_mkII/detail/mpfr_impl.hpp | head -n 80`
- `nl -ba include/gmpfrxx_mkII/detail/mpfr_impl.hpp | sed -n '1800,2115p'`
- `cmake --build build --target Rdot_mpfr_kernel_openmp_08_mkII -j`
- `cmake --build build --target test_mpfr_evaluation_context -j`
- `env OMP_NUM_THREADS=2 build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_08_mkII 128 512`
- `printf '%s\n' ... | g++ -x c++ - -lmpfr -lgmp -o /tmp/find_mpfr_round && /tmp/find_mpfr_round`
- `printf '%s\n' ... | g++ -x c++ -std=c++17 -Iinclude - -lmpfr -lgmp -o /tmp/test_wrapper_round && /tmp/test_wrapper_round`
- `build/tests/test_mpfr_evaluation_context`
- `cmake --build build -j`
- `nm -C build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_08_mkII | rg "_Rdot|mpfr_mul|mpfr_add|mpfr_fma|mpfr_get_default_rounding_mode"`
- `objdump -Cd build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_08_mkII | rg -n "mpfr_mul@plt|mpfr_add@plt|mpfr_fma@plt|mpfr_get_default_rounding_mode@plt|%fs:"`
- `cmake --build build --target Rdot_mpfr_kernel_07_mkII Rdot_mpfr_kernel_07_mkII_FMA Rdot_mpfr_kernel_08_mkII -j`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_mkII 128 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_mkII_FMA 128 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_08_mkII 128 512`
- `nm -C build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_mkII_FMA | rg "mpfr_fma|mpfr_mul|mpfr_add|mpfr_get_default_rounding_mode"`
- `nm -C build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_08_mkII | rg "mpfr_fma|mpfr_mul|mpfr_add|mpfr_get_default_rounding_mode"`
- `objdump -Cd build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_mkII_FMA | rg -n "mpfr_fma@plt|mpfr_mul@plt|mpfr_add@plt|mpfr_get_default_rounding_mode@plt"`

Pass/fail result:
- Build: PASS.
- `Rdot_mpfr_kernel_openmp_07_mkII`: PASS.
- `Rdot_mpfr_kernel_openmp_07_mkII_FMA`: PASS.
- `Rdot_mpfr_kernel_openmp_07_mkII_FMA 128 512`: PASS.  `DIFF ... OK`.
- `Rdot_mpfr_kernel_openmp_07_mkII 128 512`: PASS.  `DIFF ... OK`.
- `Rdot_mpfr_kernel_openmp_08_mkII`: PASS.
- `Rdot_mpfr_kernel_openmp_08_mkII 128 512`: PASS.  `DIFF ... OK`.
- `Rdot_mpfr_kernel_07_mkII`: PASS.
- `Rdot_mpfr_kernel_07_mkII_FMA`: PASS.
- `Rdot_mpfr_kernel_08_mkII`: PASS.
- `Rdot_mpfr_kernel_07_mkII 128 512`: PASS.  `DIFF ... OK`.
- `Rdot_mpfr_kernel_07_mkII_FMA 128 512`: PASS.  `DIFF ... OK`.
- `Rdot_mpfr_kernel_08_mkII 128 512`: PASS.  `DIFF ... OK`.
- FMA disassembly check: PASS.  `Rdot_mpfr_kernel_openmp_07_mkII_FMA`
  calls `mpfr_fma@plt`.
- Serial FMA disassembly check: PASS.  `Rdot_mpfr_kernel_07_mkII_FMA`
  calls `mpfr_fma@plt`.
- `kernel_openmp_08` disassembly check: PASS.  It calls `mpfr_mul@plt` and
  `mpfr_add@plt`, matching the reusable product source shape from
  `kernel_openmp_03`.
- `kernel_08` symbol check: PASS.  It references `mpfr_mul` and `mpfr_add`,
  matching the reusable product source shape from `kernel_03`.
- `test_mpfr_evaluation_context`: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- The default `mpfr_class` operators still use the default rounding path.
  Rounding lookup is avoided only when callers explicitly use
  `mpfrxx::with_context`.

## Phase: GMP Rdot Bandwidth Estimates

Implemented features:
- Added `benchmarks/gmp/00_Rdot/README.md` memory-bandwidth estimates for
  representative top Rdot paths.
- Documented the 512-bit `mpf_t` layout used for the estimate:
  24-byte `__mpf_struct`, 8-byte limbs, 8 used limbs, and 9 allocated limbs.
- Added active-limb, header-inclusive, and allocated-footprint GB/s formulas.
- Recorded representative serial and OpenMP GB/s estimates for top native,
  upstream `gmpxx.h`, and mkII paths.

Missing features:
- No hardware-counter memory-bandwidth measurement was added.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `pgrep -af 'Rdot_mpfr|benchmark_rdot_mpfr|build_bench_release/benchmarks/mpfr/00_Rdot'`
- `sed -n '80,260p' benchmarks/gmp/00_Rdot/README.md`
- `sed -n '1,80p' benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/summary_rdot_gmp_n10000000_p512_repeat10.csv`
- `printf '%s\n' ... | g++ -x c++ -std=c++17 - -lgmp -o /tmp/gmp_rdot_layout && /tmp/gmp_rdot_layout`
- `printf '%s\n' ... | g++ -x c++ -std=c++17 - -lgmp -o /tmp/gmp_rdot_used_limbs && /tmp/gmp_rdot_used_limbs`
- `python3 - <<'PY' ...`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- GMP 512-bit layout check: PASS.  `sizeof(__mpf_struct)=24`,
  `sizeof(mp_limb_t)=8`, `mpf_get_prec=512`, `used_limbs=8`,
  `allocated_limbs=9`.
- Bandwidth conversion: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  157/157 tests passed.

Known issues:
- GB/s values are modeled from MFLOPS and object layout, not measured with
  performance counters.

## Phase: GMP Rdot Sortable Views and Vertical Repeat Plots

Implemented features:
- Changed `benchmarks/gmp/00_Rdot/plot_repeat_summary.py` back to vertical
  average-MFLOPS bars with min/max whiskers and numeric labels.
- Regenerated the committed GMP Rdot serial and OpenMP repeat-10 plots.
- Added README collapsible sorted views for serial and OpenMP results sorted by
  `Max MFLOPS` and `Avg MFLOPS`.
- Documented that GitHub Markdown cannot run JavaScript table sorting inside a
  README, so the sorted views are precomputed from the committed summary CSV.

Missing features:
- No JavaScript-powered interactive sorting is available in GitHub README
  rendering.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/gmp/00_Rdot/plot_repeat_summary.py`
- `benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/rdot_gmp_n10000000_p512_repeat10_serial.png`
- `benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/rdot_gmp_n10000000_p512_repeat10_openmp.png`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/00_Rdot/plot_repeat_summary.py`
- `sed -n '80,190p' benchmarks/gmp/00_Rdot/README.md`
- `head -n 40 benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/summary_rdot_gmp_n10000000_p512_repeat10.csv`
- `python3 - <<'PY' ...`
- `python3 benchmarks/gmp/00_Rdot/plot_repeat_summary.py benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/summary_rdot_gmp_n10000000_p512_repeat10.csv --output-prefix benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/rdot_gmp_n10000000_p512_repeat10 --title-prefix 'GMP Rdot N=10000000 precision=512 repeat=10'`
- `python3 -m py_compile benchmarks/gmp/00_Rdot/plot_repeat_summary.py`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Plot regeneration: PASS.
- Python syntax check: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  157/157 tests passed.

Known issues:
- None.

## Phase: GMP Rdot Repeat Plot Readability

Implemented features:
- Added `benchmarks/gmp/00_Rdot/plot_repeat_summary.py`.
- Regenerated the committed GMP Rdot repeat-10 serial and OpenMP plots as
  average-MFLOPS horizontal bars with min/max range lines and numeric labels.
- Documented the plot interpretation and regeneration command in
  `benchmarks/gmp/00_Rdot/README.md`.

Missing features:
- None.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/rdot_gmp_n10000000_p512_repeat10_serial.png`
- `benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/rdot_gmp_n10000000_p512_repeat10_openmp.png`
- `STATUS.md`

Exact commands run:
- `pgrep -af 'Rdot_mpfr|benchmark_rdot_mpfr|build_bench_release/benchmarks/mpfr/00_Rdot'`
- `kill 1570274 1573325 1573326`
- `sed -n '1,260p' benchmarks/gmp/00_Rdot/plot.py`
- `sed -n '1,180p' benchmarks/gmp/00_Rdot/README.md`
- `ls -l benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207`
- `head -n 12 benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/summary_rdot_gmp_n10000000_p512_repeat10.csv`
- `tail -n 12 benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/summary_rdot_gmp_n10000000_p512_repeat10.csv`
- `python3 benchmarks/gmp/00_Rdot/plot_repeat_summary.py benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/summary_rdot_gmp_n10000000_p512_repeat10.csv --output-prefix benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/rdot_gmp_n10000000_p512_repeat10 --title-prefix 'GMP Rdot N=10000000 precision=512 repeat=10'`
- `file benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/rdot_gmp_n10000000_p512_repeat10_serial.png benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/rdot_gmp_n10000000_p512_repeat10_openmp.png`
- `python3 -m py_compile benchmarks/gmp/00_Rdot/plot_repeat_summary.py`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Plot regeneration: PASS.
- Python syntax check: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  157/157 tests passed.

Known issues:
- None.

## Phase: GMP Rdot C-Native Kernel Realignment and Fresh Repeat-10 Results

Implemented features:
- Realigned GMP Rdot raw C kernels `01` through `06` with the existing C++
  wrapper kernel source shapes.
- Added raw C OpenMP counterparts `C_native_openmp_01` through
  `C_native_openmp_06`.
- Removed allocation-counter instrumentation from the raw C Rdot kernels so
  the raw C comparison is based on timed source shape and hotpath disassembly.
- Reran the GMP Rdot benchmark at `N=10000000`, `precision=512`,
  `repeat=10`, `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, and
  `OMP_PROC_BIND=spread`.
- Added raw CSV, summary CSV, and serial/OpenMP plots under
  `benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/`.
- Rewrote `benchmarks/gmp/00_Rdot/README.md` to use only the fresh data and
  removed old benchmark-result references.
- Added kernel 05 serial/OpenMP hotpath disassembly to document that 05 is a
  four-way unroll of the same GMP multiply/add pair rather than a separate
  arithmetic kernel class.
- Removed obsolete tracked GMP Rdot result directories under
  `benchmarks/gmp/results_raw/`.

Missing features:
- None.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/gmp/00_Rdot/results_raw/rdot_gmp_n10000000_p512_repeat10_20260516_210207/`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rdot_gmp_C_native_01 Rdot_gmp_C_native_02 Rdot_gmp_C_native_03 Rdot_gmp_C_native_04 Rdot_gmp_C_native_05 Rdot_gmp_C_native_06 Rdot_gmp_C_native_openmp_01 Rdot_gmp_C_native_openmp_02 Rdot_gmp_C_native_openmp_03 Rdot_gmp_C_native_openmp_04 Rdot_gmp_C_native_openmp_05 Rdot_gmp_C_native_openmp_06`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close ...` smoke run
  for the 12 raw C kernels.
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target Rdot_gmp_C_native_01 Rdot_gmp_C_native_02 Rdot_gmp_C_native_03 Rdot_gmp_C_native_04 Rdot_gmp_C_native_05 Rdot_gmp_C_native_06 Rdot_gmp_C_native_openmp_01 Rdot_gmp_C_native_openmp_02 Rdot_gmp_C_native_openmp_03 Rdot_gmp_C_native_openmp_04 Rdot_gmp_C_native_openmp_05 Rdot_gmp_C_native_openmp_06`
- `ctest --test-dir build --output-on-failure`
- `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread ...` full
  repeat-10 GMP Rdot run for all 48 variants.
- `python3 - <<'PY' ...` to generate raw CSV, summary CSV, and plots.
- `objdump -Cd --no-show-raw-insn --start-address=... --stop-address=...`
  for `C_native_01`, `C_native_03`, `kernel_03_mkII`,
  `C_native_openmp_03`, and `kernel_openmp_03_mkII`.
- `objdump -Cd --no-show-raw-insn --start-address=... --stop-address=...`
  for `C_native_05`, `kernel_05_mkII`, `C_native_openmp_05`, and
  `kernel_openmp_05_mkII`.
- `git rm -r benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513 benchmarks/gmp/results_raw/rdot_n1e7_1024_01_06_env1024_20260513 benchmarks/gmp/results_raw/rdot_n1e7_20260509 benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513 benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Release raw C target build: PASS.
- Raw C smoke run: PASS.
- Debug raw C target build: PASS.
- CTest before the full benchmark: PASS.  157/157 tests passed.
- Full repeat-10 benchmark: PASS.  480/480 runs reported `OK`.
- CSV and plot generation: PASS.
- `git diff --check`: PASS.
- Final CTest: PASS.  157/157 tests passed.

Known issues:
- None.

## Phase: GMP Rdot C-Native Kernel Alignment

Implemented features:
- Added independent raw `mpf_t` Rdot programs for `C_native_02` through
  `C_native_06` and `C_native_openmp_02` through `C_native_openmp_06`.
- Aligned C-native numbering with the C++ wrapper kernels:
  `C_native_NN` now mirrors `kernel_NN`, and `C_native_openmp_NN` mirrors
  `kernel_openmp_NN`.
- Kept C-native timed kernels free of allocation-counter instrumentation so
  hotpath disassembly reflects the raw GMP loop.
- Updated the Rdot common runner, local `go.sh`, CMake target registration,
  and README kernel-shape notes.

Missing features:
- No new benchmark sweep was run.
- Hotpath disassembly snippets in the README were not regenerated in this
  phase.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/gmp/00_Rdot/go.sh`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_02.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_03.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_04.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_05.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_06.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_01.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_02.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_03.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_04.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_05.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_06.cpp`
- `STATUS.md`

Exact commands run:
- `find benchmarks/gmp -maxdepth 2 -type f | sort | sed -n '1,220p'`
- `sed -n '1,260p' benchmarks/gmp/00_Rdot/README.md`
- `sed -n '1,260p' benchmarks/gmp/02_Rgemv/README.md`
- `sed -n '1,220p' benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `sed -n '1,240p' benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_01.cpp`
- `sed -n '150,225p' benchmarks/CMakeLists.txt`
- `sed -n '1,220p' benchmarks/gmp/00_Rdot/go.sh`
- `sed -n '1,220p' benchmarks/gmp/00_Rdot/plot.py`
- `rg -n "Rdot_gmp_C_native|Rdot_gmp_kernel_0|C_native_openmp_0" benchmarks/common benchmarks/gmp/00_Rdot benchmarks/CMakeLists.txt`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rdot_gmp_C_native_01 Rdot_gmp_C_native_02 Rdot_gmp_C_native_03 Rdot_gmp_C_native_04 Rdot_gmp_C_native_05 Rdot_gmp_C_native_06 Rdot_gmp_C_native_openmp_01 Rdot_gmp_C_native_openmp_02 Rdot_gmp_C_native_openmp_03 Rdot_gmp_C_native_openmp_04 Rdot_gmp_C_native_openmp_05 Rdot_gmp_C_native_openmp_06`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close bash -lc 'set -euo pipefail; dir=build_bench_release/benchmarks/gmp/00_Rdot; for exe in ...; do ...; done'`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target Rdot_gmp_C_native_01 Rdot_gmp_C_native_02 Rdot_gmp_C_native_03 Rdot_gmp_C_native_04 Rdot_gmp_C_native_05 Rdot_gmp_C_native_06 Rdot_gmp_C_native_openmp_01 Rdot_gmp_C_native_openmp_02 Rdot_gmp_C_native_openmp_03 Rdot_gmp_C_native_openmp_04 Rdot_gmp_C_native_openmp_05 Rdot_gmp_C_native_openmp_06`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Release target build: PASS.
- C-native smoke run: PASS.  12/12 variants reported `OK`.
- Allocation-counter output check for C-native smoke run: PASS.  No
  `BENCH_ALLOC_COUNTS` output was emitted.
- Debug target build: PASS.
- CTest: PASS.  157/157 tests passed.

Known issues:
- Current README disassembly addresses are from earlier binaries and should be
  regenerated after the next benchmark/disassembly pass.

## Phase: MPFR Fixed-Precision Fastpath Coverage

Implemented features:
- Fixed MPFR benchmark variant generation so suffixes containing
  `FIXED_PRECISION_FASTPATH` now define
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`.
- Kept FMA and stable-rounding benchmark options independent:
  `mkII_FIXED_PRECISION_FASTPATH_FMA` now receives both the fixed-precision
  fastpath definition and `MPFRXX_ENABLE_FMA`.
- Added an MPFR thread-local scratch pool mirroring the `mpf_class` fixed
  precision fastpath.
- Used the scratch pool for non-FMA `mpfr_class` compound multiply-add and
  multiply-subtract paths so fixed-precision `lhs += x * y` and `lhs -= x * y`
  avoid steady-state `mpfr_init2`/`mpfr_clear` traffic.

Missing features:
- None.

Tests added:
- `test_mpfr_fixed_precision_tls_scratch`

Tests updated:
- `benchmarks/CMakeLists.txt`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `grep -n "mpfr_set_prec_raw" /usr/include/mpfr.h`
- `sed -n '1600,1882p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1918,2058p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1,180p' tests/test_mpf_fixed_precision_tls_scratch.cpp`
- `sed -n '1,160p' tests/test_mpfr_alloc_count.cpp`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpfr_fixed_precision_tls_scratch test_mpfr_fixed_precision_fma_alloc_count test_mpfr_compound_assign`
- `ctest --test-dir build -R "test_mpfr_fixed_precision_tls_scratch|test_mpfr_fixed_precision_fma_alloc_count|test_mpfr_compound_assign" --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `mpfr_set_prec_raw` availability check: PASS.
- Focused build: PASS.
- Focused CTest: PASS.  3/3 tests passed.
- Full build: PASS.
- Full CTest: PASS.  157/157 tests passed.

Known issues:
- None.

## Phase: GMP Rgemv Repeat-10 Full Rerun

Implemented features:
- Removed the committed
  `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/`
  result set as requested.
- Rebuilt all GMP Rgemv benchmark targets and reran the complete 44-variant
  `benchmarks/gmp/02_Rgemv` suite with `repeat=10`, `M=4000`, `N=4000`,
  512-bit precision, and 32 OpenMP threads.
- Added the new repeat-10 result set under
  `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/`.
- Generated raw CSV, summary CSV, and serial/OpenMP repeat-10 plots.
- Updated `benchmarks/gmp/02_Rgemv/README.md` to use repeat-10 averages,
  min/max ranges, memory-bandwidth estimates, and the new inline plots.

Missing features:
- No hardware-counter measurement was added.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/02_Rgemv/README.md`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/benchmark_rgemv_gmp_m4000_n4000_p512_repeat10.log`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/raw_rgemv_gmp_m4000_n4000_p512_repeat10.csv`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/summary_rgemv_gmp_m4000_n4000_p512_repeat10.csv`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/singlecore_mflops_repeat10_summary.png`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/singlecore_mflops_repeat10_summary.pdf`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/openmp_mflops_repeat10_summary.png`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/openmp_mflops_repeat10_summary.pdf`
- `STATUS.md`

Exact commands run:
- `git status --short`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/go.sh`
- `git ls-files benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342`
- `git rm -r benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342`
- `cmake --build build_bench_release -j --target Rgemv_gmp_C_native_01 Rgemv_gmp_C_native_02 Rgemv_gmp_C_native_03 Rgemv_gmp_C_native_04 Rgemv_gmp_C_native_openmp_01 Rgemv_gmp_C_native_openmp_02 Rgemv_gmp_C_native_openmp_03 Rgemv_gmp_C_native_openmp_04 Rgemv_gmp_C_native_openmp_05 Rgemv_gmp_C_native_openmp_06 Rgemv_gmp_C_native_openmp_07 Rgemv_gmp_kernel_01_orig Rgemv_gmp_kernel_01_mkII Rgemv_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_02_orig Rgemv_gmp_kernel_02_mkII Rgemv_gmp_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_03_orig Rgemv_gmp_kernel_03_mkII Rgemv_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_04_orig Rgemv_gmp_kernel_04_mkII Rgemv_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_01_orig Rgemv_gmp_kernel_openmp_01_mkII Rgemv_gmp_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_02_orig Rgemv_gmp_kernel_openmp_02_mkII Rgemv_gmp_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_03_orig Rgemv_gmp_kernel_openmp_03_mkII Rgemv_gmp_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_04_orig Rgemv_gmp_kernel_openmp_04_mkII Rgemv_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_05_orig Rgemv_gmp_kernel_openmp_05_mkII Rgemv_gmp_kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_06_orig Rgemv_gmp_kernel_openmp_06_mkII Rgemv_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_07_orig Rgemv_gmp_kernel_openmp_07_mkII Rgemv_gmp_kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH`
- `/bin/bash -lc 'set -euo pipefail; ts=$(date +%Y%m%d_%H%M%S); outdir=/home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_${ts}; mkdir -p "$outdir"; cd /home/docker/gmpfrxx_mkII/build_bench_release/benchmarks/gmp/02_Rgemv; ... repeat 10 times ...'`
- `python3 - "$outdir" <<'PY' ... PY`
- `perl -0pi -e 's/\r\n/\n/g' benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/raw_rgemv_gmp_m4000_n4000_p512_repeat10.csv benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_repeat10_20260516_184101/summary_rgemv_gmp_m4000_n4000_p512_repeat10.csv`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Target build: PASS.
- Full repeat-10 Rgemv benchmark rerun: PASS.  440/440 timed runs reported
  `Result OK`.
- CSV and plot generation: PASS.  440 raw rows and 44 summary rows generated.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The OpenMP 06/07 variants still show visible run-to-run variation; use
  average and max separately when discussing peak versus typical performance.

## Phase: GMP Rgemv Full Rerun After Result Cleanup

Implemented features:
- Removed the superseded GMP Rgemv result sets requested for deletion:
  `20260516_122201` and `20260516_132406`.
- Rebuilt the release benchmark tree and reran the complete
  `benchmarks/gmp/02_Rgemv/go.sh` set at `M=4000`, `N=4000`, 512-bit
  precision, and 32 OpenMP threads.
- Generated a new raw log, CSV summary, and serial/OpenMP plots under
  `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/`.
- Updated `benchmarks/gmp/02_Rgemv/README.md` to reference the new full rerun
  instead of the deleted result sets, including the MFLOPS table and logical
  memory-bandwidth estimates.

Missing features:
- No repeat-count sweep or hardware-counter measurement was added in this
  phase.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/02_Rgemv/README.md`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/benchmark_rgemv_gmp_m4000_n4000_p512.log`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/summary_rgemv_gmp_m4000_n4000_p512.csv`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/singlecore_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.png`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/singlecore_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.pdf`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/openmp_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.png`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/openmp_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.pdf`
- `STATUS.md`

Exact commands run:
- `git status --short`
- `git ls-files benchmarks/gmp/02_Rgemv/results_raw | rg '20260516_(122201|13240)'`
- `rg -n '20260516_(122201|13240|132406)|rgemv_gmp_m4000_n4000_p512' benchmarks/gmp/02_Rgemv/README.md STATUS.md`
- `git rm benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_122201.log benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_summary_20260516_122201.csv benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/benchmark_rgemv_gmp_m4000_n4000_p512.log benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/openmp_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.pdf benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/openmp_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.png benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/singlecore_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.pdf benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/singlecore_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.png benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/summary_rgemv_gmp_m4000_n4000_p512.csv`
- `cmake --build build_bench_release -j`
- `/bin/bash -lc 'set -euo pipefail; ts=$(date +%Y%m%d_%H%M%S); outdir=/home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_${ts}; mkdir -p "$outdir"; cd /home/docker/gmpfrxx_mkII/build_bench_release/benchmarks/gmp/02_Rgemv; OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread /home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/go.sh 2>&1 | tee "$outdir/benchmark_rgemv_gmp_m4000_n4000_p512.log"; printf "OUTDIR=%s\n" "$outdir"'`
- `/bin/bash -lc 'set -euo pipefail; outdir=/home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342; cd "$outdir"; python3 /home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/plot.py benchmark_rgemv_gmp_m4000_n4000_p512.log'`
- `/bin/bash -lc 'set -euo pipefail; outdir=/home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342; python3 - "$outdir" <<"PY" ... PY'`
- `ls -lh benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342`
- `sed -n '1,80p' benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_135342/summary_rgemv_gmp_m4000_n4000_p512.csv`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Release build: PASS.
- Full Rgemv benchmark rerun: PASS.  All 44 variants reported `Result OK`.
- CSV summary generation: PASS.  44 rows parsed.
- Plot generation: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The run is still a single measurement per variant; OpenMP variants should be
  compared with repeat-count data before drawing fine-grained conclusions.

## Phase: GMP Rgemv C Native OpenMP Numbering Alignment

Implemented features:
- Added raw GMP C native OpenMP `Rgemv` kernels 01, 02, and 03 so their
  source shapes match the existing C++ wrapper OpenMP kernels.
- Renamed the previous raw OpenMP baseline to
  `Rgemv_gmp_C_native_openmp_03`.
- Added `Rgemv_gmp_C_native_openmp_02` with per-thread reusable temporaries
  and copy-then-multiply steps.
- Added `Rgemv_gmp_C_native_openmp_01` with loop-local product materialization
  as the OpenMP counterpart of `kernel_openmp_01`.
- Registered the new OpenMP native targets in `benchmarks/CMakeLists.txt` and
  `benchmarks/gmp/02_Rgemv/go.sh`.
- Updated `benchmarks/gmp/02_Rgemv/README.md` with serial and OpenMP C native
  numbering tables aligned with the wrapper kernels.

Missing features:
- No new full-size benchmark sweep was run for the newly aligned OpenMP native
  numbering.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_01.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_02.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_03.cpp`
- `benchmarks/gmp/02_Rgemv/go.sh`
- `benchmarks/gmp/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `git status --short`
- `rg -n "Rgemv_gmp_C_native_openmp|Rgemv_gmp_C_native_0|Rgemv_gmp_kernel_openmp" benchmarks/CMakeLists.txt benchmarks/gmp/02_Rgemv/go.sh benchmarks/gmp/02_Rgemv/README.md`
- `rg --files benchmarks/gmp/02_Rgemv | sort`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_01.cpp`
- `sed -n '1,240p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_02.cpp`
- `sed -n '1,240p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_03.cpp`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_01.cpp && sed -n '1,240p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_02.cpp && sed -n '1,240p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_03.cpp`
- `sed -n '280,310p' benchmarks/CMakeLists.txt`
- `sed -n '24,62p' benchmarks/gmp/02_Rgemv/go.sh`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/README.md`
- `sed -n '220,420p' benchmarks/gmp/02_Rgemv/README.md`
- `sed -n '1,80p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_03.cpp`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemv_gmp_C_native_openmp_01 Rgemv_gmp_C_native_openmp_02 Rgemv_gmp_C_native_openmp_03 Rgemv_gmp_kernel_openmp_01_mkII Rgemv_gmp_kernel_openmp_02_mkII Rgemv_gmp_kernel_openmp_03_mkII`
- `OMP_NUM_THREADS=4 OMP_PLACES=cores OMP_PROC_BIND=close /bin/bash -lc 'for exe in Rgemv_gmp_C_native_openmp_01 Rgemv_gmp_C_native_openmp_02 Rgemv_gmp_C_native_openmp_03 Rgemv_gmp_kernel_openmp_01_mkII Rgemv_gmp_kernel_openmp_02_mkII Rgemv_gmp_kernel_openmp_03_mkII; do echo "== $exe"; build_bench_release/benchmarks/gmp/02_Rgemv/$exe 17 19 128 | tail -n 3; done'`
- `git diff -- benchmarks/CMakeLists.txt benchmarks/gmp/02_Rgemv/go.sh benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_03.cpp benchmarks/gmp/02_Rgemv/README.md`
- `git diff -- benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_01.cpp benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_02.cpp benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_03.cpp`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Release reconfigure: PASS.
- OpenMP Rgemv native/wrapper target build: PASS.
- OpenMP smoke run: PASS.  All six sampled executables reported `Result OK`.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The recorded full-size Rgemv sample predates the new C native OpenMP 01/02
  executables, so the README marks them as `Not in this run`.

## Phase: GMP Rgemv 4000x4000 Benchmark

Implemented features:
- Ran the current GMP `02_Rgemv` benchmark matrix with the aligned C native
  serial/OpenMP 01, 02, and 03 kernels included.
- Used the current `benchmarks/gmp/02_Rgemv/go.sh` benchmark order.
- Generated raw log, summary CSV, and serial/OpenMP plots for the run.

Missing features:
- No repeat-count sweep was run; this is a single `go.sh` pass.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209/benchmark_rgemv_gmp_m4000_n4000_p512.log`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209/summary_rgemv_gmp_m4000_n4000_p512.csv`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209/singlecore_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.png`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209/singlecore_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.pdf`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209/openmp_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.png`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209/openmp_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.pdf`
- `STATUS.md`

Exact commands run:
- `sed -n '1,180p' benchmarks/gmp/02_Rgemv/go.sh`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/plot.py`
- `rg -n "Rgemv_gmp_C_native|Rgemv_gmp_kernel" benchmarks/CMakeLists.txt benchmarks/gmp/02_Rgemv/go.sh`
- `cmake --build build_bench_release -j`
- `/bin/bash -lc 'set -euo pipefail; ts=$(date +%Y%m%d_%H%M%S); outdir=/home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_${ts}; mkdir -p "$outdir"; cd /home/docker/gmpfrxx_mkII/build_bench_release/benchmarks/gmp/02_Rgemv; OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread /home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/go.sh 2>&1 | tee "$outdir/benchmark_rgemv_gmp_m4000_n4000_p512.log"; printf "%s\n" "$outdir"'`
- `/bin/bash -lc 'set -euo pipefail; outdir=/home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209; cd "$outdir"; python3 /home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/plot.py benchmark_rgemv_gmp_m4000_n4000_p512.log'`
- `python3 -c 'import csv,re,pathlib; ...'`
- `ls -lh /home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_130209`
- `git status --short`

Pass/fail result:
- Full benchmark target build: PASS.
- Benchmark run: PASS.  All 27 executables reported `Result OK`.
- Plot generation: PASS.
- Summary CSV generation: PASS.

Known issues:
- OpenMP results are single-run measurements and show normal run-to-run
  variation.

## Phase: GMP Rgemv Kernel 04 Native/OpenMP Alignment

Implemented features:
- Added `Rgemv_gmp_C_native_04` as the raw GMP counterpart of wrapper
  `kernel_04`.
- Added `Rgemv_gmp_C_native_openmp_04` as the raw GMP OpenMP counterpart of
  wrapper `kernel_openmp_04`.
- Added `Rgemv_gmp_kernel_openmp_04.cpp` so upstream `gmpxx.h`, `gmpxx_mkII`,
  and fixed-precision-fastpath OpenMP 04 variants are generated.
- Registered the new 04 targets in `benchmarks/CMakeLists.txt` and
  `benchmarks/gmp/02_Rgemv/go.sh`.
- Updated `benchmarks/gmp/02_Rgemv/README.md` so serial/OpenMP C native and
  wrapper numbering now covers 01, 02, 03, and 04.

Missing features:
- No full-size benchmark rerun was performed after adding the new 04 targets.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_04.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_04.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_04.cpp`
- `benchmarks/gmp/02_Rgemv/go.sh`
- `benchmarks/gmp/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_04.cpp`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_03.cpp`
- `rg -n "Rgemv_gmp_C_native_04|Rgemv_gmp_C_native_openmp_04|Rgemv_gmp_kernel_openmp_04|Rgemv_gmp_kernel_04" benchmarks/CMakeLists.txt benchmarks/gmp/02_Rgemv`
- `git status --short`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemv_gmp_C_native_04 Rgemv_gmp_C_native_openmp_04 Rgemv_gmp_kernel_openmp_04_orig Rgemv_gmp_kernel_openmp_04_mkII Rgemv_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH`
- `OMP_NUM_THREADS=4 OMP_PLACES=cores OMP_PROC_BIND=close /bin/bash -lc 'for exe in Rgemv_gmp_C_native_04 Rgemv_gmp_C_native_openmp_04 Rgemv_gmp_kernel_openmp_04_orig Rgemv_gmp_kernel_openmp_04_mkII Rgemv_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH; do echo "== $exe"; build_bench_release/benchmarks/gmp/02_Rgemv/$exe 17 19 128 | tail -n 3; done'`
- `git diff --check`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Release reconfigure: PASS.
- New 04 target build: PASS.
- New 04 smoke run: PASS.  All five sampled executables reported `Result OK`.
- `git diff --check`: PASS.
- Debug reconfigure: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- OpenMP 04 is intentionally a loop-local object stress case, not an optimized
  kernel.

## Phase: GMP Rgemv OpenMP Optimization Candidates 05-07

Implemented features:
- Added `Rgemv_gmp_kernel_openmp_05.cpp` and matching raw GMP
  `Rgemv_gmp_C_native_openmp_05.cpp`.
- Added `Rgemv_gmp_kernel_openmp_06.cpp` and matching raw GMP
  `Rgemv_gmp_C_native_openmp_06.cpp`.
- Added `Rgemv_gmp_kernel_openmp_07.cpp` and matching raw GMP
  `Rgemv_gmp_C_native_openmp_07.cpp`.
- Kernel 05 precomputes `scaled_x[j] = alpha * x[j]` once, then performs
  row-partitioned updates.
- Kernel 06 uses 256-row blocks so each OpenMP block owns a `y` slice while
  keeping contiguous `A` access inside the block.
- Kernel 07 partitions columns and accumulates into thread-local `y` partial
  vectors before a final reduction.
- Registered the new native and wrapper targets in `benchmarks/CMakeLists.txt`
  and `benchmarks/gmp/02_Rgemv/go.sh`.
- Updated `benchmarks/gmp/02_Rgemv/README.md` with the new kernel meanings.

Missing features:
- No full-size benchmark rerun was performed after adding kernels 05, 06, and
  07.
- Kernel 06 uses a fixed 256-row block size; no block-size sweep is included
  yet.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_05.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_06.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_07.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_05.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_06.cpp`
- `benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_07.cpp`
- `benchmarks/gmp/02_Rgemv/go.sh`
- `benchmarks/gmp/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '280,325p' benchmarks/CMakeLists.txt`
- `sed -n '30,70p' benchmarks/gmp/02_Rgemv/go.sh`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_03.cpp && sed -n '1,180p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_03.cpp`
- `git status --short`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemv_gmp_C_native_openmp_05 Rgemv_gmp_C_native_openmp_06 Rgemv_gmp_C_native_openmp_07 Rgemv_gmp_kernel_openmp_05_orig Rgemv_gmp_kernel_openmp_05_mkII Rgemv_gmp_kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_06_orig Rgemv_gmp_kernel_openmp_06_mkII Rgemv_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_07_orig Rgemv_gmp_kernel_openmp_07_mkII Rgemv_gmp_kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH`
- `OMP_NUM_THREADS=4 OMP_PLACES=cores OMP_PROC_BIND=close /bin/bash -lc 'for exe in Rgemv_gmp_C_native_openmp_05 Rgemv_gmp_C_native_openmp_06 Rgemv_gmp_C_native_openmp_07 Rgemv_gmp_kernel_openmp_05_orig Rgemv_gmp_kernel_openmp_05_mkII Rgemv_gmp_kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_06_orig Rgemv_gmp_kernel_openmp_06_mkII Rgemv_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_07_orig Rgemv_gmp_kernel_openmp_07_mkII Rgemv_gmp_kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH; do echo "== $exe"; build_bench_release/benchmarks/gmp/02_Rgemv/$exe 17 19 128 | tail -n 3; done'`
- `git diff --check`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Release reconfigure: PASS.
- New 05/06/07 target build: PASS.
- New 05/06/07 smoke run: PASS.  All twelve sampled executables reported
  `Result OK`.
- `git diff --check`: PASS.
- Debug reconfigure: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- Kernel 07 changes the accumulation order, so tiny nonzero L1 differences are
  expected for some variants while remaining within the correctness threshold.

## Phase: GMP Rgemv 4000x4000 Benchmark With OpenMP 05-07

Implemented features:
- Ran the current GMP `02_Rgemv` benchmark matrix after adding OpenMP kernels
  05, 06, and 07.
- Covered 44 executables: C native serial 01-04, C native OpenMP 01-07,
  wrapper serial 01-04, and wrapper OpenMP 01-07 for upstream `gmpxx.h`,
  `gmpxx_mkII`, and fixed-precision-fastpath builds.
- Generated raw log, summary CSV, and serial/OpenMP plots for the run.

Missing features:
- No repeat-count sweep was run; this is a single `go.sh` pass.
- Kernel 06 still uses a fixed 256-row block size.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/benchmark_rgemv_gmp_m4000_n4000_p512.log`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/summary_rgemv_gmp_m4000_n4000_p512.csv`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/singlecore_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.png`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/singlecore_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.pdf`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/openmp_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.png`
- `benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/openmp_operations_Linux_Ryzen_3970X_32-Core_4000_4000_512.pdf`
- `STATUS.md`

Exact commands run:
- `cmake --build build_bench_release -j`
- `/bin/bash -lc 'set -euo pipefail; ts=$(date +%Y%m%d_%H%M%S); outdir=/home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_${ts}; mkdir -p "$outdir"; cd /home/docker/gmpfrxx_mkII/build_bench_release/benchmarks/gmp/02_Rgemv; OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread /home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/go.sh 2>&1 | tee "$outdir/benchmark_rgemv_gmp_m4000_n4000_p512.log"; printf "%s\n" "$outdir"'`
- `/bin/bash -lc 'set -euo pipefail; outdir=/home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406; cd "$outdir"; python3 /home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/plot.py benchmark_rgemv_gmp_m4000_n4000_p512.log'`
- `python3 -c 'import csv,re,pathlib; ...'`
- `ls -lh /home/docker/gmpfrxx_mkII/benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406`
- `git status --short`

Pass/fail result:
- Full benchmark target build: PASS.
- Benchmark run: PASS.  All 44 executables reported `Result OK`.
- Plot generation: PASS.
- Summary CSV generation: PASS.
- Best raw C native OpenMP result: `Rgemv_gmp_C_native_openmp_07`,
  544.743 MFLOPS.
- Best wrapper OpenMP result: `Rgemv_gmp_kernel_openmp_07_mkII`,
  552.458 MFLOPS.
- Best row-blocked OpenMP result: `Rgemv_gmp_kernel_openmp_06_orig`,
  410.404 MFLOPS.
- Best precomputed-scaled-x OpenMP result:
  `Rgemv_gmp_kernel_openmp_05_mkII`, 293.997 MFLOPS.

Known issues:
- OpenMP results are single-run measurements and should be confirmed with a
  repeat-count sweep before drawing tight ordering conclusions.
- Kernel 07 changes the accumulation order, so tiny nonzero L1 differences are
  expected while remaining within the correctness threshold.

## Phase: GMP Rgemv Memory Bandwidth Documentation

Implemented features:
- Added a memory bandwidth estimate section to
  `benchmarks/gmp/02_Rgemv/README.md`.
- Documented the local GMP `mpf_t` layout used for the estimate:
  `sizeof(__mpf_struct) = 24`, `sizeof(mp_limb_t) = 8`,
  `_mp_prec = 9` limbs for `mpf_init2(..., 512)`, and 8 active limbs for the
  random benchmark values.
- Added active-limb conversion formulas:
  `A-only GB/s = MFLOPS * 0.044`,
  `A+y GB/s = MFLOPS * 0.132`, and
  `A+x+y GB/s = MFLOPS * 0.176`.
- Added a bandwidth table for `kernel_openmp_03_mkII`,
  `kernel_openmp_05_mkII`, `kernel_openmp_06_mkII`,
  `kernel_openmp_07_mkII`, and `C_native_openmp_07`.

Missing features:
- No hardware-counter validation was added.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,380p' benchmarks/gmp/02_Rgemv/README.md`
- `sed -n '1,80p' benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_20260516_132406/summary_rgemv_gmp_m4000_n4000_p512.csv`
- `git status --short`
- `printf '%s\n' ... | g++ -x c++ -std=c++17 - -lgmp -o /tmp/gmpfrxx_mkII_mpf_size && /tmp/gmpfrxx_mkII_mpf_size`

Pass/fail result:
- GMP layout probe: PASS.

Known issues:
- The bandwidth table is a logical active-limb estimate, not a measured memory
  controller bandwidth.

## Phase: GMP Rgemv C Native Numbering Alignment

Implemented features:
- Renamed the existing optimized GMP Rgemv C native implementation from
  `Rgemv_gmp_C_native_01.cpp` to `Rgemv_gmp_C_native_03.cpp` because its
  source shape matches C++ `kernel_03`.
- Added a new `Rgemv_gmp_C_native_01.cpp` that mirrors C++ `kernel_01` by
  materializing a loop-local product in the inner loop.
- Added `Rgemv_gmp_C_native_02.cpp` that mirrors C++ `kernel_02` with reusable
  temporaries and copy-then-multiply steps.
- Registered `Rgemv_gmp_C_native_01`, `Rgemv_gmp_C_native_02`, and
  `Rgemv_gmp_C_native_03` in the benchmark CMake file and `go.sh`.
- Added an explicit C/C++ numbering correspondence table to
  `benchmarks/gmp/02_Rgemv/README.md`.

Missing features:
- No large benchmark rerun was performed after the C native renumbering.
- OpenMP C native variants are still a separate baseline and were not
  renumbered in this phase.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/02_Rgemv/README.md`
- `benchmarks/gmp/02_Rgemv/go.sh`
- `STATUS.md`

Exact commands run:
- `git status --short benchmarks/gmp/02_Rgemv benchmarks/CMakeLists.txt`
- `rg -n "void _Rgemv|mpf_mul|mpf_add|mpf_set|temp =|templ =|y\\[i\\] \\+=" benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_01.cpp benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_01.cpp benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_02.cpp benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_03.cpp`
- `rg -n "Rgemv_gmp_C_native_0|Rgemv_gmp_kernel_0" benchmarks/CMakeLists.txt benchmarks/gmp/02_Rgemv/go.sh`
- `git status --short`
- `sed -n '280,305p' benchmarks/CMakeLists.txt`
- `sed -n '28,60p' benchmarks/gmp/02_Rgemv/go.sh`
- `sed -n '1,230p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_01.cpp`
- `git mv benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_01.cpp benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_03.cpp`
- `rg -n "C_native_01|C_native_02|C_native_03|Raw C native" benchmarks/gmp/02_Rgemv/README.md`
- `rg -n "Rgemv_gmp_C_native_0" benchmarks/CMakeLists.txt benchmarks/gmp/02_Rgemv/go.sh`
- `git status --short benchmarks/gmp/02_Rgemv benchmarks/CMakeLists.txt`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemv_gmp_C_native_01 Rgemv_gmp_C_native_02 Rgemv_gmp_C_native_03 Rgemv_gmp_kernel_01_mkII Rgemv_gmp_kernel_02_mkII Rgemv_gmp_kernel_03_mkII`
- `/bin/bash -lc 'set -euo pipefail; for exe in Rgemv_gmp_C_native_01 Rgemv_gmp_C_native_02 Rgemv_gmp_C_native_03 Rgemv_gmp_kernel_01_mkII Rgemv_gmp_kernel_02_mkII Rgemv_gmp_kernel_03_mkII; do echo "== $exe"; build_bench_release/benchmarks/gmp/02_Rgemv/$exe 17 19 128 | tail -n 2; done'`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `tail -n 50 STATUS.md`

Pass/fail result:
- Release benchmark reconfigure: PASS.
- C native 01/02/03 and C++ mkII 01/02/03 build: PASS.
- Rgemv smoke run: PASS.  All six checked variants reported `Result OK` for
  `M=17`, `N=19`, `precision=128`.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The recorded 4000x4000 Rgemv result table predates the C native
  renumbering; it documents that the old `C_native_01` result maps to the new
  `C_native_03`.

## Phase: GMP Rgemv README Hotpath Documentation

Implemented features:
- Reworked `benchmarks/gmp/02_Rgemv/README.md` to follow the same broad
  structure as `benchmarks/gmp/01_Raxpy/README.md`.
- Added the refactored `M=4000`, `N=4000`, 512-bit benchmark table from
  `results_raw/rgemv_gmp_m4000_n4000_p512_summary_20260516_122201.csv`.
- Documented the current serial and OpenMP Rgemv kernel shapes.
- Added hotpath disassembly notes for:
  `C_native_01`, `kernel_01_mkII`, `kernel_03_mkII`, and
  `kernel_openmp_03_mkII`.
- Added Rgemv lessons learned, especially the difference between serial
  column-major AXPY traversal and row-partitioned OpenMP traversal.

Missing features:
- No new benchmark run was performed in this phase.
- No new plot was generated in this phase.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/02_Rgemv/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/01_Raxpy/README.md`
- `sed -n '1,220p' benchmarks/gmp/02_Rgemv/README.md`
- `cat benchmarks/gmp/02_Rgemv/results_raw/rgemv_gmp_m4000_n4000_p512_summary_20260516_122201.csv`
- `ls -lh build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_01 build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_03_mkII build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_03_mkII`
- `nm -C build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_01 | rg "_Rgemv|omp"`
- `nm -C build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_03_mkII | rg "_Rgemv|omp"`
- `nm -C build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_03_mkII | rg "_Rgemv|omp|\\._omp"`
- `nm -C build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_01_mkII | rg "_Rgemv|omp"`
- `objdump -Cd --start-address=0x55c0 --stop-address=0x5750 build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_01`
- `objdump -Cd --start-address=0x56a0 --stop-address=0x5960 build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_03_mkII`
- `objdump -Cd --start-address=0x56c0 --stop-address=0x59b0 build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_01_mkII`
- `objdump -Cd --start-address=0x3f10 --stop-address=0x42b0 build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_03_mkII`
- `rg -n "Recorded Refactored Sample|Hotpath Disassembly|kernel_01_mkII|Lessons Learned" benchmarks/gmp/02_Rgemv/README.md`
- `sed -n '1,260p' benchmarks/gmp/02_Rgemv/README.md`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `tail -n 45 STATUS.md`

Pass/fail result:
- Hotpath symbol lookup: PASS.
- Hotpath disassembly extraction: PASS.
- README update: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The recorded Rgemv result table is a single run, not repeat-10 data.

## Phase: MPFR Raxpy Packed Custom Layout Test Program

Implemented features:
- Added `Raxpy_mpfr_C_native_packed_custom_layout_FMA.cpp`.
- Implemented an experimental packed MPFR vector using `mpfr_custom_init_set`.
- Stored each `__mpfr_struct` header and its significand limbs in one flat
  aligned allocation to test the pointer-chase hypothesis in MPFR Raxpy.
- Kept the timed hot loop as one `mpfr_fma` call per element so the main
  variable under test is x/y data layout.
- Printed packed significand bytes and element stride at runtime.
- Registered the target as
  `Raxpy_mpfr_C_native_packed_custom_layout_FMA`.
- Added the packed target to the common MPFR benchmark runner.
- Documented ownership constraints in `benchmarks/mpfr/01_Raxpy/README.md`:
  do not call `mpfr_clear` on packed elements and do not call `mpfr_set_prec`.

Missing features:
- No large repeat benchmark was run for the packed layout.
- No OpenMP packed-layout variant was added.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/01_Raxpy/README.md`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `STATUS.md`

Exact commands run:
- `rg -n "Raxpy_mpfr|mpfr/01_Raxpy" benchmarks/CMakeLists.txt benchmarks/mpfr/01_Raxpy/go.sh benchmarks/mpfr/01_Raxpy/README.md`
- `ls benchmarks/mpfr/01_Raxpy`
- `sed -n '1,220p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA.cpp`
- `sed -n '1,220p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01.cpp`
- `sed -n '1,260p' benchmarks/mpfr/01_Raxpy/Raxpy_common.hpp`
- `rg -n "mpfr_custom_init|mpfr_custom_get|mpfr_custom_move" /usr/include /usr/local/include 2>/dev/null`
- `sed -n '820,860p' /usr/include/mpfr.h && sed -n '1030,1065p' /usr/include/mpfr.h`
- `sed -n '1065,1088p' /usr/include/mpfr.h`
- `sed -n '1,220p' benchmarks/common/run_mpfr_benchmarks.sh`
- `sed -n '200,238p' benchmarks/CMakeLists.txt`
- `sed -n '260,360p' benchmarks/mpfr/01_Raxpy/README.md`
- `rg -n "Variant names|C_native|Unlike the current" benchmarks/mpfr/01_Raxpy/README.md`
- `sed -n '1,95p' benchmarks/mpfr/01_Raxpy/README.md`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Raxpy_mpfr_C_native_packed_custom_layout_FMA Raxpy_mpfr_C_native_01_FMA`
- `build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_packed_custom_layout_FMA 1000 512`
- `build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_packed_custom_layout_FMA 1000 256`
- `build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA 1000 512`
- `build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA 1000 256`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `cmake --build build_bench_release -j --target Raxpy_mpfr_C_native_packed_custom_layout_FMA`
- `tail -n 90 STATUS.md`
- `tail -n 25 STATUS.md`

Pass/fail result:
- Release benchmark reconfigure: PASS.
- Packed layout target build: PASS.
- Native FMA baseline build: PASS.
- Packed 512-bit smoke: PASS.  `stride=96`, `Result OK`.
- Packed 256-bit smoke: PASS.  `stride=64`, `Result OK`.
- Native FMA baseline smoke: PASS for the same `N=1000` sizes.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- Small smoke runs are not performance evidence; use large N and repeat runs
  before judging whether packed layout helps.

## Phase: GMP Rgemv Kernel Pattern Refactor

Implemented features:
- Refactored `benchmarks/gmp/02_Rgemv` to follow the GMP Raxpy benchmark
  source-shape split more closely.
- Updated `Rgemv_gmp_C_native_01.cpp` to reuse `mpf_t` work temporaries and
  compute the column-oriented AXPY update as `temp_b = alpha * x[j]`.
- Updated `Rgemv_gmp_C_native_openmp_01.cpp` to remove timed-loop
  `mpf_init`/`mpf_clear` calls and use row-partitioned OpenMP with per-thread
  `mpf_t` temporaries.
- Changed `kernel_01` to a direct-expression column-major AXPY form.
- Kept `kernel_02` as the reusable `temp`/`templ` copy-then-multiply form.
- Added `kernel_03` for reusable `temp`/`templ` expression assignment.
- Added `kernel_04` for loop-local product objects.
- Reworked OpenMP kernels as row-partitioned variants:
  direct expression, copy-then-multiply reusable temporaries, and expression
  assignment reusable temporaries.
- Registered the new targets in `benchmarks/CMakeLists.txt`.
- Added the new executables to `benchmarks/gmp/02_Rgemv/go.sh`.
- Documented the current Rgemv kernel source shapes in
  `benchmarks/gmp/02_Rgemv/README.md`.

Missing features:
- No repeat benchmark sweep was run in this phase.
- No plots were generated in this phase.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/02_Rgemv/README.md`
- `benchmarks/gmp/02_Rgemv/go.sh`
- `STATUS.md`

Exact commands run:
- `sed -n '1,240p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_01.cpp`
- `sed -n '1,260p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_02.cpp`
- `sed -n '1,260p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_01.cpp`
- `sed -n '1,260p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_02.cpp`
- `sed -n '1,280p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_01.cpp`
- `sed -n '1,300p' benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_01.cpp`
- `sed -n '1,220p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01.cpp`
- `sed -n '1,220p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_02.cpp`
- `sed -n '1,220p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_03.cpp`
- `sed -n '1,220p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_04.cpp`
- `sed -n '1,220p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_03.cpp`
- `rg -n "02_Rgemv|Rgemv_gmp" benchmarks/CMakeLists.txt benchmarks/gmp/02_Rgemv/go.sh benchmarks/gmp/02_Rgemv/README.md`
- `sed -n '1,140p' benchmarks/gmp/02_Rgemv/go.sh`
- `sed -n '250,310p' benchmarks/CMakeLists.txt`
- `sed -n '1,180p' benchmarks/gmp/02_Rgemv/README.md`
- `sed -n '1,140p' benchmarks/gmp/01_Raxpy/README.md`
- `git diff -- benchmarks/gmp/02_Rgemv benchmarks/CMakeLists.txt | sed -n '1,260p'`
- `git status --short`
- `ls -d build build_bench_release 2>/dev/null`
- `rg -n "Rgemv_gmp_kernel_03|Rgemv_gmp_kernel_openmp_03" benchmarks/CMakeLists.txt benchmarks/gmp/02_Rgemv/go.sh`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemv_gmp_C_native_01 Rgemv_gmp_C_native_openmp_01 Rgemv_gmp_kernel_01_orig Rgemv_gmp_kernel_01_mkII Rgemv_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_02_orig Rgemv_gmp_kernel_02_mkII Rgemv_gmp_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_03_orig Rgemv_gmp_kernel_03_mkII Rgemv_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_04_orig Rgemv_gmp_kernel_04_mkII Rgemv_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_01_orig Rgemv_gmp_kernel_openmp_01_mkII Rgemv_gmp_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_02_orig Rgemv_gmp_kernel_openmp_02_mkII Rgemv_gmp_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_03_orig Rgemv_gmp_kernel_openmp_03_mkII Rgemv_gmp_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH`
- `OMP_NUM_THREADS=4 OMP_PLACES=cores OMP_PROC_BIND=close /bin/bash -lc 'for exe in build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_01 build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_C_native_openmp_01 build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_0{1,2,3,4}_{orig,mkII,mkII_FIXED_PRECISION_FASTPATH} build_bench_release/benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_0{1,2,3}_{orig,mkII,mkII_FIXED_PRECISION_FASTPATH}; do echo "== $exe"; "$exe" 17 19 128 | tail -n 2; done'`
- `ctest --test-dir build --output-on-failure`
- `tail -n 80 STATUS.md`

Pass/fail result:
- Release benchmark reconfigure: PASS.
- All selected Rgemv benchmark targets: PASS.
- Rgemv smoke run: PASS.  All native, orig, mkII, fastpath, and OpenMP
  variants reported `Result OK` for `M=17`, `N=19`, `precision=128`.
- CTest: PASS.  156/156 tests passed.

Known issues:
- Large-size performance has not been measured after this refactor.

## Phase: MPFR Raxpy N=10000000 1024-bit Repeat-10 Benchmark

Implemented features:
- Ran the MPFR Raxpy benchmark matrix at `N=10000000`, 1024-bit precision,
  and repeat count 10.
- Covered C native, C native FMA, C native OpenMP, C native OpenMP FMA,
  wrapper serial kernels 01-04, and wrapper OpenMP kernels 01-03.
- Used explicit OpenMP affinity for benchmark runs:
  `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread`.
- Parsed the benchmark log into raw and summary CSV files.

Missing features:
- No README analysis or plot was added for the 1024-bit run yet.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821/benchmark_raxpy_n10000000_p1024_repeat10.log`
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821/raw_raxpy_n10000000_p1024_repeat10.csv`
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821/summary_raxpy_n10000000_p1024_repeat10.csv`
- `STATUS.md`

Exact commands run:
- `pgrep -af 'Raxpy_mpfr|benchmark_raxpy_n10000000_p1024|01_Raxpy' || true`
- `find benchmarks/mpfr/01_Raxpy/results_raw -maxdepth 1 -type d -name 'raxpy_n1e7_1024_repeat10_*' -printf '%f\n' | sort`
- `git status --short`
- `/bin/bash -lc 'set -euo pipefail; outdir="benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_$(date +%Y%m%d_%H%M%S)"; ...; OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread /usr/bin/time -f "WALL_SECONDS %e" "${exe_dir}/${exe}" 10000000 1024; ...'`
- `python3 - <<'PY' ...`
- `sed -n '1,80p' benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821/summary_raxpy_n10000000_p1024_repeat10.csv`
- `git status --short`
- `tail -n 80 STATUS.md`
- `ctest --test-dir build_bench_release --output-on-failure`
- `ls -lh benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821`

Pass/fail result:
- Benchmark run: PASS.  320/320 timed runs reported `Result OK`.
- Raw CSV and summary CSV generation: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- A previous interrupted partial output directory remains at
  `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_200214/`.
- OpenMP measurements contain normal run-to-run variation; use max and average
  separately when comparing variants.

## Phase: MPFR Raxpy 1024-bit README Analysis

Implemented features:
- Added the 1024-bit repeat-10 MPFR Raxpy benchmark result summary to
  `benchmarks/mpfr/01_Raxpy/README.md`.
- Added an inline summary plot for the `N=10000000`, 1024-bit precision run.
- Added 1024-bit serial and OpenMP result tables.
- Added a 1024-bit logical memory-bandwidth estimate and compared it with the
  512-bit traffic estimate.
- Kept `Lessons Learned` as the final README section and updated it to include
  the 1024-bit observations.

Missing features:
- No hardware-counter validation was added.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/01_Raxpy/README.md`
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821/benchmark_raxpy_n10000000_p1024_repeat10_summary.png`
- `STATUS.md`

Exact commands run:
- `sed -n '1,420p' benchmarks/mpfr/01_Raxpy/README.md`
- `sed -n '1,80p' benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821/summary_raxpy_n10000000_p1024_repeat10.csv`
- `tail -n 90 STATUS.md`
- `python3 - <<'PY' ...`
- `sed -n '150,430p' benchmarks/mpfr/01_Raxpy/README.md`
- `ls -lh benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_1024_repeat10_20260515_201821`
- `git diff --check`
- `tail -n 80 benchmarks/mpfr/01_Raxpy/README.md`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Plot generation: PASS.
- README section check: PASS.  `Lessons Learned` is the final section.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- None.

## Phase: GMP Raxpy README Benchmark Analysis

Implemented features:
- Reworked `benchmarks/gmp/01_Raxpy/README.md` to follow the structure and
  analysis style of `benchmarks/gmp/00_Rdot/README.md`.
- Added focused repeat-10 Raxpy result summaries for N=1000000 and N=10000000
  at 512-bit precision.
- Documented current Raxpy kernel shapes, hotpath expectations, payload
  bandwidth estimates, and lessons learned.
- Clarified why Raxpy currently stops at 01-04 plus OpenMP 01-03 rather than
  adding 4-way unrolled 05/06 variants immediately.

Missing features:
- Raxpy still does not print timed-kernel GMP allocator profiles like Rdot.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/01_Raxpy/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/00_Rdot/README.md`
- `sed -n '260,620p' benchmarks/gmp/00_Rdot/README.md`
- `sed -n '1,260p' benchmarks/gmp/01_Raxpy/README.md`
- `ls -1 benchmarks/gmp/01_Raxpy/results_raw 2>/dev/null | sort | tail -20`
- `rg -n "benchmark_raxpy_n1000000|benchmark_raxpy_n10000000|Lessons Learned|Bandwidth Estimate|Hotpath Expectations|kernel_03" benchmarks/gmp/01_Raxpy/README.md`
- `test -f benchmarks/gmp/01_Raxpy/results_raw/benchmark_raxpy_n1000000_p512_repeat10_20260515_142301.log && test -f benchmarks/gmp/01_Raxpy/results_raw/benchmark_raxpy_n1000000_p512_repeat10_20260515_142301.csv && test -f benchmarks/gmp/01_Raxpy/results_raw/benchmark_raxpy_n10000000_p512_repeat10_20260515_142725.log && test -f benchmarks/gmp/01_Raxpy/results_raw/benchmark_raxpy_n10000000_p512_repeat10_20260515_142725.csv && echo links-ok`

Pass/fail result:
- README section scan: PASS.
- README-linked local benchmark files: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The linked focused repeat-10 Raxpy logs are local generated results and are
  not staged by this documentation-only phase.

## Phase: GMP Raxpy Hotpath Disassembly Notes

Implemented features:
- Added a hotpath disassembly comparison section to
  `benchmarks/gmp/01_Raxpy/README.md`.
- Compared serial C native, `kernel_01_mkII`,
  `kernel_01_mkII_FIXED_PRECISION_FASTPATH`, `kernel_02_mkII`,
  `kernel_03_mkII`, and `kernel_04_mkII`.
- Compared OpenMP C native, upstream `gmpxx.h` OpenMP 03, and mkII OpenMP 03
  worker-loop bodies.
- Documented that serial `kernel_03_mkII` matches the raw C native multiply-add
  call class, while `kernel_01`/`kernel_04` still materialize product objects
  inside the loop and `kernel_02` pays an explicit `mpf_set`.

Missing features:
- No standalone disassembly extraction script was added.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/01_Raxpy/README.md`
- `STATUS.md`

Exact commands run:
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_C_native_01 | rg 'Raxpy|_Raxpy|main'`
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_03_mkII | rg 'Raxpy|_Raxpy|main'`
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_03_mkII | rg 'Raxpy|_Raxpy|omp|main'`
- `objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_C_native_01 | c++filt`
- `objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_03_mkII | c++filt`
- `objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_03_mkII | c++filt`
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII | rg ' _Raxpy|_Raxpy'`
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH | rg ' _Raxpy|_Raxpy'`
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_02_mkII | rg ' _Raxpy|_Raxpy'`
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_04_mkII | rg ' _Raxpy|_Raxpy'`
- `objdump -Cd --no-show-raw-insn --start-address=0x50e0 --stop-address=0x5320 build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x5d70 --stop-address=0x6020 build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x50e0 --stop-address=0x5280 build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_02_mkII | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x51a0 --stop-address=0x53d0 build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_04_mkII | c++filt`
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_C_native_openmp_01 | rg ' _Raxpy|_Raxpy|omp'`
- `nm -C build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_03_orig | rg ' _Raxpy|_Raxpy|omp'`
- `objdump -Cd --no-show-raw-insn --start-address=0x37c0 --stop-address=0x38d0 build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_C_native_openmp_01 | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x2ed0 --stop-address=0x2ff0 build_bench_release/benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_03_orig | c++filt`

Pass/fail result:
- Disassembly extraction: PASS.
- README section scan: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- Assembly addresses are build-specific and should not be used as stable
  identifiers.

## Phase: GMP Raxpy Kernel Shape Refactor

Implemented features:
- Added `Raxpy_gmp_kernel_03.cpp` as a reusable product object assigned from
  the product expression: `temp = alpha * x[i]; y[i] += temp`.
- Added `Raxpy_gmp_kernel_04.cpp` as a loop-local product object comparison:
  `mpf_class temp = alpha * x[i]; y[i] += temp`.
- Added `Raxpy_gmp_kernel_openmp_03.cpp` as the OpenMP reusable expression
  product shape with one private product object per thread.
- Registered the new serial and OpenMP kernels in CMake, the common benchmark
  runner, and the legacy `go.sh` executable list.
- Normalized existing Raxpy OpenMP 01/02 kernels to `schedule(static)`.
- Documented the Raxpy-specific kernel stages and why 4-way unroll is deferred
  relative to Rdot.

Missing features:
- No 4-way unrolled Raxpy 05/06 kernels yet; those remain a follow-up if
  01-04/OpenMP 01-03 leave clear loop-overhead or scheduling headroom.

Tests added:
- None.

Tests updated:
- `benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_03.cpp`
- `benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_04.cpp`
- `benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_03.cpp`
- `benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_01.cpp`
- `benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_02.cpp`
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/gmp/01_Raxpy/go.sh`
- `benchmarks/gmp/01_Raxpy/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/00_Rdot/README.md`
- `find benchmarks/gmp/01_Raxpy -maxdepth 1 -type f -printf '%f\n' | sort`
- `rg -n "Raxpy_gmp|add_gmp|Raxpy|kernel_0[1-9]|openmp" benchmarks/CMakeLists.txt benchmarks/common/run_benchmarks.sh benchmarks/gmp/01_Raxpy -g '*.cpp' -g '*.hpp' -g '*.h' -g '*.md' -g '*.sh'`
- `sed -n '1,170p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_01.cpp`
- `sed -n '1,180p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_02.cpp`
- `sed -n '1,190p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_01.cpp`
- `sed -n '1,200p' benchmarks/gmp/01_Raxpy/Raxpy_gmp_kernel_openmp_02.cpp`
- `sed -n '1,170p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03.cpp`
- `sed -n '1,190p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_06.cpp`
- `sed -n '1,210p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_06.cpp`
- `sed -n '1,160p' benchmarks/gmp/01_Raxpy/Raxpy.hpp`
- `sed -n '250,280p' benchmarks/CMakeLists.txt`
- `sed -n '136,158p' benchmarks/common/run_benchmarks.sh`
- `sed -n '1,180p' benchmarks/gmp/01_Raxpy/README.md`
- `sed -n '1,90p' benchmarks/gmp/01_Raxpy/go.sh`
- `tail -80 STATUS.md`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Raxpy_gmp_kernel_03_orig Raxpy_gmp_kernel_03_mkII Raxpy_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH Raxpy_gmp_kernel_04_orig Raxpy_gmp_kernel_04_mkII Raxpy_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH Raxpy_gmp_kernel_openmp_03_orig Raxpy_gmp_kernel_openmp_03_mkII Raxpy_gmp_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH Raxpy_gmp_kernel_openmp_01_mkII Raxpy_gmp_kernel_openmp_02_mkII`
- `/bin/bash -lc 'set -euo pipefail; for exe in Raxpy_gmp_kernel_03_mkII Raxpy_gmp_kernel_04_mkII Raxpy_gmp_kernel_openmp_03_mkII Raxpy_gmp_kernel_03_orig Raxpy_gmp_kernel_04_orig Raxpy_gmp_kernel_openmp_03_orig; do echo "=== ${exe} ==="; OMP_NUM_THREADS=4 build_bench_release/benchmarks/gmp/01_Raxpy/${exe} 1000 256 | tail -5; done'`
- `benchmarks/common/run_benchmarks.sh build_bench_release 128 8 8 2 2 2 2 2 /tmp/gmpfrxx_mkII_gmp_raxpy_runner_smoke 1`
- `cmake --build build_bench_release -j`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- CMake configure: PASS.
- New and affected Raxpy target build: PASS.
- Direct small Raxpy smoke: PASS.  The new 03/04/OpenMP 03 `orig` and `mkII`
  executables all printed `Result OK`.
- Common GMP benchmark runner smoke: PASS.  The runner resolved the new Raxpy
  03/04/OpenMP 03 executable families and the small run completed.
- Full release build: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The Raxpy executables still do not print the Rdot-style timed-kernel GMP
  allocator profile; this phase only aligns source-shape coverage.

Post-phase GMP Rdot OpenMP kernel shape alignment:
DONE

Implemented features:
- Changed `Rdot_gmp_kernel_openmp_02.cpp` to match the serial `kernel_02`
  source shape: loop-local `mpf_class templ = dx[i] * dy[i]` followed by
  accumulation into a per-thread partial sum.
- Added `Rdot_gmp_kernel_openmp_03.cpp` to match serial `kernel_03`: a
  reusable per-thread product object assigned from `dx[i] * dy[i]`.
- Added `Rdot_gmp_kernel_openmp_04.cpp` to match serial `kernel_04`: a
  reusable per-thread product object assigned from `dx[i]`, multiplied
  in place by `dy[i]`, and added to a per-thread partial sum.
- Added CMake targets and benchmark-runner entries for OpenMP 03/04 across
  upstream `gmpxx.h`, `gmpxx_mkII`, and fixed-precision fastpath variants.
- Updated the Rdot README to document the 01/02/03/04 serial-to-OpenMP
  mapping and to mark older `kernel_openmp_02` benchmark rows as historical
  names for the current `kernel_openmp_04` source shape.

Tests added:
- None. Benchmark source and documentation phase.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_02.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_03.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_04.cpp`
- `benchmarks/gmp/00_Rdot/go.sh`
- `benchmarks/gmp/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,240p' benchmarks/CMakeLists.txt`
- `sed -n '1,140p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp`
- `sed -n '1,140p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02.cpp`
- `sed -n '1,150p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03.cpp`
- `sed -n '1,150p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04.cpp`
- `sed -n '1,160p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_01.cpp`
- `sed -n '1,160p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_02.cpp`
- `rg -n "Rdot_gmp_kernel_openmp|Rdot_gmp_kernel_0|00_Rdot" benchmarks CMakeLists.txt cmake`
- `sed -n '70,115p' benchmarks/common/run_benchmarks.sh`
- `sed -n '1,80p' benchmarks/gmp/00_Rdot/go.sh`
- `git status --short`
- `cmake --build build_bench_release -j --target Rdot_gmp_kernel_openmp_01_orig Rdot_gmp_kernel_openmp_01_mkII Rdot_gmp_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_02_orig Rdot_gmp_kernel_openmp_02_mkII Rdot_gmp_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_03_orig Rdot_gmp_kernel_openmp_03_mkII Rdot_gmp_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_04_orig Rdot_gmp_kernel_openmp_04_mkII Rdot_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH`
- `cmake --build build_bench_release -j --target Rdot_gmp_kernel_openmp_03_orig Rdot_gmp_kernel_openmp_03_mkII Rdot_gmp_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_04_orig Rdot_gmp_kernel_openmp_04_mkII Rdot_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH`
- `OMP_NUM_THREADS=4 /bin/bash -lc 'set -e; for exe in Rdot_gmp_kernel_openmp_01_orig Rdot_gmp_kernel_openmp_01_mkII Rdot_gmp_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_02_orig Rdot_gmp_kernel_openmp_02_mkII Rdot_gmp_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_03_orig Rdot_gmp_kernel_openmp_03_mkII Rdot_gmp_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_04_orig Rdot_gmp_kernel_openmp_04_mkII Rdot_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH; do echo "COMMAND $exe"; build_bench_release/benchmarks/gmp/00_Rdot/$exe 1000 128 | tail -n 4; done'`
- `tail -n 80 STATUS.md`
- `git diff --check`
- `cmake --build build -j`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- First `cmake --build build_bench_release ...` built OpenMP 01/02 targets
  but failed to find newly configured OpenMP 03/04 targets before the CMake
  regeneration was visible to that invocation.
- Second `cmake --build build_bench_release ...` for OpenMP 03/04 targets:
  PASS.
- Smoke run for OpenMP 01/02/03/04 orig/mkII/fixed-fastpath variants at
  `N=1000`, `precision=128`, `OMP_NUM_THREADS=4`: PASS, all 12 variants
  reported `OK`.
- Initial `cmake --build build -j`: FAIL, `build` directory did not exist.
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- None.

Post-phase GMP Rdot README hotpath documentation:
DONE

Implemented features:
- Documented the timed `00_Rdot` kernel shapes for C native, upstream
  `gmpxx.h`, `gmpxx_mkII`, fixed-precision fastpath, and OpenMP variants.
- Added disassembly-based hotpath notes comparing the C native baseline with
  `kernel_01_mkII`, `kernel_01_mkII_FIXED_PRECISION_FASTPATH`,
  `kernel_03_mkII`, and `kernel_04_mkII`.
- Added a repeat-10 `N=10000000`, 512-bit maximum-MFLOPS summary from the
  committed `rdot_n1e7_20260509` log.

Tests added:
- None. Documentation-only phase.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/00_Rdot/README.md`
- `rg -n "void _Rdot|mpf_class|#pragma omp|mpf_mul|mpf_add|templ" benchmarks/gmp/00_Rdot/*.cpp benchmarks/gmp/00_Rdot/Rdot.hpp`
- `git status --short`
- `nm -C build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 | rg " _Rdot"`
- `nm -C build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII | rg " _Rdot|mpf_compound|scoped_mpf|thread_scratch"`
- `nm -C build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII | rg " _Rdot|mpf_compound|scoped_mpf|thread_scratch"`
- `nm -C build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH | rg " _Rdot|mpf_compound|scoped_mpf|thread_scratch"`
- `objdump -Cd --no-show-raw-insn --start-address=0x34d0 --stop-address=0x3590 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x33e0 --stop-address=0x3550 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x3310 --stop-address=0x3450 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x3490 --stop-address=0x3620 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH | c++filt`
- `nm -C build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_orig | rg " _Rdot"`
- `nm -C build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_orig | rg " _Rdot"`
- `nm -C build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII | rg " _Rdot"`
- `nm -C build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04_mkII | rg " _Rdot"`
- `objdump -Cd --no-show-raw-insn --start-address=0x3450 --stop-address=0x35d0 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_orig | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x33d0 --stop-address=0x34f0 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_orig | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x3350 --stop-address=0x34d0 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII | c++filt`
- `objdump -Cd --no-show-raw-insn --start-address=0x3310 --stop-address=0x3450 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04_mkII | c++filt`
- `rg -n "^Running|^Elapsed time|^MFLOPS|BENCH_ALLOC_COUNTS|Result OK" benchmarks/gmp/results_raw/rdot_n1e7_20260509/benchmark_rdot_n1e7_20260509_143146.log`
- `find benchmarks/gmp/results_raw -maxdepth 3 -type f \( -name '*Rdot*.png' -o -name '*rdot*.csv' -o -name '*Rdot*.csv' \) | sort`
- `sed -n '1,140p' benchmarks/gmp/results_raw/rdot_n1e7_20260509/benchmark_rdot_n1e7_20260509_143146.log`
- `awk '/^COMMAND Rdot /{label=$3; if ($4 ~ /^build/) label=$3; else label=$3"_"$4} /^MFLOPS:/{if ($2+0 > max[label]) max[label]=$2+0} END{for (l in max) print l, max[l]}' benchmarks/gmp/results_raw/rdot_n1e7_20260509/benchmark_rdot_n1e7_20260509_143146.log | sort`
- `tail -n 80 STATUS.md`
- `git diff -- benchmarks/gmp/00_Rdot/README.md STATUS.md`
- `git diff --check`
- `git status --short`

Pass/fail result:
- Documentation update completed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase GMP Rgemm 1024-bit OpenMP 02/04/05/06 sweep notes:
DONE

Implemented features:
- Ran the GMP Rgemm OpenMP 02/04/05/06 sweep at 1024-bit precision with 32
  OpenMP threads and the same size set as the recorded 512-bit sweep.
- Added the 1024-bit CSV, raw log, and plot artifacts under
  `benchmarks/results_raw`.
- Added a 1024-bit README section with inline plot rendering, representative
  winners, selected-size winners, and analysis of why the 1024-bit ranking
  differs from the 512-bit sweep.

Tests added:
- None. Benchmark artifact and documentation phase.

Tests updated:
- `benchmarks/gmp/03_Rgemm/README.md`
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.csv`
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.log`
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.png`
- `STATUS.md`

Exact commands run:
- `python3 /tmp/run_rgemm_gmp_1024_sweep.py`
- `python3 - <<'PY' ... summarize 1024-bit CSV ... PY`
- `ls -lh benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.csv benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.log benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.png`
- `git status --short`
- `sed -n '90,190p' benchmarks/gmp/03_Rgemm/README.md`
- `tail -80 STATUS.md`
- `perl -0pi -e 's/\r\n/\n/g; s/\r/\n/g' benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.csv`
- `perl -0pi -e 's/\n+\z/\n/' benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_1024.log`
- `python3 - <<'PY' ... verify normalized 1024-bit CSV rows and failures ... PY`

Pass/fail result:
- 1024-bit benchmark sweep: PASS, 456/456 rows report `Result OK`.
- Generated artifacts: PASS, CSV/log/png written.

Known issues:
- None.

Post-phase GMP Rgemm README inline plot:
DONE

Implemented features:
- Changed the recorded GMP Rgemm OpenMP 02/04/05/06 512-bit plot reference in
  `benchmarks/gmp/03_Rgemm/README.md` from a plain link to an inline Markdown
  image so the graph renders directly in the README.

Tests added:
- None. Documentation-only phase.

Tests updated:
- `benchmarks/gmp/03_Rgemm/README.md`
- `STATUS.md`

Exact commands run:
- `git status --short`
- `rg -n "Plot|rgemm_gmp_openmp_02_04_05_06_step31_core32_512" benchmarks/gmp/03_Rgemm/README.md`
- `git diff --check`
- `git diff -- benchmarks/gmp/03_Rgemm/README.md`
- `tail -80 STATUS.md`

Pass/fail result:
- Documentation update completed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase GMP Rgemm README 02/04/05/06 sweep notes:
DONE

Implemented features:
- Added a `Recorded OpenMP 02/04/05/06 Sweep` section to the GMP Rgemm
  benchmark README.
- Documented commit `f996de7`, benchmark conditions, result artifact links,
  kernel shapes, representative winners, selected-size winners, and the main
  interpretation of kernels 02, 04, 05, and 06.

Tests added:
- None. Documentation-only phase.

Tests updated:
- `benchmarks/gmp/03_Rgemm/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/03_Rgemm/README.md`
- `git status --short`
- `tail -n 30 benchmarks/gmp/03_Rgemm/README.md`
- `tail -n 60 STATUS.md`

Pass/fail result:
- Documentation update completed.

Known issues:
- None.

Post-phase GMP Rgemm kernel 06 fixed row blocking:
DONE

Implemented features:
- Added GMP Rgemm kernel 06 as a column-panel kernel with fixed 256-row
  blocking.
- Added C native, upstream `gmpxx.h` orig, and `gmpxx_mkII` benchmark targets
  for both serial and OpenMP execution.
- Kernel 06 uses `min(M, 256)` as its row block size and keeps the existing
  four-column panel over `B`.
- OpenMP kernel 06 parallelizes over `(column panel, row block)` tasks.

Tests added:
- None. Benchmark-only phase.

Tests updated:
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_06_common.hpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_06.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_openmp_06.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_06_common.hpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_06.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_06.cpp`
- `benchmarks/CMakeLists.txt`
- `benchmarks/gmp/03_Rgemm/go.sh`
- `benchmarks/common/run_benchmarks.sh`
- `STATUS.md`

Exact commands run:
- `ps -eo pid,ppid,stat,etime,cmd | rg 'Rgemm_gmp|rgemm_gmp_openmp_02_04_05|python|bash'`
- `ls benchmarks/gmp/03_Rgemm`
- `sed -n '1,260p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_05.cpp`
- `rg -n "Rgemm_gmp_.*05|03_Rgemm|kernel_openmp_05|C_native_openmp_05" -S .`
- `sed -n '1,260p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_openmp_05.cpp`
- `sed -n '1,240p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_02.cpp`
- `sed -n '235,290p' benchmarks/CMakeLists.txt`
- `sed -n '1,140p' benchmarks/gmp/03_Rgemm/go.sh`
- `git status --short`
- `sed -n '1,130p' benchmarks/CMakeLists.txt`
- `sed -n '1,220p' benchmarks/common/run_benchmarks.sh`
- `cmake --build build_bench_release -j --target Rgemm_gmp_C_native_06 Rgemm_gmp_C_native_openmp_06 Rgemm_gmp_kernel_06_orig Rgemm_gmp_kernel_06_mkII Rgemm_gmp_kernel_06_mkII_FIXED_PRECISION_FASTPATH Rgemm_gmp_kernel_openmp_06_orig Rgemm_gmp_kernel_openmp_06_mkII Rgemm_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH`
- `ls build_bench_release`
- `sed -n '1,80p' build_bench_release/CMakeCache.txt`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemm_gmp_C_native_06 Rgemm_gmp_C_native_openmp_06 Rgemm_gmp_kernel_06_orig Rgemm_gmp_kernel_06_mkII Rgemm_gmp_kernel_06_mkII_FIXED_PRECISION_FASTPATH Rgemm_gmp_kernel_openmp_06_orig Rgemm_gmp_kernel_openmp_06_mkII Rgemm_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH`
- `for exe in Rgemm_gmp_C_native_06 Rgemm_gmp_C_native_openmp_06 Rgemm_gmp_kernel_06_orig Rgemm_gmp_kernel_06_mkII Rgemm_gmp_kernel_openmp_06_orig Rgemm_gmp_kernel_openmp_06_mkII; do OMP_NUM_THREADS=4 ./build_bench_release/benchmarks/gmp/03_Rgemm/$exe 15 13 15 128 | rg "Fixed row|Elapsed time|MFLOPS|Result"; done`

Pass/fail result:
- Initial build before reconfigure: FAIL, build tree did not know the new
  target names.
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`: PASS.
- 06 benchmark target build: PASS.
- `15 13 15 128` serial/OpenMP C native/orig/mkII smoke check: PASS, all
  reported `Result OK`.

Known issues:
- An interrupted earlier benchmark left
  `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_step31_core32_512.csv`
  untracked in the worktree; this phase did not modify it.

Post-phase GMP Rgemm OpenMP 02/04/05/06 sweep:
DONE

Implemented features:
- Ran the GMP Rgemm OpenMP sweep for kernels 02, 04, 05, and fixed-256 06.
- Covered C native, upstream `gmpxx.h` orig, and `gmpxx_mkII` variants.
- Used sizes `13 + 31*k <= 1024`, plus `128,256,512,768,1024`.
- Generated CSV, log, and PNG plot under `benchmarks/results_raw`.

Tests added:
- None. Benchmark-result phase.

Tests updated:
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_512.csv`
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_512.log`
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_512.png`
- `STATUS.md`

Exact commands run:
- `ps -eo pid,ppid,stat,etime,cmd | rg 'Rgemm_gmp|rgemm_gmp|python|bash'`
- `kill 1259846 1260477`
- `cmake --build build_bench_release -j --target Rgemm_gmp_C_native_openmp_02 Rgemm_gmp_C_native_openmp_04 Rgemm_gmp_C_native_openmp_05 Rgemm_gmp_C_native_openmp_06 Rgemm_gmp_kernel_openmp_02_orig Rgemm_gmp_kernel_openmp_02_mkII Rgemm_gmp_kernel_openmp_04_orig Rgemm_gmp_kernel_openmp_04_mkII Rgemm_gmp_kernel_openmp_05_orig Rgemm_gmp_kernel_openmp_05_mkII Rgemm_gmp_kernel_openmp_06_orig Rgemm_gmp_kernel_openmp_06_mkII`
- `OMP_NUM_THREADS=32 OMP_PROC_BIND=close OMP_PLACES=cores` sweep over 38
  sizes and 12 OpenMP variants, writing
  `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_512.csv`.
- `python3 -c '...'` to validate the CSV row count, error count, size count,
  variant count, and best result.
- `python3 -c '...'` to generate
  `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step31_core32_512.png`.
- `python3 -c '...'` to summarize selected sizes and best-per-variant results.

Pass/fail result:
- Build: PASS.
- Sweep: PASS, 456/456 rows reported `Result OK`.
- CSV validation: PASS, 38 sizes and 12 variants.
- Best overall result: `Rgemm_gmp_C_native_openmp_06`, `N=1024`,
  883.060 MFLOPS.

Known issues:
- Single-run benchmark values have normal OpenMP placement and system-load
  noise; no repeated-run max/min statistics were collected in this sweep.

Post-phase GMP Rgemm kernel 05 panel blocking:
DONE

Implemented features:
- Added `Rgemm_gmp_kernel_05` as a GMP C++ wrapper column-panel Rgemm
  benchmark with panel width 4.
- Added `Rgemm_gmp_kernel_openmp_05`, parallelized by output column panels.
- Reused only `scaled_b[4]` and one `prod` scratch object per thread instead
  of using a 4x4 accumulator array.
- Registered kernel 05 serial and OpenMP variants for upstream gmpxx, mkII,
  and mkII fixed-precision fastpath benchmark builds.

Missing features:
- No C native 05 benchmark was added in this phase.
- Panel width is fixed at 4; no `JB=2/8` tuning variants were added yet.

Tests added:
- None. This phase adds benchmark kernels.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_05.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_05.cpp`
- `benchmarks/gmp/03_Rgemm/go.sh`
- `STATUS.md`

Exact commands run:
- `sed -n '1,240p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_04.cpp`
- `sed -n '1,280p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04.cpp`
- `sed -n '248,276p' benchmarks/CMakeLists.txt`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemm_gmp_kernel_05_orig Rgemm_gmp_kernel_05_mkII Rgemm_gmp_kernel_05_mkII_FIXED_PRECISION_FASTPATH Rgemm_gmp_kernel_openmp_05_orig Rgemm_gmp_kernel_openmp_05_mkII Rgemm_gmp_kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH`
- `OMP_NUM_THREADS=4` correctness sweep for sizes `13`, `15`, and `64`
  over all kernel 05 serial and OpenMP variants.
- `OMP_NUM_THREADS=32 OMP_PROC_BIND=close OMP_PLACES=cores` comparison sweep
  for sizes `64`, `128`, `256`, and `512` over mkII kernel 04/05 serial and
  OpenMP variants.
- `cmake --build build_bench_release -j`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Target build for all kernel 05 variants: PASS.
- Correctness sweep at `13 13 13 512`, `15 15 15 512`, and `64 64 64 512`:
  PASS, every variant printed `Result OK`.
- Short CORE=32 comparison: PASS, every variant printed `Result OK`.
- `cmake --build build_bench_release -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS,
  156/156 tests passed.

Known issues:
- OpenMP kernel 05 has only `N / 4` panel tasks, so small `N` can underuse
  32 cores.

Post-phase GMP Rgemm C native kernel 05:
DONE

Implemented features:
- Added `Rgemm_gmp_C_native_05` as a GMP C API column-panel Rgemm benchmark
  with panel width 4.
- Added `Rgemm_gmp_C_native_openmp_05` with one thread-local GMP scratch
  workspace per OpenMP worker.
- Reused only `scaled_b[4]` and one `prod` scratch value, matching the wrapper
  kernel 05 algorithm.
- Registered C native 05 targets in CMake and benchmark runner lists.

Missing features:
- No `JB=2/8` C native tuning variants were added yet.

Tests added:
- None. This phase adds benchmark kernels.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_05.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_openmp_05.cpp`
- `benchmarks/gmp/03_Rgemm/go.sh`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemm_gmp_C_native_05 Rgemm_gmp_C_native_openmp_05`
- `OMP_NUM_THREADS=4` correctness sweep for sizes `13`, `15`, and `64`
  over `Rgemm_gmp_C_native_05` and `Rgemm_gmp_C_native_openmp_05`.
- `OMP_NUM_THREADS=32 OMP_PROC_BIND=close OMP_PLACES=cores` comparison sweep
  for sizes `64`, `128`, `256`, and `512` over C native 04/05 serial and
  OpenMP variants.
- `cmake --build build_bench_release -j`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Target build for C native 05 variants: PASS.
- Correctness sweep at `13 13 13 512`, `15 15 15 512`, and `64 64 64 512`:
  PASS, every variant printed `Result OK`.
- Short CORE=32 comparison: PASS, every variant printed `Result OK`.
- `cmake --build build_bench_release -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS,
  156/156 tests passed.

Known issues:
- OpenMP C native 05 also has only `N / 4` panel tasks, so small `N` can
  underuse 32 cores.

Post-phase GMP Rgemm OpenMP 04/05 benchmark sweep:
DONE

Implemented features:
- Collected CORE=32 benchmark data for OpenMP Rgemm 04/05 variants through
  `N=1024`, including prime-size tail cases.
- Generated a CSV result file and a PNG plot for the sweep.

Missing features:
- No multi-run median/min/max aggregation was collected; each point is one run.

Tests added:
- None. Benchmark-only phase.

Tests updated:
- `benchmarks/results_raw/rgemm_gmp_openmp_04_05_core32_512.csv`
- `benchmarks/results_raw/rgemm_gmp_openmp_04_05_core32_512.png`
- `STATUS.md`

Exact commands run:
- `ls build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_openmp_04 build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_openmp_05 build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04_orig build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04_mkII build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_05_orig build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_05_mkII`
- `python3 -c 'import matplotlib; print(matplotlib.__version__)'`
- `OMP_NUM_THREADS=32 OMP_PROC_BIND=close OMP_PLACES=cores` benchmark sweep
  over sizes `64`, `127`, `128`, `192`, `256`, `257`, `384`, `509`, `512`,
  `768`, `769`, `1009`, and `1024` for C native, upstream gmpxx, and mkII
  OpenMP 04/05 variants.
- Python/matplotlib plot generation from the CSV file.
- Python CSV summary table generation.

Pass/fail result:
- Every benchmark point printed `Result OK`.
- CSV written to
  `benchmarks/results_raw/rgemm_gmp_openmp_04_05_core32_512.csv`.
- Plot written to
  `benchmarks/results_raw/rgemm_gmp_openmp_04_05_core32_512.png`.

Known issues:
- Benchmark timings are single-run measurements and include only each
  executable's timed loop, not total wall time including reference checking.

Post-phase GMP Rgemm native and wrapper 4x4 blocking:
DONE

Implemented features:
- Added `Rgemm_gmp_C_native_04` as a GMP C API 4x4 blocked Rgemm benchmark.
- Added `Rgemm_gmp_C_native_openmp_04` with one thread-local GMP scratch
  workspace per OpenMP worker.
- Reworked `Rgemm_gmp_kernel_04` and `Rgemm_gmp_kernel_openmp_04` to reuse
  scratch objects across blocks instead of constructing temporaries inside
  every 4x4 block.
- Implemented Nath-style pointer redirecting for tail blocks by redirecting
  out-of-range A/B reads to zero and out-of-range C writes to local sink
  values.
- Registered C native 04 targets in CMake and in the benchmark runner lists.

Missing features:
- No architecture-specific assembly micro-kernel was added.

Tests added:
- None. This phase adds benchmark kernels.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_04.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_openmp_04.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_04.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04.cpp`
- `benchmarks/gmp/03_Rgemm/go.sh`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_04.cpp`
- `sed -n '1,300p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04.cpp`
- `sed -n '245,272p' benchmarks/CMakeLists.txt`
- `rg -n "get_prec|precision\\(" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp gmpxx_mkII.h.in`
- `sed -n '140,190p' benchmarks/common/run_benchmarks.sh`
- `sed -n '28,60p' benchmarks/gmp/03_Rgemm/go.sh`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rgemm_gmp_C_native_04 Rgemm_gmp_C_native_openmp_04 Rgemm_gmp_kernel_04_orig Rgemm_gmp_kernel_04_mkII Rgemm_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH Rgemm_gmp_kernel_openmp_04_orig Rgemm_gmp_kernel_openmp_04_mkII Rgemm_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH`
- `OMP_NUM_THREADS=4` tail correctness sweep for sizes `13` and `15` over
  `Rgemm_gmp_C_native_04`, `Rgemm_gmp_C_native_openmp_04`,
  `Rgemm_gmp_kernel_04_orig`, `Rgemm_gmp_kernel_04_mkII`,
  `Rgemm_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH`,
  `Rgemm_gmp_kernel_openmp_04_orig`, `Rgemm_gmp_kernel_openmp_04_mkII`, and
  `Rgemm_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH`.
- `cmake --build build_bench_release -j`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Target build for all Rgemm 04 C native and wrapper variants: PASS.
- Tail correctness sweep at `13 13 13 512`: PASS, every variant printed
  `Result OK`.
- Tail correctness sweep at `15 15 15 512`: PASS, every variant printed
  `Result OK`.
- `cmake --build build_bench_release -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS,
  156/156 tests passed.

Known issues:
- None.

Post-phase GMP Rgemm 4x4 blocking benchmark:
DONE

Implemented features:
- Added `Rgemm_gmp_kernel_04` as a 4x4 blocked GMP Rgemm benchmark.
- Added `Rgemm_gmp_kernel_openmp_04` with OpenMP parallelization over 4x4
  output blocks.
- Implemented tail handling with pointer redirection for incomplete 4x4
  blocks, redirecting out-of-range A/B inputs to zero and out-of-range C
  outputs to local sink objects.
- Registered serial and OpenMP 04 variants for original GMP C++, mkII, and
  mkII fixed-precision fastpath benchmark builds.
- Added the 04 variants to the common GMP benchmark runner and the local
  `benchmarks/gmp/03_Rgemm/go.sh` executable list.

Missing features:
- No tuned assembly or architecture-specific micro-kernel was added.

Tests added:
- None. This phase adds benchmark kernels.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_04.cpp`
- `benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04.cpp`
- `benchmarks/gmp/03_Rgemm/go.sh`
- `STATUS.md`

Exact commands run:
- `rg -n "Rgemm_gmp_kernel_0|03_Rgemm" benchmarks/CMakeLists.txt benchmarks/common/run_benchmarks.sh benchmarks/gmp/03_Rgemm/go.sh`
- `sed -n '1,220p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_03.cpp`
- `sed -n '1,280p' benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_03.cpp`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `git diff -- benchmarks/CMakeLists.txt benchmarks/common/run_benchmarks.sh benchmarks/gmp/03_Rgemm/go.sh`
- `cmake --build build_bench_release -j --target Rgemm_gmp_kernel_04_orig Rgemm_gmp_kernel_04_mkII Rgemm_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH Rgemm_gmp_kernel_openmp_04_orig Rgemm_gmp_kernel_openmp_04_mkII Rgemm_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH`
- `./build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_04_orig 5 6 7 128`
- `./build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_04_mkII 5 6 7 128`
- `./build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04_orig 5 6 7 128`
- `./build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04_mkII 5 6 7 128`
- `./build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH 5 6 7 128`
- `./build_bench_release/benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH 5 6 7 128`
- `cmake --build build_bench_release -j`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- `cmake --build build_bench_release -j --target ...`: PASS.
- `Rgemm_gmp_kernel_04_orig 5 6 7 128`: PASS, `Result OK`.
- `Rgemm_gmp_kernel_04_mkII 5 6 7 128`: PASS, `Result OK`.
- `Rgemm_gmp_kernel_openmp_04_orig 5 6 7 128`: PASS, `Result OK`.
- `Rgemm_gmp_kernel_openmp_04_mkII 5 6 7 128`: PASS, `Result OK`.
- `Rgemm_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH 5 6 7 128`: PASS,
  `Result OK`.
- `Rgemm_gmp_kernel_openmp_04_mkII_FIXED_PRECISION_FASTPATH 5 6 7 128`:
  PASS, `Result OK`.
- `cmake --build build_bench_release -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build_bench_release --output-on-failure`: PASS,
  156/156 tests passed.

Known issues:
- None.

Post-phase MPFR Raxpy stable-rounding benchmark targets:
DONE

Implemented features:
- Added stable-rounding and stable-rounding-plus-FMA target variants for MPFR
  Raxpy kernels.
- Added the new MPFR Raxpy stable-rounding variants to the common MPFR
  benchmark runner.

Benchmarks run:
- Ran MPFR Raxpy with `N=10000000` and `precision=512`.
- Measured `MFLOPS` for C native, C native FMA, C native OpenMP, C native
  OpenMP FMA, and mkII Raxpy 01/02/OpenMP variants.
- Each benchmark variant was run five times and summarized by maximum MFLOPS.
- Extracted `_Raxpy` and OpenMP outlined-function disassembly call sites with
  `nm -C` and `objdump -Cd --demangle`.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release --target Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_01_FMA Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_C_native_openmp_01_FMA Raxpy_mpfr_kernel_01_mkII Raxpy_mpfr_kernel_01_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_01_mkII_FMA Raxpy_mpfr_kernel_01_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_02_mkII Raxpy_mpfr_kernel_02_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_02_mkII_FMA Raxpy_mpfr_kernel_02_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_openmp_01_mkII Raxpy_mpfr_kernel_openmp_01_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_openmp_01_mkII_FMA Raxpy_mpfr_kernel_openmp_01_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_openmp_02_mkII Raxpy_mpfr_kernel_openmp_02_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_openmp_02_mkII_FMA Raxpy_mpfr_kernel_openmp_02_mkII_STABLE_ROUNDING_FMA -j`
- `ctest --test-dir build --output-on-failure`
- `ctest --test-dir build_bench_release --output-on-failure`
- `cmake --build build_bench_release -j`
- `ctest --test-dir build_bench_release --output-on-failure`
- `git diff --check`

Pass/fail result:
- Focused MPFR Raxpy benchmark target build: PASS.
- MPFR Raxpy benchmark runs: PASS, all measured runs reported `Result OK`.
- First `ctest --test-dir build --output-on-failure`: FAIL, the `build`
  directory did not exist in this checkout.
- First `ctest --test-dir build_bench_release --output-on-failure`: FAIL
  because the directory had only benchmark targets built and many registered
  example/test executables were not present.
- `cmake --build build_bench_release -j`: PASS.
- Final `ctest --test-dir build_bench_release --output-on-failure`: PASS,
  156/156 tests passed.
- `git diff --check`: PASS.

Known issues:
- MPFR Raxpy has no upstream `orig` benchmark target; comparison is against
  C native baselines and mkII variants.

Post-phase MPFR C-native benchmark rounding path:
DONE

Implemented features:
- Replaced `mpfrxx::default_rounding_mode()` calls in MPFR C-native benchmark
  kernels with direct `mpfr_get_default_rounding_mode()` reads.
- Cached the MPFR rounding mode at the C-native kernel entry or OpenMP worker
  entry before passing it to raw `mpfr_*` calls.
- Updated the raw `Rdot_mpfr_kernel_05` benchmark to use direct MPFR rounding
  access as well.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_openmp_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_openmp_01.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_01.cpp`
- `benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_C_native_01.cpp`
- `benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_C_native_02.cpp`
- `benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_C_native_openmp_01.cpp`
- `benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_C_native_openmp_02.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "mpfrxx::default_rounding_mode\\(\\)|default_rounding_mode\\(\\)" benchmarks/mpfr`
- `sed -n '40,90p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01.cpp`
- `sed -n '40,95p' benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01.cpp`
- `sed -n '28,70p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_01.cpp`
- `sed -n '30,68p' benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_01.cpp`
- `sed -n '18,45p' benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_C_native_01.cpp`
- `sed -n '18,45p' benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_C_native_02.cpp`
- `sed -n '18,48p' benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_C_native_openmp_01.cpp`
- `sed -n '18,48p' benchmarks/mpfr/03_Rgemm/Rgemm_mpfr_C_native_openmp_02.cpp`
- `cmake --build build-release-nocount -j --target Rdot_mpfr_C_native_01 Rdot_mpfr_C_native_01_FMA Rdot_mpfr_C_native_openmp_01 Rdot_mpfr_C_native_openmp_01_FMA Rdot_mpfr_kernel_05_mkII Rdot_mpfr_kernel_05_mkII_FMA Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_01_FMA Rgemv_mpfr_C_native_01 Rgemm_mpfr_C_native_01 Rgemm_mpfr_C_native_02`
- `cmake --build build-release-nocount -j --target Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_C_native_openmp_01_FMA Rgemv_mpfr_C_native_openmp_01 Rgemm_mpfr_C_native_openmp_01 Rgemm_mpfr_C_native_openmp_02`
- `rg -n "mpfrxx::default_rounding_mode\\(\\)" benchmarks/mpfr`
- `git diff --check`
- `nm -C build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01 | rg ' _Rdot| Rdot\\('`
- `objdump -Cd build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01 --start-address=0x3ae0 --stop-address=0x3b50`

Pass/fail result:
- `rg -n "mpfrxx::default_rounding_mode\\(\\)" benchmarks/mpfr`: PASS, no matches.
- Modified MPFR C-native benchmark targets: PASS.
- `git diff --check`: PASS.
- Disassembly check: `Rdot_mpfr_C_native_01` hot loop now contains only
  `mpfr_mul` and `mpfr_add` for arithmetic. Wrapper default/env checks are gone.

Known issues:
- OpenMP C-native benchmarks read MPFR default rounding mode once per worker
  region or work item depending on loop structure. They no longer route through
  the wrapper default API.

Post-phase MPFR eval-context hot-path narrowing:
DONE

Implemented features:
- Narrowed `gmpfrxx_mkII::detail::current_eval_context()` to read only the
  MPFR default rounding mode needed by arithmetic calls.
- Removed per-operation reads of MPFR default precision, `emin`, and `emax`
  from the eval-context path. MPFR exponent range remains libmpfr-owned TLS
  state and is not copied into arithmetic eval contexts.
- Added `gmpfrxx_mkII::detail::current_rounding_mode()` as the direct MPFR
  rounding getter used by `current_eval_context()`.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/eval_context.hpp`
- `STATUS.md`

Exact commands run:
- `sed -n '90,340p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '2328,2495p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '2288,2344p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1,90p' include/gmpfrxx_mkII/detail/eval_context.hpp`
- `rg -n "\\.emin|\\.emax|context\\." include/gmpfrxx_mkII/detail`
- `cmake --build build-release-nocount -j`
- `rg -n "struct eval_context|current_eval_context|current_rounding_mode|\\.emin|\\.emax" include/gmpfrxx_mkII/detail/eval_context.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `ctest --test-dir build-release-nocount -R "test_mpfr_(precision_policy|compound_assign|numeric_equivalence|aliasing|environment|defaults|initialization_sentinel|thread_safety|comparisons)" --output-on-failure`
- `nm -C build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03_mkII | rg ' _Rdot| Rdot\\('`
- `nm -C build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII | rg ' _Rdot| Rdot\\('`
- `nm -C build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05_mkII | rg ' _Rdot| Rdot\\('`
- `objdump -Cd build-release-nocount/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03_mkII --start-address=0x3930 --stop-address=0x3984`

Pass/fail result:
- `cmake --build build-release-nocount -j`: PASS.
- Focused MPFR CTest run: PASS, 10/10 tests passed.
- Disassembly check: `Rdot_mpfr_kernel_03_mkII` hot loop now contains
  `mpfr_get_default_rounding_mode`, `mpfr_mul`, and `mpfr_add`; it no longer
  contains `mpfr_get_default_prec`, `mpfr_get_emin`, or `mpfr_get_emax` in the
  loop body.

Known issues:
- Public wrapper arithmetic still reads MPFR default rounding mode at each
  operation. Kernels that need a fully fixed rounding mode should pass or cache
  `mpfr_rnd_t` at the function entry and use raw MPFR calls in the inner loop.

Post-phase MPFR MPC MPFC leaf precision fastpath:
DONE

Implemented features:
- Added MPFR constructor materialization precision helper matching the MPF
  leaf-leaf nonzero precision policy.
- Added MPC constructor materialization precision helper.  For binary
  expressions whose leaves are both `mpfrxx::mpc_class`, real and imaginary
  precision are treated as known nonzero.
- Added MPFC constructor materialization real/imag precision helpers.  For
  binary expressions whose leaves are both `gmpxx::mpfc_class`, both component
  precisions are treated as known nonzero.
- Generic fallback to the default precision remains for scalar/exact
  expressions that can contribute zero precision.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `STATUS.md`

Exact commands run:
- `rg -n "expression_.*precision|materialization_precision|constructor_materialization_precision|is_.*class_leaf|class_leaf_v|mpfr_class::mpfr_class\\(const Expr|mpc_class::mpc_class\\(const Expr|mpfc_class::mpfc_class\\(const Expr" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1608,1705p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '2326,2342p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '780,875p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1268,1284p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '500,665p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1098,1118p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `cmake --build build -j --target test_mpfr_precision_policy test_mpfr_numeric_equivalence test_mpfr_aliasing test_mpc_precision_policy test_mpc_aliasing test_mpc_alloc_count test_mpfc_precision_policy test_mpfc_basic test_mpfc_math`
- `ctest --test-dir build -R 'test_mpfr_precision_policy|test_mpfr_numeric_equivalence|test_mpfr_aliasing|test_mpc_precision_policy|test_mpc_aliasing|test_mpc_alloc_count|test_mpfc_precision_policy|test_mpfc_basic|test_mpfc_math' --output-on-failure`
- `git diff --check`

Pass/fail result:
- Focused MPFR/MPC/MPFC tests: PASS, 9/9.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase remove floating zero-allocation moves:
DONE

Implemented features:
- Removed MPF moved-from placeholder metadata and restored the dense
  `sizeof(gmpxx::mpf_class) == sizeof(mpf_t)` layout.
- Restored MPF move construction to `mpf_init2` plus `mpf_swap`, keeping the
  moved-from object as a normal valid `mpf_t`.
- Removed MPF moved-from validity guards from assignment, scalar setters,
  precision setters, shift helpers, and compound-assignment fallback.
- Replaced MPFR and MPC raw-copy move construction with `mpfr_init2`/`mpfr_swap`
  and `mpc_init3`/`mpc_swap`.
- `gmpxx::mpfc_class` follows the MPF behavior through its two `mpf_class`
  components.
- Updated layout and allocation-count tests and the move-semantics
  specification.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `tests/test_abi_fingerprint.cpp`
- `tests/test_mpf_alloc_count.cpp`
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `rg -n "valid_|moved_from|is_valid\\(|ensure_valid|memcpy\\(|mpf_class\\(mpf_class&&|mpfr_class\\(mpfr_class&&|mpc_class\\(mpc_class&&|mpfc_class\\(mpfc_class&&|operator=\\(.*&&" include/gmpfrxx_mkII/detail tests SPECIFICATIONS.md STATUS.md`
- `sed -n '80,145p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '240,290p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '560,650p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '115,140p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '250,275p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '200,220p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '340,385p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `rg -n "ensure_valid_for_assignment|is_valid\\(|moved_from_precision_bits_|std::memcpy" include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `rg -n "std::memcpy|moved_from|valid_|ensure_valid|is_valid\\(" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp tests/test_abi_fingerprint.cpp tests/test_mpf_alloc_count.cpp SPECIFICATIONS.md`
- `cmake --build build -j --target test_abi_fingerprint test_mpf_alloc_count test_mpfr_alloc_count test_mpc_alloc_count test_mpfc_basic test_construction_copy`
- `ctest --test-dir build -R 'test_abi_fingerprint|test_mpf_alloc_count|test_mpfr_alloc_count|test_mpc_alloc_count|test_mpfc_basic|test_construction_copy' --output-on-failure`

Pass/fail result:
- Focused ABI/allocation/move-adjacent tests: PASS, 6/6.

Known issues:
- Move construction now allocates again for MPF/MPFR/MPC, by design. This
  restores dense backend-sized wrapper layout and removes moved-from guard
  branches from MPF hot paths.

Post-phase MPF leaf precision fastpath:
DONE

Implemented features:
- Added a constructor materialization precision helper for MPF expressions.
- For binary expressions whose leaves are both `gmpxx::mpf_class`, the
  constructor now treats materialization precision as known nonzero and skips
  the generic `precision == 0 ? default : precision` fallback branch.
- This removes the `or/jne` precision-zero check from the Rdot kernel_02 mkII
  hot path while preserving the generic fallback for scalar/exact expressions.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `STATUS.md`

Exact commands run:
- `rg -n "mpf_expression_precision|mpf_materialization_precision|scalar_leaf|is_mpf_class_leaf_v" include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1310,1425p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1990,2012p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `cmake --build build-release-nocount -j --target Rdot_gmp_kernel_02_mkII`
- `nm -C build-release-nocount/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII | rg ' T _Rdot'`
- `objdump -Cd build-release-nocount/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII --start-address=0x3290 --stop-address=0x3380`
- `cmake --build build -j --target test_mpf_precision_policy test_mpf_alloc_count test_mpf_numeric_equivalence test_mpf_aliasing`
- `ctest --test-dir build -R 'test_mpf_precision_policy|test_mpf_alloc_count|test_mpf_numeric_equivalence|test_mpf_aliasing' --output-on-failure`

Pass/fail result:
- ASM check: Rdot kernel_02 mkII hot loop now matches the original shape:
  `get_prec`, `get_prec`, max, `init2`, `mul`, `add`, `clear`.
- Focused MPF tests: PASS, 4/4.

Known issues:
- None.

Post-phase GMP MPF direct leaf-binary construction:
DONE

Implemented features:
- Reused the direct MPF leaf-binary assignment fastpath for expression
  construction.
- `gmpxx::mpf_class result = lhs * rhs` now materializes direct MPF leaf
  products with `mpf_mul` instead of routing through a standalone
  `mpf_evaluate<mul_op,...>` call.
- Applied the same direct path to the explicit-precision expression
  constructor.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `STATUS.md`

Exact commands run:
- `rg -n "mpf_evaluate|mpf_class\\(const Expr|is_mpf_mul_direct_expr|binary_expr<mul_op|operator=\\(const Expr" include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '200,310p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1480,1870p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1860,2065p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `cmake --build build-release-nocount -j --target Rdot_gmp_kernel_02_mkII`
- `nm -C build-release-nocount/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII | rg ' T _Rdot|mpf_evaluate<.*mul_op'`
- `objdump -Cd build-release-nocount/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII --start-address=0x3620 --stop-address=0x3890 | rg '__gmpf_(get_prec|init2|mul|add|clear|set_d|set_ui)|mpf_evaluate<.*mul_op|<.*_Rdot|add\\s+\\$0x20|cmp\\s|jne|jle|call'`
- Ran `Rdot_gmp_kernel_02_orig` and `Rdot_gmp_kernel_02_mkII` five times each
  with `100000 512`.
- `cmake --build build -j --target test_mpf_alloc_count test_mpf_precision_policy test_mpf_aliasing test_mpf_numeric_equivalence test_temporary_expression_lifetime`
- `ctest --test-dir build -R 'test_mpf_alloc_count|test_mpf_precision_policy|test_mpf_aliasing|test_mpf_numeric_equivalence|test_temporary_expression_lifetime' --output-on-failure`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- ASM check: kernel_02/mkII steady path now calls `__gmpf_mul` directly; the
  previous hot-path `mpf_evaluate<mul_op,...>` call is gone.
- Best of 5 at Rdot kernel_02 `100000 512`:
  - `Rdot_gmp_kernel_02_orig`: `16.4132 MFLOPS`
  - `Rdot_gmp_kernel_02_mkII`: `16.6216 MFLOPS`
- Focused MPF/expression tests: PASS, 5/5.
- `git diff --check`: PASS.
- Full build: PASS.
- Full CTest: PASS, 154/154.

Known issues:
- `mpf_evaluate<mul_op,...>` symbols may still exist for other expression
  shapes; they are no longer on the inspected kernel_02 direct product
  construction hot path.

Post-phase GMP MPF fixed-precision scratch release:
DONE

Implemented features:
- Stopped restoring a retained MPF TLS scratch slot to its allocated precision
  on every release in the fixed-precision fastpath.
- The scratch slot now keeps the active requested precision between uses and is
  restored only before reallocation or final clear.
- This removes the per-iteration `mpf_set_prec_raw` restore from fixed-precision
  loops such as `a += b * c`, while preserving the scratch allocation boundary.
- Updated the fixed-precision specification to state that fastpath scratch may
  retain active precision between uses.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `rg -n "mpf_thread_scratch|set_prec_raw|mpf_compound_mul_apply|assume_fixed_precision_fastpath" include/gmpfrxx_mkII/detail/mpf_impl.hpp SPECIFICATIONS.md STATUS.md`
- `sed -n '1670,1790p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1888,1930p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '145,175p' SPECIFICATIONS.md`
- `sed -n '5721,5742p' STATUS.md`
- `cmake --build build -j --target test_mpf_fixed_precision_tls_scratch test_mpf_alloc_count`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpf_alloc_count' --output-on-failure`
- `cmake --build build-release-nocount -j --target Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH`
- `nm -C build-release-nocount/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH | rg ' T _Rdot|mpf_thread_scratch|mpf_compound'`
- `objdump -Cd build-release-nocount/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH --start-address=0x37a0 --stop-address=0x39a8 | rg '__gmpf_(get_prec|init2|mul|add|clear|set_prec|set_prec_raw)|<.*_Rdot|call|add\\s+\\$0x20|test\\s+%|cmp'`
- `git diff --check`

Pass/fail result:
- Focused MPF fixed-precision scratch and allocation tests: PASS, 2/2.
- Release fixed-fastpath benchmark target rebuild: PASS.
- ASM check: no `__gmpf_set_prec_raw` call is present in the inspected Rdot
  fixed-fastpath hot-loop range after the scratch release change.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase GMP MPF aligned move and fixed-precision addmul scratch:
DONE

Implemented features:
- Restored `gmpxx::mpf_class` moved-from ownership metadata for the GMP-only
  floating wrapper.  On the common 64-bit GMP ABI this makes
  `gmpxx::mpf_class` 32 bytes and `gmpxx::mpfc_class` 64 bytes, giving both
  power-of-two array strides.
- Kept MPFR and MPC wrappers in backend-sized layout; the moved-from metadata
  was restored only for MPF/MPFC.
- Restored the GMP MPF fixed-precision fastpath for direct leaf-product
  compound assignment.  With
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`, `a += b * c` and
  `a -= b * c` reuse a thread-local product scratch instead of allocating a
  per-operation `mpf_t`.
- Kept the operation semantically two-step MPF arithmetic: `mpf_mul` followed
  by `mpf_add` or `mpf_sub`.  This is not fused arithmetic.

Tests added:
- Restored `tests/test_mpf_fixed_precision_tls_scratch.cpp`, which verifies
  the direct `+= a * b` and `-= a * b` paths and checks that the warmed
  fixed-precision scratch path performs zero GMP allocations.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `tests/CMakeLists.txt`
- `tests/test_abi_fingerprint.cpp`
- `tests/test_mpf_alloc_count.cpp`
- `tests/test_mpf_fixed_precision_tls_scratch.cpp`
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `rg -n "class mpf_class|mpf_class\\(mpf_class&&|operator=\\(mpf_class&&|~mpf_class|ensure_valid|class mpfc_class|mpfc_class\\(mpfc_class&&|operator=\\(mpfc_class&&|~mpfc_class" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp tests/test_abi_fingerprint.cpp SPECIFICATIONS.md STATUS.md`
- `git status --short`
- `sed -n '90,280p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '60,135p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1,180p' tests/test_abi_fingerprint.cpp`
- `rg -n "ASSUME_FIXED|ENABLE_FMA|mpf_compound_mul_apply|mpf_compound_submul_apply|mpf_compound_assign|scoped_mpf_temporary product|GMP.*FMA|MPF.*FMA" include benchmarks tests SPECIFICATIONS.md CMakeLists.txt benchmarks/CMakeLists.txt`
- `git show 499d257:include/gmpfrxx_mkII/detail/mpf_impl.hpp | sed -n '1540,1685p'`
- `git show 499d257:include/gmpfrxx_mkII/detail/mpf_impl.hpp | sed -n '1685,1760p'`
- `git show 499d257:tests/test_mpf_fixed_precision_tls_scratch.cpp`
- `cmake --build build -j --target test_abi_fingerprint test_mpf_alloc_count test_mpfc_basic test_construction_copy`
- `ctest --test-dir build -R 'test_abi_fingerprint|test_mpf_alloc_count|test_mpfc_basic|test_construction_copy' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpf_fixed_precision_tls_scratch test_mpf_fixed_precision_materialization test_mpf_alloc_count test_mpf_scalar_alloc_count test_abi_fingerprint test_mpfc_basic`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpf_fixed_precision_materialization|test_mpf_alloc_count|test_mpf_scalar_alloc_count|test_abi_fingerprint|test_mpfc_basic' --output-on-failure`

Pass/fail result:
- Focused MPF/MPFC layout tests before restoring scratch: PASS, 4/4.
- Full build before restoring scratch: PASS.
- Full CTest before restoring scratch: PASS, 153/153.
- Reconfigure after restoring scratch: PASS.
- Focused fixed-precision scratch and MPF/MPFC tests: PASS, 6/6.

Known issues:
- The restored scratch is a header-local `thread_local` performance cache.
  It is intentionally limited to the fixed-precision fastpath and direct
  `mpf_class` leaf products.

Post-phase GMP Rdot fixed-precision benchmark target:
DONE

Implemented features:
- Restored GMP Rdot `*_mkII_FIXED_PRECISION_FASTPATH` benchmark targets.
- Made GMP benchmark targets with the `mkII_FIXED_PRECISION_FASTPATH` suffix
  define `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`, so the direct
  `a += b * c` MPF path borrows the thread-local product scratch only for
  fixed-precision fastpath builds.

Tests added:
- No unit tests were added.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `sed -n '70,88p' benchmarks/CMakeLists.txt`
- `sed -n '120,140p' benchmarks/CMakeLists.txt`
- `sed -n '1860,1940p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `rg -n "GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH" CMakeLists.txt benchmarks/CMakeLists.txt tests/CMakeLists.txt cmake include`
- `cmake -S . -B build-release-nocount -DCMAKE_BUILD_TYPE=Release -DGMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS=OFF -DGMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS=OFF -DGMPFRXX_MKII_BENCHMARK_DISABLE_ALLOCATOR_COUNTER=ON`
- `cmake --build build-release-nocount -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_01_orig Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH`
- Ran `Rdot_gmp_C_native_01`, `Rdot_gmp_kernel_01_orig`,
  `Rdot_gmp_kernel_01_mkII`, and
  `Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH` five times each with
  `100000 512`.
- `nm -C build-release-nocount/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH | rg ' T _Rdot|mpf_thread_scratch|mpf_compound'`
- `objdump -Cd build-release-nocount/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH | rg -A110 '<_Rdot' | rg '__gmpf_(get_prec|init2|mul|add|clear|set_prec|set_prec_raw)|mpf_thread_scratch|add\\s+\\$0x20|test\\s+%|cmpq'`

Pass/fail result:
- Benchmark build: PASS.
- Fixed-fastpath benchmark target exists and builds.
- Fixed-fastpath binary contains `mpf_thread_scratch` symbols.
- Best of 5 at Rdot kernel_01 `100000 512`:
  - `Rdot_gmp_C_native_01`: `19.8496 MFLOPS`
  - `Rdot_gmp_kernel_01_orig`: `17.2955 MFLOPS`
  - `Rdot_gmp_kernel_01_mkII`: `15.6376 MFLOPS`
  - `Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH`: `18.1196 MFLOPS`

Known issues:
- The fixed-fastpath benchmark still has TLS scratch acquire/release and
  precision-restore overhead, so it does not exactly match a hand-written C
  native loop-local `mpf_t` scratch.

Post-phase MPFR Rdot raw-kernel benchmark:
DONE

Implemented features:
- Added `Rdot_mpfr_kernel_05`, an MPFR wrapper benchmark kernel that stores
  inputs and results as `mpfrxx::mpfr_class` but exposes each object's
  underlying `mpfr_t` inside the timed loop.
- The kernel snapshots the current rounding mode once at function entry and
  then calls `mpfr_mul`/`mpfr_add` directly in the non-FMA build.
- The FMA variant uses `mpfr_fma` directly and keeps only one accumulator
  object in the timed kernel.
- Registered the new benchmark in CMake as `Rdot_mpfr_kernel_05_mkII` and
  `Rdot_mpfr_kernel_05_mkII_FMA`.
- Updated the MPFR benchmark runner's Rdot target list to use the current
  `*_mkII` / `*_mkII_FMA` target names and include kernel 05.

Tests added:
- None. Benchmark-only phase.

Tests updated:
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05.cpp`
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `STATUS.md`

Exact commands run:
- `rg -n "kernel_0[1-9]_mkII|Rdot_mpfr_kernel|Raxpy_mpfr_kernel|add_executable\\(Rdot_mpfr|add_executable\\(Raxpy_mpfr" benchmarks CMakeLists.txt`
- `ls benchmarks/mpfr/00_Rdot benchmarks/mpfr/01_Raxpy`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01.cpp`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_04.cpp`
- `sed -n '120,190p' benchmarks/CMakeLists.txt`
- `sed -n '1,120p' benchmarks/CMakeLists.txt`
- `sed -n '1,120p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_02.cpp`
- `sed -n '1,120p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03.cpp`
- `sed -n '1,110p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01.cpp`
- `sed -n '1,120p' benchmarks/mpfr/00_Rdot/Rdot.hpp`
- `rg -n "mpfr_data\\(|get_mpfr_t\\(" include/gmpfrxx_mkII/detail/mpfr_impl.hpp | head -60`
- `rg -n "#define MPFRXX_ENABLE_FMA|enable_mpfr_fma|MPFR_C_NATIVE_USE_FMA" include benchmarks CMakeLists.txt`
- `sed -n '560,620p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '70,115p' benchmarks/common/run_mpfr_benchmarks.sh`
- `git diff -- benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05.cpp benchmarks/CMakeLists.txt benchmarks/common/run_mpfr_benchmarks.sh`
- `git status --short`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target Rdot_mpfr_kernel_05_mkII Rdot_mpfr_kernel_05_mkII_FMA`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05_mkII 1000 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05_mkII_FMA 1000 512`

Pass/fail result:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j --target Rdot_mpfr_kernel_05_mkII Rdot_mpfr_kernel_05_mkII_FMA`: PASS.
- `Rdot_mpfr_kernel_05_mkII 1000 512`: PASS, DIFF OK.
- `Rdot_mpfr_kernel_05_mkII_FMA 1000 512`: PASS, DIFF OK.

Known issues:
- Full CTest was not run for this benchmark-only phase.

Post-phase borrowed expression leaves for MPFR/MPC/MPFC:
DONE

Implemented features:
- Extended borrowed lvalue expression leaves from GMP MPF to MPFR, MPC, and
  GMP MPFC paths.
- Kept rvalue operands as owning expression leaves, so temporary lifetime
  safety is unchanged.
- Updated MPFR/MPC/MPFC precision discovery, evaluation, alias detection, and
  direct leaf fast paths to accept both owning and borrowed object leaves.
- Added ABI fingerprint assertions covering borrowed MPFR, MPC, and MPFC
  expression operands.

Tests added:
- None.

Tests updated:
- `tests/test_abi_fingerprint.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "make_mpfr_operand|object_leaf<mpfrxx::mpfr_class|is_mpfr_object_leaf|mpfr_try_assign_direct_leaf_binary|mpfr_compound_assign" include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `rg -n "make_mpc_operand|object_leaf<mpfrxx::mpc_class|is_mpc|mpc_compound_assign|mpc_evaluate" include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `rg -n "make_mpfc_operand|object_leaf<gmpxx::mpfc_class|is_mpfc|mpfc_compound_assign|mpfc_evaluate" include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1618,1820p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1938,2285p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '793,975p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '466,940p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `cmake --build build -j --target test_abi_fingerprint test_et_contract_mpfr test_et_contract_mpc test_et_contract_mpfc test_mpfr_numeric_equivalence test_mpc_basic test_mpfc_basic test_mpfr_compound_assign`
- `ctest --test-dir build -R "test_abi_fingerprint|test_et_contract_mpfr|test_et_contract_mpc|test_et_contract_mpfc|test_mpfr_numeric_equivalence|test_mpc_basic|test_mpfc_basic|test_mpfr_compound_assign" --output-on-failure`
- `git diff --check`
- `cmake --build build-release-nocount -j --target Raxpy_mpfr_kernel_01_mkII Raxpy_mpfr_kernel_01_mkII_FMA`
- `build-release-nocount/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_01_mkII 1000 512`
- `build-release-nocount/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_01_mkII_FMA 1000 512`

Pass/fail result:
- Focused debug build: PASS.
- Focused CTest: PASS, 9/9 tests passed.
- `git diff --check`: PASS.
- MPFR Raxpy release targets: PASS.
- MPFR Raxpy smoke run: PASS, result OK.
- MPFR Raxpy FMA smoke run: PASS, result OK.
- Full debug build: PASS.
- Initial full CTest exposed fixed-precision move-assignment allocation-count
  regressions in MPFR and MPC; fixed by separating fastpath move assignment
  from precision-preserving normal move assignment with `if constexpr`.
- Rerun of failing alloc-count tests: PASS, 2/2 tests passed.
- Final full CTest: PASS, 153/153 tests passed.

Known issues:
- None.

Post-phase zero-allocation move constructors:
DONE

Implemented features:
- Changed `gmpxx::mpf_class`, `mpfrxx::mpfr_class`, and
  `mpfrxx::mpc_class` move constructors to steal the backend object storage
  directly instead of allocating a fresh backend object and swapping.
- Added a moved-from validity flag so destructors do not clear stolen backend
  storage.
- Kept move assignment precision-preserving for valid left-hand-side objects
  in the normal precision-policy build.
- In `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` builds, move assignment
  swaps backend object storage for valid left-hand-side objects in `mpf_class`,
  `mpfr_class`, and `mpc_class`. This avoids clearing the old storage in the
  hot path and intentionally does not preserve left-hand-side precision.
- Invalid moved-from left-hand sides use direct storage steal because there is
  no valid backend object to swap with; they can still be repopulated by object
  assignment.
- `mpfc_class` inherits the fastpath behavior through its two `mpf_class`
  components.
- Split multiply-fusion compound-assignment rewrites out of the fixed-precision
  fastpath flag. `GMPXX_ENABLE_FMA` now controls GMP MPF `a += b * c` and
  `a -= b * c` scratch paths, while `MPFRXX_ENABLE_FMA` controls the MPFR
  `mpfr_fma` / `mpfr_fms` / `mpfr_fmma` / `mpfr_fmms` paths.
- Updated swap handling for `mpf_class` and `mpfr_class` so it remains safe
  when one side is moved-from.
- Added allocation-count coverage proving direct move construction and vector
  reallocation do not allocate GMP/MPFR/MPC backend storage.

Tests added:
- `tests/test_mpc_alloc_count.cpp`

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `include/gmpfrxx_mkII/detail/config.hpp`
- `README.md`
- `SPECIFICATIONS.md`
- `tests/CMakeLists.txt`
- `tests/test_mpf_alloc_count.cpp`
- `tests/test_mpfr_alloc_count.cpp`
- `tests/test_mpfr_fixed_precision_materialization.cpp`
- `tests/test_mpf_fixed_precision_tls_scratch.cpp`
- `tests/test_mpfr_fixed_precision_tls_scratch.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "mpf_class\\(mpf_class&&|mpfr_class\\(mpfr_class&&|mpc_class\\(mpc_class&&|mpfc_class\\(mpfc_class&&|~mpf_class|~mpfr_class|~mpc_class|operator=\\(.*&&|mpf_swap|mpfr_swap|mpc_swap|object_leaf" include/gmpfrxx_mkII/detail`
- `sed -n '1,150p' include/gmpfrxx_mkII/detail/expr.hpp`
- `sed -n '80,280p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '110,290p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '190,365p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '55,135p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpf_alloc_count test_mpfr_alloc_count test_mpc_alloc_count test_mpf_basic test_mpfr_expression_eval test_mpc_basic`
- `ctest --test-dir build -R 'test_mpf_alloc_count|test_mpfr_alloc_count|test_mpc_alloc_count|test_mpf_basic|test_mpfr_expression_eval|test_mpc_basic' --output-on-failure`
- `cmake --build build -j --target test_mpf_fixed_precision_tls_scratch test_mpfr_fixed_precision_tls_scratch test_mpc_alloc_count test_mpfc_precision_policy`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpfr_fixed_precision_tls_scratch|test_mpc_alloc_count|test_mpfc_precision_policy' --output-on-failure`
- `cmake --build build -j --target test_mpf_fixed_precision_tls_scratch test_mpfr_fixed_precision_tls_scratch test_mpfr_compound_assign test_mpc_alloc_count`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpfr_fixed_precision_tls_scratch|test_mpfr_compound_assign|test_mpc_alloc_count' --output-on-failure`
- `cmake --build build -j --target test_mpfr_fixed_precision_materialization test_mpfr_math`
- `ctest --test-dir build -R 'test_mpfr_fixed_precision_materialization|test_mpfr_math' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `git diff --stat`
- `git status --short`

Pass/fail result:
- Configure: PASS.
- Targeted build: PASS.
- Targeted CTest: PASS, 6/6 tests passed.
- Fastpath targeted build: PASS.
- Fastpath targeted CTest: PASS, 4/4 tests passed.
- Full build: PASS.
- Full CTest: PASS, 153/153 tests passed.
- `git diff --check`: PASS.

Known issues:
- Moved-from numeric objects are destructor-safe and assignment-safe for
  object assignment paths, but ordinary numeric operations on moved-from
  objects remain unsupported.

Post-phase nodiscard expression nodes:
DONE

Implemented features:
- Marked `unary_expr` and `binary_expr` as `[[nodiscard]]` so discarded
  expression-template results such as a standalone `a + b;` can trigger
  compiler diagnostics.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/expr.hpp`
- `STATUS.md`

Exact commands run:
- `sed -n '90,145p' include/gmpfrxx_mkII/detail/expr.hpp`
- `sed -n '145,180p' include/gmpfrxx_mkII/detail/expr.hpp`
- `cmake --build build -j --target test_et_contract_mpf test_et_contract_mpfr test_et_contract_mpc test_et_contract_mpfc test_mpf_basic test_mpfr_expression_eval test_mpc_basic`
- `ctest --test-dir build -R 'test_et_contract_mpf|test_et_contract_mpfr|test_et_contract_mpc|test_et_contract_mpfc|test_mpf_basic|test_mpfr_expression_eval|test_mpc_basic' --output-on-failure`

Pass/fail result:
- Targeted build: PASS.
- Targeted CTest: PASS, 8/8 tests passed.

Known issues:
- `[[nodiscard]]` is diagnostic-only and does not prevent storing an expression
  node in `auto e = a + b;`.

Post-phase expression-template lifetime documentation:
DONE

Implemented features:
- Added README guidance warning users not to bind expression templates to
  `auto` and let them outlive their operands.
- Added the same lifetime rule to `AGENTS.md` for future implementation work.
- Added the lifetime contract to `SPECIFICATIONS.md`.
- Documented that `[[nodiscard]]` catches discarded expressions but does not
  make saved expression nodes lifetime-safe.

Tests added:
- None. Documentation-only phase.

Tests updated:
- `README.md`
- `AGENTS.md`
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `ls -la README.md AGENTS.md SPECIFICATIONS.md`
- `rg -n "Expression|expression|auto e|pitfall|lifetime|dangling|nodiscard|Header Roles" README.md AGENTS.md SPECIFICATIONS.md`
- `git status --short`
- `sed -n '55,105p' README.md`
- `sed -n '160,210p' AGENTS.md`
- `sed -n '1,95p' SPECIFICATIONS.md`

Pass/fail result:
- Documentation update completed.

Known issues:
- None.

Post-phase header role specification:
DONE

Implemented features:
- Documented the public header role split in `SPECIFICATIONS.md`.
- Documented that `gmpxx_mkII.h` remains GMP-only and must not depend on MPFR
  or MPC.
- Documented that `mpfrxx_mkII.h` is the real MPFR header and does not require
  GNU MPC or `-lmpc`.
- Documented that `mpcxx_mkII.h` is the opt-in complex MPC header requiring
  GNU MPC and `-lmpc`.
- Documented public type ownership, `mpfrxx::mpz_class` /
  `mpfrxx::mpq_class` aliasing, and the separation between GMP-only
  `gmpxx::mpfc_class` and MPC-backed `mpfrxx::mpc_class`.
- Documented that `mpfrxx::mpc_class` defines equality and inequality only,
  with NaN components comparing unequal.

Tests added:
- None. Documentation-only phase.

Tests updated:
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `rg --files | rg '(^|/)include/.*(gmpxx|mpfrxx|mpcxx|gmpfrxx)_mkII\\.h$|(^|/)(gmpxx|mpfrxx|mpcxx|gmpfrxx)_mkII\\.h$'`
- `sed -n '1,220p' SPECIFICATIONS.md`
- `git status --short`
- `sed -n '1,180p' include/mpfrxx_mkII.h`
- `sed -n '1,180p' include/mpcxx_mkII.h`
- `sed -n '1,180p' include/gmpxx_mkII.h`
- `sed -n '1,180p' include/gmpfrxx_mkII.h`

Pass/fail result:
- Documentation update completed.

Known issues:
- None.

Post-phase MPFR/MPC default-state specification:
DONE

Implemented features:
- Added `SPECIFICATIONS.md` to record the MPFR/MPC default-state contract.
- Documented that MPFR default precision, rounding, and exponent range are
  owned by libmpfr and initialized once per thread through MPFR TLS state.
- Documented the MPFR TLS build requirement and the configure-time
  `mpfr_buildopt_tls_p()` expectation.
- Documented that MPFR default setters and reload affect only the calling
  thread.
- Documented that MPC defaults share MPFR TLS defaults and only symmetric
  real/imaginary defaults are supported.

Tests added:
- None.

Tests updated:
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `ls`
- `rg -n "SPEC|SPEF|default state|MPFR|MPC|TLS|thread" *.md docs include/gmpfrxx_mkII/detail/environment.hpp include/gmpfrxx_mkII/detail/mpc_environment.hpp`
- `git status --short`

Pass/fail result:
- Documentation update only; no runtime tests added in this phase.

Known issues:
- None.

Post-phase MPFR TLS default-state hard requirement:
DONE

Implemented features:
- Made MPFR TLS support a CMake configure-time hard requirement.  The wrapper
  routes MPFR default precision, rounding mode, and exponent range through
  libmpfr's default state, so non-TLS MPFR builds are unsupported.
- Documented the MPFR default-context policy in `README.md`: libmpfr TLS is the
  default-state owner, the wrapper does not define header-owned MPFR default
  TLS, and the wrapper initializes the default precision to 512 bits on first
  default access.
- Added direct test coverage that MPFR default construction starts at the
  wrapper 512-bit default without an explicit reload call.

Tests added:
- None.

Tests updated:
- `CMakeLists.txt`
- `README.md`
- `tests/test_mpfr_defaults.cpp`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpfr_defaults test_mpfr_thread_safety test_library_tls_detection example16_mpfr_thread_context`
- `ctest --test-dir build -R 'test_mpfr_defaults|test_mpfr_thread_safety|test_library_tls_detection|example16_mpfr_thread_context' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Configure: PASS with `MPFR buildopt TLS probe: has API=1, tls=1`.
- Focused build: PASS.
- Focused CTest: PASS, 4/4 tests passed.
- Full build: PASS.
- Full CTest: PASS, 152/152 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase expression leaf performance contract:
DONE

Implemented features:
- Kept lvalue expression operands as borrowed leaves to preserve inner-loop
  zero-allocation fast paths for existing wrapper objects.
- Documented the expression lifetime contract in `README.md`: expression nodes
  borrow lvalue operands, own rvalue operands, and saved lvalue expressions
  must not outlive their referenced operands.
- Backed out the owned-lvalue-leaf experiment because it made allocation-count
  tests fail and would regress BLAS-style inner loops.

Tests added:
- None.

Tests updated:
- `README.md`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target test_mpf_alloc_count test_mpfr_alloc_count test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count test_mpf_fixed_precision_tls_scratch test_mpfr_fixed_precision_tls_scratch test_mpfc_precision_policy test_mpz_addmul_alloc_count test_mpz_mpq_alloc_count test_temporary_expression_lifetime`
- `ctest --test-dir build -R 'test_mpf_alloc_count|test_mpfr_alloc_count|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|test_mpf_fixed_precision_tls_scratch|test_mpfr_fixed_precision_tls_scratch|test_mpfc_precision_policy|test_mpz_addmul_alloc_count|test_mpz_mpq_alloc_count|test_temporary_expression_lifetime' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused allocation/lifetime build: PASS.
- Focused allocation/lifetime CTest: PASS, 10/10 tests passed.
- Full build: PASS.
- Full CTest: PASS, 152/152 tests passed.

Known issues:
- Saved expressions containing lvalue wrapper operands are non-owning views.
  Users must materialize to a wrapper object when they need a stable snapshot.

Post-phase MPC default component policy:
DONE

Implemented features:
- Enforced the current MPC default policy that default MPC precision and
  rounding are backed by the shared libmpfr default state.
- `set_default_mpc_precision_bits(real, imag)` now throws
  `std::invalid_argument` when `real != imag` instead of silently using the
  maximum precision for both components.
- `set_default_mpc_rounding_mode(real, imag)` now throws
  `std::invalid_argument` when the rounding modes differ instead of silently
  doing nothing.
- MPC environment loading now throws `std::invalid_argument` when both real and
  imaginary component-specific precision or rounding variables are present and
  disagree.
- Documented that per-component defaults are not supported; use explicit
  `mpc_class::with_precision(real, imag)` for individual objects that need
  different component precision.

Tests added:
- Added exception coverage for mismatched MPC default precision and rounding
  setters.
- Added exception coverage for mismatched MPC real/imag environment defaults.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpc_environment.hpp`
- `tests/test_mpc_defaults.cpp`
- `tests/test_mpc_environment.cpp`
- `tests/test_mpc_precision_policy.cpp`
- `README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,220p' include/gmpfrxx_mkII/detail/mpc_environment.hpp`
- `rg -n "mpc_default_options|set_default_mpc|default_mpc|rounding|real_precision|imag_precision|mpc_rnd" include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/environment.hpp include/gmpfrxx_mkII/detail/mpc_environment.hpp tests`
- `cmake --build build -j --target test_mpc_defaults test_mpc_environment test_mpc_environment_invalid test_mpc_precision_policy`
- `ctest --test-dir build -R 'test_mpc_defaults|test_mpc_environment|test_mpc_environment_invalid|test_mpc_precision_policy' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused MPC default/environment build: PASS.
- Focused MPC default/environment CTest: PASS, 4/4 tests passed.
- Full build: PASS.
- Full CTest: PASS, 152/152 tests passed.

Known issues:
- MPC per-component default precision/rounding is intentionally unsupported
  while MPC defaults are routed through the shared libmpfr default state.

Post-phase MPFR fused compound multiply assignment:
DONE

Implemented features:
- Replaced the MPFR `a += b * c` direct multiply-expression compound path with
  `mpfr_fma(a, b, c, a, rnd)`.
- Replaced the MPFR `a -= b * c` direct multiply-expression compound path with
  `mpfr_fms(a, b, c, a, dual_rnd)` followed by an exact sign negation.
- Removed the `assume_fixed_precision_fastpath` dependency from MPFR direct
  multiply-expression add/sub compound assignment.  The fused path is now used
  whenever both multiply operands are MPFR object leaves.
- Removed the old product scratch helpers for MPFR add/sub multiply compound
  assignment.

Tests added:
- Added fused-rounding coverage for `a += b * c` and `a -= b * c` under
  `MPFR_RNDN`, `MPFR_RNDU`, `MPFR_RNDD`, and `MPFR_RNDZ`.

Tests updated:
- Updated MPFR compound assignment references to compare against `mpfr_fma`
  and dual-rounding `mpfr_fms` semantics.
- Updated the fixed-precision scratch allocation test to expect fused MPFR
  add/sub multiply results while preserving zero steady-state allocations.
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/test_mpfr_compound_assign.cpp`
- `tests/test_mpfr_fixed_precision_tls_scratch.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '2180,2270p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `rg -n "mpfr_compound|fma|fms|addmul|submul|mul_scratch" include tests`
- `cmake --build build -j --target test_mpfr_compound_assign test_mpfr_fixed_precision_tls_scratch test_mpfr_alloc_count`
- `ctest --test-dir build -R 'test_mpfr_compound_assign|test_mpfr_fixed_precision_tls_scratch|test_mpfr_alloc_count' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused MPFR compound/allocation build: PASS.
- Focused MPFR compound/allocation CTest: PASS, 3/3 tests passed.
- Full build: PASS.
- Full CTest: PASS, 152/152 tests passed.
- Final full CTest after test reference adjustment: PASS, 152/152 tests passed.

Known issues:
- `a -= b * c` uses dual rounding for the intermediate `b*c - a` value before
  exact negation; this is required for directed rounding correctness.

Post-phase MPFR default one-time initialization:
DONE

Implemented features:
- Added one-time MPFR default initialization on first wrapper default access.
- `mpfrxx::default_precision_bits()` and default `mpfr_class` construction now
  initialize libmpfr's default precision to the wrapper default 512 bits without
  requiring an explicit `reload_mpfr_defaults_from_environment()` call.
- Removed the explicit reload call from `example16_mpfr_thread_context`; the
  example still reports the main thread default as 512.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/environment.hpp`
- `examples/example16_mpfr_thread_context.cpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target example16_mpfr_thread_context test_mpfr_defaults test_mpfr_environment test_mpfr_precision_policy`
- `build/examples/example16_mpfr_thread_context`
- `ctest --test-dir build -R 'example16_mpfr_thread_context|test_mpfr_defaults|test_mpfr_environment|test_mpfr_precision_policy' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Manual example run: PASS.  Without an explicit reload call, output showed
  `thread-a` using default 160, `thread-b` using default 768, and the main
  thread default remaining 512.
- Focused CTest: PASS, 5/5 tests passed.
- Full build: PASS.
- Full CTest: PASS, 152/152 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase MPFR default precision correction:
DONE

Implemented features:
- Restored the wrapper built-in MPFR default precision fallback to 512 bits
  instead of inheriting libmpfr's raw C default precision.
- Kept MPFR default state routed through libmpfr: reload with no
  `MPFRXX_DEFAULT_PRECISION_BITS` now sets the libmpfr default precision to
  512, while an invalid precision environment value preserves the current
  libmpfr default precision.
- Updated `example16_mpfr_thread_context` to reload wrapper MPFR defaults at
  startup so the main thread shows the wrapper default 512-bit precision before
  worker threads use their own MPFR TLS defaults.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/environment.hpp`
- `examples/example16_mpfr_thread_context.cpp`
- `STATUS.md`

Exact commands run:
- `cmake --build build -j --target example16_mpfr_thread_context test_mpfr_defaults test_mpfr_environment`
- `build/examples/example16_mpfr_thread_context`
- `ctest --test-dir build -R 'example16_mpfr_thread_context|test_mpfr_defaults|test_mpfr_environment' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Initial focused CTest: FAIL in `test_mpfr_defaults` because invalid
  `MPFRXX_DEFAULT_PRECISION_BITS` fell back to 512 instead of preserving the
  current libmpfr default precision.
- Updated invalid-env handling to preserve the current libmpfr default.
- Focused CTest retry: PASS, 4/4 tests passed.
- Manual example run: PASS.  Output showed `thread-a` using default 160,
  `thread-b` using default 768, and the main thread default remaining 512.
- Full build: PASS.
- Full CTest: PASS, 152/152 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase GMP default-context provider shared owner:
DONE

Implemented features:
- Changed `gmpxx_mkII_default_context_provider` from a static library target to
  a shared library target, making the provider a single runtime owner for the
  mutable default-context storage.
- Updated README wording to require a dedicated provider shared library or one
  designated owner, and to warn against embedding the provider object into
  multiple shared libraries.

Tests added:
- None.

Tests updated:
- `CMakeLists.txt`
- `README.md`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target gmpxx_mkII_default_context_provider example16_mpf_thread_context test_mpf_external_provider`
- `ctest --test-dir build -R 'example16_mpf_thread_context|test_mpf_external_provider|link_fail_mpf_external_provider_missing' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Provider/example/external-provider build: PASS.  The provider now links as
  `libgmpxx_mkII_default_context_provider.so`.
- Focused provider CTest: PASS, 3/3 tests passed.
- Full build: PASS.
- Full CTest: PASS, 151/151 tests passed.
- `git diff --check`: PASS.

Known issues:
- A user can still manually compile the provider source into multiple DSOs
  outside this CMake target.  The documented supported configuration is one
  dedicated provider shared library or one designated owner.

Post-phase MPFR threaded default-precision example:
DONE

Implemented features:
- Added `examples/example16_mpfr_thread_context.cpp`, an MPFR variant of the
  threaded AGM pi example.
- The example uses `mpfrxx::set_default_precision_bits()` in two worker threads
  with 160-bit and 768-bit defaults, relying on libmpfr's default precision
  state rather than a wrapper provider.
- Preserved result precision when transferring worker results back to the main
  thread by move-constructing the result object instead of assigning into an
  existing `mpfr_class`, whose assignment intentionally preserves left-hand-side
  precision.

Tests added:
- Added CTest coverage for `example16_mpfr_thread_context`.

Tests updated:
- `examples/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target example16_mpfr_thread_context`
- `ctest --test-dir build -R example16_mpfr_thread_context --output-on-failure`
- `build/examples/example16_mpfr_thread_context`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j --target example16_mpfr_thread_context`: PASS.
- `ctest --test-dir build -R example16_mpfr_thread_context --output-on-failure`: PASS, 1/1 test passed.
- Manual example run: PASS.  Output showed `thread-a` using default 160,
  `thread-b` using default 768, and the main thread default remaining 53.
- Full build: PASS.
- Full CTest: PASS, 152/152 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase GMP-only MPF default precision context:
DONE

Implemented features:
- Added GMP-only default-context mode selection macros with frozen-env mode as
  the default and external-provider mode as an opt-in build mode.
- Added a GMP-only default-context implementation for `mpf_class` default
  construction.  Frozen-env mode reads
  `GMPXX_MKII_DEFAULT_MPF_PREC_BITS`, then
  `GMPFRXX_MKII_DEFAULT_MPF_PREC_BITS`, then the legacy
  `MPFXX_DEFAULT_PREC_BITS`, and otherwise uses 512 bits.
- Treats the frozen-env value as immutable after first use and fail-fast aborts
  on invalid environment values.
- Changed GMP `mpf_class` default construction to use explicit wrapper policy
  precision via `mpf_init2`, avoiding GMP's process-global MPF default.
- Added unsafe compatibility wrappers for the GMP C global default with names
  that explicitly mark them as ambient GMP-global APIs.
- Added an optional C ABI external-provider implementation with provider-owned
  thread-local storage and no header fallback.
- Added a GMP-only scoped default MPF precision guard for external-provider
  mode.
- Documented frozen-env and external-provider modes in `README.md`.

Tests added:
- `tests/test_mpf_default_precision_env.cpp`
- `tests/test_mpf_default_precision_invalid_env.cpp`
- `tests/test_mpf_external_provider.cpp`
- `tests/test_mpf_external_provider_missing.cpp`
- `tests/expect_process_failure.cmake`

Tests updated:
- `tests/test_mpf_precision_policy.cpp`
- `tests/test_mpf_thread_safety.cpp`
- `tests/test_gmpxx_mkII.cpp`
- `tests/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpf_precision_policy test_mpf_default_precision_env test_mpf_default_precision_invalid_env test_mpf_external_provider test_mpf_thread_safety test_mpf_fixed_precision_materialization test_random`
- `ctest --test-dir build -R 'test_mpf_precision_policy|test_mpf_default_precision_env|test_mpf_default_precision_invalid_env|test_mpf_external_provider|link_fail_mpf_external_provider_missing|test_mpf_thread_safety|test_mpf_fixed_precision_materialization|test_random' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `cmake --build build -j --target test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_gmpxx_mkII|test_mpf_precision_policy|test_mpf_default_precision_env|test_mpf_default_precision_invalid_env|test_mpf_external_provider|link_fail_mpf_external_provider_missing|test_mpf_thread_safety' --output-on-failure`
- `ctest --test-dir build --output-on-failure`
- `rg -n "#include <mpfr.h>|#include <mpc.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/gmp_default_context.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp src/gmpxx_mkII_default_context_provider.cpp`
- `rg -n "mpf_set_default_prec|mpf_get_default_prec" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/gmp_default_context.hpp src/gmpxx_mkII_default_context_provider.cpp`
- `git diff --check`

Pass/fail result:
- Targeted GMP MPF default-context build: PASS.
- Targeted GMP MPF default-context CTest: PASS, 8/8 tests passed.
- Full build: PASS.
- Initial full CTest: FAIL because existing `test_gmpxx_mkII` constant-cache
  tests assumed runtime GMP global default precision mutation affected wrapper
  default precision.
- Updated those tests to use explicit precision overloads.
- Retried targeted CTest: PASS, 7/7 tests passed.
- Retried full CTest: PASS, 150/150 tests passed.
- GMP-only MPFR/MPC source scan: PASS, no matches.
- GMP global default source scan: PASS, only the unsafe ambient GMP wrapper
  functions reference `mpf_set_default_prec` and `mpf_get_default_prec`.
- `git diff --check`: PASS.

Known issues:
- In frozen-env mode, `set_default_mpf_precision_bits()` remains as a source
  compatibility no-op.  Use explicit precision construction or external-provider
  mode for mutable thread-local defaults.

Post-phase GMP MPF threaded default-context example:
DONE

Implemented features:
- Added `examples/example16_mpf_thread_context.cpp`, a two-thread
  Gauss-Legendre / Brent-Salamin pi example derived from `example04_mpf`.
- Linked the example with the optional GMP default-context provider so each
  worker thread can use a different wrapper default MPF precision.
- The example demonstrates `gmpxx::default_mpf_precision_guard` with
  160-bit and 768-bit worker defaults while the main thread remains at the
  provider initial default.

Tests added:
- Added CTest coverage for `example16_mpf_thread_context`.

Tests updated:
- `examples/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target example16_mpf_thread_context`
- `ctest --test-dir build -R example16_mpf_thread_context --output-on-failure`
- `build/examples/example16_mpf_thread_context`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j --target example16_mpf_thread_context`: PASS.
- `ctest --test-dir build -R example16_mpf_thread_context --output-on-failure`: PASS, 1/1 test passed.
- Manual example run: PASS.  Output showed `thread-a` using default 160,
  `thread-b` using default 768, and the main thread default remaining 512.
- Full build: PASS.
- Full CTest: PASS, 151/151 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase MPFR Raxpy benchmark port:
DONE

Implemented features:
- Added MPFR Raxpy benchmark sources under `benchmarks/mpfr/01_Raxpy`.
- Ported the GMP Raxpy benchmark variants to MPFR:
  C native, C native OpenMP, mkII kernel 01/02, and mkII OpenMP kernel 01/02.
- Added fixed-precision-fastpath builds for the mkII MPFR Raxpy variants.
- Registered MPFR Raxpy targets in `benchmarks/CMakeLists.txt`.
- Extended `benchmarks/common/run_mpfr_benchmarks.sh` to run and plot both
  Rdot and Raxpy.  The optional sixth argument overrides the Raxpy vector
  size; otherwise it uses the Rdot vector size.

Tests added:
- None.

Tests updated:
- `benchmarks/README.md`
- `benchmarks/mpfr/README.md`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build_bench_clean_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_clean_release -j --target Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_kernel_01_mkII Raxpy_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Raxpy_mpfr_kernel_02_mkII Raxpy_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH Raxpy_mpfr_kernel_openmp_01_mkII Raxpy_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Raxpy_mpfr_kernel_openmp_02_mkII Raxpy_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH`
- `benchmarks/common/run_mpfr_benchmarks.sh build_bench_clean_release 512 200 benchmarks/mpfr/results_raw/raxpy_port_smoke_20260509 1`

Pass/fail result:
- CMake configure: PASS.
- All new MPFR Raxpy benchmark targets built successfully.
- MPFR Rdot/Raxpy smoke run: PASS; every Raxpy variant printed `Result OK`,
  and Rdot variants printed `OK`.

Known issues:
- MPFR Rgemv and Rgemm benchmark families are still not ported.

Post-phase MPFR Rgemv benchmark port:
DONE

Implemented features:
- Added MPFR Rgemv benchmark sources under `benchmarks/mpfr/02_Rgemv`.
- Ported the GMP Rgemv benchmark variants to MPFR:
  C native, C native OpenMP, mkII kernel 01/02, and mkII OpenMP kernel 01/02.
- Added fixed-precision-fastpath builds for the mkII MPFR Rgemv variants.
- Registered MPFR Rgemv targets in `benchmarks/CMakeLists.txt`.
- Extended `benchmarks/common/run_mpfr_benchmarks.sh` to run and plot Rdot,
  Raxpy, and Rgemv.  The optional seventh and eighth arguments override Rgemv
  row and column counts.

Tests added:
- None.

Tests updated:
- `benchmarks/README.md`
- `benchmarks/mpfr/README.md`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build_bench_clean_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_clean_release -j --target Rgemv_mpfr_C_native_01 Rgemv_mpfr_C_native_openmp_01 Rgemv_mpfr_kernel_01_mkII Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_02_mkII Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_01_mkII Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_02_mkII Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH`
- `benchmarks/common/run_mpfr_benchmarks.sh build_bench_clean_release 512 50 benchmarks/mpfr/results_raw/rgemv_port_smoke_20260510 1 50 8 7`

Pass/fail result:
- CMake configure: PASS.
- All new MPFR Rgemv benchmark targets built successfully.
- MPFR Rdot/Raxpy/Rgemv smoke run: PASS; every Rgemv variant printed
  `Result OK`, and Rdot variants printed `OK`.

Known issues:
- MPFR Rgemm benchmark family is still not ported.

Post-phase MPFR Rgemm benchmark port:
DONE

Implemented features:
- Added MPFR Rgemm benchmark sources under `benchmarks/mpfr/03_Rgemm`.
- Ported the GMP Rgemm benchmark variants to MPFR:
  C native 01/02, C native OpenMP 01/02, mkII kernel 01/02/03, and mkII
  OpenMP kernel 01/02/03.
- Added fixed-precision-fastpath builds for the mkII MPFR Rgemm variants.
- Registered MPFR Rgemm targets in `benchmarks/CMakeLists.txt`.
- Extended `benchmarks/common/run_mpfr_benchmarks.sh` to run and plot Rdot,
  Raxpy, Rgemv, and Rgemm.  The optional ninth through eleventh arguments
  override Rgemm `m`, `k`, and `n`.

Tests added:
- None.

Tests updated:
- `benchmarks/README.md`
- `benchmarks/mpfr/README.md`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build_bench_clean_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_clean_release -j --target Rgemm_mpfr_C_native_01 Rgemm_mpfr_C_native_02 Rgemm_mpfr_C_native_openmp_01 Rgemm_mpfr_C_native_openmp_02 Rgemm_mpfr_kernel_01_mkII Rgemm_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rgemm_mpfr_kernel_02_mkII Rgemm_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rgemm_mpfr_kernel_03_mkII Rgemm_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rgemm_mpfr_kernel_openmp_01_mkII Rgemm_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rgemm_mpfr_kernel_openmp_02_mkII Rgemm_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemm_mpfr_kernel_openmp_03_mkII Rgemm_mpfr_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH`
- `benchmarks/common/run_mpfr_benchmarks.sh build_bench_clean_release 512 30 benchmarks/mpfr/results_raw/rgemm_port_smoke_20260510 1 30 6 5 4 3 5`

Pass/fail result:
- CMake configure: PASS.
- All new MPFR Rgemm benchmark targets built successfully.
- MPFR Rdot/Raxpy/Rgemv/Rgemm smoke run: PASS; every Rgemm variant printed
  `Result OK`, and Rdot variants printed `OK`.

Known issues:
- None.

Post-phase benchmark runner parity for MPFR:
DONE

Implemented features:
- Split benchmark raw result directories by backend: GMP results live under
  `benchmarks/gmp/results_raw`, and MPFR results live under
  `benchmarks/mpfr/results_raw`.
- Added `benchmarks/common/run_mpfr_benchmarks.sh` with the same log format,
  repeat handling, host naming, and plot generation flow as the GMP runner.
- Made `benchmarks/common/plot.py` backend-aware via `--backend` and kernel-list
  aware via `--kernels`, so MPFR Rdot-only logs can be plotted without requiring
  GMP Raxpy/Rgemv/Rgemm sections.
- Kept the GMP runner behavior unchanged except for passing `--backend GMP` to
  the shared plotter.

Tests added:
- None.

Tests updated:
- `benchmarks/README.md`
- `benchmarks/mpfr/README.md`
- `STATUS.md`

Exact commands run:
- `bash -n benchmarks/common/run_benchmarks.sh`
- `bash -n benchmarks/common/run_mpfr_benchmarks.sh`
- `python3 -m py_compile benchmarks/common/plot.py`
- `cmake -S . -B build_bench_clean_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_clean_release -j`
- `benchmarks/common/run_benchmarks.sh build_bench_clean_release 512 2000 2000 40 40 16 16 16 benchmarks/gmp/results_raw/clean_release_20260509_smoke 1`
- `benchmarks/common/run_mpfr_benchmarks.sh build_bench_clean_release 512 2000 benchmarks/mpfr/results_raw/clean_release_20260509_smoke 1`
- `find benchmarks/gmp/results_raw/clean_release_20260509_smoke -maxdepth 1 -type f | sort`
- `find benchmarks/mpfr/results_raw/clean_release_20260509_smoke -maxdepth 1 -type f | sort`

Pass/fail result:
- `bash -n benchmarks/common/run_benchmarks.sh`: PASS.
- `bash -n benchmarks/common/run_mpfr_benchmarks.sh`: PASS.
- `python3 -m py_compile benchmarks/common/plot.py`: PASS.
- `cmake -S . -B build_bench_clean_release -DCMAKE_BUILD_TYPE=Release`: PASS.
- `cmake --build build_bench_clean_release -j`: PASS.
- GMP clean Release smoke benchmark: PASS; generated log and serial/openmp PNG
  plots under `benchmarks/gmp/results_raw/clean_release_20260509_smoke`.
- MPFR clean Release smoke benchmark: PASS; generated log and serial/openmp PNG
  plots under `benchmarks/mpfr/results_raw/clean_release_20260509_smoke`.

Known issues:
- The clean benchmark runs used smoke-size inputs for runner validation, not
  full publication-size benchmark parameters.

Post-phase Rdot production benchmark at reduced size:
DONE

Implemented features:
- Ran GMP and MPFR Rdot-only benchmark sets at one decimal order below the
  full publication Rdot size.
- Generated raw logs and serial/OpenMP plots for both backends.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `/bin/bash -lc '... GMP Rdot-only benchmark loop ...'`
- `benchmarks/common/run_mpfr_benchmarks.sh build_bench_clean_release 512 10000000 benchmarks/mpfr/results_raw/rdot_n1e7_20260509 10`
- `python3 benchmarks/common/plot.py benchmarks/gmp/results_raw/rdot_n1e7_20260509/benchmark_rdot_n1e7_20260509_143146.log --output-dir benchmarks/gmp/results_raw/rdot_n1e7_20260509 --backend GMP --kernels Rdot`
- `python3 -c '... aggregate benchmark logs ...'`
- `find benchmarks/gmp/results_raw/rdot_n1e7_20260509 -maxdepth 1 -type f | sort`
- `find benchmarks/mpfr/results_raw/rdot_n1e7_20260509 -maxdepth 1 -type f | sort`

Pass/fail result:
- GMP Rdot-only reduced production run: PASS; 20 variants, 10 repeats each,
  `n=10000000`, precision 512.
- MPFR Rdot-only reduced production run: PASS; 14 variants, 10 repeats each,
  `n=10000000`, precision 512.
- GMP plot generation: PASS.
- MPFR plot generation: PASS.

Known issues:
- GMP Rdot-only run was executed with an ad hoc shell loop because the common
  GMP runner currently runs all GMP kernels together.

Post-phase MPC string API, comparison semantics, and SFINAE notes:
DONE

Implemented features:
- Added a shared comment in `detail/expr.hpp` documenting why some public
  operator templates use distinct dummy SFINAE parameter types across numeric
  domains.
- Added `mpfrxx::mpc_class::set_str(const char*, int)` and
  `set_str(const std::string&, int)`.
- `mpc_class::set_str` first tries GNU MPC `mpc_set_str`, then falls back to
  this repo's existing `(real,imag)` / `a+bi` parser so native MPC strings and
  existing stream-style strings both work.
- Added `mpc_class::get_str(int, std::size_t)` backed by `mpc_get_str` and
  `mpc_class::to_string(std::size_t)` as the decimal convenience wrapper.
- Added `real_get_d()` and `imag_get_d()` aliases for the existing
  `real_to_double()` and `imag_to_double()` accessors.
- Changed string assignment to route through `set_str`, preserving destination
  precision and throwing `std::invalid_argument` on invalid input.
- Made `mpc_class` equality explicitly NaN-aware.  GNU MPC in this build
  returns zero from `mpc_cmp(nan,nan)`, so the wrapper now checks NaN
  components before calling `mpc_cmp`.
- Documented that `mpc_class` has equality/inequality only, no ordering
  operators, and that NaN components compare unequal.

Missing features:
- No single `get_d()` is provided for `mpc_class` because a complex value does
  not have a single unambiguous double conversion.  Use `real_get_d()` and
  `imag_get_d()`.

Tests added:
- Extended `tests/test_mpc_io.cpp` with `set_str`, `get_str`, `to_string`,
  `real_get_d`, `imag_get_d`, round-trip parsing, base-0 hex parsing, invalid
  input preservation, and throwing assignment checks.
- Extended `tests/test_mpc_basic.cpp` to assert that an MPC value with a NaN
  component compares unequal to itself and has no ordering operators.

Exact commands run:
- `cmake --build build -j --target test_mpc_io test_mpc_basic`
- `ctest --test-dir build -R 'test_mpc_io|test_mpc_basic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- First focused ctest failed because GNU MPC `mpc_cmp(nan,nan)` returned zero
  on this platform; fixed wrapper equality to check NaN components explicitly.
- Focused build after the fix: PASS.
- Focused ctest after the fix: PASS, 2/2 tests passed.
- Full build: PASS.
- Full ctest: PASS, 144/144 tests passed.

Known issues:
- None for the added MPC string API and equality semantics.

Post-phase MPFC polar cleanup and unary negation simplification:
DONE

Implemented features:
- Removed the unused local precision variable from `gmpxx::polar(mpf_class,
  mpf_class)`.
- Added unary-expression simplification for `-(+expr)` so odd-depth negation
  chains such as `-(-(-x))` collapse back to a direct `neg_op` node instead
  of leaving a nested `neg(pos(x))` shape.
- Kept `mpz_class(__int128_t)` and `mpz_class(__uint128_t)` implicit by
  policy, matching the existing integer-constructor rule.  Floating
  construction remains explicit and expression-template scalar leaves still
  reject `__int128`.

Missing features:
- None.

Tests added:
- Extended `tests/test_unary_minus_simplification.cpp` with static assertions
  that three nested unary minuses on MPF and MPFR expressions produce a
  `neg_op` expression node.
- Extended `tests/test_type_conversions.cpp` to assert that `double` is not
  implicitly convertible to `mpz_class`, while `__int128_t` and
  `__uint128_t` are intentionally implicitly convertible when the compiler
  supports them.

Exact commands run:
- `cmake --build build -j --target test_unary_minus_simplification test_type_conversions test_mpfc_basic`
- `ctest --test-dir build -R 'test_unary_minus_simplification|test_type_conversions|test_mpfc_basic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused build: PASS.
- Focused ctest: PASS, 3/3 tests passed.
- Full build: PASS.
- Full ctest: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase checked MPFR shift counts and MPZ divide-by-zero:
DONE

Implemented features:
- Replaced the generic MPFR shift apply path's direct `mpfr_get_ui` conversion
  with a checked `mpfr_t` to `mpz_t` conversion followed by
  `zq_shift_count_from_mpz`.
- The generic `mpfr_apply_binary<shl_op/shr_op>` path now rejects NaN,
  infinity, non-integer, negative, and `unsigned long`-overflowing shift
  counts with C++ exceptions instead of relying on MPFR erange/truncation
  behavior.
- Added explicit divide-by-zero checks before `mpz_tdiv_q` in MPZ expression
  evaluation and direct compound division.
- Routed `mpz_class /= double` through the checked `mpz_class` compound
  division path so `0.0` is handled consistently.

Missing features:
- MPZ modulo-by-zero still follows the existing GMP-style raw `mpz_tdiv_r`
  path; this phase only changed division as requested.

Tests added:
- Extended `tests/test_mpfr_power_of_two_fusion.cpp` with direct regression
  coverage for `mpfr_apply_binary<shl_op/shr_op>` using valid, negative,
  fractional, and `unsigned long`-overflowing shift counts.
- Extended `tests/test_mpz_arithmetic.cpp` to assert `std::domain_error` for
  expression division by zero and compound division by `mpz_class`, integral,
  and double zero operands.

Exact commands run:
- `cmake --build build -j --target test_mpfr_power_of_two_fusion test_mpz_arithmetic`
- `ctest --test-dir build -R 'test_mpfr_power_of_two_fusion|test_mpz_arithmetic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Focused build: PASS.
- Focused ctest: PASS, 2/2 tests passed.
- Full build: PASS.
- Full ctest: PASS, 144/144 tests passed.

Known issues:
- None for the changed division and checked MPFR shift-count paths.

Post-phase MPC precision max and integral scalar fast paths:
DONE

Implemented features:
- Changed `mpfrxx::mpc_class::precision()` from the minimum component
  precision to the maximum component precision.
- This aligns MPC with `gmpxx::mpfc_class::precision()` and with internal MPC
  expression contexts that already use the maximum real/imaginary precision.
- Added direct `mpf_set_si` / `mpf_set_ui` fast paths in
  `gmpxx::mpf_class::set_integral` when the public integral type fits in
  `long` or `unsigned long`.
- Added direct `mpfr_set_si` / `mpfr_set_ui` fast paths in
  `mpfrxx::mpfr_class::set_integral` under the current destination precision
  and rounding context.
- Kept the existing `mpz_class` conversion fallback for wider public integer
  types, preserving exact `int64_t` / `uint64_t` behavior on platforms where
  they do not fit in C `long`.

Missing features:
- None.

Tests added:
- Extended `tests/test_mpc_precision_policy.cpp` to assert that mixed
  real/imaginary MPC precision reports the maximum component precision.
- Extended `tests/test_mpf_scalar_alloc_count.cpp` and
  `tests/test_mpfr_scalar_alloc_count.cpp` with direct signed/unsigned long
  construction and assignment checks.
- Constructor cases now expect only the wrapper-owned value allocation, while
  existing-object assignment from fitting integral values expects zero
  allocations.

Exact commands run:
- `rg -n "precision\\(\\) const noexcept|set_integral\\(|mpf_set_si|mpfr_set_si|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|long_width" include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests && sed -n '390,420p' include/gmpfrxx_mkII/detail/mpc_impl.hpp && sed -n '560,620p' include/gmpfrxx_mkII/detail/mpf_impl.hpp && sed -n '1160,1205p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '540,575p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp && sed -n '1,180p' tests/test_mpf_scalar_alloc_count.cpp && sed -n '1,190p' tests/test_mpfr_scalar_alloc_count.cpp && sed -n '1,120p' tests/test_mpc_precision_policy.cpp`
- `sed -n '35,55p' include/gmpfrxx_mkII/detail/mpf_impl.hpp && sed -n '35,60p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp && sed -n '120,180p' tests/test_mpc_precision_policy.cpp`
- `cmake --build build -j --target test_mpc_precision_policy test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count test_construction_copy && ctest --test-dir build -R 'test_mpc_precision_policy|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|test_construction_copy' --output-on-failure`
- `cmake --build build -j --target test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count && ctest --test-dir build -R 'test_mpc_precision_policy|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|test_construction_copy' --output-on-failure`

Pass/fail result:
- First focused ctest failed because the new constructor allocation tests
  expected zero allocations but counted the unavoidable wrapper-owned
  `mpf_init2` / `mpfr_init2` value allocation; corrected constructor
  expectations to one allocation and kept assignment expectations at zero.
- `cmake --build build -j --target test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count`: PASS after fixes.
- `ctest --test-dir build -R 'test_mpc_precision_policy|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|test_construction_copy' --output-on-failure`: PASS, 4/4 tests passed.

Known issues:
- None.

Post-phase MPFC Smith division:
DONE

Implemented features:
- Replaced `gmpxx::mpfc_class` division with Smith-style scaled complex
  division.
- Avoided forming `c*c + d*d` directly in the denominator.
- Kept the non-alias division path at two temporary `mpf_t` values by using
  only the Smith ratio and scale scratch values.
- Preserved the alias-safe path by computing into temporary real/imaginary
  components before assigning back to the destination.
- Used an allocation-free `mpf_get_d_2exp` based magnitude comparison for the
  Smith branch choice because this GMP environment does not provide
  `mpf_cmpabs`.
- Clarified README limitations: MPFC division is scaled, but MPFC remains an
  MPF-backed convenience type without MPC-style correct rounding or systematic
  guard-bit evaluation.

Missing features:
- `mpfc_class` multiplication still evaluates `ac - bd` and `ad + bc` at the
  destination MPF precision without systematic guard bits.
- `mpfc_class` inverse and transcendental functions still compose MPF-backed
  formulas directly; cancellation-sensitive workloads should use
  `mpfrxx::mpc_class`.

Tests added:
- Updated `tests/test_mpfc_precision_policy.cpp` division checks to compare
  within MPF tolerance rather than bit-for-bit equality, because Smith division
  changes the finite-precision operation order for non-binary-finite results.
- Existing MPFC arithmetic smoke tests cover both Smith branch directions via
  denominators where either the real or imaginary component is dominant.

Exact commands run:
- `sed -n '720,780p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp && sed -n '100,140p' tests/test_mpfc_precision_policy.cpp && sed -n '1,80p' tests/test_mpfc_basic.cpp`
- `sed -n '1,170p' tests/test_mpfc_precision_policy.cpp`
- `cmake --build build -j --target test_mpfc_basic test_mpfc_precision_policy test_mpfc_math && ctest --test-dir build -R 'test_mpfc_basic|test_mpfc_precision_policy|test_mpfc_math' --output-on-failure`
- `cmake --build build -j --target test_mpfc_precision_policy && ctest --test-dir build -R 'test_mpfc_basic|test_mpfc_precision_policy|test_mpfc_math' --output-on-failure`

Pass/fail result:
- First focused build failed because this GMP environment does not expose
  `mpf_cmpabs`; replaced the branch comparison with an allocation-free
  `mpf_get_d_2exp` helper.
- First focused ctest failed because the new operation order no longer matched
  the old bit-for-bit expected value for a non-binary-finite quotient; changed
  division assertions to MPF tolerance checks.
- `cmake --build build -j --target test_mpfc_precision_policy`: PASS after fixes.
- `ctest --test-dir build -R 'test_mpfc_basic|test_mpfc_precision_policy|test_mpfc_math' --output-on-failure`: PASS, 3/3 tests passed.

Known issues:
- Smith division improves scaling and avoids direct denominator squaring, but
  it is still MPF finite-precision arithmetic and does not imply correct
  rounding.

Post-phase MPFC numeric caveats and stable sqrt:
DONE

Implemented features:
- Replaced `gmpxx::sqrt(mpfc_class)` with Smith-style branch formulas.
- For `Re(z) >= 0`, the imaginary part is computed as `Im(z) / (2w)` instead
  of `sqrt((|z| - Re(z)) / 2)`, avoiding the cancellation path near the
  positive real axis.
- For `Re(z) < 0`, the real part is computed from `abs(Im(z)) / (2w)` while
  preserving the sign of the imaginary part.
- Documented that `gmpxx::mpfc_class` is a GMP-only convenience complex type,
  not an MPC replacement, and does not promise MPC-style correctly rounded
  complex arithmetic or systematic guard-bit evaluation.

Missing features:
- `mpfc_class` multiplication and division still evaluate with the destination
  MPF precision and do not use systematic guard bits.
- `mpfc_class` inverse and transcendental functions still compose MPF-backed
  formulas directly; cancellation-sensitive workloads should use
  `mpfrxx::mpc_class`.

Tests added:
- Extended `tests/test_mpfc_math.cpp` with a near-positive-real-axis square
  root case where the previous `sqrt((|z| - Re(z)) / 2)` formula could round
  the imaginary component to zero.

Exact commands run:
- `rg -n "mpfc_apply_binary|sqrt\\(.*mpfc|asin\\(|acos\\(|atan\\(|asinh\\(|atanh\\(|mpfc" include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp README.md STATUS.md docs tests -g '*.*' && sed -n '1,220p' include/gmpfrxx_mkII/detail/math_mpfc.hpp && sed -n '680,790p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1,220p' tests/test_mpfc_math.cpp && sed -n '180,220p' README.md && sed -n '1,130p' STATUS.md`
- `cmake --build build -j --target test_mpfc_math && ctest --test-dir build -R 'test_mpfc_math' --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfc_math`: PASS.
- `ctest --test-dir build -R 'test_mpfc_math' --output-on-failure`: PASS, 1/1 tests passed.

Known issues:
- `mpfc_class` remains an MPF-backed compromise complex type. It is suitable
  for GMP-only convenience and compatibility coverage, but MPC should be used
  when complex correct rounding or robust guard-bit behavior matters.

Post-phase MPC move assignment precision semantics:
DONE

Implemented features:
- Aligned `mpfrxx::mpc_class` move assignment with the destination-precision
  semantics used by `gmpxx::mpf_class` and `mpfrxx::mpfr_class`.
- `mpc_class::operator=(mpc_class&&)` now keeps the existing destination real
  and imaginary precisions when the source precision differs.
- The move-assignment fast path still uses `mpc_swap` when both real and
  imaginary precisions match.
- The differing-precision path now uses `mpc_set` under the destination
  evaluation context and checks component exponent ranges.

Missing features:
- None.

Tests added:
- Extended `tests/test_mpc_precision_policy.cpp` to cover move assignment
  from a same-precision source and from a lower-precision source.
- The lower-precision source case verifies that destination precision remains
  unchanged while the moved value is copied correctly.

Exact commands run:
- `rg -n "mpc_class\\(mpc_class&&|operator=\\(mpc_class&&|void swap\\(mpc_class|mpc_class& operator=|real_precision\\(\\)" include/gmpfrxx_mkII/detail/mpc_impl.hpp tests/test_mpc_basic.cpp tests/test_mpc_precision_policy.cpp tests/test_construction_copy.cpp && sed -n '60,150p' include/gmpfrxx_mkII/detail/mpc_impl.hpp && sed -n '1,220p' tests/test_mpc_precision_policy.cpp && sed -n '1,180p' tests/test_construction_copy.cpp`
- `sed -n '150,335p' include/gmpfrxx_mkII/detail/mpc_impl.hpp && sed -n '180,270p' tests/test_mpc_basic.cpp`
- `rg -n "mpfr_class& operator=\\(mpfr_class&&|mpf_class& operator=\\(mpf_class&&|mpfc_class& operator=\\(mpfc_class&&" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp && sed -n '180,235p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp && sed -n '180,235p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '229,255p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp && sed -n '302,325p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `rg -n "mpc_class" tests/test_construction_copy.cpp`
- `cmake --build build -j --target test_mpc_precision_policy && ctest --test-dir build -R 'test_mpc_precision_policy' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpc_precision_policy`: PASS.
- `ctest --test-dir build -R 'test_mpc_precision_policy' --output-on-failure`: PASS, 1/1 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase MPFC exact real promotion:
DONE

Implemented features:
- Treated the documented `mpfc + mpz/mpq -> mpfc` and
  `mpz/mpq + mpfc -> mpfc` promotion rules as implementation gaps, not a
  specification change.
- Added `gmpxx::mpz_class` and `gmpxx::mpq_class` as valid MPFC expression
  operands.
- Added MPFC operand wrapping, precision propagation, alias-reference checks,
  and real-only complex evaluation for exact `mpz_class` and `mpq_class`
  leaves.
- Exact real leaves evaluate as `(value, 0)` and do not contribute their own
  precision; the surrounding MPFC/MPF operands or destination precision govern
  materialization.
- Exact Z/Q expression nodes may now participate in an MPFC expression when an
  MPFC operand is present, for example `(z + q) + c`.

Missing features:
- None.

Tests added:
- Extended `tests/test_et_contract_mpfc.cpp` with `mpfc +/-/*// mpz/mpq`
  expression-node formation checks, both operand orders, and an exact
  subexpression promotion case.
- Extended `tests/test_abi_fingerprint.cpp` to assert MPFC operand eligibility
  and result type promotion for `mpz_class` and `mpq_class`.
- Extended `tests/test_mpfc_basic.cpp` with runtime arithmetic, precision
  preservation, exact subexpression, and compound-assignment coverage for
  `mpfc_class` with `mpz_class` / `mpq_class`.

Exact commands run:
- `git status --short && sed -n '360,575p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp && sed -n '1,180p' tests/test_et_contract_mpfc.cpp && sed -n '1,220p' tests/test_mpfc_basic.cpp && sed -n '1,140p' tests/test_abi_fingerprint.cpp`
- `sed -n '140,260p' tests/test_abi_fingerprint.cpp && sed -n '220,520p' tests/test_mpfc_basic.cpp && sed -n '575,735p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp && rg -n "mpf_set_q_at_precision|mpf_set_z" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1410,1445p' include/gmpfrxx_mkII/detail/mpf_impl.hpp && sed -n '735,825p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `cmake --build build -j --target test_et_contract_mpfc test_mpfc_basic test_abi_fingerprint test_common_type`
- `ctest --test-dir build -R 'test_et_contract_mpfc|test_mpfc_basic|test_abi_fingerprint|test_common_type' --output-on-failure`
- `cmake --build build -j --target test_mpfc_basic && ctest --test-dir build -R 'test_et_contract_mpfc|test_mpfc_basic|test_abi_fingerprint|test_common_type' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- First focused build exposed a missing `mpfc_expression_references` overload
  for exact Z/Q leaves; fixed by adding false-returning exact-leaf overloads.
- First focused ctest exposed a non-binary-finite expected value in the new
  runtime test; fixed the test inputs to use binary-finite exact results.
- `cmake --build build -j --target test_et_contract_mpfc test_mpfc_basic test_abi_fingerprint test_common_type`: PASS after fixes.
- `ctest --test-dir build -R 'test_et_contract_mpfc|test_mpfc_basic|test_abi_fingerprint|test_common_type' --output-on-failure`: PASS, 4/4 tests passed after fixes.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase MPC constructor and assignment surface:
DONE

Implemented features:
- Added direct `mpfrxx::mpc_class` construction from:
  - `mpfrxx::mpfr_class`
  - `mpfrxx::mpz_class`
  - `mpfrxx::mpq_class`
  - supported scalar real values (`int`, `uint64_t`, `float`, `double`, etc.;
    `bool` remains rejected)
  - supported scalar real/imag pairs
  - `const char*` and `std::string`
- Added matching assignment from:
  - `mpfrxx::mpfr_class`
  - `mpfrxx::mpz_class`
  - `mpfrxx::mpq_class`
  - supported scalar real values
  - `const char*` and `std::string`
- String construction and assignment now accept both the stream-compatible
  `(real,imag)` form and an `a+bi` / `a-bi` suffix form.
- String parsing preserves exponent signs such as `1.25e+2-3.75e-1i` when
  finding the real/imaginary separator.
- String assignment materializes into a temporary at the destination
  precision, so failed parsing does not clobber the existing value.
- Single-real construction and assignment set the imaginary part to zero.
- `mpc_class(mpfr_class)` uses the MPFR value precision for both real and
  imaginary components, matching the MPFC real-value constructor policy.

Missing features:
- The string parser intentionally supports practical wrapper forms rather than
  every textual form accepted by `mpc_set_str`.

Tests added:
- Extended `tests/test_mpc_basic.cpp` with compile-time constructibility checks
  for scalar, exact, MPFR, and string constructors.
- Added runtime constructor coverage for `double`, `int`, `mpz_class`,
  `mpq_class`, `mpfr_class`, stream-form strings, `a+bi` strings, exponent
  strings, and base-0 hex strings.
- Added assignment coverage for scalar, MPFR, exact values, `const char*`, and
  `std::string`, including destination precision preservation.

Exact commands run:
- `sed -n '40,155p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1260,1345p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '60,150p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1,180p' tests/test_mpc_basic.cpp`
- `sed -n '155,245p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `rg -n "mpc_set_|mpc_assign|make_mpc_operand|is_supported.*scalar|scalar_leaf<.*mpc" include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '280,520p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1318,1368p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1180,1235p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '640,780p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1368,1398p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '780,835p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `rg -n "void swap\\(mpc_class|swap\\(mpc_class|mpc_class::swap" include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '28,45p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `cmake --build build -j --target test_mpc_basic test_mpc_io test_mpc_defaults test_mpc_environment test_et_contract_mpc`
- `ctest --test-dir build -R 'test_mpc_basic|test_mpc_io|test_mpc_defaults|test_mpc_environment|test_et_contract_mpc' --output-on-failure`
- `cmake --build build -j --target test_mpc_basic`
- `ctest --test-dir build -R 'test_mpc_basic|test_mpc_io|test_mpc_defaults|test_mpc_environment|test_et_contract_mpc' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpc_basic test_mpc_io test_mpc_defaults test_mpc_environment test_et_contract_mpc`: PASS.
- `ctest --test-dir build -R 'test_mpc_basic|test_mpc_io|test_mpc_defaults|test_mpc_environment|test_et_contract_mpc' --output-on-failure`: PASS, 6/6 tests passed because the regex also matched `test_mpc_environment_invalid`.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase MPF/MPFC move noexcept consistency:
DONE

Implemented features:
- Marked `gmpxx::mpf_class` move constructor `noexcept`, matching its move
  assignment operator and the existing `mpfrxx::mpfr_class` move constructor.
- This makes `gmpxx::mpfc_class(mpfc_class&&) noexcept = default` valid with
  its `mpf_class real_` and `mpf_class imag_` members, instead of depending on
  a potentially non-noexcept member move constructor.

Missing features:
- None.

Tests added:
- Updated `tests/test_construction_copy.cpp` so `gmpxx::mpf_class` is asserted
  as nothrow move constructible.
- Added compile-time `mpfc_class` construction/assignment trait checks,
  including `std::is_nothrow_move_constructible_v<gmpxx::mpfc_class>` and
  `noexcept(mpfc = std::move(other))`.

Exact commands run:
- `rg -n "mpf_class\\(mpf_class&&|operator=\\(mpf_class&&|mpfc_class\\(mpfc_class&&|operator=\\(mpfc_class&&|mpfr_class\\(mpfr_class&&|is_nothrow_move" include tests`
- `sed -n '140,190p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '90,135p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '110,160p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '50,80p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '88,108p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '40,115p' tests/test_construction_copy.cpp`
- `rg -n "mpfc_class|nothrow_move_constructible" tests/test_construction_copy.cpp tests include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '24,38p' tests/test_construction_copy.cpp`
- `cmake --build build -j --target test_construction_copy test_mpfc_basic test_abi_fingerprint`
- `ctest --test-dir build -R 'test_construction_copy|test_mpfc_basic|test_abi_fingerprint' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_construction_copy test_mpfc_basic test_abi_fingerprint`: PASS.
- `ctest --test-dir build -R 'test_construction_copy|test_mpfc_basic|test_abi_fingerprint' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase MPF math loop bounds and pi literal fast path:
DONE

Implemented features:
- Removed unused `mpf_math_detail::apply_unary`; it had no call sites and was
  a leftover double-backed helper.
- Removed the now-unused `make_from_double` helper from `math_mpf.hpp`.
- Added a common MPF iteration-limit helper and applied it to convergence-only
  loops:
  - `compute_pi_gauss_legendre`
  - `agm_converged`
  - `theta3_from_power_of_two_q`
  - `theta2_from_power_of_two_q`
  - `log1p_taylor_small`
  - `log1p_atanh_series`
  - `exp_taylor_reduced`
  - `expm1_taylor_small`
  - `sincos_taylor_small`
  - `atan_taylor_small`
  - `compute_atan` argument reduction
- Convergence loops now throw `std::runtime_error` with the function name if
  the iteration limit is exceeded.
- The iteration limit is `max(64, 16 * precision)`.  A first pass using
  `max(64, precision)` was too tight for existing high-precision
  `log1p_atanh_series` coverage.
- Changed `has_hardcoded_pi()` from exact precision matches
  (`512/1024/2048`) to a conservative decimal-literal capacity check.  The
  current 1000-decimal-digit pi literal is now used for non-standard
  precisions such as 600 and 1500 bits, while larger requests such as 4096
  bits still use the Gauss-Legendre cache path.

Missing features:
- `log_two` still uses exact hardcoded precision matches.  This phase only
  addressed the reported pi literal behavior.

Tests added:
- Extended `tests/test_mpf_pi.cpp` to verify that 600-bit and 1500-bit pi
  requests use the hardcoded-literal eligibility path and still match the
  reference prefix.
- Existing MPF math/transcendent tests cover the bounded convergence loops on
  normal inputs.

Exact commands run:
- `rg -n "apply_unary|compute_pi_gauss_legendre|sincos_taylor_small|exp_taylor_reduced|expm1_taylor_small|compute_log|while \\(|for \\(" include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '600,840p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '1120,1205p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '120,240p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '240,560p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '840,1045p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '1,90p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `rg -n "has_hardcoded_pi|hardcoded_pi|pi_decimal_literal|compute_pi_gauss_legendre|for \\(;;\\)|while \\(true\\)" include/gmpfrxx_mkII/detail/math_mpf.hpp tests`
- `awk '/return "3\\.1415/{flag=1} flag{print} /9216420199";/{flag=0}' include/gmpfrxx_mkII/detail/math_mpf.hpp | tr -cd '0-9' | wc -c`
- `sed -n '500,575p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '1128,1192p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '1368,1395p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `rg -n "mpf_math_detail::apply_unary|apply_unary\\(" include tests examples`
- `rg -n "apply_unary|make_from_double\\(" include/gmpfrxx_mkII/detail/math_mpf.hpp include tests`
- `rg -n "for \\(;;\\)|while \\(true\\)" include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '1,160p' tests/test_mpf_pi.cpp`
- `sed -n '160,280p' tests/test_mpf_pi.cpp`
- `sed -n '1,180p' tests/test_mpf_math_functions.cpp`
- `cmake --build build -j --target test_mpf_pi test_mpf_math_functions test_mpf_transcendent_functions test_mpf_extended_transcendent_functions`
- `ctest --test-dir build -R 'test_mpf_pi|test_mpf_math_functions|test_mpf_transcendent_functions|test_mpf_extended_transcendent_functions' --output-on-failure`
- `cmake --build build -j --target test_mpf_transcendent_functions`
- `ctest --test-dir build -R 'test_mpf_pi|test_mpf_math_functions|test_mpf_transcendent_functions|test_mpf_extended_transcendent_functions' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- First focused CTest with `max(64, precision)`: FAIL, `test_mpf_transcendent_functions`
  hit `log1p_atanh_series failed to converge within iteration limit`.
- After increasing the limit to `max(64, 16 * precision)`,
  `ctest --test-dir build -R 'test_mpf_pi|test_mpf_math_functions|test_mpf_transcendent_functions|test_mpf_extended_transcendent_functions' --output-on-failure`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase thread-safe default options and common-type macro consolidation:
DONE

Implemented features:
- Made `gmpxx::default_mpf_precision_bits()` storage atomic, with synchronized
  loads and stores from the public setter and environment reload path.
- Made MPFR default options (`precision_bits`, `emin`, `emax`,
  `rounding_mode`) copy-in/copy-out under a mutex, so readers no longer race
  with `set_default_*` or `reload_mpfr_defaults_from_environment()`.
- Made MPC default options (`real_precision_bits`, `imag_precision_bits`,
  `real_rounding_mode`, `imag_rounding_mode`) copy-in/copy-out under a mutex.
  `reload_mpc_defaults_from_environment()` computes inherited MPFR defaults
  before taking the MPC mutex to avoid nested lock surprises.
- Added `include/gmpfrxx_mkII/detail/common_type_macros.hpp` and moved the
  duplicated `GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(S)` definitions there.
- Updated `zq_impl.hpp`, `mpf_impl.hpp`, and `mpfc_impl.hpp` to include the
  shared common-type macro header instead of redefining the macros locally.

Missing features:
- This removes C++ data races on the wrapper-owned default stores.  It does
  not make multiple independent setter calls an atomic transaction; callers
  that need a consistent multi-field snapshot should use `default_options()`
  or `default_mpc_options()`.

Tests added:
- Extended `tests/test_mpf_thread_safety.cpp` with concurrent MPF default
  precision writer/reader coverage.
- Extended `tests/test_mpfr_thread_safety.cpp` with concurrent MPFR exponent
  range writer/reader coverage through `default_options()`.
- Extended `tests/test_mpc_environment.cpp` with concurrent MPC real/imag
  precision writer/reader coverage through `default_mpc_options()`.

Exact commands run:
- `git status --short`
- `sed -n '1,140p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '180,320p' include/gmpfrxx_mkII/detail/environment.hpp`
- `sed -n '1,170p' include/gmpfrxx_mkII/detail/mpc_environment.hpp`
- `sed -n '170,260p' include/gmpfrxx_mkII/detail/mpc_environment.hpp`
- `sed -n '1260,1360p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '620,690p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '240,300p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1,60p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '26,50p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `rg -n "default_emin\\(|default_emax\\(|default_options\\(\\)" include tests`
- `rg -n "default_mpc_(real|imag)_precision_bits\\(|default_mpc_(real|imag)_rounding_mode\\(|default_mpc_rounding_mode\\(|default_mpc_options\\(\\)" include tests`
- `rg -n "thread|default.*thread|set_default" tests/test_*thread* tests/test_mpf_thread_safety.cpp tests/test_mpfr_thread_safety.cpp 2>/dev/null`
- `sed -n '1,180p' tests/test_mpf_thread_safety.cpp`
- `sed -n '1,190p' tests/test_mpfr_thread_safety.cpp`
- `rg -n "mpf_thread_safety|mpfr_thread_safety|mpc_.*thread|mpc_environment" CMakeLists.txt tests/CMakeLists.txt`
- `sed -n '1,140p' tests/test_mpc_environment.cpp`
- `rg -n "#define GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE|#undef GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE|mutable_default_mpf_precision_bits\\(" include tests`
- `git diff --stat`
- `cmake --build build -j --target test_mpf_thread_safety test_mpfr_thread_safety test_mpc_environment test_common_type`
- `ctest --test-dir build -R 'test_mpf_thread_safety|test_mpfr_thread_safety|test_mpc_environment|test_common_type' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpf_thread_safety test_mpfr_thread_safety test_mpc_environment test_common_type`: PASS.
- `ctest --test-dir build -R 'test_mpf_thread_safety|test_mpfr_thread_safety|test_mpc_environment|test_common_type' --output-on-failure`: PASS, 5/5 tests passed because the regex also matched `test_mpc_environment_invalid`.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase expression-constructor exception safety and random expression lifetime:
DONE

Implemented features:
- Added clear-on-throw guards to expression-template constructors that own raw
  GMP/MPFR/MPC storage:
  - `gmpxx::mpz_class(const Expr&)`
  - `gmpxx::mpq_class(const Expr&)`
  - `gmpxx::mpf_class(const Expr&)`
  - `gmpxx::mpf_class(const Expr&, mp_bitcnt_t)`
  - `mpfrxx::mpfr_class(const Expr&)`
  - `mpfrxx::mpfr_class(const Expr&, mpfr_prec_t)`
  - `mpfrxx::mpc_class(const Expr&)`
- Added the same guard to the MPF constructor path that materializes exact Z/Q
  expression results before assigning them into the initialized `mpf_t`.
- Left `gmpxx::mpfc_class(const Expr&)` unchanged because it owns two
  `mpf_class` members; if its constructor body throws, the already-constructed
  member objects are destroyed by C++ object construction unwinding.
- Reworked `gmpxx::gmp_randclass` and `mpfrxx::gmp_randclass` to hold their GMP
  random state through a shared `gmp_randstate_holder`.
- Reworked `random_mpf_expr` and `random_mpfr_expr` to hold a `shared_ptr` to
  that state holder instead of a raw `gmp_randclass*`, so random expressions
  remain valid even if the originating `gmp_randclass` object has already gone
  out of scope.

Missing features:
- No synthetic throwing GMP allocator regression was added.  The constructors
  now use the standard `try { evaluate; } catch (...) { clear; throw; }`
  pattern around initialized raw storage.

Tests added:
- Extended `tests/test_random.cpp` with a regression where a
  `random_mpf_expr` outlives the local `gmp_randclass` that produced it.
- Extended `tests/test_mpfr_random.cpp` with the same lifetime regression for
  `random_mpfr_expr`.

Exact commands run:
- `rg -n "class\\(const .*Expr|template <.*Expr|mp[czqfr]*_class\\(.*expr|_evaluate\\(value_|random_.*expr|get_f\\(|get_z_bits|get_z_range|gmp_randclass|state_" include/gmpfrxx_mkII/detail include tests`
- `sed -n '900,930p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '752,790p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1908,1950p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '2160,2202p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '2360,2495p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1080,1198p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1190,1395p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `cmake --build build -j --target test_random test_mpfr_random test_mpf_fixed_precision_materialization test_mpfr_fixed_precision_materialization test_gmpxx_mkII test_mpfrxx_mkII`
- `ctest --test-dir build -R 'test_random|test_mpfr_random|test_mpf_fixed_precision_materialization|test_mpfr_fixed_precision_materialization|test_gmpxx_mkII|test_mpfrxx_mkII' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_random test_mpfr_random test_mpf_fixed_precision_materialization test_mpfr_fixed_precision_materialization test_gmpxx_mkII test_mpfrxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_random|test_mpfr_random|test_mpf_fixed_precision_materialization|test_mpfr_fixed_precision_materialization|test_gmpxx_mkII|test_mpfrxx_mkII' --output-on-failure`: PASS, 6/6 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase MPC stream output component materialization:
DONE

Implemented features:
- Removed the two `mpfr_class` temporaries from `mpc_class` stream insertion.
- `operator<<(std::ostream&, const mpc_class&)` now formats
  `mpc_realref(value.mpc_data())` and `mpc_imagref(value.mpc_data())` directly
  as `mpfr_srcptr` components.
- Kept the existing MPFR stream formatting policy by reusing
  `mpfr_stream_text`, preserving fixed/scientific/defaultfloat, precision,
  uppercase, showpoint, showpos, and locale decimal-point behavior.
- Kept MPC complex output width behavior unchanged: stream width is consumed
  by the complex value as a whole and is not applied separately to the real
  and imaginary components.
- Removed the now-unused `<sstream>` include from `mpc_impl.hpp`.

Missing features:
- `mpc_get_str` is still not used for ostream output because it does not match
  the current iostream formatting surface without additional formatting
  reconstruction.

Tests added:
- No new test file was needed.  Existing `tests/test_mpc_io.cpp` already covers
  default, scientific, showpos/fixed, expression output, and locale decimal
  output for MPC streams.

Exact commands run:
- `rg -n "operator<<\\(std::ostream&.*mpc|mpc_get_str|mpfr_class\\(mpc_realref|print_mpc|mpc_realref\\(value" include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpc_io.cpp tests/test_mpfr_string_io.cpp`
- `sed -n '1288,1325p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1040,1128p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `cmake --build build -j --target test_mpc_io test_mpc_basic test_mpfr_string_io`
- `ctest --test-dir build -R 'test_mpc_io|test_mpc_basic|test_mpfr_string_io' --output-on-failure`
- `rg -n "mpfr_class\\(mpc_(real|imag)ref|mpc_format_component|operator<<\\(std::ostream& out, const mpc_class" include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpc_io test_mpc_basic test_mpfr_string_io`: PASS.
- `ctest --test-dir build -R 'test_mpc_io|test_mpc_basic|test_mpfr_string_io' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase Z/Q direct comparison fast paths:
DONE

Implemented features:
- Added direct `mpz_class` ordering operators for `<`, `<=`, `>`, and `>=`
  using `mpz_cmp`.
- Added direct `mpq_class` ordering operators for `<`, `<=`, `>`, and `>=`
  using `mpq_cmp`.
- Added direct `cmp` overloads for:
  - `mpz_class` vs `mpz_class` through `mpz_cmp`
  - `mpq_class` vs `mpq_class` through `mpq_cmp`
  - `mpq_class` vs `mpz_class` through `mpq_cmp_z`
  - `mpz_class` vs `mpq_class` through `mpq_cmp_z`
  - `mpz_class` vs supported integral scalars through `mpz_cmp_si` /
    `mpz_cmp_ui` when the scalar fits `long` / `unsigned long`
  - `mpq_class` vs supported integral scalars through `mpq_cmp_si` /
    `mpq_cmp_ui` when the scalar fits `long` / `unsigned long`
  - `mpz_class` vs `float` / `double` through `mpz_cmp_d`
- Kept too-wide integral comparison fallback exact by materializing one
  `mpz_class`, not two `mpq_class` values.
- Changed generic Z/Q comparison operators and generic `cmp` to take operands
  by `const&`, preventing forwarding-reference overloads from hijacking direct
  exact-object comparison paths.

Missing features:
- Floating-point scalar comparisons with `mpq_class` still use the existing
  exact rational materialization path because GMP has no `mpq_cmp_d`.

Tests added:
- Extended `tests/test_mpz_mpq_alloc_count.cpp` with zero-allocation checks for
  `mpz/mpz` ordering, `mpz` vs integral scalar comparison, `mpz` vs `double`
  comparison, `mpz/mpq` mixed comparison, and `mpq` vs integral scalar
  comparison.

Exact commands run:
- `rg -n "inline bool operator|bool operator|int cmp\\(|mpq_cmp|mpz_cmp|cmp\\(" include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1120,1585p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `rg -n "mpq_cmp_z|mpq_cmp_ui|mpq_cmp_si|mpz_cmp_d|mpz_cmp_si|mpz_cmp_ui" /usr/include/gmp.h /usr/include/x86_64-linux-gnu/gmp.h`
- `cmake --build build -j --target test_mpz_mpq_alloc_count test_comparisons test_mpfr_comparisons test_gmpxx_mkII test_mpfrxx_mkII`
- `ctest --test-dir build -R 'test_mpz_mpq_alloc_count|test_comparisons|test_mpfr_comparisons|test_gmpxx_mkII|test_mpfrxx_mkII' --output-on-failure`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpz_mpq_alloc_count test_comparisons test_mpfr_comparisons test_gmpxx_mkII test_mpfrxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_mpz_mpq_alloc_count|test_comparisons|test_mpfr_comparisons|test_gmpxx_mkII|test_mpfrxx_mkII' --output-on-failure`: PASS, 5/5 tests passed.
- `git diff --check`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase MPFC alias review and scalar shift-expression fast path:
DONE

Implemented features:
- Rechecked the B6 concern for `mpfc_class` expression evaluation.  The current
  MPFC evaluator already has `mpfc_expression_references` alias detection and
  direct non-aliasing binary-expression evaluation paths, so it no longer
  always routes through two `mpfc_class` temporaries.
- Rechecked the B7 compound-shift concern.  `mpf_class::operator<<=`,
  `mpf_class::operator>>=`, `mpfr_class::operator<<=`, and
  `mpfr_class::operator>>=` already call the direct `*_mul_2exp` /
  `*_div_2exp` or `mpfr_*_2ui` paths without an `mpz_t` roundtrip.
- Removed the remaining scalar shift-expression roundtrip for `dst = a << 7`
  and `dst = a >> 3` in MPF and MPFR expression evaluation.
- Added `zq_shift_count_from_scalar` and scalar-leaf detection so integral
  shift counts are checked directly against `unsigned long` without
  materializing a temporary `mpz_t`.

Missing features:
- Non-scalar shift counts, such as `mpz_class` or expression RHS values, still
  use the checked `mpz_t` path by design.

Tests added:
- Extended `tests/test_mpf_scalar_alloc_count.cpp` with zero-allocation checks
  for `dst = a << 7` and `dst = a >> 3`.
- Extended `tests/test_mpfr_scalar_alloc_count.cpp` with the same
  zero-allocation checks for MPFR scalar shift expressions.

Exact commands run:
- `rg -n "mpfc_expression_references|mpfc_evaluate|operator<<=|operator>>=|scalar_leaf<std::uint64_t, mpz_class>|zq_shift_count_from_mpz" include/gmpfrxx_mkII/detail tests STATUS.md`
- `sed -n '600,705p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1600,1665p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1870,1925p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `git diff -- include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpf_scalar_alloc_count.cpp tests/test_mpfr_scalar_alloc_count.cpp`
- `cmake --build build -j --target test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count test_mpfc_precision_policy`
- `ctest --test-dir build -R 'test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|test_mpfc_precision_policy' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count test_mpfc_precision_policy`: PASS.
- `ctest --test-dir build -R 'test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|test_mpfc_precision_policy' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase MPFR whitespace parsing and MPC exponent-range guards:
DONE

Implemented features:
- Replaced `mpfr_remove_grouping_whitespace` with
  `mpfr_trim_surrounding_whitespace`.
- MPFR string constructors and `set_str` now trim only leading/trailing
  whitespace; whitespace inside the numeric token is no longer deleted.
- Inputs such as `"3 14"`, `"1.25 00"`, and `"0x1 p+5"` are rejected instead
  of being silently rewritten to a different value.
- Added MPC exponent-range guard coverage around MPC-backed construction,
  assignment, expression materialization, compound assignment, and public MPC
  math helpers.
- Added `mpc_check_component_ranges` so MPC result real/imag MPFR components
  are checked under the wrapper MPFR exponent range after MPC C API calls.
- MPF/MPFC remain outside `MPFRXX_EMIN`/`MPFRXX_EMAX` by design because they
  are GMP MPF-backed, not MPFR-backed.

Missing features:
- None for the current MPFR/MPC exponent policy.

Tests added:
- Updated `tests/test_mpfr_string_io.cpp` to require surrounding whitespace
  trimming while rejecting whitespace inside MPFR numeric tokens.
- Extended `tests/test_mpc_environment.cpp` with a white-box guard-scope check
  proving MPC math runs while the wrapper MPFR exponent range is installed and
  restores the previous MPFR global range afterward.

Exact commands run:
- `sed -n '45,80p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `rg -n "mpfr_remove_grouping_whitespace|grouping whitespace|0x1 p|1.25 00|3 14|exponent_range_guard|default_emin|default_emax|emin|emax" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp tests STATUS.md`
- `sed -n '1,120p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '96,120p' tests/test_mpfr_string_io.cpp`
- `rg -n "mpc_(set|add|sub|mul|div|neg|sqr|sqrt|exp|log|sin|cos|tan|pow)|mpc_apply|mpc_evaluate|current_eval_context|exponent_range_guard" include/gmpfrxx_mkII/detail/mpc_impl.hpp tests/test_mpc*`
- `sed -n '1,180p' include/gmpfrxx_mkII/detail/eval_context.hpp`
- `sed -n '1,140p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '400,635p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '900,1180p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1,130p' tests/test_mpc_environment.cpp tests/test_mpc_environment_invalid.cpp tests/test_mpc_math.cpp`
- `git diff --check`
- `cmake --build build -j --target test_mpfr_string_io test_mpc_environment test_mpc_math`
- `ctest --test-dir build -R 'test_mpfr_string_io|test_mpc_environment|test_mpc_math' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `git diff --check`: PASS.
- `cmake --build build -j --target test_mpfr_string_io test_mpc_environment test_mpc_math`: PASS.
- `ctest --test-dir build -R 'test_mpfr_string_io|test_mpc_environment|test_mpc_math' --output-on-failure`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

Post-phase string base parity regression review:
DONE

Implemented features:
- Rechecked the A7 constructor/assignment base policy across `mpz_class`,
  `mpq_class`, `mpf_class`, and `mpfr_class`.
- Current `mpz_class` and `mpq_class` constructors and string assignments use
  base 0 auto-detection consistently.
- Current `mpfr_class` string constructors and string assignments use base 0
  auto-detection consistently.
- Current `mpf_class` string constructors and string assignments intentionally
  use base 10 consistently, matching the GMP MPF decimal-string policy kept by
  this wrapper.
- Rechecked the A8 imaginary literal policy.  Current string `_mpfc_i` accepts
  prefixed hexadecimal and binary forms by normalizing them to explicit GMP MPF
  base-specific parsing, and string `_mpc_i` continues to use MPFR auto-base
  parsing.

Missing features:
- None for the current policy.  MPF still intentionally does not accept C99
  hex-float text such as `0x1p+5`; MPFR does.

Tests added:
- Extended `tests/test_mpf_string_io.cpp` to lock the MPF constructor side of
  the base-10 policy: `mpf_class("0x10")` throws just like assignment from
  `"0x10"` already did.
- Extended `tests/test_mpfr_string_io.cpp` with MPFR constructor/assignment
  coverage for `"0xff"` as a base-0 hexadecimal integer.

Existing tests relied on:
- `tests/test_zq_string_io.cpp` already covers `mpz_class` and `mpq_class`
  constructor/assignment parity for prefixed hexadecimal and octal text.
- `tests/test_user_defined_literals.cpp` already covers `"0xff"_mpfc_i` and
  `"0x1.8"_mpfc_i`.
- `tests/test_mpfr_user_defined_literals.cpp` already covers `"0xff"_mpc_i`.

Exact commands run:
- `rg -n "mpz_class\\(const char|mpq_class\\(const char|mpf_class\\(const char|mpfr_class\\(const char|operator=\\(const char|set_str\\(|set\\(value, 0|_mpfc_i|_mpc_i|parse_mpfc_imaginary_literal|parse_mpc_imaginary_literal" include/gmpfrxx_mkII/detail tests STATUS.md`
- `sed -n '320,455p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '220,355p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '136,280p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '680,825p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '45,125p' tests/test_zq_string_io.cpp`
- `sed -n '180,230p' tests/test_mpf_string_io.cpp`
- `sed -n '60,95p' tests/test_mpfr_string_io.cpp`
- `git diff --check`
- `cmake --build build -j --target test_zq_string_io test_mpf_string_io test_mpfr_string_io test_user_defined_literals test_mpfr_user_defined_literals`
- `ctest --test-dir build -R 'test_zq_string_io|test_mpf_string_io|test_mpfr_string_io|test_user_defined_literals|test_mpfr_user_defined_literals' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `git diff --check`: PASS.
- `cmake --build build -j --target test_zq_string_io test_mpf_string_io test_mpfr_string_io test_user_defined_literals test_mpfr_user_defined_literals`: PASS.
- `ctest --test-dir build -R 'test_zq_string_io|test_mpf_string_io|test_mpfr_string_io|test_user_defined_literals|test_mpfr_user_defined_literals' --output-on-failure`: PASS, 5/5 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.  The apparent MPF/MPFR difference is intentional policy, not a
  constructor/assignment mismatch within a class.

Post-phase MPFC compound assignment review and literal precision note:
DONE

Implemented features:
- Rechecked the C5 concern for `mpfc_class` compound assignment.  The current
  implementation no longer uses the old `lhs = lhs op rhs` pattern; `+=`,
  `-=`, `*=`, and `/=` dispatch through `mpfc_compound_assign<Op>`.
- Documented the numeric `_mpfc_i` literal behavior in code: C++ numeric UDLs
  receive an already-rounded `long double`, and this library intentionally
  routes that value through the ordinary double-based MPF floating constructor.
- Clarified that string `_mpfc_i` literals such as `"0.1"_mpfc_i` should be
  used when decimal text must be parsed at MPF default precision.

Missing features:
- None for the current literal policy.  A future source-preserving numeric
  MPFC literal would require a raw literal operator and an explicit API policy.

Tests added:
- Extended `tests/test_user_defined_literals.cpp` to show that `0.1_mpfc_i`
  follows the double-based numeric path while `"0.1"_mpfc_i` parses decimal
  text at MPF precision, and that the two values differ at the default
  precision.

Exact commands run:
- `rg -n "operator\\+=|operator-=|operator\\*=|operator/=|_mpfc_i|operator\\\"\\\"_mpfc|complex_literals|long double|mpfc literal|mpfc_class& operator=" include/gmpfrxx_mkII/detail/mpfc_impl.hpp tests STATUS.md`
- `sed -n '880,972p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '2738,2788p' STATUS.md`
- `sed -n '82,136p' tests/test_user_defined_literals.cpp`
- `git diff --check`
- `cmake --build build -j --target test_user_defined_literals test_mpfc_precision_policy test_mpfc_basic`
- `ctest --test-dir build -R 'test_user_defined_literals|test_mpfc_precision_policy|test_mpfc_basic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `git diff --check`: PASS.
- `cmake --build build -j --target test_user_defined_literals test_mpfc_precision_policy test_mpfc_basic`: PASS.
- `ctest --test-dir build -R 'test_user_defined_literals|test_mpfc_precision_policy|test_mpfc_basic' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- Numeric `_mpfc_i` literals are not source-spelling preserving.  This is
  documented behavior; string literals are the precision-preserving path.

Post-phase int128 constructor policy note and Spouge coefficient guard:
DONE

Implemented features:
- Documented the `mpz_class` conversion policy near the 128-bit integer
  constructors: integer construction is intentionally implicit, floating-point
  construction remains explicit, and expression-template scalar leaves still
  reject `__int128`.
- Added an explicit `gamma_spouge_coefficient` precondition guard before
  converting `a - k` to `unsigned long`.
- The Spouge coefficient helper now rejects invalid internal calls with
  `std::invalid_argument` unless `k == 0` or `0 < k < a`.

Missing features:
- None.

Tests added:
- Extended `tests/test_mpf_transcendent_functions.cpp` with a regression check
  that `gamma_spouge_coefficient(k, a, work)` rejects `k >= a`.

Exact commands run:
- `rg -n "__int128|gamma_spouge_coefficient|gamma_spouge_terms|mpz_class\\(" include tests STATUS.md`
- `sed -n '340,390p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1300,1345p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '350,385p' tests/test_mpf_transcendent_functions.cpp`
- `git diff --check`
- `cmake --build build -j --target test_mpf_transcendent_functions test_type_conversions compile_fail_test_int128_scalar`
- `cmake --build build -j`
- `ctest --test-dir build -R 'test_mpf_transcendent_functions|test_type_conversions|compile_fail_test_int128_scalar' --output-on-failure`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `git diff --check`: PASS.
- `cmake --build build -j --target test_mpf_transcendent_functions test_type_conversions compile_fail_test_int128_scalar`: FAIL only because compile-fail tests are CTest entries, not build targets; `test_mpf_transcendent_functions` and `test_type_conversions` built before the target-name error.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build -R 'test_mpf_transcendent_functions|test_type_conversions|compile_fail_test_int128_scalar' --output-on-failure`: PASS, 3/3 tests passed.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- None.

MPF exp scaling exponent guard:
DONE

Implemented features:
- Added an explicit exponent-range guard before `compute_exp` calls `mpf_mul_2exp` or `mpf_div_2exp`.
- The guard checks the current Taylor result exponent plus the requested power-of-two shift against `mp_exp_t` limits.
- Added a checked `mp_exp_t` magnitude helper for shift-count conversion to `mp_bitcnt_t`, avoiding `-LONG_MIN` style signed overflow.
- Reused the checked magnitude helper in `mul_signed_exp`, which computes the `k * log(2)` correction term during argument reduction.
- Kept huge `exp(x)` results outside the MPF exponent range as `std::overflow_error` instead of letting GMP scaling wrap or corrupt the exponent.

Missing features:
- This does not add arbitrary-range exponential results beyond GMP MPF's exponent model.

Tests added:
- Extended tests/test_mpf_transcendent_functions.cpp with direct positive and negative exponent-boundary guard coverage.
- Added a public `gmpxx::exp(gmpxx::mpf_class("1e100", precision))` overflow regression.

Exact commands run:
- sed -n '600,690p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- rg -n "round_to_nearest_mp_exp|mpf_mul_2exp|mpf_div_2exp|overflow_error\\(\\\"exp|compute_exp|exp\\(" include/gmpfrxx_mkII/detail/math_mpf.hpp tests/test_mpf*
- rg -n "mpf_get_d_2exp|mpf_get_str|mpf_get_prec|mpf_get" include/gmpfrxx_mkII/detail tests -g'*.hpp' -g'*.cpp'
- cmake --build build -j --target test_mpf_transcendent_functions
- ctest --test-dir build -R test_mpf_transcendent_functions --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpf_transcendent_functions: PASS.
- ctest --test-dir build -R test_mpf_transcendent_functions --output-on-failure: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 140/140 tests passed.

Known issues:
- GMP MPF still has finite exponent limits; very large exponentials are reported as overflow rather than represented symbolically or as infinity.

MPF Taylor denominator counter width:
DONE

Implemented features:
- Changed MPF `exp_taylor_reduced`, `expm1_taylor_small`, and `sincos_taylor_small` loop counters from `unsigned long` to `std::uint64_t`.
- Changed Taylor denominator materialization to use `mpf_class(std::uint64_t, precision)` instead of `make_ui(unsigned long, precision)`.
- Added checked `std::uint64_t` product and counter increment helpers so denominator arithmetic cannot silently wrap before division.
- Kept the fix scoped to the practical 32-bit `unsigned long` failure mode; extremely large Taylor iteration counts now fail with `std::overflow_error` instead of wrapping.

Missing features:
- This does not make the naive Taylor algorithms practical for billion-bit workloads. It prevents integer-width corruption in the current algorithms.

Tests added:
- Extended tests/test_mpf_math_functions.cpp with direct coverage for denominator values beyond 32-bit `unsigned long`, exact `mpf_class(uint64_t, precision)` materialization, product overflow detection, and counter overflow detection.

Exact commands run:
- sed -n '120,180p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '640,790p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- rg -n "uint64|UINT64|long_width|mpf_class\\(.*precision|mpf_class\\(.*get_prec" tests include/gmpfrxx_mkII/detail/mpf_impl.hpp
- cmake --build build -j --target test_mpf_math_functions test_mpf_transcendent_functions
- ctest --test-dir build -R 'test_mpf_math_functions|test_mpf_transcendent_functions' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpf_math_functions test_mpf_transcendent_functions: PASS.
- ctest --test-dir build -R 'test_mpf_math_functions|test_mpf_transcendent_functions' --output-on-failure: PASS, 2/2 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 140/140 tests passed.

Known issues:
- Other MPF Taylor/helper loops still use `unsigned long` where their current denominator expressions do not multiply two loop-derived factors. They should be reviewed separately if the MPF transcendental algorithms are reworked for very high precision.

MPFC/MPC imaginary literal base policy:
DONE

Implemented features:
- Changed `_mpfc_i` string literals to accept prefixed hexadecimal and binary forms such as `"0xff"_mpfc_i` and `"0b101.1"_mpfc_i`.
- Kept decimal `_mpfc_i` behavior unchanged.
- Kept C99-style MPFR hex float syntax out of MPFC; GMP `mpf_set_str` does not accept `0x1p+5` as an MPF input form.
- Added matching regression coverage showing `"0xff"_mpfc_i` and `"0xff"_mpc_i` both materialize as imaginary 255.

Missing features:
- None for the current MPFC/MPC imaginary literal policy.

Tests added:
- Extended tests/test_user_defined_literals.cpp for `_mpfc_i` hex integer and hex fractional string literals.
- Extended tests/test_mpfr_user_defined_literals.cpp for `_mpc_i` hex integer string literals.

Exact commands run:
- sed -n '760,820p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- sed -n '1130,1185p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- rg -n "_mpfc_i|_mpc_i|mpfc.*literal|mpc.*literal|0xff|base 0|base 10" tests include STATUS.md
- cmake --build build -j --target test_user_defined_literals test_mpfr_user_defined_literals
- ctest --test-dir build -R 'test_user_defined_literals|test_mpfr_user_defined_literals' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_user_defined_literals test_mpfr_user_defined_literals: PASS.
- ctest --test-dir build -R 'test_user_defined_literals|test_mpfr_user_defined_literals' --output-on-failure: PASS, 2/2 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 140/140 tests passed.

Known issues:
- MPFC uses GMP MPF parsing under the hood. Prefixes are normalized before calling `mpf_set_str`; this is intentionally narrower than MPFR's full string parser.

Public version API:
DONE

Implemented features:
- Added generated `GMPFRXX_MKII_VERSION` from the CMake project version.
- Added a source-tree fallback `GMPFRXX_MKII_VERSION "0.0.1"` for direct includes without a generated header.
- Added `gmpxx::version()`, `mpfrxx::version()`, `gmpxx::print_version(std::ostream&)`, and `mpfrxx::print_version(std::ostream&)`.
- Kept the existing generated Git commit hash API unchanged.

Missing features:
- None for this phase.

Tests added:
- Extended tests/test_version_info.cpp to check public version APIs, the generated macro, and stream-print helpers.

Exact commands run:
- sed -n '1,120p' include/gmpfrxx_mkII/detail/version.hpp.in
- sed -n '1,120p' include/gmpfrxx_mkII/detail/config.hpp
- sed -n '1,90p' CMakeLists.txt
- sed -n '1,120p' tests/test_version_info.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- sed -n '1,80p' build/generated/gmpfrxx_mkII/detail/version.hpp
- cmake --build build -j --target test_version_info
- ctest --test-dir build -R test_version_info --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Generated header contains `GMPFRXX_MKII_VERSION "0.0.1"`.
- cmake --build build -j --target test_version_info: PASS.
- ctest --test-dir build -R test_version_info --output-on-failure: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 140/140 tests passed.

Known issues:
- The generated Git hash remains configure-time data; rerun CMake after commits to refresh it.

Git-archive distribution target:
DONE

Implemented features:
- Added CMake project version 0.0.1.
- Added a git-archive based `dist` custom target.
- The target writes `${CMAKE_BINARY_DIR}/gmpfrxx_mkII-0.0.1.tar.xz`.
- The archive top directory is `gmpfrxx_mkII-0.0.1/`.
- The target uses tracked files from `HEAD`, so untracked files and build artifacts are not included.

Missing features:
- None for this phase.

Tests added:
- None. This phase adds a build target and verifies it by running the target.

Exact commands run:
- sed -n '1,90p' CMakeLists.txt
- rg -n "PROJECT_VERSION|VERSION|gmpfrxx_mkII_VERSION|configure_file|version" CMakeLists.txt include/gmpfrxx_mkII/detail/version.hpp.in README.md
- git status --short
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- git diff -- CMakeLists.txt
- cmake --build build --target dist
- ls -lh build/gmpfrxx_mkII-0.0.1.tar.xz
- tar -tf build/gmpfrxx_mkII-0.0.1.tar.xz | sed -n '1,10p'
- tar -tf build/gmpfrxx_mkII-0.0.1.tar.xz | rg 'AGENTS.md~|^gmpfrxx_mkII-0\\.0\\.1/build/'

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build --target dist: PASS.
- Archive created: build/gmpfrxx_mkII-0.0.1.tar.xz, 771K.
- Archive top-level listing starts with `gmpfrxx_mkII-0.0.1/`.
- Archive scan for AGENTS.md~ and build artifacts: PASS, no matches.

Known issues:
- The `dist` target is available only when both Git and xz are found by CMake.
- Because the target uses `git archive HEAD`, uncommitted changes are not included until committed.

MPFR/MPC header and link separation:
DONE

Implemented features:
- Split MPC-facing public entry point into include/mpcxx_mkII.h.
- Kept include/mpfrxx_mkII.h MPFR-only: it no longer includes <mpc.h> or detail/mpc_impl.hpp.
- Moved MPC default environment helpers out of detail/environment.hpp into detail/mpc_environment.hpp.
- Added the mpcxx_mkII interface target for code that needs mpfrxx::mpc_class.
- Changed MPFR-only CMake target mpfrxx_mkII to link GMP + MPFR only.
- Changed the combined gmpfrxx_mkII target to link gmpxx_mkII + mpfrxx_mkII + mpcxx_mkII explicitly.
- Changed MPC examples/tests to explicitly include both <mpfrxx_mkII.h> for real MPFR APIs and <mpcxx_mkII.h> for MPC APIs, and to link mpcxx_mkII.
- Changed include/mpcxx_mkII.h to require that <mpfrxx_mkII.h> has already been included instead of including it implicitly.
- Kept the combined include/gmpfrxx_mkII.h aggregator exposing both MPFR and MPC by including <mpcxx_mkII.h>.
- Added header-boundary coverage proving mpfrxx_mkII.h does not expose mpc_class.
- Updated stale MPFR scalar allocation-count expectation for dst += 5LL from 2 to the current MPF-matching 1 allocation.

Missing features:
- None for this phase. MPC remains a required project dependency for the full repository; the separation is at the public include and target-use boundary.

Tests added:
- tests/test_mpc_header_smoke.cpp
- tests/compile_fail/mpfr_header_must_not_expose_mpc.cpp
- tests/compile_fail/mpc_header_requires_mpfr_first.cpp

Exact commands run:
- sed -n '1,150p' tests/test_mpfr_exception_support.cpp
- rg -n "test_mpfr_exception_support|#include <mpfrxx_mkII\\.h>|mpc_class|_mpc_i|mpc_" tests examples -g'*.cpp' -g'CMakeLists.txt'
- git status --short
- rg -n "add_phase0_test\\(test_.*mpc|test_mpfr_exception_support|#include <mpfrxx_mkII\\.h>|#include <mpcxx_mkII\\.h>" tests examples -g'*.cpp' -g'CMakeLists.txt'
- cmake --build build -j --target test_mpfr_exception_support
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- sed -n '1p' build/examples/CMakeFiles/example02_mpfr.dir/link.txt
- sed -n '1p' build/examples/CMakeFiles/example02_mpfr_mpc.dir/link.txt
- sed -n '1,220p' tests/test_mpfr_scalar_alloc_count.cpp
- ./build/tests/test_mpfr_scalar_alloc_count
- rg -n "expected 2 allocations|expect_allocations|mpfr_scalar_alloc_count|allocation" tests/test_mpfr_scalar_alloc_count.cpp include/gmpfrxx_mkII/detail -g'*.hpp'
- sed -n '1,180p' tests/test_mpf_scalar_alloc_count.cpp
- git diff -- tests/test_mpfr_scalar_alloc_count.cpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build -j --target test_mpfr_scalar_alloc_count
- ctest --test-dir build --output-on-failure
- git diff --check
- git status --short
- cmake --build build -j --target example02_mpfr_mpc test_mpc_header_smoke test_header_boundaries test_mpfrxx_mkII test_mpfr_user_defined_literals
- ctest --test-dir build -R 'example02_mpfr_mpc|test_mpc_header_smoke|test_header_boundaries|test_mpfrxx_mkII|test_mpfr_user_defined_literals|compile_fail_mpc_header_requires_mpfr_first|compile_fail_mpfr_header_must_not_expose_mpc' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpfr_exception_support: PASS
- cmake --build build -j: PASS
- Initial full ctest failed only in test_mpfr_scalar_alloc_count because dst += 5LL expected 2 allocations while current behavior is 1.
- ./build/tests/test_mpfr_scalar_alloc_count: initially FAIL with "expected 2 allocations, got 1".
- After updating the stale expectation, cmake --build build -j --target test_mpfr_scalar_alloc_count: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 139/139 tests passed.
- git diff --check: PASS.
- MPFR-only link check: example02_mpfr links libgmp + libmpfr and does not link libmpc.
- MPC link check: example02_mpfr_mpc links libgmp + libmpfr + libmpc.
- Focused include-order ctest: PASS, 7/7 tests passed.
- Final cmake --build build -j after include-order enforcement: PASS.
- Final ctest --test-dir build --output-on-failure after include-order enforcement: PASS, 140/140 tests passed.

Known issues:
- Full-tree CMake configure intentionally still requires MPC. MPFR-only users should include <mpfrxx_mkII.h> and use the mpfrxx_mkII target; MPC users must include <mpfrxx_mkII.h> before <mpcxx_mkII.h> and use the mpcxx_mkII target.
- AGENTS.md~ is an existing untracked local file and was not touched.

Phase 0 status:
DONE

Implemented features:
- Created the Phase 0 public headers: include/gmpxx_mkII.h, include/mpfrxx_mkII.h, and include/gmpfrxx_mkII.h.
- Created private implementation headers under include/gmpfrxx_mkII/detail/.
- Added public namespaces gmpxx and mpfrxx.
- Added internal namespace gmpfrxx_mkII::detail.
- Added shell complete classes for gmpxx::mpz_class, gmpxx::mpq_class, gmpxx::mpf_class, mpfrxx::mpfr_class, and mpfrxx::mpc_class.
- Added mpfrxx::mpz_class and mpfrxx::mpq_class aliases to the gmpxx exact classes.
- Added CMake interface targets: gmpxx_mkII links GMP only, mpfrxx_mkII links GMP/MPFR/MPC, and gmpfrxx_mkII links GMP/MPFR/MPC.
- Added cmake/FindGMP.cmake, cmake/FindMPFR.cmake, and cmake/FindMPC.cmake.
- Added Phase 0 smoke, namespace, header-boundary, integer-model, and compile-fail tests.

Missing features:
- None for Phase 0.

Tests added:
- tests/test_gmp_header_smoke.cpp
- tests/test_mpfr_header_smoke.cpp
- tests/test_aggregator_header_smoke.cpp
- tests/test_namespace_aliases.cpp
- tests/test_header_boundaries.cpp
- tests/test_integer_model_diagnostics.cpp
- tests/compile_fail/mpfr_header_must_not_expose_mpf.cpp
- tests/compile_fail/mpfr_header_must_not_expose_mpfc.cpp
- tests/compile_fail/gmp_header_must_not_expose_mpfr_or_mpc.cpp

Exact commands run:
- git status --short
- rg --files
- sed -n '1,260p' STATUS.md
- rg --files include tests cmake CMakeLists.txt 2>/dev/null
- sed -n '1,260p' CMakeLists.txt
- find include tests cmake -maxdepth 4 -type f -print
- sed -n '140,235p' PROMPTS.md
- mkdir -p include/gmpfrxx_mkII/detail tests/compile_fail cmake
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "#include <gmpxx\\.h>" include tests CMakeLists.txt cmake
- git diff --stat

Pass/fail result:
- Initial CTest run failed because test_header_boundaries read source paths relative to the build directory.
- Fixed test_header_boundaries by passing GMPFRXX_MKII_SOURCE_DIR from CMake.
- Final cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 9/9 tests passed
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h>: PASS, no matches

Known issues:
- Phase 0 intentionally does not implement arithmetic operators, expression-template nodes, numeric storage, conversion, comparisons, streaming, math functions, environment handling, or real GMP/MPFR/MPC RAII behavior.
- gmpxx::mpf_class, mpfrxx::mpfr_class, and mpfrxx::mpc_class are shell classes only in this phase.

Phase 1 status:
DONE

Implemented features:
- Implemented mpfrxx::mpfr_class as an RAII wrapper around mpfr_t.
- Added copy construction, move construction, copy assignment, move assignment, and destruction for mpfrxx::mpfr_class.
- Added precision allocation through mpfr_init2().
- Kept raw precision construction out of the public constructor API; precision-specific objects are created with mpfr_class::with_precision(bits).
- Added with_precision(bits), with_precision(bits, double), set(double), to_double(), precision(), and mpfr_data() helpers for Phase 1 tests and evaluation.
- Added expression node types in include/gmpfrxx_mkII/detail/expr.hpp:
  - object_leaf
  - scalar_leaf
  - unary_expr
  - binary_expr
- Added operation tags:
  - add_op
  - sub_op
  - mul_op
  - div_op
  - neg_op
  - pos_op
- Added expression traits:
  - is_expression_node
  - is_expression_node_v
  - expression_result_type
  - expression_result_type_t
- Added MPFR expression operators for +, -, *, /, unary +, and unary -.
- Public MPFR arithmetic operators return expression nodes, not eager mpfrxx::mpfr_class objects.
- Added expression materialization through mpfr_class construction from expression.
- Added expression assignment into existing mpfr_class while preserving destination precision.
- New expression materialization uses the maximum precision of MPFR object leaves.
- Binary expression evaluation uses temporaries at the selected evaluation precision, which makes assignment aliasing safe.

Missing features:
- None for Phase 1.

Tests added:
- tests/test_et_contract_mpfr.cpp
- tests/test_mpfr_expression_eval.cpp
- tests/test_mpfr_precision_policy.cpp
- tests/test_mpfr_aliasing.cpp

Exact commands run:
- sed -n '1,220p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/eval_context.hpp
- sed -n '1,220p' tests/CMakeLists.txt
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+mpfrxx::mpfr_class\\s+operator|mpfr_class\\s+operator\\+|mpfr_class\\s+operator-|mpfr_class\\s+operator\\*|mpfr_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "#include <gmpxx\\.h>" include tests CMakeLists.txt cmake
- git diff --stat
- git status --short
- rg -n "mpfr_class\\s+[A-Za-z_][A-Za-z0-9_]*\\([0-9]|mpfr_class\\([0-9]" include tests
- rg -n "explicit mpfr_class\\(mpfr_prec_t|mpfr_class\\(mpfr_prec_t" include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- CMake configure: PASS
- Initial build: FAIL because mpfr_class(int) was ambiguous between precision construction and double value construction.
- Removed the direct double constructor and kept with_precision(bits, double) plus set(double) for Phase 1 value setup.
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 13/13 tests passed
- Public numeric precision constructor scan: PASS, no matches
- Eager MPFR operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h>: PASS, no matches

Known issues:
- Phase 1 intentionally does not implement scalar expression operands, bool/long double/__int128 rejection tests, MPFR environment variables, MPFR exponent-range handling, exact mpz/mpq wrappers beyond Phase 0 shells, MPC arithmetic, MPF arithmetic, comparisons, streaming, or math functions.
- mpfrxx::mpfr_class currently uses fixed Phase 1 defaults recorded at that time; Phase 3 replaces the precision default with a configurable 512-bit wrapper-owned default.

Phase 2 status:
DONE

Implemented features:
- Added MPFR scalar expression operands for signed and unsigned standard integral types except bool.
- Added MPFR scalar expression operands for float and double.
- Rejected bool, long double, __int128, and unsigned __int128 as MPFR expression scalar leaves.
- Normalized MPFR scalar leaves to ABI-stable storage:
  - signed integrals -> std::int64_t
  - unsigned integrals -> std::uint64_t
  - float and double -> double
- Preserved the scalar/scalar boundary: expression-template operators require at least one MPFR object or MPFR expression operand.
- Added minimal RAII storage and exact integral construction for gmpxx::mpz_class.
- Implemented integral-to-MPFR scalar evaluation through a gmpxx::mpz_class temporary and mpfr_set_z().
- Implemented float/double scalar evaluation through mpfr_set_d().
- Verified std::uint64_t max and std::int64_t min convert exactly without negating the signed minimum value.
- Ported the allocation-count test from the sibling gmpxx_mkII repository as tests/test_mpfr_alloc_count.cpp.
- Added allocation-aware MPFR expression evaluation for non-aliasing destinations:
  - simple leaf-only assignments such as dst = a + b allocate zero times
  - left-associated addition chains such as dst = a + b + c + d allocate zero times
  - compound two-sided expressions such as dst = (a + b) * (c + d) use one temporary mpfr_t
- Kept aliasing assignments on the safe temporary path.

Missing features:
- None for Phase 2.

Tests added:
- tests/test_et_contract_mpfr_scalar.cpp
- tests/test_mpfr_scalar_eval.cpp
- tests/test_mpfr_long_width_dispatch.cpp
- tests/test_mpfr_alloc_count.cpp
- tests/compile_fail/test_bool_scalar.cpp
- tests/compile_fail/test_long_double_scalar.cpp
- tests/compile_fail/test_int128_scalar.cpp
- tests/compile_fail/test_scalar_scalar_et_operator.cpp

Exact commands run:
- sed -n '1,260p' include/gmpfrxx_mkII/detail/integer_conversion.hpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,420p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,220p' tests/CMakeLists.txt
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+mpfrxx::mpfr_class\\s+operator|mpfr_class\\s+operator\\+|mpfr_class\\s+operator-|mpfr_class\\s+operator\\*|mpfr_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>" include tests CMakeLists.txt cmake
- git diff --stat
- sed -n '1,260p' ../gmpxx_mkII/tests/test_alloc_count.cpp
- rg -n "alloc_count|mp_set_memory_functions|allocation" ../gmpxx_mkII/tests ../gmpxx_mkII/include 2>/dev/null
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- cmake --build build -j && ctest --test-dir build --output-on-failure -R test_mpfr_alloc_count
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+mpfrxx::mpfr_class\\s+operator|mpfr_class\\s+operator\\+|mpfr_class\\s+operator-|mpfr_class\\s+operator\\*|mpfr_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- git diff --stat

Pass/fail result:
- CMake configure: PASS
- Initial build: FAIL because is_mpfr_object_or_node_v referenced result_type for non-expression operands during SFINAE substitution.
- Fixed is_mpfr_object_or_node with partial specializations instead of a variable-template boolean expression.
- Build after trait fix: PASS
- Initial source scan: FAIL because generic integer_conversion.hpp contained MPFR-specific trait names and is included by gmpxx_mkII.h.
- Moved MPFR scalar trait names into mpfr_impl.hpp, leaving integer_conversion.hpp generic.
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 20/20 tests passed
- Eager MPFR operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h>: PASS, no matches
- Initial migrated allocation-count test: FAIL because the first allocation-aware implementation evaluated both sides of (a + b) * (c + d) into separate temporary mpfr_t objects.
- Updated compound non-aliasing evaluation to evaluate the left side directly into the destination and only the right side into one temporary mpfr_t.
- Restored the migrated allocation-count expectation for (a + b) * (c + d) to 1; simple addition chains still assert zero allocations.
- Final ctest --test-dir build --output-on-failure after the migrated allocation-count test: PASS, 21/21 tests passed
- Final eager MPFR operator scan after allocation-aware evaluation: PASS, no matches
- Final source scan for forbidden GMP-header MPFR/MPC includes and symbols after allocation-aware evaluation: PASS, no matches

Known issues:
- Phase 2 intentionally does not implement direct scalar constructors for mpfrxx::mpfr_class.
- Phase 2 intentionally does not implement MPFR environment variables, rounding configuration beyond fixed MPFR_RNDN, exponent-range handling, mpz/mpq mixed expressions, MPC arithmetic, MPF arithmetic, comparisons, streaming, or math functions.

Phase 3 status:
DONE

Implemented features:
- Added MPFR defaults API in namespace mpfrxx:
  - mpfrxx::default_options()
  - mpfrxx::default_precision_bits()
  - mpfrxx::set_default_precision_bits()
  - mpfrxx::default_rounding_mode()
  - mpfrxx::set_default_rounding_mode()
  - mpfrxx::default_emin()
  - mpfrxx::default_emax()
  - mpfrxx::set_default_exponent_range()
  - mpfrxx::reload_mpfr_defaults_from_environment()
- Added MPFR environment loading from:
  - MPFRXX_DEFAULT_PRECISION_BITS
  - MPFRXX_EMIN
  - MPFRXX_EMAX
  - MPFRXX_ROUNDING_MODE
- Renamed the Phase 3 MPFR environment variables in AGENTS.md and PROMPTS.md from the previous long project-prefixed MPFR names to the shorter MPFRXX_* names.
- Set the initial wrapper-owned MPFR default precision to 512 bits.
- Documented that the initial wrapper-owned GMP mpf_class default precision is also 512 bits for the later MPF phase.
- Added rounding string parsing for RNDN, RNDZ, RNDU, RNDD, RNDA, RNDF and their MPFR_* spellings.
- Replaced fixed MPFR_RNDN evaluation with the current wrapper-owned MPFR rounding mode.
- Added MPFR exponent-range defaults and a guard that applies MPFR emin/emax during wrapper evaluation, then restores the previous MPFR global range.
- Extended eval_context to carry precision, rounding mode, emin, and emax.

Missing features:
- None for Phase 3.

Tests added:
- tests/test_mpfr_defaults.cpp
- tests/test_mpfr_environment.cpp
- tests/test_mpfr_environment_invalid.cpp

Exact commands run:
- rg -n for previous long project-prefixed MPFR env names and MPFRXX names in AGENTS.md PROMPTS.md STATUS.md docs include tests CMakeLists.txt cmake
- git status --short
- sed -n '1,220p' include/gmpfrxx_mkII/detail/environment.hpp
- sed -n '1,120p' include/gmpfrxx_mkII/detail/eval_context.hpp
- sed -n '1,540p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,90p' tests/CMakeLists.txt
- rg -n "default precision|DEFAULT_PRECISION|53|512|MPF_DEFAULT|MPFRXX_DEFAULT" AGENTS.md PROMPTS.md docs include tests STATUS.md
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+mpfrxx::mpfr_class\\s+operator|mpfr_class\\s+operator\\+|mpfr_class\\s+operator-|mpfr_class\\s+operator\\*|mpfr_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>" include tests CMakeLists.txt cmake
- rg -n for previous long project-prefixed MPFR env names and obsolete short rounding env spelling in AGENTS.md PROMPTS.md STATUS.md docs include tests CMakeLists.txt cmake

Pass/fail result:
- CMake configure: PASS
- cmake --build build -j: PASS
- ctest --test-dir build --output-on-failure: PASS, 24/24 tests passed
- Eager MPFR operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h>: PASS, no matches
- Source scan for old MPFR env var names and obsolete short rounding env spelling: PASS, no matches

Known issues:
- Phase 3 intentionally does not implement direct scalar constructors for mpfrxx::mpfr_class.
- Phase 3 intentionally does not implement mpz/mpq mixed expressions, MPC arithmetic, MPF arithmetic, comparisons, streaming, or math functions.
- MPFR exponent-range guards use MPFR's process-global exponent range during the guarded operation and restore the previous values afterward.

Schedule-change MPF status:
DONE

Implemented features:
- Implemented gmpxx::mpf_class as an RAII wrapper around mpf_t.
- Added wrapper-owned GMP MPF default precision API:
  - gmpxx::default_mpf_precision_bits()
  - gmpxx::set_default_mpf_precision_bits()
  - gmpxx::reload_default_mpf_precision_bits_from_environment()
- Set the initial wrapper-owned GMP MPF default precision to 512 bits.
- Added MPF default precision environment loading from MPFXX_DEFAULT_PREC_BITS.
- Used mpf_init2() for wrapper-owned MPF construction.
- Did not call mpf_set_default_prec().
- Added MPF expression-template operators for +, -, *, /, unary +, and unary -.
- Public MPF arithmetic operators return expression nodes, not eager gmpxx::mpf_class objects.
- Added MPF scalar expression operands for signed and unsigned standard integrals except bool, plus float and double.
- Normalized MPF scalar leaves to std::int64_t, std::uint64_t, or double.
- Evaluated integral MPF scalar leaves through gmpxx::mpz_class temporaries and mpf_set_z().
- New MPF expression materialization uses the maximum effective precision of MPF object leaves.
- Assignment into existing MPF objects preserves destination effective precision.
- Added alias-safe MPF expression assignment using temporaries when the destination appears in the source expression.
- Added compile-fail tests for forbidden MPF/MPFR and MPF/MPC mixed-family operations.

Missing features:
- Full mpz/mpq + mpf mixed exact promotion is not implemented yet.
- MPF environment variable handling is not implemented yet.

Tests added:
- tests/test_et_contract_mpf.cpp
- tests/test_mpf_basic.cpp
- tests/test_mpf_precision_policy.cpp
- tests/test_mpf_aliasing.cpp
- tests/compile_fail/test_mpf_plus_mpfr.cpp
- tests/compile_fail/test_mpfr_plus_mpf.cpp
- tests/compile_fail/test_mpf_plus_mpc.cpp
- tests/compile_fail/test_mpc_plus_mpf.cpp

Exact commands run:
- sed -n '1,220p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,540p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,120p' include/gmpxx_mkII.h
- sed -n '1,130p' tests/CMakeLists.txt
- git status --short
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- ./build/tests/test_mpf_precision_policy
- gdb -batch -ex run -ex bt --args ./build/tests/test_mpf_precision_policy
- nl -ba tests/test_mpf_precision_policy.cpp
- gdb -batch -ex 'break tests/test_mpf_precision_policy.cpp:13' -ex run -ex 'print default_value.precision()' --args ./build/tests/test_mpf_precision_policy
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- rg -n "friend\\s+gmpxx::mpf_class\\s+operator|mpf_class\\s+operator\\+|mpf_class\\s+operator-|mpf_class\\s+operator\\*|mpf_class\\s+operator/" include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- git diff --stat

Pass/fail result:
- CMake configure: PASS
- Initial cmake --build build -j after MPF implementation: PASS
- Initial ctest --test-dir build --output-on-failure after MPF implementation: FAIL, test_mpf_precision_policy aborted.
- Root cause: GMP mpf_init2(160) reports an effective precision of 192 via mpf_get_prec() because GMP rounds precision to its internal limb allocation. This does not change the requested wrapper default, which is 512 unless set explicitly or loaded from MPFXX_DEFAULT_PREC_BITS.
- Fixed the MPF precision test to compare effective precision and destination preservation instead of requiring exact requested precision from mpf_get_prec().
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 32/32 tests passed
- Eager MPF operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPF scalar constructors are intentionally not implemented; value construction currently uses with_precision(bits, value) or set(value).
- GMP may report effective MPF precision greater than the requested value through mpf_get_prec().
- MPFXX_DEFAULT_PREC_BITS controls the requested wrapper default precision; mpf_get_prec() reports GMP's effective precision.
- mpz/mpq + mpf mixed-family promotion remains for the later exact-wrapper phase.

Phase 4 status:
DONE

Implemented features:
- Implemented gmpxx::mpq_class as an RAII wrapper around mpq_t.
- Extended gmpxx::mpz_class and gmpxx::mpq_class with construction and assignment from expression nodes.
- Added exact GMP-only expression-template arithmetic for mpz/mpq:
  - mpz + mpz -> gmpxx::mpz_class
  - mpz/mpq combinations -> gmpxx::mpq_class
  - mpq + mpq -> gmpxx::mpq_class
  - division produces gmpxx::mpq_class
- Added mpq canonicalization for construction and expression materialization.
- Preserved mpfrxx::mpz_class and mpfrxx::mpq_class as aliases to the gmpxx exact classes.
- Added MPFR mixed exact operand support:
  - mpz/mpq + mpfr -> mpfrxx::mpfr_class expression nodes
  - mpfr + mpz/mpq -> mpfrxx::mpfr_class expression nodes
- Exact mpz/mpq leaves do not contribute MPFR precision; mixed exact/MPFR materialization keeps the MPFR leaf precision.
- Kept exact mpz/mpq implementation in zq_impl.hpp, with no MPFR/MPC references in GMP-only headers.

Missing features:
- Stronger exact/MPFR mixed expression evaluation from Phase 5 is not implemented yet.
- mpz/mpq + mpf promotion remains to be completed with the GMP-only mixed MPF phase.

Tests added:
- tests/test_et_contract_zq_mpfr.cpp
- tests/test_mpz_basic.cpp
- tests/test_mpq_basic.cpp
- tests/test_mpq_canonicalization.cpp
- tests/test_mixed_zq_mpfr_promotion.cpp

Exact commands run:
- sed -n '1,220p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,540p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,120p' tests/CMakeLists.txt
- git status --short
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- gdb -batch -ex run -ex bt --args ./build/tests/test_mixed_zq_mpfr_promotion
- nl -ba tests/test_mixed_zq_mpfr_promotion.cpp
- cmake --build build -j && ctest --test-dir build --output-on-failure -R test_mixed_zq_mpfr_promotion
- ctest --test-dir build --output-on-failure
- rg -n for eager mpz/mpq/mpfr/mpf arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- git diff --stat

Pass/fail result:
- CMake configure: PASS
- cmake --build build -j: PASS
- Initial ctest --test-dir build --output-on-failure: FAIL, test_mixed_zq_mpfr_promotion aborted.
- Root cause: exact mpz/mpq leaves were initially counted as MPFR default-precision leaves, so z + r(160) materialized at 512 bits instead of preserving the MPFR leaf precision.
- Fixed exact mpz/mpq leaves to contribute zero MPFR precision in mixed exact/MPFR expressions.
- Focused mixed promotion test rerun: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 37/37 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- Phase 5 still needs to strengthen mixed exact/MPFR evaluation policy.
- Exact expression division by zero is not given a wrapper-specific diagnostic yet; it follows GMP behavior.

Phase 5 status:
DONE

Implemented features:
- Strengthened mixed exact/MPFR evaluation for exact-only subexpressions embedded in MPFR expressions.
- Exact mpz-result subexpressions are now evaluated with the GMP exact mpz evaluator and converted to MPFR once with mpfr_set_z().
- Exact mpq-result subexpressions are now evaluated with the GMP exact mpq evaluator and converted to MPFR once with mpfr_set_q().
- Kept public arithmetic expression-template based; no eager public mpz/mpq/mpfr/mpf arithmetic operators were added.
- Kept GMP-only headers free of MPFR/MPC includes and symbols.

Missing features:
- mpz/mpq + mpf promotion remains to be completed with the GMP-only mixed MPF phase.
- MPC arithmetic is still a later phase.
- Exact expression division by zero still follows GMP behavior without a wrapper-specific diagnostic.

Tests added:
- tests/test_mixed_zq_mpfr_exact_subexpression.cpp

Exact commands run:
- git status --short
- tail -120 STATUS.md
- rg -n "TODO|Phase 5|missing|mpf|mpq|mpz|mpfr_set_q|mpf_set_q|result_type|promot" include tests STATUS.md
- sed -n '1,620p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,470p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,220p' tests/test_mixed_zq_mpfr_promotion.cpp
- sed -n '1,180p' tests/CMakeLists.txt
- sed -n '1,220p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '1,220p' tests/test_et_contract_zq_mpfr.cpp
- rg -n "mpfr_evaluate\\(|mpq_evaluate\\(|mpz_evaluate\\(" include/gmpfrxx_mkII/detail tests
- temporary /tmp MPFR probe to find a compact exact-subexpression rounding regression
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_mixed_zq_mpfr_exact_subexpression
- rg -n for eager mpz/mpq/mpfr/mpf arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- ctest --test-dir build --output-on-failure
- git diff -- include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/CMakeLists.txt tests/test_mixed_zq_mpfr_exact_subexpression.cpp
- git status --short

Pass/fail result:
- cmake --build build -j: PASS
- New exact-subexpression regression test: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 38/38 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- mpz/mpq + mpf mixed-family promotion remains for the GMP-only mixed MPF phase.
- MPC arithmetic is not implemented yet.

Phase 6 status:
DONE

Implemented features:
- Implemented GMP-only mixed exact/MPF expression promotion.
- Added mpz/mpq object leaves as valid MPF expression operands.
- Added exact mpz/mpq expression leaves as valid MPF expression operands when at least one side of a binary expression is MPF.
- Added mpz/mpq + mpf, mpf + mpz/mpq, and exact-subexpression + mpf support for +, -, *, and /.
- Exact mpz leaves are converted to MPF through mpf_set_z().
- Exact mpq leaves and mpq-result subexpressions are converted by evaluating numerator and denominator as MPF values and dividing, because GMP MPF has no mpf_set_q().
- Exact mpz/mpq leaves do not contribute MPF precision; mixed exact/MPF materialization keeps the MPF leaf precision.
- Kept MPF/MPFR and MPF/MPC mixed-family operations forbidden.
- Kept public arithmetic expression-template based; no eager public mpz/mpq/mpf arithmetic operators were added.
- Kept GMP-only headers free of MPFR/MPC includes and symbols.

Missing features:
- MPC arithmetic is still a later phase.
- Exact expression division by zero still follows GMP behavior without a wrapper-specific diagnostic.
- MPQ-to-MPF conversion currently uses two temporary mpf_t values for numerator and denominator.

Tests added:
- tests/test_et_contract_zq_mpf.cpp
- tests/test_mixed_zq_mpf_promotion.cpp

Exact commands run:
- sed -n '1,620p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,260p' tests/test_et_contract_mpf.cpp
- sed -n '1,260p' tests/test_mpf_basic.cpp
- git status --short
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_et_contract_zq_mpf|test_mixed_zq_mpf_promotion"
- rg -n for eager mpz/mpq/mpfr/mpf arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j: PASS
- New mixed exact/MPF tests: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 40/40 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPC arithmetic is not implemented yet.
- MPQ-to-MPF conversion allocates temporary MPF numerator and denominator storage.

Phase 7 status:
DONE

Implemented features:
- Implemented mpfrxx::mpc_class as an MPC-backed RAII wrapper around mpc_t.
- Added copy construction, move construction, copy assignment, move assignment, and destruction for mpfrxx::mpc_class.
- Added with_precision(bits), with_precision(bits, real, imag), precision(), real_to_double(), imag_to_double(), and mpc_data() helpers.
- Added MPC expression-template arithmetic for +, -, *, /, unary +, and unary -.
- Added type promotion to mpc_class when at least one operand is an MPC object or MPC expression node.
- Added mixed MPC operands for mpfr_class, mpz_class, mpq_class, and supported scalar leaves.
- Converted MPFR operands through mpc_set_fr(), exact mpz operands through mpc_set_z(), and exact mpq operands through mpc_set_q().
- Kept mpfr + mpfr as MPFR arithmetic and exact-only arithmetic as exact arithmetic; MPC promotion requires an MPC operand.
- Kept MPF/MPFR and MPF/MPC mixed-family operations forbidden.
- Kept public arithmetic expression-template based; no eager public mpc/mpfr/mpf/mpz/mpq arithmetic operators were added.
- Kept GMP-only headers free of MPFR/MPC includes and symbols.

Missing features:
- MPC default precision and rounding environment variables are not implemented yet.
- MPC real/imag independent precision support is not implemented yet; Phase 7 uses same precision for both components.
- MPC math functions, comparisons, and streaming are not implemented yet.
- Exact expression division by zero still follows GMP behavior without a wrapper-specific diagnostic.

Tests added:
- tests/test_et_contract_mpc.cpp
- tests/test_mpc_basic.cpp
- tests/test_mpc_aliasing.cpp

Exact commands run:
- sed -n '1,260p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '1,140p' include/mpfrxx_mkII.h
- sed -n '1,180p' tests/CMakeLists.txt
- tail -90 STATUS.md
- git status --short
- rg -n "mpc_(init|set|add|sub|mul|div|real|imag|get|clear|swap|neg|conj|set_.*fr|set_.*q|set_.*z)" /usr/include /usr/local/include
- sed -n '1,220p' tests/test_aggregator_header_smoke.cpp
- sed -n '1,80p' include/gmpfrxx_mkII.h
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_et_contract_mpc|test_mpc_basic|test_mpc_aliasing"
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_et_contract_mpc|test_mpc_basic|test_mpc_aliasing"
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial cmake --build build -j: PASS
- Initial new MPC tests: FAIL, test_mpc_aliasing expected the wrong imaginary result.
- Fixed test_mpc_aliasing expectation for (1+2i)*(3-i)+(1+2i) from 10i to 7i.
- Final cmake --build build -j: PASS
- Final new MPC tests: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 43/43 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPC environment variables remain for a later phase.
- MPC uses MPC_RNDNN directly in Phase 7.
- MPC real and imaginary component precision are currently the same.

Phase 8 status:
DONE

Implemented features:
- Renamed planned MPC environment variables from long GMPFRXX_MKII_MPC_* names to shorter MPFRXX_MPC_* names in AGENTS.md and PROMPTS.md:
  - MPFRXX_MPC_DEFAULT_PRECISION_BITS
  - MPFRXX_MPC_REAL_PRECISION_BITS
  - MPFRXX_MPC_IMAG_PRECISION_BITS
  - MPFRXX_MPC_ROUNDING_MODE
  - MPFRXX_MPC_REAL_ROUNDING_MODE
  - MPFRXX_MPC_IMAG_ROUNDING_MODE
- Added MPC default options API in namespace mpfrxx:
  - mpfrxx::default_mpc_options()
  - mpfrxx::default_mpc_precision_bits()
  - mpfrxx::default_mpc_real_precision_bits()
  - mpfrxx::default_mpc_imag_precision_bits()
  - mpfrxx::set_default_mpc_precision_bits(bits)
  - mpfrxx::set_default_mpc_precision_bits(real_bits, imag_bits)
  - mpfrxx::default_mpc_rounding_mode()
  - mpfrxx::default_mpc_real_rounding_mode()
  - mpfrxx::default_mpc_imag_rounding_mode()
  - mpfrxx::set_default_mpc_rounding_mode(rounding)
  - mpfrxx::set_default_mpc_rounding_mode(real_rounding, imag_rounding)
  - mpfrxx::reload_mpc_defaults_from_environment()
- MPC defaults inherit current MPFR defaults unless MPFRXX_MPC_* variables are set.
- MPC default precision can be configured independently for real and imaginary components.
- MPC default rounding can be configured independently for real and imaginary components.
- Updated mpc_class construction and expression materialization to use mpc_init3(real_precision, imag_precision).
- Added mpc_class::real_precision() and mpc_class::imag_precision().
- Existing-object MPC expression assignment preserves destination real and imaginary precision.
- New MPC expression materialization uses max real and max imaginary precision across leaves.

Missing features:
- MPC math functions, comparisons, and streaming are not implemented yet.
- Exact expression division by zero still follows GMP behavior without a wrapper-specific diagnostic.

Tests added:
- tests/test_mpc_defaults.cpp
- tests/test_mpc_environment.cpp
- tests/test_mpc_environment_invalid.cpp
- tests/test_mpc_precision_policy.cpp

Exact commands run:
- rg -n "#define MPC_RND|MPC_RND\\(|MPC_RND_RE|MPC_RND_IM|typedef.*mpc_rnd_t|mpc_rnd_t" /usr/include/mpc.h
- sed -n '1,260p' include/gmpfrxx_mkII/detail/environment.hpp
- sed -n '1,180p' tests/test_mpfr_environment.cpp
- sed -n '1,620p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- git status --short
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpc_defaults|test_mpc_environment|test_mpc_environment_invalid|test_mpc_precision_policy|test_mpc_basic|test_mpc_aliasing"
- rg -n "GMPFRXX_MKII_MPC|MPFRXX_MPC" AGENTS.md PROMPTS.md STATUS.md include tests docs README.md
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpc_defaults|test_mpc_environment|test_mpc_environment_invalid|test_mpc_precision_policy|test_mpc_basic|test_mpc_aliasing"
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake
- ctest --test-dir build --output-on-failure
- rg -n "GMPFRXX_MKII_MPC" AGENTS.md PROMPTS.md STATUS.md include tests docs README.md
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests CMakeLists.txt cmake

Pass/fail result:
- Initial cmake --build build -j: PASS
- Initial new/affected MPC tests: FAIL, test_mpc_environment_invalid expected precision "1" to be invalid, but MPFR_PREC_MIN is 1 in this environment.
- Fixed invalid precision test value from "1" to "0".
- Final cmake --build build -j: PASS
- Final new/affected MPC tests: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 47/47 tests passed
- Old long MPC environment variable scan: PASS, no matches in tracked files scanned
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPC math functions, comparisons, and streaming remain for later phases.
- mpc_class::precision() returns the common precision when real and imaginary precision match, otherwise the smaller component precision; use real_precision() and imag_precision() for exact component values.

Phase 9 status:
DONE

Implemented features:
- Added gmpxx::mpf_class construction from decimal strings with explicit precision:
  - mpf_class(const char*, mp_bitcnt_t)
  - mpf_class(const std::string&, mp_bitcnt_t)
- Added gmpxx::mpf_class::set(const char*) and set(const std::string&).
- Added gmpxx::mpf_class::get_str() and get_str(exp, base, digits) for decimal/string formatting.
- Added gmpxx::mpf_class::set_str(const char*, base) and set_str(const std::string&, base), preserving the destination value when parsing fails.
- Added std::ostream output for gmpxx::mpf_class using gmp_asprintf() and basic ostream formatting flags.
- Added internal GMP-allocated string RAII cleanup for strings returned by GMP formatting functions.
- Added examples/example01.cpp demonstrating explicit-precision MPF construction, expression-template addition, and stream output.
- Added examples/CMakeLists.txt and wired examples into the top-level CMake build.

Missing features:
- MPF stream input is not implemented yet.
- Stream output does not yet fully emulate every gmpxx.h formatting edge case.
- String constructors are currently MPF-only; mpz/mpq/mpfr/mpc string constructors remain for later phases.

Tests added:
- tests/test_mpf_string_io.cpp
- CTest entry for example01

Exact commands run:
- sed -n '1,190p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,220p' CMakeLists.txt
- git status --short
- sed -n '1,120p' README.md
- rg -n "Copyright|All rights reserved|Redistribution" tests include docs README.md | head -40
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpf_string_io|example01"
- ./build/examples/example01
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests examples
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples CMakeLists.txt cmake
- ctest --test-dir build --output-on-failure
- rg -n "mpf_class|mpf_get_str|mpf_set_str|operator<<|get_str|void set\\(|set_str|string" ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '617,1015p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '2280,2675p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '4180,4235p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpf_string_io|example01"
- ./build/examples/example01
- nm -C build/examples/example01 | rg "gmp_allocated_string|set_str|get_str|to_string|operator<<|gmp_asprintf|__gmpf_set_str|__gmpf_get_str"
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests examples
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples CMakeLists.txt cmake

Pass/fail result:
- cmake --build build -j: PASS
- New MPF string I/O and example tests: PASS
- example01 output: 1.5 + 2.5 = 4
- Final ctest --test-dir build --output-on-failure: PASS, 49/49 tests passed
- Final ctest after gmpxx_mkII.h.in refinement: PASS, 49/49 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- MPF output formatting now covers basic ostream precision, fixed/scientific, uppercase, showpoint, showpos, width, fill, and alignment flags, but is still not a complete gmpxx.h stream-format clone.

Phase 10 status:
DONE

Implemented features:
- Added gmpxx::mpz_class construction from const char* and std::string with explicit base.
- Added gmpxx::mpz_class::get_str(), to_string(), set_str(), and throwing set() string APIs.
- Added gmpxx::mpq_class construction from const char* and std::string with explicit base.
- Added gmpxx::mpq_class::get_str(), to_string(), set_str(), and throwing set() string APIs.
- mpq string construction and set canonicalize successful parses.
- set_str() parses into a temporary object first, preserving the destination value when parsing fails.
- Added std::ostream output for gmpxx::mpz_class and gmpxx::mpq_class with decimal/hex/octal base selection, showbase, uppercase, showpos, width, fill, and alignment support.
- Moved the GMP-allocated string RAII helper into the shared z/q implementation path for reuse by mpf string formatting.

Missing features:
- Stream input for mpz/mpq is not implemented yet.
- Stream output does not fully emulate every gmpxx.h formatting edge case.
- mpfr/mpc string constructors and stream output remain for later phases.

Tests added:
- tests/test_zq_string_io.cpp

Exact commands run:
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_zq_string_io
- git diff -- include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/CMakeLists.txt tests/test_zq_string_io.cpp
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpf/mpz/mpq arithmetic operators in include tests examples
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples CMakeLists.txt cmake
- tail -n 120 STATUS.md
- git status --short

Pass/fail result:
- Initial cmake --build build -j: PASS
- Initial test_zq_string_io: FAIL, test reused hex/showbase/uppercase stream flags after clear().
- Fixed the test to reset stream formatting flags between independent output cases.
- Final cmake --build build -j: PASS
- Final test_zq_string_io: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 50/50 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- z/q stream output is intentionally basic and may need refinement for obscure iostream formatting combinations.

Phase 11 prerequisite status:
DONE

Implemented features:
- Added optional GMP-only gmpxx::mpfc_class core backed by two gmpxx::mpf_class components.
- Exposed mpfc_class from gmpxx_mkII.h and therefore from the combined gmpfrxx_mkII.h aggregator.
- Kept mpfc_class out of mpfrxx_mkII.h.
- Added mpfc_class::with_precision() factories for common and per-component precision.
- Added real()/imag() component accessors and real_precision()/imag_precision()/precision().
- Added expression-template arithmetic for mpfc + mpfc, mpfc + mpf, mpf + mpfc, and supported real scalars.
- Added expression-template unary plus/minus for mpfc expressions.
- New mpfc expression materialization uses max leaf component precision.
- Existing-object mpfc expression assignment preserves destination component precision.
- Added basic equality comparison and ostream output for mpfc_class.
- Added compile-fail coverage forbidding mpfc mixed arithmetic with mpfr_class and mpc_class.

Missing features:
- Phase 11 complex math functions are not implemented yet:
  sqrt, exp, log, sin, cos, tan, sinh, cosh, tanh, pow, gamma.
- mpfc stream input is not implemented.
- mpfc string constructors are not implemented.
- mpfc is currently limited to GMP MPF component arithmetic and does not use MPC or MPFR.

Tests added:
- tests/test_et_contract_mpfc.cpp
- tests/test_mpfc_basic.cpp
- tests/test_mpfc_precision_policy.cpp
- tests/test_mpfc_header_boundary.cpp
- tests/compile_fail/test_mpfc_plus_mpfr.cpp
- tests/compile_fail/test_mpfr_plus_mpfc.cpp
- tests/compile_fail/test_mpfc_plus_mpc.cpp
- tests/compile_fail/test_mpc_plus_mpfc.cpp

Exact commands run:
- rg -n "mpfc_class|mpfc|complex" ../gmpxx_mkII/include/gmpxx_mkII.h.in include tests CMakeLists.txt
- rg -n "class mpc_class|class mpf_class|OpPlus|eval_into|result_type|precision_policy" include/gmpfrxx_mkII/detail/*.hpp
- git status --short
- sed -n '1,220p' include/gmpxx_mkII.h
- sed -n '1,220p' include/gmpfrxx_mkII.h
- sed -n '1,520p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '6849,7318p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '520,760p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '7335,7628p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- rg -n "inline mpf_class (sqrt|exp|log|sin|cos|tan|sinh|cosh|tanh|gamma|pow|abs|atan2)|const_pi|gamma_spouge|transcendent" include ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '1,130p' tests/CMakeLists.txt
- sed -n '1,110p' include/mpfrxx_mkII.h
- sed -n '1,130p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '1,130p' include/gmpfrxx_mkII/detail/type_traits.hpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "mpfc"
- gdb -batch -ex run -ex bt ./build/tests/test_mpfc_precision_policy
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpfc/mpf/mpz/mpq arithmetic operators in include tests examples
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples CMakeLists.txt cmake
- git diff --stat
- tail -n 70 STATUS.md

Pass/fail result:
- Initial cmake --build build -j: PASS before adding mpfc tests.
- Initial mpfc test build: FAIL due ambiguous with_precision(precision, value) overload versus per-component precision overload.
- Fixed value-initializing with_precision overload to require real and imaginary values explicitly.
- Initial test_mpfc_precision_policy: FAIL because tests compared requested precision bits instead of actual GMP mpf component precision.
- Fixed test expectations to compare actual leaf/destination precision, matching existing MPF precision policy tests.
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure -R "mpfc": PASS, 9/9 tests passed
- Final ctest --test-dir build --output-on-failure: PASS, 58/58 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- Phase 11 math still depends on adding GMP-only MPF transcendentals or porting the required helpers from gmpxx_mkII.h.in.
- mpfc arithmetic currently favors correctness and header separation over allocation-count optimization.

Phase 11 status:
DONE

Implemented features:
- Added GMP-only math_mpf.hpp with the MPF scalar math needed by mpfc_class:
  sqrt, abs, exp, log, sin, cos, tan, sinh, cosh, tanh, atan2, pow, gamma.
- Added GMP-only math_mpfc.hpp with required mpfc_class math:
  sqrt, exp, log, sin, cos, tan, sinh, cosh, tanh, pow, gamma.
- Added mpfc helpers:
  conj, real, imag, norm, abs, arg, polar.
- Added expression overloads for mpfc math functions so documented eager math functions can materialize mpfc expression nodes.
- Hooked math_mpf.hpp and math_mpfc.hpp into gmpxx_mkII.h.
- Kept all mpfc math out of mpfrxx_mkII.h.
- Implemented complex gamma with a double Lanczos path converted back into MPF components.

Missing features:
- The MPF transcendentals are currently GMP-only but double-backed approximations, not the full arbitrary-precision algorithms from ../gmpxx_mkII/include/gmpxx_mkII.h.in.
- Complex gamma is also double-backed through the internal Lanczos helper.
- mpfc inverse trig/hyperbolic functions from the source gmpxx_mkII.h.in implementation remain unported because they are outside the Phase 11 required list.

Tests added:
- tests/test_mpfc_math.cpp

Exact commands run:
- ls include/gmpfrxx_mkII/detail
- nl -ba include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp
- sed -n '4826,6608p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '7335,7812p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- git status --short
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "mpfc_math|mpfc"
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpfc/mpf/mpz/mpq arithmetic operators in include tests examples
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples CMakeLists.txt cmake
- git diff --stat
- tail -n 110 STATUS.md

Pass/fail result:
- cmake --build build -j: PASS
- ctest --test-dir build --output-on-failure -R "mpfc_math|mpfc": PASS, 10/10 tests passed
- Final ctest --test-dir build --output-on-failure: PASS, 59/59 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- Phase 11 API is present and tested, but high-precision MPF transcendental accuracy still requires porting the full algorithms from gmpxx_mkII.h.in.

Phase 12 status:
DONE

Implemented features:
- Rewrote README.md to match the current public API:
  gmpxx namespace, mpfrxx namespace, and gmpfrxx_mkII::detail internals.
- Documented the from-scratch implementation boundary:
  no gmpxx.h, no libgmpxx, no bridge to an existing C++ wrapper.
- Documented header roles:
  gmpxx_mkII.h is GMP-only, mpfrxx_mkII.h is GMP+MPFR+MPC, and gmpfrxx_mkII.h is the combined aggregator.
- Documented mpfrxx::mpz_class/mpq_class aliasing to gmpxx::mpz_class/mpq_class.
- Documented mpfrxx::mpc_class as MPC-backed.
- Documented gmpxx::mpfc_class as GMP-only and separate from mpfrxx::mpc_class.
- Documented expression-template materialization and precision policy.
- Documented runtime environment variables and compile-time fixed-precision fast-path option.
- Added MPFR/MPC example program:
  examples/example02_mpfr_mpc.cpp.
- Added MPFC math example program:
  examples/example03_mpfc_math.cpp.
- Added benchmark build directory and two local benchmark programs:
  benchmarks/bench_gmp_arithmetic.cpp
  benchmarks/bench_mpfr_mpc_arithmetic.cpp.
- Hooked examples and benchmarks into CMake.

Missing features:
- Phase plan in PROMPTS.md ends at Phase 12; no Phase 13+ is currently defined.
- Benchmarks are simple local timing executables, not a full benchmark framework.
- README documents the current double-backed MPF transcendental limitation.

Tests added:
- CTest entries for example02_mpfr_mpc and example03_mpfc_math.
- Build targets for bench_gmp_arithmetic and bench_mpfr_mpc_arithmetic.

Exact commands run:
- git status --short
- ls -la
- find . -maxdepth 2 -type f | sort | sed -n '1,160p'
- sed -n '1,240p' README.md
- sed -n '1,220p' CMakeLists.txt
- sed -n '1,220p' examples/CMakeLists.txt
- wc -l README.md
- rg -n "with_precision|to_double|real\\(|imag\\(" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp examples tests/test_mpc_basic.cpp tests/test_mpfr_expression_eval.cpp
- sed -n '1,180p' examples/example01.cpp
- sed -n '1,180p' tests/test_mpc_basic.cpp
- sed -n '1,140p' tests/test_mpfr_expression_eval.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "example|mpfc_math"
- ./build/benchmarks/bench_gmp_arithmetic
- ./build/benchmarks/bench_mpfr_mpc_arithmetic
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpfc/mpf/mpz/mpq arithmetic operators in include tests examples benchmarks
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples benchmarks CMakeLists.txt cmake
- git diff --stat
- tail -n 90 STATUS.md

Pass/fail result:
- cmake --build build -j: PASS
- Added examples plus mpfc math focused CTest: PASS, 4/4 tests passed
- bench_gmp_arithmetic execution: PASS
- bench_mpfr_mpc_arithmetic execution: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 61/61 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- Documentation and examples are now current, but README intentionally records the Phase 11 MPF transcendental accuracy limitation.

Post-phase construction/copy test migration status:
DONE

Implemented features:
- Ported ../gmpxx_mkII/tests/test_construction_copy.cpp into tests/test_construction_copy.cpp using the current gmpxx namespace API.
- Added mpf_class value constructors for supported non-bool integral values and double values.
- Added mpf_class explicit-precision constructors for supported non-bool integral values, double values, expression nodes, raw mpf_t values, and copy-with-precision.
- Added mpf_class scalar and string assignment while preserving destination precision.
- Added mpf_class swap member/free function and compatibility accessors get_prec()/get_mpf_t().
- Expanded mpf_class migrated construction/copy coverage for double-with-precision, string-with-precision, and zero-with-explicit-precision construction.
- Added mpz_class and mpq_class swap member/free functions, equality comparisons, and non-bool integral assignment.
- Added mpq_class non-bool integral numerator/denominator construction.
- Kept bool rejected for direct mpf/mpz/mpq construction in this repo's policy-aligned migrated test.

Tests added:
- tests/test_construction_copy.cpp

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/tests/test_construction_copy.cpp
- sed -n '260,620p' ../gmpxx_mkII/tests/test_construction_copy.cpp
- g++ -std=c++17 -Iinclude -I../gmpxx_mkII/include ../gmpxx_mkII/tests/test_construction_copy.cpp -lgmp -o /tmp/test_construction_copy_gmpfrxx
- sed -n '1,260p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,360p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- rg -n "operator==|operator<=|swap\\(|mpf_class\\(|mpz_class\\(|mpq_class\\(" include/gmpfrxx_mkII/detail tests/test_*basic.cpp tests/test_*precision*.cpp
- sed -n '260,760p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,130p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,120p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_construction_copy
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpfc/mpf/mpz/mpq arithmetic operators in include tests examples benchmarks
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples benchmarks CMakeLists.txt cmake

Pass/fail result:
- Direct compile of original ../gmpxx_mkII/tests/test_construction_copy.cpp against this repo: FAIL, source assumes unqualified legacy API and helper namespaces.
- Migrated test_construction_copy: PASS
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 62/62 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- The migrated test intentionally follows this repo's bool rejection policy instead of the older gmpxx_mkII bool-constructor surface.

Post-phase MPFR construction/copy test migration status:
DONE

Implemented features:
- Extended tests/test_construction_copy.cpp with mpfrxx::mpfr_class coverage modeled after ../gmpxx_mkII/tests/test_construction_copy.cpp.
- Added mpfr_class value constructors for supported non-bool integral values, double values, string values, raw mpfr_t values, and copy-with-precision.
- Added mpfr_class explicit-precision constructors for supported non-bool integral values, double values, string values, expression nodes, raw mpfr_t values, and copy-with-precision.
- Added mpfr_class scalar and string assignment while preserving destination precision.
- Changed mpfr_class move assignment to preserve destination precision when source and destination precisions differ.
- Added mpfr_class swap member/free function and compatibility accessors get_prec()/get_mpfr_t().
- Retargeted test_construction_copy to gmpfrxx_mkII because it now covers both GMP-only and MPFR APIs.

Tests added:
- mpfr_class construction/copy/move/assignment/swap cases in tests/test_construction_copy.cpp

Exact commands run:
- sed -n '1,360p' tests/test_construction_copy.cpp
- sed -n '1,420p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "mpfr_class\\(|precision\\(|mpfr_data\\(|get_mpfr_t|get_prec|swap\\(|operator=|mpfr_evaluate" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests
- sed -n '1,220p' tests/CMakeLists.txt
- sed -n '420,620p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/environment.hpp
- sed -n '1,180p' include/gmpfrxx_mkII.h
- rg -n "default_precision_bits|default_rounding_mode|mpfr_exponent_range_guard|current_eval_context" include/gmpfrxx_mkII/detail
- sed -n '1,80p' include/mpfrxx_mkII.h
- sed -n '1,220p' include/gmpxx_mkII.h
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_construction_copy
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpfc/mpf/mpz/mpq arithmetic operators in include tests examples benchmarks
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples benchmarks CMakeLists.txt cmake
- git diff --stat
- tail -n 90 STATUS.md

Pass/fail result:
- Final cmake --build build -j: PASS
- Migrated test_construction_copy with mpfr_class coverage: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 62/62 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- The migrated MPFR test follows this repo's bool rejection policy instead of the older gmpxx_mkII bool-constructor surface.

Post-phase ABI fingerprint test migration status:
DONE

Implemented features:
- Ported ../gmpxx_mkII/tests/test_abi_fingerprint.cpp into tests/test_abi_fingerprint.cpp using the current C++17 detail API.
- Added compile-time fingerprint coverage for scalar normalization, supported scalar rejection, operand trait classification, expression-node recognition, operation tags, leaf storage types, and result_type promotion.
- Covered GMP-only, MPFR/MPC, and MPFC expression families through the gmpfrxx_mkII aggregate header.
- Kept bool and long double rejected according to this repo's scalar policy.
- Registered test_abi_fingerprint with CTest via the gmpfrxx_mkII interface target.

Tests added:
- tests/test_abi_fingerprint.cpp

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/tests/test_abi_fingerprint.cpp
- rg -n "abi|fingerprint|typeid|sizeof|is_standard_layout|is_trivially|nothrow|constructible" tests include/gmpfrxx_mkII/detail
- sed -n '1,220p' tests/CMakeLists.txt
- git status --short
- sed -n '1,260p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/type_traits.hpp
- rg -n "result_type|common_type|numeric_limits|is_.*operand|promot|value_kind|kind_of|scalar_leaf|object_leaf|binary_expr|unary_expr" include/gmpfrxx_mkII/detail tests/test_et_contract* tests/test_mixed*
- sed -n '580,820p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '480,850p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '850,940p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '800,880p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_abi_fingerprint
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpfc/mpf/mpz/mpq arithmetic operators in include tests examples benchmarks
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples benchmarks CMakeLists.txt cmake

Pass/fail result:
- Final cmake --build build -j: PASS
- Migrated test_abi_fingerprint: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 63/63 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- The upstream test's C++20 concepts, std::common_type, and std::numeric_limits checks were adapted to current C++17/detail contracts rather than copied verbatim.

Post-phase alias safety test migration status:
DONE

Implemented features:
- Ported ../gmpxx_mkII/tests/test_alias_safety.cpp into tests/test_alias_safety.cpp.
- Added destructive alias-assignment coverage for gmpxx::mpf_class and mpfrxx::mpfr_class.
- Covered direct self-reference cases a=a+b, a=a-a, a=a*a, a=a/a, nested reuse a=(a+b)*a, and repeated destination reuse a=(a-b)/(a+b).
- Added mixed precision alias coverage where destination precision is lower than another expression leaf and must be preserved.
- Registered test_alias_safety with CTest via the gmpfrxx_mkII interface target.

Tests added:
- tests/test_alias_safety.cpp

Exact commands run:
- sed -n '1,320p' ../gmpxx_mkII/tests/test_alias_safety.cpp
- sed -n '1,220p' tests/test_mpf_aliasing.cpp
- sed -n '1,220p' tests/test_mpfr_aliasing.cpp
- sed -n '1,120p' tests/CMakeLists.txt
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_alias_safety
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpfc/mpf/mpz/mpq arithmetic operators in include tests examples benchmarks
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples benchmarks CMakeLists.txt cmake

Pass/fail result:
- Final cmake --build build -j: PASS
- Migrated test_alias_safety: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 64/64 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- None for this migration.

Post-phase MPF pi special function status:
DONE

Implemented features:
- Added gmpxx::pi(mp_bitcnt_t), gmpxx::const_pi(), and gmpxx::const_pi(mp_bitcnt_t) for gmpxx::mpf_class.
- Implemented pi with a GMP-only Gauss-Legendre iteration adapted from ../gmpxx_mkII/include/gmpxx_mkII.h.in.
- Added internal working precision guard bits and a mutex-protected precision cache in math_mpf.hpp.
- Preserved the GMP-only header boundary: implementation uses GMP mpf_* APIs only and does not include or reference MPFR/MPC.
- Added tests for requested precision, default precision, cache reuse from lower precision, and decimal prefix accuracy.

Tests added:
- tests/test_mpf_pi.cpp

Exact commands run:
- rg -n "pi\\(|const_pi|acos|atan|special|math" ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '1,260p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- rg -n "pi\\(|math_mpf|sqrt\\(|sin\\(|cos\\(|tan\\(" tests examples README.md STATUS.md include
- git status --short
- sed -n '4920,5035p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '5035,5055p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '5990,6030p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- rg -n "set_prec_copy|sqrt_prec|add\\(|sub\\(|mul\\(|div\\(|precision_type" ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '4820,4925p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- rg -n "operator<|operator==|mpf_cmp|to_double|with_precision|precision\\(" include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_mpf*
- sed -n '220,430p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,120p' tests/test_mpf_string_io.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_mpf_pi
- ctest --test-dir build --output-on-failure
- temporary /tmp/check_pi and /tmp/check_pi_test programs to inspect pi decimal output
- ctest --test-dir build --output-on-failure -R test_mpf_pi
- ctest --test-dir build --output-on-failure
- rg -n for eager mpc/mpfr/mpfc/mpf/mpz/mpq arithmetic operators in include tests examples benchmarks
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples benchmarks CMakeLists.txt cmake

Pass/fail result:
- Initial test_mpf_pi: FAIL, 128-bit test asserted one rounded decimal digit too many.
- Adjusted test_mpf_pi: PASS
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 65/65 tests passed
- Eager arithmetic operator scan: PASS, no matches
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- Existing MPF transcendental functions other than pi still use double-backed implementations.

Post-phase MPF log_two special function status:
DONE

Implemented features:
- Confirmed gmpxx::pi(mp_bitcnt_t) and gmpxx::const_pi() use a mutex-protected precision cache.
- Added gmpxx::log_two(mp_bitcnt_t), gmpxx::const_log2(), and gmpxx::const_log2(mp_bitcnt_t) for gmpxx::mpf_class.
- Implemented log_two with a GMP-only Jacobi theta series plus AGM algorithm adapted from ../gmpxx_mkII/include/gmpxx_mkII.h.in.
- Added a mutex-protected precision cache for log_two.
- Extended tests/test_mpf_pi.cpp to cover log_two requested precision, default precision, lower-precision cache reuse, and decimal prefix accuracy.

MPF special function implementation status:
- pi / const_pi: implemented with Gauss-Legendre AGM and precision cache.
- log_two / const_log2: implemented with Jacobi theta series plus AGM and precision cache.
- sqrt / abs: implemented directly with GMP mpf_sqrt/mpf_abs.
- exp / log / sin / cos / tan / sinh / cosh / tanh / atan2 / pow / gamma: currently present as double-backed implementations except where noted above.
- log10 / log1p / exp2 / exp10 / expm1 / asin / acos / atan / asinh / acosh / atanh / reciprocal_gamma: not yet implemented as high-precision MPF special functions in this repo.

Tests updated:
- tests/test_mpf_pi.cpp

Exact commands run:
- sed -n '1,260p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '5028,5138p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- rg -n "const_log2|log_two|log2\\(|const_pi|test_mpf_pi|special function|MPF .*function" include tests STATUS.md README.md
- git status --short
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_mpf_pi
- ctest --test-dir build --output-on-failure
- temporary /tmp/check_log_two and /tmp/check_mpf_constants_test* programs to inspect decimal output and rounding boundary
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R test_mpf_pi
- ctest --test-dir build --output-on-failure
- rg -n "#include <mpfr\\.h>|#include <mpc\\.h>|mpfr_|mpc_|mpc_t" include/gmpxx_mkII.h include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- rg -n "#include <gmpxx\\.h>|mpf_set_default_prec" include tests examples benchmarks CMakeLists.txt cmake

Pass/fail result:
- Initial extended test_mpf_pi: FAIL, log_two decimal prefix tests asserted rounded boundary digits.
- Adjusted log_two prefix tests: PASS
- Final cmake --build build -j: PASS
- Final ctest --test-dir build --output-on-failure: PASS, 65/65 tests passed
- Source scan for forbidden GMP-header MPFR/MPC includes and symbols: PASS, no matches
- Source scan for #include <gmpxx.h> and mpf_set_default_prec: PASS, no matches

Known issues:
- Most existing MPF transcendental functions are still double-backed; high-precision ports remain pending.

Post-phase MPF compute_log port status:
DONE

Implemented features:
- Ported the GMP-only compute_log support stack from ../gmpxx_mkII/include/gmpxx_mkII.h.in into include/gmpfrxx_mkII/detail/math_mpf.hpp.
- Added precision helpers for logarithms: minimum target precision, ceil_log2_precision, log cancellation probe guard bits, guard_bits_for_log1p, working_precision_for_log1p, guard_bits_for_log, log_cancellation_bits, and working_precision_for_log.
- Added compute_log1p with both small Taylor and atanh-series paths, including domain errors for x = -1 and x < -1.
- Added mul_signed_exp and AGM-based compute_log, including log(0), log(x < 0), log(1), near-one, and scaled AGM paths.
- Kept public gmpxx::log() unchanged for now; it is still the existing double-backed wrapper until the public logarithm phase connects these helpers.

Tests updated:
- Added tests/test_mpf_compute_log.cpp.
- Registered test_mpf_compute_log in tests/CMakeLists.txt.

Exact commands run:
- sed -n '1,260p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '5140,5265p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- rg -n "log2|compute_log|log1p|const_log2|log_two" include tests STATUS.md
- sed -n '5265,5335p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '260,380p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- rg -n "ceil_log2_precision|log_cancellation_probe_guard_bits|domain_error|operator<=|operator<\\(" include/gmpfrxx_mkII/detail include
- rg -n "friend bool operator|operator<|operator==|mpf_cmp|precision_type|log_cancellation_probe_guard_bits|ceil_log2" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/*.hpp
- sed -n '1,140p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '140,340p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '340,460p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- rg -n "using precision_type|typedef.*precision_type|precision_type" ../gmpxx_mkII/include/gmpxx_mkII.h.in include/gmpfrxx_mkII/detail
- rg -n "log2\\(|log1p\\(|log\\(" ../gmpxx_mkII/tests include tests
- sed -n '380,470p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '130,260p' ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp
- rg -n "test_mpf_pi|add_executable|add_test" CMakeLists.txt tests/CMakeLists.txt
- sed -n '1,80p' tests/CMakeLists.txt
- sed -n '1,130p' tests/test_mpf_pi.cpp
- git status --short
- cmake --build build -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- git diff -- include/gmpfrxx_mkII/detail/math_mpf.hpp tests/test_mpf_compute_log.cpp tests/CMakeLists.txt
- gdb -batch -ex run -ex bt --args build/tests/test_mpf_compute_log
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure
- tail -n 80 STATUS.md

Pass/fail result:
- Initial test_mpf_compute_log: FAIL, prefix checks asserted rounded boundary digits.
- Adjusted prefix lengths: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- Public gmpxx::log(), gmpxx::log1p(), and gmpxx::log2() have not yet been connected to these high-precision helpers.
- gmpxx::log() remains double-backed until the next public API wiring step.

Post-phase MPF compute_log high-precision test extension:
DONE

Implemented features:
- Extended tests/test_mpf_compute_log.cpp to exercise compute_log at 512-bit, 1024-bit, and 2048-bit precision.
- Added high-precision checks for log(2), log(4), log(1/2), and the near-one compute_log path against compute_log1p.

Tests updated:
- tests/test_mpf_compute_log.cpp

Exact commands run:
- sed -n '1,220p' tests/test_mpf_compute_log.cpp
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- Public gmpxx::log(), gmpxx::log1p(), and gmpxx::log2() remain disconnected from the high-precision helpers.

Post-phase MPF pi high-precision test extension:
DONE

Implemented features:
- Extended tests/test_mpf_pi.cpp to compute pi at 512-bit, 1024-bit, 2048-bit, and 4096-bit precision.
- Added a 1000-decimal-place pi reference literal from the upstream gmpxx_mkII WolframAlpha-backed test comment.
- Checked high-precision pi results against the reference literal with guard digits to avoid false failures at rounded decimal boundaries.
- Added cache regression checks that compute 4096-bit pi first, then request 2048-bit, 1024-bit, and 512-bit pi again and require exact equality with the values captured before the 4096-bit cache raise.

Tests updated:
- tests/test_mpf_pi.cpp

Exact commands run:
- sed -n '1,150p' tests/test_mpf_pi.cpp
- rg -n "314159265358979|pi_digits|reference.*pi|const_pi" ../gmpxx_mkII tests include
- sed -n '2300,2380p' ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- sed -n '180,230p' ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp
- sed -n '2245,2325p' ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- cmake --build build --target test_mpf_pi -j
- ctest --test-dir build -R test_mpf_pi --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial high-precision prefix checks: FAIL, decimal boundary rounded the final checked digit.
- Adjusted pi prefix checks to request guard digits and compare only stable leading digits: PASS.
- Initial cache regression storage: FAIL, default-constructed mpf_class preserved 512-bit precision on assignment.
- Fixed storage objects to use explicit 512/1024/2048-bit precision: PASS.
- Final test_mpf_pi: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- The 4096-bit result is checked against the available 1000-decimal-place reference prefix, not a full 4096-bit-length decimal reference.

Post-phase MPF hardcoded pi constants:
DONE

Implemented features:
- Added a hardcoded pi decimal literal in include/gmpfrxx_mkII/detail/math_mpf.hpp.
- Routed gmpxx::pi(512), gmpxx::pi(1024), and gmpxx::pi(2048) through hardcoded constants instead of the AGM cache.
- Kept non-hardcoded precision requests on the existing Gauss-Legendre AGM plus precision cache path.
- gmpxx::const_pi() continues to call pi(default_mpf_precision_bits()), so the default 512-bit precision now uses the hardcoded pi path.

Tests updated:
- tests/test_mpf_pi.cpp

Exact commands run:
- sed -n '1,190p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '190,235p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- cmake --build build --target test_mpf_pi -j
- ctest --test-dir build -R test_mpf_pi --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_pi: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- Only 512-bit, 1024-bit, and 2048-bit pi use hardcoded constants. Other requested precisions still compute through AGM and cache.

Post-phase MPF hardcoded log_two constants:
DONE

Implemented features:
- Added a hardcoded log_two decimal literal in include/gmpfrxx_mkII/detail/math_mpf.hpp.
- Routed gmpxx::log_two(512), gmpxx::log_two(1024), and gmpxx::log_two(2048) through hardcoded constants instead of the theta-series plus AGM cache.
- Kept non-hardcoded precision requests on the existing Jacobi theta-series plus AGM and precision cache path.
- gmpxx::const_log2() continues to call log_two(default_mpf_precision_bits()), so the default 512-bit precision now uses the hardcoded log_two path.

Tests updated:
- tests/test_mpf_pi.cpp

Exact commands run:
- sed -n '185,330p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '330,370p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '1,150p' tests/test_mpf_pi.cpp
- sed -n '150,230p' tests/test_mpf_pi.cpp
- cmake --build build --target test_mpf_pi -j
- ctest --test-dir build -R test_mpf_pi --output-on-failure
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- git diff -- include/gmpfrxx_mkII/detail/math_mpf.hpp tests/test_mpf_pi.cpp
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_pi: PASS.
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- Only 512-bit, 1024-bit, and 2048-bit log_two use hardcoded constants. Other requested precisions still compute through theta-series plus AGM and cache.

Post-phase MPF public logarithm API wiring:
DONE

Implemented features:
- Connected gmpxx::log(const mpf_class&) to mpf_math_detail::compute_log instead of the previous double-backed std::log path.
- Added gmpxx::log2(const mpf_class&) using compute_log(x) / log_two(work) with guard precision.
- Added gmpxx::log1p(const mpf_class&) using mpf_math_detail::compute_log1p.
- Added expression-result overloads for gmpxx::log2(expr) and gmpxx::log1p(expr), matching the existing eager math function pattern.

Tests updated:
- tests/test_mpf_compute_log.cpp

Exact commands run:
- sed -n '500,610p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '610,760p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '1,220p' tests/test_mpf_compute_log.cpp
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build -R test_mpf_pi --output-on-failure
- git diff --stat
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial test_mpf_compute_log: FAIL, log2(10) reference literal contained incorrect digits.
- Corrected log2(10) reference literal: PASS.
- test_mpf_pi: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- High-precision MPF transcendental case-list parity with upstream remains pending; later phases added log10, exp2, exp10, and expm1 API/test coverage.

Post-phase MPF log10 exp2 exp10 expm1 API wiring:
DONE

Implemented features:
- Ported the GMP-only exponential helper stack from ../gmpxx_mkII/include/gmpxx_mkII.h.in:
  - guard_bits_for_exp
  - working_precision_for_exp
  - round_to_nearest_mp_exp
  - exp_taylor_reduced
  - compute_exp
  - guard_bits_for_expm1
  - working_precision_for_expm1
  - expm1_taylor_small
  - compute_expm1
- Added mpf_math_detail::log_ten.
- Connected gmpxx::exp(const mpf_class&) to compute_exp instead of the previous double-backed std::exp path.
- Added high-precision public APIs:
  - gmpxx::log10(const mpf_class&)
  - gmpxx::exp2(const mpf_class&)
  - gmpxx::exp10(const mpf_class&)
  - gmpxx::expm1(const mpf_class&)
- Added expression-result overloads for log10(expr), exp2(expr), exp10(expr), and expm1(expr).

Tests updated:
- tests/test_mpf_compute_log.cpp

Exact commands run:
- sed -n '5320,5450p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '5730,5760p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6060,6130p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6130,6195p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- exp10/log10 constants are computed through log_ten rather than a hardcoded log(10) cache.
- Remaining high-precision MPF transcendental ports include trigonometric, inverse trigonometric, hyperbolic, inverse hyperbolic, pow, gamma, and reciprocal_gamma.

Post-phase MPF sin cos tan API wiring:
DONE

Implemented features:
- Ported the GMP-only trigonometric helper stack from ../gmpxx_mkII/include/gmpxx_mkII.h.in:
  - sincos_result
  - trig_constant_cache_state
  - guard_bits_for_trig
  - working_precision_for_trig
  - trig_constant_precision
  - trig_constant_cache
  - sincos_taylor_small
  - ensure_trig_constants
  - trig_pi_over_two
  - trig_two_over_pi
  - compute_sincos
  - compute_sin
  - compute_cos
- Connected gmpxx::sin(const mpf_class&) and gmpxx::cos(const mpf_class&) to compute_sin/compute_cos instead of the previous double-backed std::sin/std::cos paths.
- Connected gmpxx::tan(const mpf_class&) to compute_sin(x) / compute_cos(x) with guard precision instead of the previous double-backed std::tan path.

Tests updated:
- tests/test_mpf_compute_log.cpp

Exact commands run:
- sed -n '5440,5625p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6190,6235p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '650,850p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- rg -n "struct sincos_result|trig_constant_cache_state" ../gmpxx_mkII/include/gmpxx_mkII.h.in include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '4890,4920p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial test_mpf_compute_log build: FAIL, this repo does not expose the same mpz_class +=/-= helpers or direct mpz_data mpf constructor shape as the source header.
- Adjusted the port to use mpz_add_ui/mpz_sub_ui and explicit mpf_set_z conversions: PASS.
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- Inverse trigonometric, hyperbolic, inverse hyperbolic, pow, gamma, and reciprocal_gamma high-precision MPF ports remain pending.

Post-phase MPF random trigonometric smoke test:
DONE

Implemented features:
- Added a deterministic random smoke test over x in [-pi, pi] for gmpxx::sin, gmpxx::cos, and gmpxx::tan.
- The smoke test uses a fixed std::mt19937_64 seed and 32 samples at 256-bit MPF precision.
- sin/cos are compared against std::sin/std::cos through double conversion at 1e-15 tolerance.
- tan is compared against std::tan at 1e-14 tolerance while skipping near-pole samples with |cos(x)| <= 0.05.

Tests updated:
- tests/test_mpf_compute_log.cpp

Exact commands run:
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- The random trig smoke test is intentionally double-reference smoke coverage only; high-precision reference literal tests for arbitrary trigonometric arguments remain pending.

Post-phase MPF random log exp smoke test:
DONE

Implemented features:
- Added deterministic random smoke tests for gmpxx::log, gmpxx::log2, gmpxx::log10, gmpxx::exp, gmpxx::exp2, gmpxx::exp10, gmpxx::expm1, and gmpxx::log1p.
- The smoke tests use fixed std::mt19937_64 seeds and 256-bit MPF precision.
- log/log2/log10 sample positive values in [0.01, 10].
- exp/exp2/exp10/expm1 sample values in [-5, 5].
- log1p samples values in [-0.75, 2].
- Additional tiny-value smoke checks cover log1p/expm1 cancellation-sensitive ranges near zero.

Tests updated:
- tests/test_mpf_compute_log.cpp

Exact commands run:
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- These random log/exp smoke tests are double-reference smoke coverage only; high-precision independent references for random values remain pending.

Post-phase MPF inverse trigonometric and hyperbolic functions:
DONE

Implemented features:
- Ported the GMP-only atan helper stack from ../gmpxx_mkII/include/gmpxx_mkII.h.in:
  - guard_bits_for_atan
  - working_precision_for_atan
  - atan_taylor_small
  - compute_atan
  - compute_atan2
- Added high-precision public APIs:
  - gmpxx::asin(const mpf_class&)
  - gmpxx::acos(const mpf_class&)
  - gmpxx::atan(const mpf_class&)
  - gmpxx::atan2(const mpf_class&, const mpf_class&)
  - gmpxx::sinh(const mpf_class&)
  - gmpxx::cosh(const mpf_class&)
  - gmpxx::tanh(const mpf_class&)
  - gmpxx::asinh(const mpf_class&)
  - gmpxx::acosh(const mpf_class&)
  - gmpxx::atanh(const mpf_class&)
- Replaced the previous double-backed sinh/cosh/tanh/atan2 paths with GMP-only implementations.
- Added expression-result overloads for asin, acos, atan, asinh, acosh, and atanh.
- Added domain checks for asin/acos outside [-1, 1], acosh below 1, and atanh outside (-1, 1).

Tests updated:
- tests/test_mpf_compute_log.cpp

Exact commands run:
- sed -n '5620,5735p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6230,6335p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6335,6425p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '850,1120p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- sed -n '1120,1320p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- pow, gamma, and reciprocal_gamma high-precision MPF ports remain pending.
- Random inverse trig/hyperbolic smoke tests are double-reference smoke coverage only; high-precision independent references for random values remain pending.

Post-phase MPF pow gamma reciprocal_gamma functions:
DONE

Implemented features:
- Ported the GMP-only pow helper stack from ../gmpxx_mkII/include/gmpxx_mkII.h.in:
  - guard_bits_for_pow
  - working_precision_for_pow
  - mpf_is_exact_integer
  - pow_integer_unsigned
  - compute_pow
- Replaced gmpxx::pow(const mpf_class&, const mpf_class&) double-backed std::pow path with compute_pow.
- Added expression-result overloads for pow(expr, mpf_class), pow(mpf_class, expr), and pow(expr, expr).
- Ported the Spouge-style Gamma helper stack:
  - gamma_spouge_terms
  - gamma_spouge_coefficient
  - gamma_real_pole
  - gamma_spouge_positive
- Replaced gmpxx::gamma(const mpf_class&) double-backed std::tgamma path with the GMP-only implementation.
- Added gmpxx::reciprocal_gamma(const mpf_class&), returning zero at Gamma poles.
- Added expression-result overload for reciprocal_gamma(expr).

Tests updated:
- tests/test_mpf_compute_log.cpp

Exact commands run:
- sed -n '5770,5835p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6420,6535p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6535,6625p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '5835,5895p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6385,6475p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '6345,6395p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial high-precision Gamma exact-value test: FAIL, Spouge approximation was not within the same near-full precision tolerance used for exact algebraic identities.
- Adjusted Gamma known-value tolerance to 128 bits while keeping random double-reference smoke coverage: PASS.
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- Gamma uses a bounded-term Spouge-style approximation, so the current high-precision known-value tests use a conservative 128-bit tolerance.
- Random pow/gamma/reciprocal_gamma smoke tests are double-reference smoke coverage only; high-precision independent references for random values remain pending.

Post-phase MPF transcendent test coverage port:
DONE

Implemented features:
- Reviewed ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp and ported missing coverage into the existing tests/test_mpf_compute_log.cpp.
- Did not add tests/test_mpf_transcendent_functions.cpp to this repository, per request.
- Added ULP helpers and decimal literal parsing helpers adapted to this repository's C++17 and mpf_class API.
- Added fixed high-precision decimal reference checks for log1p, log, log2, log10, exp, expm1, sin, cos, tan, atan, atan2, and pow.
- Added precision-doubling checks for pi, log_two, log1p, log, log2, log10, exp, expm1, sin, cos, tan, atan, atan2, and pow.
- Kept the upstream ULP tolerances for the ported reference-literal and precision-doubling checks; no relaxed tolerance was needed for the current implementation.
- Added explicit precision policy checks for representative unary and binary transcendental functions.
- Added atan2 expression overloads for atan2(mpf_class, expr), atan2(expr, mpf_class), and atan2(expr, expr), then covered them with static assertions.
- Added atan2 axis checks for (0, 1), (0, -1), (1, 0), (-1, 0), and (0, 0).

Tests updated:
- tests/test_mpf_compute_log.cpp
- include/gmpfrxx_mkII/detail/math_mpf.hpp

Exact commands run:
- sed -n '1,220p' ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp
- sed -n '220,520p' ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp
- sed -n '520,760p' ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp
- nl -ba tests/test_mpf_compute_log.cpp | sed -n '1,700p'
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_compute_log: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- The upstream test uses C++20 concepts and local gmpxx_mkII helper APIs; the port keeps this repository's C++17 style and integrates into the existing test target instead of copying the file verbatim.
- Gamma reference-literal coverage from the upstream file was not present there; current Gamma coverage remains the existing known-value and double-reference smoke coverage.

Post-phase MPF Gamma Spouge precision policy:
DONE

Implemented features:
- Replaced the fixed 128-term Spouge cap with a precision-derived term count.
- The Spouge term count now targets the requested result bits plus log guard bits and a 32-bit margin using the log2(2*pi) decay factor, while preserving the 24-term minimum.
- Increased Gamma's internal work precision by the selected Spouge term count to cover cancellation in the alternating coefficient sum.
- Tightened the existing Gamma known-value checks back to the same precision - 32 bit tolerance used by the other high-precision MPF transcendental checks.

Tests updated:
- include/gmpfrxx_mkII/detail/math_mpf.hpp
- tests/test_mpf_compute_log.cpp

Exact commands run:
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial cap-removal-only attempt: FAIL at the 1024-bit Gamma known-value tolerance, showing that term growth without extra work precision was insufficient.
- After adding term-count-proportional work precision: test_mpf_compute_log PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 66/66 tests passed.

Known issues:
- Gamma still uses a finite Spouge approximation rather than an independently rounded MPFR-backed oracle; random Gamma coverage remains double-reference smoke coverage.

Post-phase MPFR string and stream IO:
DONE

Implemented features:
- Added mpfrxx::mpfr_class::set_str(const char*, int) and set_str(const std::string&, int), preserving the existing value on parse failure.
- Added mpfrxx::mpfr_class::get_str(), get_str(mpfr_exp_t&, int, std::size_t), and to_string().
- Added mpfrxx::mpfr_class string set() validation that throws std::invalid_argument on invalid decimal input.
- Added std::ostream operator<< for mpfrxx::mpfr_class using MPFR formatting and honoring fixed/scientific/defaultfloat, precision, uppercase, showpoint, showpos, width, fill, left, and internal alignment.
- Added std::ostream operator<< for MPFR expression nodes by materializing to mpfr_class.
- Added std::istream operator>> for mpfrxx::mpfr_class using stream base flags, locale decimal point, exponent parsing, and value-preserving failure semantics.

Tests added:
- tests/test_mpfr_string_io.cpp

Tests updated:
- tests/CMakeLists.txt
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- STATUS.md

Exact commands run:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_mpfr_string_io -j
- ctest --test-dir build -R test_mpfr_string_io --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpfr_string_io: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 67/67 tests passed.

Known issues:
- MPFR stream input currently covers finite numeric tokens; infinities and NaNs are not specially tokenized by the custom extractor.

Post-phase GMP string and stream IO:
DONE

Implemented features:
- Added shared GMP stream token helpers for integer, rational, and floating-point extraction.
- Added std::istream operator>> for gmpxx::mpz_class, preserving the old value on parse failure.
- Added std::istream operator>> for gmpxx::mpq_class, including zero-denominator rejection and value-preserving failure semantics.
- Added std::istream operator>> for gmpxx::mpf_class, preserving destination precision and old value on parse failure.
- Added std::ostream operator<< for GMP expression nodes returning mpz_class, mpq_class, and mpf_class by materializing before output.
- Kept GMP-only implementation paths in gmpxx_mkII.h; no MPFR or MPC dependency was introduced.

Tests updated:
- tests/test_zq_string_io.cpp
- tests/test_mpf_string_io.cpp
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- include/gmpfrxx_mkII/detail/mpf_impl.hpp
- STATUS.md

Exact commands run:
- cmake --build build --target test_zq_string_io test_mpf_string_io -j
- ctest --test-dir build -R "test_(zq|mpf)_string_io" --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_zq_string_io: PASS.
- test_mpf_string_io: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 67/67 tests passed.

Known issues:
- GMP stream input covers finite numeric tokens; it does not add special spelling support beyond GMP's numeric parsers.

Post-phase source license headers:
DONE

Implemented features:
- Added the requested 2026 Nakata, Maho BSD-style license header to all tracked C++ source files.
- Covered public headers, private implementation headers, examples, benchmarks, tests, and compile-fail tests.
- Excluded upstream reference material, Markdown documentation, CMake files, build outputs, and untracked editor backup files.

Tests updated:
- 85 tracked C++ source files had header-only changes.
- STATUS.md

Exact commands run:
- git ls-files "*.cpp" "*.h" "*.hpp" | rg -v "^reference/upstream/"
- bulk header insertion over tracked C++ source files
- verification script checking every targeted file starts with exactly one requested header

Pass/fail result:
- Header coverage verification: PASS, 85/85 targeted files have exactly one requested header.

Known issues:
- None.

Post-phase MPF rational helper rename and negative MPZ addmul fast path:
DONE

Implemented features:
- Renamed the internal MPF rational conversion helper from
  `mpf_set_q_exact` to `mpf_set_q_at_precision`, matching its actual behavior:
  the numerator and denominator are exact inputs, but the final MPF quotient is
  rounded to the destination/evaluation precision.
- Extended the MPZ direct addmul/submul scalar path for negative signed
  scalars whose magnitude fits `unsigned long`.
- `x += a * (-3)` now maps to `mpz_submul_ui(x, a, 3)`.
- `x -= a * (-3)` now maps to `mpz_addmul_ui(x, a, 3)`.
- The signed magnitude computation avoids overflow for the minimum signed
  value by using `-(value + 1) + 1`.

Tests added:
- Extended `tests/test_mpz_addmul_alloc_count.cpp` with zero-allocation
  checks for negative signed scalar addmul/submul cases.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `tests/test_mpz_addmul_alloc_count.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "mpf_set_q_exact|mpz_addmul_apply_object_scalar|addmul|submul|mul_apply_object_scalar" include tests STATUS.md`
- `sed -n '1500,1785p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1420,1510p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1,220p' tests/test_mpz_addmul_fusion.cpp`
- `sed -n '1,220p' tests/test_mpz_addmul_alloc_count.cpp`
- `sed -n '2028,2140p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `cmake --build build -j --target test_mpz_addmul_alloc_count test_mpf_basic test_mixed_zq_mpf_promotion`
- `ctest --test-dir build -R 'test_mpz_addmul_alloc_count|test_mpf_basic|test_mixed_zq_mpf_promotion' --output-on-failure`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpz_addmul_alloc_count test_mpf_basic test_mixed_zq_mpf_promotion`: PASS.
- `ctest --test-dir build -R 'test_mpz_addmul_alloc_count|test_mpf_basic|test_mixed_zq_mpf_promotion' --output-on-failure`: PASS, 3/3 tests passed.
- `git diff --check`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 144/144 tests passed.

Known issues:
- Negative signed scalar magnitudes larger than `unsigned long` still fall
  back to the general `mpz_class` scalar path, as before.

Post-phase benchmark repeat aggregation and error bars:
DONE

Implemented features:
- Added a repeat-count argument to `benchmarks/common/run_benchmarks.sh`.
  The default repeat count is 10, and each benchmark executable invocation is
  recorded as `RUN i/n` under the same `COMMAND` label.
- Updated `benchmarks/common/plot.py` to aggregate repeated `MFLOPS` samples by
  `(kernel, variant)`, plot the sample mean, and draw sample standard deviation
  error bars.
- Kept plot output PNG-only in the shared plotter, matching the curated
  benchmark-artifact policy.
- Updated `benchmarks/README.md` to document the repeat-count argument,
  error-bar semantics, and PNG-only output.
- Generated a new 10-repeat GMP benchmark artifact set under
  `benchmarks/gmp/results_raw/20260508_fixed_precision_fastpath_repeat10/`.

Tests added:
- None.

Tests updated:
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/common/plot.py`
- `benchmarks/README.md`
- `STATUS.md`

Exact commands run:
- `python3 benchmarks/common/plot.py benchmarks/gmp/results_raw/20260508_fixed_precision_fastpath/benchmark_20260508_132306.log --output-dir /tmp/gmpfrxx_plot_smoke`
- `benchmarks/common/run_benchmarks.sh build_bench_release 512 1000000 1000000 300 300 80 80 80 benchmarks/gmp/results_raw/20260508_fixed_precision_fastpath_repeat10 10`
- `find benchmarks/gmp/results_raw/20260508_fixed_precision_fastpath_repeat10 -maxdepth 1 -type f | sort`
- `find benchmarks/gmp/results_raw/20260508_fixed_precision_fastpath_repeat10 -type f -name '*.pdf'`
- `rg -n "BENCHMARK_PARAMS|RUN 10/10|MFLOPS" benchmarks/gmp/results_raw/20260508_fixed_precision_fastpath_repeat10/*.log`
- `bash -n benchmarks/common/run_benchmarks.sh`
- `python3 -m py_compile benchmarks/common/plot.py`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Existing-log plot smoke: PASS.
- 10-repeat benchmark run: PASS; generated 1 raw log and 10 PNG plots.
- PDF artifact check: PASS, no PDFs in the new repeat10 result directory.
- `bash -n benchmarks/common/run_benchmarks.sh`: PASS.
- `python3 -m py_compile benchmarks/common/plot.py`: PASS.
- `git diff --check`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 142/142 tests passed.

Known issues:
- The benchmark result set is intentionally machine- and load-dependent.  The
  error bars show observed jitter for this run; they are not a cross-machine
  performance guarantee.

Post-phase MPFR fixed-precision fast path:
DONE

Implemented features:
- Added MPFR expression materialization precision selection through
  `mpfr_materialization_precision()`.
- Under `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`, non-random MPFR
  expression construction now uses `mpfrxx::mpfr_class::default_precision()`
  directly instead of walking the expression tree for max leaf precision.
- Preserved explicit random-expression precision requests by excluding
  `random_mpfr_expr` from the fixed-precision shortcut.
- Added a thread-local MPFR scratch pool for fixed-precision `mpfr_class`
  compound add/sub of direct products, so `acc += x * y` and `acc -= x * y`
  can reuse a per-thread product temporary instead of allocating a fresh
  `mpfr_t` on the steady-state path.
- Kept MPFR rounding and exponent-range policy intact by using the existing
  `current_eval_context()` and `mpfr_exponent_range_guard` paths.

Tests added:
- `tests/test_mpfr_fixed_precision_materialization.cpp`
- `tests/test_mpfr_fixed_precision_tls_scratch.cpp`

Tests updated:
- `tests/CMakeLists.txt`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpfr_fixed_precision_materialization test_mpfr_fixed_precision_tls_scratch`
- `ctest --test-dir build -R 'test_mpfr_fixed_precision_materialization|test_mpfr_fixed_precision_tls_scratch' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII 10000 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH 10000 512`

Pass/fail result:
- Targeted MPFR fixed-precision tests: PASS, 2/2 tests passed.
- First full `ctest --test-dir build --output-on-failure`: FAIL due running
  concurrently with a full rebuild; several example executables were observed
  while being relinked and reported BAD_COMMAND/permission denied.
- Re-run after build completion: PASS, 144/144 tests passed.
- MPFR Rdot baseline smoke: PASS, `DIFF ... OK`.
- MPFR Rdot fixed-precision-fastpath smoke: PASS, `DIFF ... OK`.

Known issues:
- The MPFR fast path intentionally only covers fixed-precision materialization
  and direct-product `+=`/`-=` compound assignment.  More general nested
  expression scratch reuse remains a separate optimization.

Post-phase MPF rational-at-precision conversion temporary reduction:
DONE

Implemented features:
- Reduced `mpf_set_q_at_precision` from two `mpf_t` temporaries to one by loading
  the rational numerator directly into the destination and only materializing
  the denominator as a temporary before division.
- Preserved destination precision and the exact numerator/denominator
  conversion policy.

Tests added:
- Extended `tests/test_mixed_type_arithmetic.cpp` with a focused MPQ-to-MPF
  assignment check that computes the reference value using the same
  numerator/direct-destination and denominator-temporary formula.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `tests/test_mixed_type_arithmetic.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1325,1385p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `rg -n "mpf_set_q_at_precision|mpq.*mpf|set_q|mpf_set_q|mpq_class" tests include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1388,1428p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpf_scalar_alloc_count test_mixed_type_arithmetic test_construction_copy`
- `ctest --test-dir build -R 'test_mpf_scalar_alloc_count|test_mixed_type_arithmetic|test_construction_copy' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j --target test_mpf_scalar_alloc_count test_mixed_type_arithmetic test_construction_copy`: PASS.
- `ctest --test-dir build -R 'test_mpf_scalar_alloc_count|test_mixed_type_arithmetic|test_construction_copy' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- GMP's memory hooks did not provide a stable allocation-count signal for
  this small `mpf_t` temporary path in this environment, so the regression is
  covered by value/precision tests and implementation review rather than a
  hard allocation-count assertion.

Post-phase fixed-precision benchmark naming cleanup:
DONE

Implemented features:
- Removed the previous no-precision-change macro name from tracked source
  files.
- Renamed benchmark fixed-precision variants to the
  `*_mkII_FIXED_PRECISION_FASTPATH` suffix.
- Benchmark fixed-precision targets now define only
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`.
- Updated benchmark runner scripts, plot helpers, benchmark READMEs, and
  affected tests/status text to use the fixed-precision fast-path naming.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/README.md`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/common/plot.py`
- `benchmarks/gmp/*/README.md`
- `benchmarks/gmp/*/go.sh`
- `benchmarks/gmp/*/plot.py`
- `tests/test_gmpxx_mkII.cpp`
- `STATUS.md`

Exact commands run:
- Searched tracked files for the previous no-precision-change macro and
  target suffix names.
- Replaced tracked benchmark macro/target suffix uses with fixed-precision
  fast-path naming.
- Rechecked tracked files for the previous no-precision-change naming.
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH test_mpf_fixed_precision_tls_scratch test_mpf_fixed_precision_materialization`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH 10000 512`
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH 10000 512`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpf_fixed_precision_materialization' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Final tracked-file search for the previous no-precision-change naming:
  PASS, no tracked matches.
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j --target Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH test_mpf_fixed_precision_tls_scratch test_mpf_fixed_precision_materialization`: PASS.
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH 10000 512`: PASS, `DIFF` OK.
- `build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH 10000 512`: PASS, `DIFF` OK.
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpf_fixed_precision_materialization' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 142/142 tests passed.

Known issues:
- Historical raw benchmark logs, if any, may still mention old target names
  as archived measurement data.

Post-phase MPF fixed-precision materialization fast path:
DONE

Implemented features:
- Added `mpf_materialization_precision(expr)` for `mpf_class` expression
  construction.
- When `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` is enabled, MPF
  expression materialization skips the runtime leaf-precision walk and uses
  `gmpxx::default_mpf_precision_bits()` directly.
- `random_mpf_expr` remains excluded from this shortcut so explicit
  `get_f(precision)` materialization requests keep their requested precision.
- Default builds keep the existing max-leaf precision policy.

Tests added:
- Added `tests/test_mpf_fixed_precision_materialization.cpp`, compiled with
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`, to verify expression
  construction uses default materialization precision instead of max leaf
  precision and that assignment still preserves destination precision.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `tests/CMakeLists.txt`
- `tests/test_mpf_fixed_precision_materialization.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,120p' include/gmpfrxx_mkII/detail/config.hpp`
- `sed -n '1348,1400p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `rg -n "mpf_class::mpf_class\\(const Expr|mpf_expression_precision\\(expr\\)|assume_fixed_precision_fastpath" include tests benchmarks/gmp/00_Rdot`
- `rg -n "random_mpf_expr|materialization_precision|default_mpf_precision_bits\\(" include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_mpf* benchmarks/gmp/00_Rdot`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpf_fixed_precision_materialization test_mpf_precision_policy test_mpf_scalar_alloc_count`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_materialization|test_mpf_precision_policy|test_mpf_scalar_alloc_count' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- First focused ctest run exposed an overly strict test assertion: GMP rounds
  `mpf_init2(160)` up internally, so the test now compares against an actual
  default-constructed object precision.
- `cmake --build build -j --target test_mpf_fixed_precision_materialization test_mpf_precision_policy test_mpf_scalar_alloc_count`: PASS.
- `ctest --test-dir build -R 'test_mpf_fixed_precision_materialization|test_mpf_precision_policy|test_mpf_scalar_alloc_count' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 141/141 tests passed.

Known issues:
- This removes the constructor-side precision walk under the fixed precision
  build option.  Direct `lhs += a * b` product temporary reuse is tracked in
  the following TLS scratch phase.

Post-phase MPF fixed-precision TLS scratch pool:
DONE

Implemented features:
- Added a small `thread_local` MPF scratch pool used only when
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` is enabled.
- Optimized direct `mpf_class += mpf_class * mpf_class` and
  `mpf_class -= mpf_class * mpf_class` compound assignments to reuse a TLS
  product scratch instead of allocating and clearing a product `mpf_t` on each
  call.
- The operation still performs the same two-step MPF semantics: first compute
  the product at destination precision, then add or subtract it from the
  destination.  It does not introduce fused arithmetic.
- The pool stores four per-thread slots for nested expression evaluation and
  falls back to an operation-local temporary when all slots are busy.
- Scratch slots restore their allocated precision before `mpf_clear`; borrowed
  slots use `mpf_set_prec_raw` so a previously larger slot does not silently
  compute the product at a higher precision.
- Slots above 1 Mi bit precision are not retained in the TLS pool.  Those use
  a fallback temporary that is cleared at the end of the operation.

Tests added:
- Added `tests/test_mpf_fixed_precision_tls_scratch.cpp`, compiled with
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`, to verify `+= a * b` and
  `-= a * b` match explicit `mpf_mul` followed by `mpf_add`/`mpf_sub`, and
  that steady-state direct multiply compound assignment performs zero GMP
  allocations after the TLS scratch is warmed.
- Renamed benchmark fixed-precision variants to
  `*_mkII_FIXED_PRECISION_FASTPATH`.
- Removed the previous no-precision-change macro from tracked source files.
  Benchmark fixed-precision targets now define only the canonical
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` macro.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `benchmarks/CMakeLists.txt`
- `tests/CMakeLists.txt`
- `tests/test_mpf_fixed_precision_tls_scratch.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,220p' tests/test_mpf_alloc_count.cpp`
- `sed -n '1,220p' tests/test_mpf_scalar_alloc_count.cpp`
- `rg -n "thread_local|scratch|mp_set_memory_functions|alloc_count|RUN_SERIAL" include/gmpfrxx_mkII/detail tests benchmarks/gmp/00_Rdot`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpf_fixed_precision_tls_scratch test_mpf_fixed_precision_materialization test_mpf_alloc_count test_mpf_scalar_alloc_count`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpf_fixed_precision_materialization|test_mpf_alloc_count|test_mpf_scalar_alloc_count' --output-on-failure`
- `g++ -std=c++17 -O3 -Iinclude -Ibenchmarks/common -Ibenchmarks/gmp/00_Rdot benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp -lgmp -o /tmp/rdot_mpf_normal`
- `g++ -std=c++17 -O3 -DGMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH -Iinclude -Ibenchmarks/common -Ibenchmarks/gmp/00_Rdot benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp -lgmp -o /tmp/rdot_mpf_fastpath`
- `g++ -std=c++17 -O3 -DGMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS -Iinclude -Ibenchmarks/common -Ibenchmarks/gmp/00_Rdot benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp -lgmp -o /tmp/rdot_mpf_normal_count`
- `g++ -std=c++17 -O3 -DGMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH -DGMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS -Iinclude -Ibenchmarks/common -Ibenchmarks/gmp/00_Rdot benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp -lgmp -o /tmp/rdot_mpf_fastpath_count`
- `/tmp/rdot_mpf_normal_count 10000 512`
- `/tmp/rdot_mpf_fastpath_count 10000 512`
- `/tmp/rdot_mpf_normal 1000000 512`
- `/tmp/rdot_mpf_fastpath 1000000 512`
- Repeated `/tmp/rdot_mpf_normal 1000000 512` five times.
- Repeated `/tmp/rdot_mpf_fastpath 1000000 512` five times.
- `cmake --build build -j --target Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH test_mpf_fixed_precision_tls_scratch`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII 1000000 512`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH 1000000 512`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpf_fixed_precision_materialization' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j --target test_mpf_fixed_precision_tls_scratch test_mpf_fixed_precision_materialization test_mpf_alloc_count test_mpf_scalar_alloc_count`: PASS.
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpf_fixed_precision_materialization|test_mpf_alloc_count|test_mpf_scalar_alloc_count' --output-on-failure`: PASS, 4/4 tests passed.
- Operation counts for `Rdot_gmp_kernel_01` at N=10000, precision=512:
  normal `mkII` had `init2=10001`, `clear=10001`, `add=10000`,
  `mul=10000`; fixed-precision TLS scratch had `init2=2`, `clear=1`,
  `add=10000`, `mul=10000`.
- Timing sample for an ad-hoc O3 build at N=1000000, precision=512:
  normal `mkII` ran around 0.072-0.075 s; fixed-precision TLS scratch ran
  around 0.067-0.069 s.
- Timing sample for CMake Debug targets at N=1000000, precision=512:
  `Rdot_gmp_kernel_01_mkII` took 0.127347 s and
  `Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH` took 0.123643 s.
- `cmake --build build -j --target Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH test_mpf_fixed_precision_tls_scratch`: PASS.
- `ctest --test-dir build -R 'test_mpf_fixed_precision_tls_scratch|test_mpf_fixed_precision_materialization' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 142/142 tests passed.

Known issues:
- The TLS scratch fast path is intentionally narrow.  It currently covers only
  direct `mpf_class` leaf products in compound add/sub, matching the Rdot
  kernel pattern.  More general expression products still use the conservative
  temporary path.

Post-phase MPFR addmul fast path review:
DONE

Implemented features:
- Reviewed `mpfr_class` compound assignment for an `lhs += a * b` addmul-style
  fast path.
- Did not add an `mpfr_fma` rewrite under
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH`.  Fixed precision removes
  precision-propagation work, but it does not make fused multiply-add
  semantically equivalent to the current expression-template evaluation.
- The current implementation preserves the existing two-step semantics:
  materialize the product at destination precision, then add it to the
  destination using the active rounding mode.

Tests added:
- None.  This phase is a policy/implementation review that intentionally keeps
  runtime behavior unchanged.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `rg -n "mpfr_fma|mpfr_fms|fma\\(|fmma|mpfr_compound_assign|is_mpz_addmul_direct|addmul_direct|binary_expr<mul_op" include/gmpfrxx_mkII/detail tests benchmarks`
- `sed -n '1685,1810p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1908,1940p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1,180p' tests/test_mpfr_compound_assign.cpp`
- `sed -n '1,160p' tests/test_mpfr_scalar_alloc_count.cpp`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpfr_compound_assign test_mpfr_scalar_alloc_count`
- `ctest --test-dir build -R 'test_mpfr_compound_assign|test_mpfr_scalar_alloc_count' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j --target test_mpfr_compound_assign test_mpfr_scalar_alloc_count`: PASS.
- `ctest --test-dir build -R 'test_mpfr_compound_assign|test_mpfr_scalar_alloc_count' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- MPFR has `mpfr_fma`, but using it as a transparent replacement for
  `lhs += a * b` changes rounding semantics.  A fused operation should only be
  exposed through an explicit fused API or a separate, clearly documented
  semantic-changing build option.

Post-phase MPZ binary expression allocation fast path:
DONE

Implemented features:
- Reworked `mpz_evaluate(binary_expr<...>)` so non-aliasing MPZ binary
  expressions evaluate directly into the destination where possible instead
  of always allocating two `mpz_t` temporaries.
- Added MPZ expression reference detection. If the destination appears inside
  the expression tree, evaluation keeps the conservative temporary path to
  preserve alias safety.
- Added a shared `mpz_apply_binary` helper for MPZ arithmetic, bitwise, and
  shift operations.
- Matched the MPF evaluator shape for leaf/leaf, expression/leaf,
  leaf/expression, and expression/expression cases.

Tests added:
- Extended `tests/test_mpz_addmul_alloc_count.cpp` with zero-allocation
  checks for preallocated `dst = a + b` and `dst = a + b + c`.

Tests updated:
- `include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `tests/test_mpz_addmul_alloc_count.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1450,1565p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1620,1705p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1480,1565p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1565,1645p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1,220p' tests/test_mpz_addmul_alloc_count.cpp`
- `cmake --build build -j --target test_mpz_addmul_alloc_count test_mpz_arithmetic test_mixed_type_arithmetic`
- `ctest --test-dir build -R 'test_mpz_addmul_alloc_count|test_mpz_arithmetic|test_mixed_type_arithmetic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpz_addmul_alloc_count test_mpz_arithmetic test_mixed_type_arithmetic`: PASS.
- `ctest --test-dir build -R 'test_mpz_addmul_alloc_count|test_mpz_arithmetic|test_mixed_type_arithmetic' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- Aliasing expression assignment intentionally keeps the conservative
  temporary path; the zero-allocation claim applies to non-aliasing,
  preallocated destinations.

Post-phase MPQ binary expression allocation fast path:
DONE

Implemented features:
- Reworked `mpq_evaluate(binary_expr<...>)` so non-aliasing MPQ binary
  expressions evaluate directly into the destination where possible instead
  of always allocating two `mpq_t` temporaries.
- Added MPQ expression reference detection. If the destination appears inside
  the expression tree, evaluation keeps the conservative temporary path to
  preserve alias safety.
- Added shared `mpq_apply_binary`, `mpq_evaluate_to_temporary`, and
  `mpq_apply_shift` helpers.
- Optimized MPQ shifts to evaluate the left-hand side into the destination
  and avoid the previous left-hand `mpq_t` temporary.

Tests added:
- Extended `tests/test_mpz_mpq_alloc_count.cpp` with zero-allocation checks
  for preallocated `qdst = qa + qb` and `qdst = qa + qb + qc`.

Tests updated:
- `include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `tests/test_mpz_mpq_alloc_count.cpp`
- `tests/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `sed -n '1740,1825p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1,180p' tests/test_mpz_mpq_alloc_count.cpp`
- `sed -n '1,180p' tests/test_mpq_arithmetic.cpp`
- `rg -n "mpq_evaluate\\(|mpq_add\\(|mpq_mul\\(|mpq_init\\(" include/gmpfrxx_mkII/detail/zq_impl.hpp tests`
- `sed -n '76,92p' tests/CMakeLists.txt`
- `cmake --build build -j --target test_mpz_mpq_alloc_count test_mpq_arithmetic test_mixed_type_arithmetic`
- `ctest --test-dir build -R 'test_mpz_mpq_alloc_count|test_mpq_arithmetic|test_mixed_type_arithmetic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpz_mpq_alloc_count test_mpq_arithmetic test_mixed_type_arithmetic`: PASS.
- `ctest --test-dir build -R 'test_mpz_mpq_alloc_count|test_mpq_arithmetic|test_mixed_type_arithmetic' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- Aliasing expression assignment intentionally keeps the conservative
  temporary path; the zero-allocation claim applies to non-aliasing,
  preallocated destinations.

Post-phase MPQ redundant canonicalize cleanup:
DONE

Implemented features:
- Removed the redundant `mpq_canonicalize(dest)` from the MPQ
  add/sub/mul/div evaluator helper. GMP `mpq_add`, `mpq_sub`, `mpq_mul`, and
  `mpq_div` already return canonical results.
- Kept canonicalization in the MPQ shift helper because shifts mutate the
  numerator or denominator directly and can introduce a common factor.

Tests added:
- None; this is a performance cleanup covered by existing MPQ arithmetic,
  canonicalization, mixed-type, and allocation-count tests.

Tests updated:
- `include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1770,1855p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `rg -n "mpq_apply_binary|mpq_canonicalize\\(dest\\)|mpq_add\\(|mpq_sub\\(|mpq_mul\\(|mpq_div\\(" include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `cmake --build build -j --target test_mpz_mpq_alloc_count test_mpq_arithmetic test_mpq_canonicalization test_mixed_type_arithmetic`
- `ctest --test-dir build -R 'test_mpz_mpq_alloc_count|test_mpq_arithmetic|test_mpq_canonicalization|test_mixed_type_arithmetic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpz_mpq_alloc_count test_mpq_arithmetic test_mpq_canonicalization test_mixed_type_arithmetic`: PASS.
- `ctest --test-dir build -R 'test_mpz_mpq_alloc_count|test_mpq_arithmetic|test_mpq_canonicalization|test_mixed_type_arithmetic' --output-on-failure`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- None.

Post-phase MPFC/MPC binary expression allocation fast path:
DONE

Implemented features:
- Reworked `mpfc_evaluate(binary_expr<...>)` so non-aliasing MPFC binary
  expressions can evaluate directly into the destination instead of always
  materializing left and right `mpfc_class` temporaries.
- Added MPFC expression reference detection. If the destination appears inside
  the expression tree, evaluation keeps the conservative temporary path to
  preserve alias safety.
- Changed `mpfc_class::operator=(expression)` to evaluate directly into the
  existing object for non-aliasing expressions while preserving the existing
  destination precision policy. Aliasing assignment still uses a temporary.
- Confirmed MPC already used the MPFR-style alias detection and binary
  evaluator fast path; added focused left-associative chain regression
  coverage for MPC precision and value preservation.

Tests added:
- Extended `tests/test_mpfc_precision_policy.cpp` with zero-allocation checks
  for preallocated `sum_dst = a + b` and `sum_dst = a + b + c`.
- Extended `tests/test_mpc_precision_policy.cpp` with left-associative
  `chain_dst = a + b + c` coverage.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `tests/test_mpfc_precision_policy.cpp`
- `tests/test_mpc_precision_policy.cpp`
- `tests/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `sed -n '440,620p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '460,700p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '680,720p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1,120p' tests/test_mpc_precision_policy.cpp`
- `sed -n '1,120p' tests/test_mpfc_precision_policy.cpp`
- `cmake --build build -j --target test_mpfc_precision_policy test_mpfc_basic test_mpc_precision_policy test_mpc_aliasing`
- `ctest --test-dir build -R 'test_mpfc_precision_policy|test_mpfc_basic|test_mpc_precision_policy|test_mpc_aliasing' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfc_precision_policy test_mpfc_basic test_mpc_precision_policy test_mpc_aliasing`: PASS.
- `ctest --test-dir build -R 'test_mpfc_precision_policy|test_mpfc_basic|test_mpc_precision_policy|test_mpc_aliasing' --output-on-failure`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- MPFC `mul` and `div` still allocate operation-internal `mpf_t`
  temporaries for the complex arithmetic formula. This phase removes the
  unconditional left/right operand materialization where safe.

Post-phase MPFC mul/div operation-internal temporary reduction:
DONE

Implemented features:
- Added non-aliasing fast paths inside `mpfc_apply_binary<mul_op>` and
  `mpfc_apply_binary<div_op>`.
- Reduced non-aliasing MPFC multiplication from three operation-internal
  `mpf_t` temporaries to one `mpf_t` temporary.
- Reduced non-aliasing MPFC division from four operation-internal `mpf_t`
  temporaries to two `mpf_t` temporaries.
- Kept the previous conservative temporary formulas for aliasing cases where
  `dest` is the same object as `lhs` or `rhs`.

Tests added:
- Extended `tests/test_mpfc_precision_policy.cpp` with allocation-count
  checks for preallocated `mul_dst = a * b` and `div_dst = a / b`.
- Added MPFC alias regression checks for `alias_mul = alias_mul * b` and
  `alias_div = alias_div / b`.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `tests/test_mpfc_precision_policy.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '632,700p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '70,130p' tests/test_mpfc_precision_policy.cpp`
- `cmake --build build -j --target test_mpfc_precision_policy test_mpfc_basic`
- `ctest --test-dir build -R 'test_mpfc_precision_policy|test_mpfc_basic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfc_precision_policy test_mpfc_basic`: PASS.
- `ctest --test-dir build -R 'test_mpfc_precision_policy|test_mpfc_basic' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- Non-aliasing MPFC `mul` and `div` still need temporary storage for the
  complex arithmetic formula itself. This phase minimizes, but does not
  eliminate, those formula temporaries.

Post-phase MPFC compound assignment direct evaluation:
DONE

Implemented features:
- Added `mpfc_compound_assign<Op>` mirroring the MPF/MPFR compound-assignment
  pattern.
- Reworked `mpfc_class` `operator+=`, `operator-=`, `operator*=`, and
  `operator/=` to avoid `lhs = lhs op rhs` expression assignment and the
  associated temporary `mpfc_class` plus swap.
- `+=` and `-=` with an `mpfc_class` right-hand side now apply directly
  in-place with zero allocation.
- `*=` and `/=` now call the alias-safe `mpfc_apply_binary` path directly;
  they still use formula temporaries because the destination is also the
  left operand.

Tests added:
- Extended `tests/test_mpfc_precision_policy.cpp` with allocation-count
  checks for `+=`, `-=`, `*=`, and `/=`.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `tests/test_mpfc_precision_policy.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "operator\\+=|operator-=|operator\\*=|operator/=|mpfc_compound|mpfr_compound|mpf_compound" include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_mpfc_precision_policy.cpp tests/test_compound_assign.cpp`
- `sed -n '780,875p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '1660,1730p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `cmake --build build -j --target test_mpfc_precision_policy test_mpfc_basic test_compound_assign`
- `ctest --test-dir build -R 'test_mpfc_precision_policy|test_mpfc_basic|test_compound_assign' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfc_precision_policy test_mpfc_basic test_compound_assign`: PASS.
- `ctest --test-dir build -R 'test_mpfc_precision_policy|test_mpfc_basic|test_compound_assign' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- MPFC `*=` and `/=` necessarily take the alias-safe formula path because the
  destination is also the left operand. This removes the extra expression
  assignment object and swap, not the formula temporaries.

Post-phase MPC compound assignment direct evaluation:
DONE

Implemented features:
- Added `mpc_compound_assign<Op>` mirroring the MPF/MPFR compound-assignment
  pattern.
- Reworked `mpc_class` `operator+=`, `operator-=`, `operator*=`, and
  `operator/=` to avoid `lhs = lhs op rhs` expression assignment.
- `mpc_class` right-hand operands now call `mpc_add`, `mpc_sub`, `mpc_mul`,
  or `mpc_div` directly with `lhs.mpc_data()` as both destination and left
  operand.
- Expression and non-MPC right-hand operands are evaluated into one `mpc_t`
  temporary at the destination precision before applying the operation.

Tests added:
- Extended `tests/test_mpc_precision_policy.cpp` with value and precision
  checks for `+=`, `-=`, `*=`, `/=`, and expression RHS compound assignment.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `tests/test_mpc_precision_policy.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '120,210p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '360,455p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1,160p' tests/test_mpc_precision_policy.cpp`
- `cmake --build build -j --target test_mpc_precision_policy test_mpc_basic test_mpc_aliasing`
- `ctest --test-dir build -R 'test_mpc_precision_policy|test_mpc_basic|test_mpc_aliasing' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpc_precision_policy test_mpc_basic test_mpc_aliasing`: PASS.
- `ctest --test-dir build -R 'test_mpc_precision_policy|test_mpc_basic|test_mpc_aliasing' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- None.

Post-phase MPF/MPFR direct shift compound assignment:
DONE

Implemented features:
- Reworked `gmpxx::mpf_class` `operator<<=` and `operator>>=` to call the
  direct `mul_2exp` / `div_2exp` member helpers instead of assigning from a
  shift expression.
- Reworked `mpfrxx::mpfr_class` `operator<<=` and `operator>>=` to call
  `mpfr_mul_2ui` / `mpfr_div_2ui` directly with the current evaluation
  context and exponent range guard.
- Preserved destination precision for both MPF and MPFR shift compound
  assignment.

Tests added:
- Extended `tests/test_mpf_power_of_two_fusion.cpp` with value and precision
  checks for `<<=` and `>>=`.
- Extended `tests/test_mpf_scalar_alloc_count.cpp` and
  `tests/test_mpfr_scalar_alloc_count.cpp` with zero-allocation checks for
  `<<=` and `>>=`.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/test_mpf_power_of_two_fusion.cpp`
- `tests/test_mpf_scalar_alloc_count.cpp`
- `tests/test_mpfr_scalar_alloc_count.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "shl_op|shr_op|operator<<=|operator>>=|mul_2exp|div_2exp|mpf_apply_binary|mpfr_apply_binary" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpf_power_of_two_fusion.cpp tests/test_mpfr_power_of_two_fusion.cpp tests/test_mpf_precision_policy.cpp tests/test_mpfr_precision_policy.cpp`
- `sed -n '1500,1610p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1950,1975p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1720,1835p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '2178,2205p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `cmake --build build -j --target test_mpf_power_of_two_fusion test_mpfr_power_of_two_fusion test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count`
- `ctest --test-dir build -R 'test_mpf_power_of_two_fusion|test_mpfr_power_of_two_fusion|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpf_power_of_two_fusion test_mpfr_power_of_two_fusion test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count`: PASS.
- `ctest --test-dir build -R 'test_mpf_power_of_two_fusion|test_mpfr_power_of_two_fusion|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count' --output-on-failure`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- None.

Post-phase MPF pow zero semantics:
DONE

Implemented features:
- Changed `gmpxx::pow(mpf_class(0), mpf_class(0))` to return `1`,
  matching `std::pow(0, 0)` and the IEEE-style real pow convention.
- Kept `gmpxx::pow(mpf_class(0), negative)` as `std::domain_error` because
  GMP `mpf_class` has no infinity representation to return.
- Clarified the negative zero-base exception message to describe the MPF
  infinity limitation rather than the `0^0` case.

Tests added:
- Added focused MPF coverage for `pow(0, 0) == 1` and `pow(0, positive) == 0`.
- Added focused MPF coverage that `pow(0, negative)` remains a domain error.

Tests updated:
- `tests/test_mpf_transcendent_functions.cpp`
- `tests/test_exception_support.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1258,1285p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '1285,1315p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '710,750p' tests/test_mpf_transcendent_functions.cpp`
- `sed -n '1120,1160p' tests/test_mpf_transcendent_functions.cpp`
- `cmake --build build -j --target test_mpf_transcendent_functions`
- `ctest --test-dir build -R test_mpf_transcendent_functions --output-on-failure`
- `ctest --test-dir build --output-on-failure`
- `cmake --build build -j --target test_exception_support`
- `ctest --test-dir build -R 'test_exception_support|test_mpf_transcendent_functions' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j --target test_mpf_transcendent_functions`: PASS.
- `ctest --test-dir build -R test_mpf_transcendent_functions --output-on-failure`: PASS, 1/1 test passed.
- Initial `ctest --test-dir build --output-on-failure`: FAIL because
  `tests/test_exception_support.cpp` still expected `pow(0, 0)` to throw;
  the stale expectation was updated.
- `cmake --build build -j --target test_exception_support`: PASS.
- `ctest --test-dir build -R 'test_exception_support|test_mpf_transcendent_functions' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.
- `git diff --check`: PASS.

Known issues:
- `pow(0, negative)` intentionally differs from `std::pow`, which can return
  `+inf`; GMP `mpf_class` cannot represent infinities.

Post-phase MPFC/MPC component accessor policy:
DONE

Implemented features:
- Rechecked the A5 component-accessor concern for MPFC and MPC.
- Changed `gmpxx::mpfc_class::real()` and `gmpxx::mpfc_class::imag()` so
  public access returns const component references only.  External code can
  no longer write `z.real() = ...` or mutate component precision through
  `z.real().set_prec(...)`.
- Kept explicit `mpfc_class::real(const mpf_class&)` and
  `mpfc_class::imag(const mpf_class&)` setters as the public mutation path.
  These setters preserve the destination component precision through
  `mpf_class` assignment.
- Added private friend helpers for the MPFC implementation so expression
  evaluation can still update component values without exposing mutable
  component references as public API.
- Rechecked `mpfrxx::mpc_class`: it does not expose member `real()` /
  `imag()` component references.  MPC still exposes raw `mpc_data()` for
  low-level interop, but the high-level component API is already value-return
  helpers (`mpfrxx::real(z)`, `mpfrxx::imag(z)`) and `real_to_double()` /
  `imag_to_double()`.

Tests added:
- Added static checks to `tests/test_mpfc_basic.cpp` proving
  `mpfc_class::real()` is not assignable, `real().set_prec(...)` is not
  available, and `imag()` is not assignable.
- Added a static check to `tests/test_mpc_basic.cpp` proving `mpc_class`
  does not expose a member `real()` accessor.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `tests/test_mpfc_basic.cpp`
- `tests/test_mpc_basic.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "real\\(\\)|imag\\(\\)|real\\([^)]|imag\\([^)]|real_to_double|imag_to_double|mpc_realref|mpc_imagref" include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp tests examples`
- `sed -n '120,210p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '120,185p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `rg -n "mpf_class& operator=|operator=\\(const mpf_class|void set_prec|set_prec|mpf_set_prec|mpf_init2|mpf_set\\(" include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '300,380p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `rg -n "\\.real\\(\\)\\s*=|\\.imag\\(\\)\\s*=|real\\(\\)\\.mpf_data|imag\\(\\)\\.mpf_data|mpfc_assign_scalar\\(dest\\.real|mpf_set_ui\\(dest\\.imag" include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '274,305p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1,70p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `sed -n '540,665p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `rg -n "\\.real\\(\\)\\s*=|\\.imag\\(\\)\\s*=|dest\\.real\\(\\)|dest\\.imag\\(\\)|mpfc_assign_scalar\\(dest\\.real|mpf_set_ui\\(dest\\.imag" include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `cmake --build build -j --target test_mpfc_basic test_mpc_basic`
- `ctest --test-dir build -R 'test_mpfc_basic|test_mpc_basic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfc_basic test_mpc_basic`: PASS.
- `ctest --test-dir build -R 'test_mpfc_basic|test_mpc_basic' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- `mpc_class::mpc_data()` still intentionally exposes raw mutable MPC data
  for low-level C API interop.  This is separate from the high-level
  `real()` / `imag()` accessor policy.

Post-phase MPC scalar equality and ordering policy:
DONE

Implemented features:
- Rechecked `mpfrxx::mpc_class` comparison parity with
  `gmpxx::mpfc_class`.  Before this phase, MPC arithmetic was implemented
  but equality comparison coverage was missing.
- Added symmetric equality and inequality overloads for:
  - `mpc_class` with `mpc_class`
  - `mpc_class` with `mpfr_class`
  - `mpc_class` with `mpz_class`
  - `mpc_class` with `mpq_class`
  - `mpc_class` with supported builtin scalar leaves
- The real-scalar comparisons use the same complex-number rule as MPFC:
  real components must compare equal and the imaginary component must be zero.
- Kept ordering operators (`<`, `<=`, `>`, `>=`) intentionally absent for
  `mpc_class`, including scalar mixed forms, because MPC is a complex type
  and no canonical total ordering is part of this wrapper API.

Tests added:
- Added MPC equality/inequality coverage to `tests/test_mpc_basic.cpp` for
  zero, `mpc_class`, `mpfr_class`, `mpz_class`, `mpq_class`, integer scalar,
  and double scalar cases, including reverse-order exact-type checks.
- Added static SFINAE checks proving `mpc_class < mpc_class`,
  `mpc_class < int`, and `int < mpc_class` are not available.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `tests/test_mpc_basic.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "operator==|operator!=|operator<|operator>|mpc_class" include/gmpfrxx_mkII/detail/mpc_impl.hpp tests/test_mpc* STATUS.md`
- `sed -n '1,220p' tests/test_mpc_basic.cpp`
- `sed -n '560,760p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1,110p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '260,385p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `rg -n "is_supported_mpfr_scalar|normalized_mpfr_scalar|operator==\\(.*mpfr_class|is_mpfr_comparison" include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '110,190p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '240,270p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '90,155p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '2020,2065p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `cmake --build build -j --target test_mpc_basic`
- `ctest --test-dir build -R test_mpc_basic --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `rg -n "operator==\\(const mpfc_class|operator!=\\(const mpfc_class|operator==\\(Scalar.*mpfc|operator!=\\(Scalar.*mpfc|operator==\\(const mpc_class|operator!=\\(const mpc_class|operator==\\(Scalar.*mpc|operator!=\\(Scalar.*mpc|operator<\\(.*mp[cf]c|operator>\\(.*mp[cf]c" include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `rg -n "has_less_than|mpfc_class\\(mpf_class\\(0|real_three ==|complex_three !=|mpc_class intentionally|mpfc_class intentionally" tests/test_mpfc_basic.cpp tests/test_mpc_basic.cpp`
- `sed -n '2040,2148p' STATUS.md`
- `cmake --build build -j --target test_mpfc_basic test_mpc_basic`
- `ctest --test-dir build -R 'test_mpfc_basic|test_mpc_basic' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j --target test_mpc_basic`: PASS.
- `ctest --test-dir build -R test_mpc_basic --output-on-failure`: PASS, 1/1 test passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.
- Final `cmake --build build -j --target test_mpfc_basic test_mpc_basic`: PASS.
- Final `ctest --test-dir build -R 'test_mpfc_basic|test_mpc_basic' --output-on-failure`: PASS, 2/2 tests passed.
- Final `cmake --build build -j`: PASS.
- Final `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.
- Final `git diff --check`: PASS.

Known issues:
- None.

Post-phase MPFC scalar equality and ordering policy:
DONE

Implemented features:
- Added symmetric equality and inequality overloads between
  `gmpxx::mpfc_class` and supported MPF scalar leaves.
- `mpfc_class == 0`, `0 == mpfc_class`, `mpfc_class != scalar`, and
  `scalar != mpfc_class` now compile and compare as a real scalar with zero
  imaginary part.
- Kept ordering operators (`<`, `<=`, `>`, `>=`) intentionally absent for
  `mpfc_class`, including scalar mixed forms, because MPFC is a complex type
  and no canonical total ordering is part of this wrapper API.

Tests added:
- Added scalar equality/inequality coverage to `tests/test_mpfc_basic.cpp`
  for integer zero, integer nonzero, and double scalar cases.
- Added static SFINAE checks proving `mpfc_class < mpfc_class`,
  `mpfc_class < int`, and `int < mpfc_class` are not available.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `tests/test_mpfc_basic.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "mpfc_class|operator==|operator!=|operator<|operator>" include/gmpfrxx_mkII/detail/mpfc_impl.hpp tests/test_mpfc* tests/test_comparisons.cpp STATUS.md`
- `sed -n '1,220p' tests/test_mpfc_basic.cpp`
- `sed -n '1,220p' tests/test_mpfc_math.cpp`
- `sed -n '330,470p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `rg -n "is_supported_mpf_scalar|operator==\\(const mpf_class|operator<\\(const mpf_class|is_mpf_comparison" include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1740,1838p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '50,110p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '180,225p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '150,190p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '220,315p' tests/test_mpfc_basic.cpp`
- `cmake --build build -j --target test_mpfc_basic`
- `ctest --test-dir build -R test_mpfc_basic --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfc_basic`: PASS.
- `ctest --test-dir build -R test_mpfc_basic --output-on-failure`: PASS, 1/1 test passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.

Known issues:
- None.

Post-phase MPF exponent-overflow guards:
DONE

Implemented features:
- Routed `compute_log` dynamic scaling through checked exponent helpers.
  The desired scaling exponent is checked before conversion to `mp_exp_t`,
  the `desired - x_exponent + 1` arithmetic is done in `mpz_t`, and the
  final `mpf_mul_2exp` / `mpf_div_2exp` scaling checks both shift magnitude
  and result exponent range.
- Added result-exponent guards to public `gmpxx::mpf_class::mul_2exp` and
  `gmpxx::mpf_class::div_2exp`.
- Added the same result-exponent guards to MPF expression shift evaluation
  for `<<` and `>>`.
- Added multiplication exponent guards to `pow_integer_unsigned`, so integer
  exponent `compute_pow` checks the result exponent before each multiply and
  squaring step.
- Added an `mp_exp_t` to `mpz_t` helper so guard arithmetic does not depend
  on `mp_exp_t` being exactly `long`.

Updated exponent-overflow status:

| Function/site | Status | Comment |
| --- | --- | --- |
| `compute_exp` | Done | Uses `round_to_nearest_mp_exp` to reject exponents outside `mp_exp_t`, then checks shift magnitude and result exponent before binary scaling. |
| `compute_log` | Done | Dynamic scaling now checks `mp_bitcnt_t` to `mp_exp_t` conversion, computes `desired - x_exponent + 1` in `mpz_t`, and validates the final result exponent before `mpf_mul_2exp` / `mpf_div_2exp`. |
| `mpf_class::mul_2exp` | Done | Public in-place left shift now rejects results whose exponent would exceed `mp_exp_t::max()` before calling GMP. |
| `mpf_class::div_2exp` | Done | Public in-place right shift now rejects results whose exponent would go below `mp_exp_t::min()` before calling GMP. |
| MPF expression shift `<<` | Done | Shift count is still checked through `zq_shift_count_from_mpz`; materialization now also checks the resulting exponent. |
| MPF expression shift `>>` | Done | Same as `<<`, but for right shift / exponent decrease before `mpf_div_2exp`. |
| `pow_integer_unsigned` | Done | Repeated multiplication and squaring now check the approximate product exponent before each multiply/square step. |
| Integer-exponent `compute_pow` | Done | Exact integer exponents route through `pow_integer_unsigned`, so the guarded multiply/square path is used. Negative integer exponents then take a reciprocal; that part is not a binary-scaling overflow site. |
| Non-integer `compute_pow` | Covered by callees | Uses `compute_log` and `compute_exp`; the dynamic scaling overflow paths are guarded in those callees. |
| `exp2` / `exp10` | Covered by callees | These route through `compute_exp`, so they share the exponent scaling guard. |
| Hyperbolic functions | Covered by callees | `sinh` / `cosh` / `tanh` route through `compute_exp` and ordinary arithmetic; no separate dynamic binary-scaling exponent remains. |
| Gamma / reciprocal gamma | Partially covered by callees | Spouge code uses `compute_exp` / `compute_pow` in the risky exponent paths. Remaining ordinary multiply/divide sites are not direct `mpf_*_2exp` scaling paths. |
| Fixed small binary shifts | No issue found | Half-pi, fixed `/2`, and similar constant shifts are not user-controlled dynamic exponent paths. |

Tests added:
- Added `mul_2exp` / `div_2exp` overflow checks to
  `tests/test_mpf_math_functions.cpp`.
- Added expression shift overflow checks for MPF `<<` and `>>`.
- Added `checked_log_scaling_exponent` normal and overflow cases to
  `tests/test_mpf_transcendent_functions.cpp`.
- Added a `log` scaling overflow regression using a value near the
  `mp_exp_t` exponent floor.
- Added integer-exponent `pow` overflow regression coverage.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `tests/test_mpf_math_functions.cpp`
- `tests/test_mpf_transcendent_functions.cpp`
- `STATUS.md`

Exact commands run:
- `tail -80 STATUS.md`
- `git diff -- include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/math_mpf.hpp tests/test_mpf_math_functions.cpp tests/test_mpf_transcendent_functions.cpp`
- `git status --short`
- `rg -n "checked_mp_exp_magnitude|mpz_init_set_si\\(|std::numeric_limits<mp_exp_t>|round_to_nearest_mp_exp|ensure_exp_scaling_exponent_fits" include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '560,760p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '80,140p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `rg -n "mpz_import|set_.*mpz|uintmax|intmax" include/gmpfrxx_mkII/detail/*.hpp`
- `sed -n '1180,1260p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '1260,1335p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '1,40p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `cmake --build build -j --target test_mpf_math_functions test_mpf_transcendent_functions`
- `git diff -- include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `ctest --test-dir build -R 'test_mpf_math_functions|test_mpf_transcendent_functions' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j --target test_mpf_math_functions test_mpf_transcendent_functions`: PASS.
- `ctest --test-dir build -R 'test_mpf_math_functions|test_mpf_transcendent_functions' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.
- `git diff --check`: PASS.

Known issues:
- None identified in the targeted exponent-scaling paths.

Post-phase MPF Taylor counter overflow recheck:
DONE

Implemented features:
- Rechecked the MPF Taylor loops called out in A2.  `exp_taylor_reduced`,
  `expm1_taylor_small`, and `sincos_taylor_small` already used
  `std::uint64_t` counters with checked multiplication before materializing
  denominators.
- Added a checked addition helper for Taylor counters and used it in the
  sine/cosine denominator path, so `(2*k) + 1` is guarded as explicitly as
  `(2*k) * (2*k+1)`.
- Extended the same checked-counter policy to the remaining MPF series loops
  that still used `unsigned long k` with denominator arithmetic:
  `log1p_taylor_small`, `log1p_atanh_series`, and `atan_taylor_small`.
- Extended the same guard style to theta/log2 shift-count updates, replacing
  raw `q_exponent * (2*k + c)` arithmetic with checked `std::uint64_t`
  intermediate arithmetic followed by an `mp_bitcnt_t` range check.
- Rechecked 32-bit integer-width coverage.  MPF expression scalar leaves
  normalize signed integral scalars to `std::int64_t` and unsigned integral
  scalars to `std::uint64_t`, with GMP `mpz` fallback when `long`/`unsigned
  long` is narrower than the value.  The remaining theta/log2 scale conversion
  was moved from `unsigned long` to `make_u64`.

Tests added:
- Extended `tests/test_mpf_math_functions.cpp` with coverage for checked
  Taylor addition, odd-denominator construction, and shift-count overflow.
- Extended `tests/test_mpf_basic.cpp` with focused `std::int32_t::min()` and
  `std::uint32_t::max()` scalar and expression-scalar coverage.

Tests updated:
- `include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `tests/test_mpf_basic.cpp`
- `tests/test_mpf_math_functions.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "exp_taylor_reduced|expm1_taylor_small|sincos_taylor_small|make_ui\\(|make_u64|checked_.*increment|checked_.*product|uint64_t|unsigned long" include/gmpfrxx_mkII/detail/math_mpf.hpp tests/test_mpf_math_functions.cpp tests/test_mpf_transcendent_functions.cpp`
- `sed -n '60,110p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '695,875p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '465,515p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `sed -n '985,1010p' include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `rg -n "for \\(unsigned long k|2ul \\* k|make_ui\\([^,]*\\*[^,]*," include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `cmake --build build -j --target test_mpf_math_functions test_mpf_transcendent_functions`
- `ctest --test-dir build -R 'test_mpf_math_functions|test_mpf_transcendent_functions' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `rg -n "uint32|int32|unsigned long|signed long|static_cast<unsigned long>|static_cast<long>|make_ui\\(|mpf_set_ui|mpf_set_si|mpz_set_ui|mpz_set_si|mpq_set_ui|mpq_set_si" include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp tests`
- `rg -n "static_cast<unsigned long>\\(|for \\(unsigned long k|2ul \\* k|make_ui\\([^,]*\\*[^,]*," include/gmpfrxx_mkII/detail/math_mpf.hpp`
- `rg -n "int32_t|uint32_t|INT32|UINT32" tests/test_type_conversions.cpp tests/test_construction_copy.cpp tests/test_gmpxx_mkII.cpp tests/test_mpfrxx_mkII.cpp tests/test_mpf_math_functions.cpp`
- `rg -n "normalized_mpf_scalar|is_supported_mpf_scalar|scalar_leaf" include/gmpfrxx_mkII/detail/type_traits.hpp include/gmpfrxx_mkII/detail/expr.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `cmake --build build -j --target test_mpf_basic test_mpf_math_functions`
- `ctest --test-dir build -R 'test_mpf_basic|test_mpf_math_functions' --output-on-failure`

Pass/fail result:
- Initial scan found no remaining `unsigned long` counters in the original A2
  functions, but did find related `log1p`/`atan`/theta series arithmetic.
- Final `rg -n "for \\(unsigned long k|2ul \\* k|make_ui\\([^,]*\\*[^,]*," include/gmpfrxx_mkII/detail/math_mpf.hpp`: PASS, no matches.
- `cmake --build build -j --target test_mpf_math_functions test_mpf_transcendent_functions`: PASS.
- `ctest --test-dir build -R 'test_mpf_math_functions|test_mpf_transcendent_functions' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.
- `git diff --check`: PASS.
- 32-bit width scan: PASS after replacing the theta/log2 `q_exponent`
  materialization with `make_u64`; remaining `static_cast<unsigned long>`
  cases are bounded small `int` values in Spouge gamma terms.
- `cmake --build build -j --target test_mpf_basic test_mpf_math_functions`: PASS.
- `ctest --test-dir build -R 'test_mpf_basic|test_mpf_math_functions' --output-on-failure`: PASS, 2/2 tests passed.

Known issues:
- None.

Post-phase string constructor base-policy parity:
DONE

Implemented features:
- Aligned `gmpxx::mpz_class` string construction with string assignment by
  making constructor defaults use GMP base 0 auto-detection.  Inputs such as
  `"0x10"` and `"020"` now behave consistently across construction and
  assignment.
- Aligned `gmpxx::mpq_class` string construction with string assignment by
  making constructor defaults use GMP base 0 auto-detection for numerator and
  denominator strings.
- Aligned `mpfrxx::mpfr_class` string construction and throwing `set()` with
  MPFR base 0 parsing, matching existing string assignment behavior and
  accepting C99-style hexadecimal MPFR inputs such as `"0x1p+5"`.
- Restored `gmpxx::mpf_class` string assignment to the same decimal/default
  base policy as string construction.  This intentionally follows GMP MPF
  string parsing policy instead of adopting `mpz`/MPFR-style auto-base input.
- Rechecked upstream `t-iostream.cc`, `t-istream.cc`, and `t-ostream.cc`.
  The stream tests exercise basefield-controlled formatted I/O separately
  from string constructor/assignment defaults, so stream extraction/insertion
  behavior was left unchanged.

Tests added:
- Added constructor/assignment base-policy regression coverage to
  `tests/test_zq_string_io.cpp`, `tests/test_mpf_string_io.cpp`, and
  `tests/test_mpfr_string_io.cpp`.

Tests updated:
- `include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/test_zq_string_io.cpp`
- `tests/test_mpf_string_io.cpp`
- `tests/test_mpfr_string_io.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "0x|base|set_str|istream|ostream|mpf|mpz|mpq" /home/docker/gmpxx_mkII/cxx/t-iostream.cc /home/docker/gmpxx_mkII/cxx/t-istream.cc /home/docker/gmpxx_mkII/cxx/t-ostream.cc`
- `rg -n "class mpz_class|mpz_class\\(|operator=\\(const char|set\\(const char|set_str\\(|class mpq_class|mpq_class\\(" include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `rg -n "mpf_class\\(|operator=\\(const char|set\\(const char|set_str\\(" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `cmake --build build -j --target test_zq_string_io test_mpf_string_io test_mpfr_string_io`
- `ctest --test-dir build -R 'test_zq_string_io|test_mpf_string_io|test_mpfr_string_io' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j --target test_zq_string_io test_mpf_string_io test_mpfr_string_io`: PASS.
- `ctest --test-dir build -R 'test_zq_string_io|test_mpf_string_io|test_mpfr_string_io' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 140/140 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase MPFR Rdot benchmarks:
DONE

Implemented features:
- Added `benchmarks/mpfr/00_Rdot` as the MPFR counterpart of
  `benchmarks/gmp/00_Rdot`.
- Ported the native C, native OpenMP, kernel 01-04, and kernel OpenMP 01-02
  Rdot benchmark variants to `mpfrxx::mpfr_class`.
- Kept the source shape close to the GMP benchmark and used
  `using namespace mpfrxx;`.
- Wired the MPFR Rdot benchmarks into `benchmarks/CMakeLists.txt` through
  `mpfrxx_mkII::mpfrxx_mkII`.
- Preserved command-line precision handling by setting both MPFR's C default
  precision and the wrapper-owned MPFR default precision.

Tests added:
- No CTest test was added; this phase adds benchmark executables.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/mpfr/00_Rdot/*`
- `STATUS.md`

Exact commands run:
- `find benchmarks/gmp/00_Rdot -maxdepth 1 -type f | sort`
- `sed -n '1,220p' benchmarks/CMakeLists.txt`
- `rg -n "set_default_precision|default_prec|default_rounding|mpfr_class\\(" include tests benchmarks -g'*.hpp' -g'*.cpp'`
- `sed -n '1,180p' benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `sed -n '1,180p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp`
- `sed -n '1,180p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04.cpp`
- `rg -n "mpfr_class\\(.*mpfr_t|mpfr_class\\(const mpfr|operator=.*mpfr_t|get_fr_urandomb|get_fr_uniform" include tests`
- `sed -n '1,160p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02.cpp`
- `sed -n '1,170p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_02.cpp`
- `mkdir -p benchmarks/mpfr/00_Rdot`
- `cp benchmarks/gmp/00_Rdot/... benchmarks/mpfr/00_Rdot/...`
- `perl -0pi ... benchmarks/mpfr/00_Rdot/*.cpp benchmarks/mpfr/00_Rdot/Rdot.hpp`
- `rg -n "mpf_|gmp_printf|benchmark_mpf|USE_ORIGINAL|#if|#endif|gmpxx|mpf_class|mpf_t|get_mpf|mpfr_set_default_prec" benchmarks/mpfr/00_Rdot`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target Rdot_mpfr_C_native_01 Rdot_mpfr_C_native_openmp_01 Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_02_mkII Rdot_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_04_mkII Rdot_mpfr_kernel_04_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_openmp_01_mkII Rdot_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rdot_mpfr_kernel_openmp_02_mkII Rdot_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH`
- `for exe in build/benchmarks/mpfr/00_Rdot/Rdot_mpfr_*; do "$exe" 12 128 | tail -n 1; done`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- MPFR Rdot benchmark target build: PASS.
- MPFR Rdot smoke run with `N=12`, `precision=128`: PASS, all checked
  variants reported `OK`.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- No MPFR operation-counter instrumentation was added in this phase; the
  existing operation counter remains MPF-specific.

Post-phase MPFR Rdot operation counters:
DONE

Implemented features:
- Added `benchmarks/common/mpfr_operation_counter.hpp`, mirroring the MPF
  benchmark counter technique.
- The MPFR counter wraps `mpfr_init`, `mpfr_init2`, `mpfr_clear`,
  `mpfr_add`, and `mpfr_mul` when
  `GMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS` is enabled.
- Added `MPFR_KERNEL_COUNTS` / `MPFR_OPERATION_COUNTS` output with
  `init`, `init2`, `total_init`, `clear`, `add`, and `mul` fields.
- Wired the counter into all `benchmarks/mpfr/00_Rdot` native, kernel, and
  OpenMP variants using the same include-before-wrapper-header approach as
  the GMP benchmark counter.
- Added the CMake option
  `GMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS`.

Tests added:
- No CTest test was added; this phase adds optional benchmark
  instrumentation.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/mpfr_operation_counter.hpp`
- `benchmarks/mpfr/00_Rdot/*`
- `STATUS.md`

Exact commands run:
- `sed -n '1,240p' benchmarks/common/mpf_operation_counter.hpp`
- `sed -n '1,150p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp`
- `rg -n "COUNT_MPF|mpf_operation_counter|begin_kernel|print_kernel" benchmarks`
- `cp benchmarks/common/mpf_operation_counter.hpp benchmarks/common/mpfr_operation_counter.hpp`
- `perl -0pi ... benchmarks/common/mpfr_operation_counter.hpp`
- `sed -n '1,230p' benchmarks/common/mpfr_operation_counter.hpp`
- `perl -0pi ... benchmarks/mpfr/00_Rdot/*.cpp`
- `rg -n "mpfr_operation_counter|begin_kernel|print_kernel|auto start|auto end" benchmarks/mpfr/00_Rdot/*.cpp`
- `cmake -S . -B build_mpfr_ops_count -DCMAKE_BUILD_TYPE=Debug -DGMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS=ON`
- `cmake --build build_mpfr_ops_count -j --target Rdot_mpfr_C_native_01 Rdot_mpfr_C_native_openmp_01 Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_02_mkII Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_04_mkII Rdot_mpfr_kernel_openmp_01_mkII Rdot_mpfr_kernel_openmp_02_mkII`
- `for exe in build_mpfr_ops_count/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01 build_mpfr_ops_count/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII build_mpfr_ops_count/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_02_mkII build_mpfr_ops_count/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03_mkII build_mpfr_ops_count/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_04_mkII; do "$exe" 12 128 | rg "MPFR_KERNEL_COUNTS|DIFF"; done`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target Rdot_mpfr_C_native_01 Rdot_mpfr_C_native_openmp_01 Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_02_mkII Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_04_mkII Rdot_mpfr_kernel_openmp_01_mkII Rdot_mpfr_kernel_openmp_02_mkII`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Counter-enabled MPFR Rdot target build: PASS.
- Counter-enabled smoke run with `N=12`, `precision=128`: PASS; sampled
  variants emitted `MPFR_KERNEL_COUNTS` and reported `DIFF ... OK`.
- Normal MPFR Rdot target build: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- The MPFR counter currently covers Rdot benchmark instrumentation only; other
  MPFR benchmark families can opt in when they are added.

Post-phase MPFR compound assignment temporary reduction:
DONE

Implemented features:
- Added `mpfr_compound_assign`, mirroring the MPF compound-assignment fast
  path.
- `mpfr_class::operator+=`, `operator-=`, `operator*=`, and `operator/=`
  now evaluate compound assignment directly instead of materializing
  `lhs op rhs` through ordinary assignment.
- Direct `mpfr_class` RHS operands are applied without an MPFR temporary.
- Expression RHS operands are evaluated into a single destination-precision
  temporary before applying the compound operation.
- This reduces MPFR Rdot benchmark kernel counts:
  - `Rdot_mpfr_kernel_01_mkII 100 512`: `init2` from 201 to 101.
  - `Rdot_mpfr_kernel_02_mkII 100 512`: `init2` from 301 to 101.
  - `Rdot_mpfr_kernel_03_mkII 100 512`: `init2` from 202 to 2.
  - `Rdot_mpfr_kernel_04_mkII 100 512`: `init2` from 402 to 2.

Tests added:
- Added MPFR compound-assignment expression RHS regression coverage for
  `a += b * c`, `a -= b * c`, `a *= b * c`, and `a /= b * c`.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/test_mpfr_compound_assign.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "operator\\+=|operator\\*=|mpfr_evaluate|evaluate_binary|compound|assign" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/expr.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '360,620p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '360,640p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1560,1825p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1620,1715p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '2040,2165p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `cmake --build build_mpfr_ops_count -j --target Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_02_mkII Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_04_mkII test_mpfr_compound_assign test_mpfr_numeric_equivalence`
- `for exe in Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_02_mkII Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_04_mkII; do echo "$exe"; ./build_mpfr_ops_count/benchmarks/mpfr/00_Rdot/$exe 100 512 | rg "MPFR_KERNEL_COUNTS|DIFF"; done`
- `sed -n '1,230p' tests/test_mpfr_compound_assign.cpp`
- `cmake --build build -j --target test_mpfr_compound_assign test_mpfr_numeric_equivalence Rdot_mpfr_kernel_01_mkII Rdot_mpfr_kernel_02_mkII Rdot_mpfr_kernel_03_mkII Rdot_mpfr_kernel_04_mkII`
- `ctest --test-dir build -R 'test_mpfr_compound_assign|test_mpfr_numeric_equivalence' --output-on-failure`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Counter-enabled MPFR Rdot target build: PASS.
- Counter-enabled MPFR Rdot measurement: PASS, all four sampled variants
  reported `DIFF ... OK`.
- Focused tests: PASS, 2/2 tests passed.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- `kernel_01` and `kernel_02` still allocate one temporary per loop for the
  `dx[i] * dy[i]` expression RHS. The manual compound style used by
  `kernel_03` and `kernel_04` is now reduced to two timed-kernel
  initializations.

Post-phase benchmark MPF counter rename:
DONE

Implemented features:
- Renamed the benchmark helper from `mpf_init_counter.hpp` to
  `mpf_operation_counter.hpp`.
- Renamed namespace `benchmark_mpf_init_counter` to
  `benchmark_mpf_operation_counter`.
- Renamed CMake option `GMPFRXX_MKII_BENCHMARK_COUNT_MPF_INIT` to
  `GMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS`.
- Renamed aggregate non-kernel output label from `MPF_INIT_COUNTS` to
  `MPF_OPERATION_COUNTS`; kernel output remains `MPF_KERNEL_COUNTS`.
- Updated Rdot counter includes and call sites to the new operation-counter
  naming.

Tests added:
- None; this is naming cleanup for benchmark instrumentation.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/mpf_operation_counter.hpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "mpf_init_counter|MPF_INIT_COUNTS|GMPFRXX_MKII_BENCHMARK_COUNT_MPF_INIT" benchmarks STATUS.md CMakeLists.txt cmake -g '*'`
- `sed -n '1,150p' benchmarks/CMakeLists.txt`
- `ls benchmarks/common`
- `mv benchmarks/common/mpf_init_counter.hpp benchmarks/common/mpf_operation_counter.hpp`
- `perl -0pi -e 's/mpf_init_counter\\.hpp/mpf_operation_counter.hpp/g; s/benchmark_mpf_init_counter/benchmark_mpf_operation_counter/g; s/GMPFRXX_MKII_BENCHMARK_COUNT_MPF_INIT/GMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS/g; s/MPF_INIT_COUNTS/MPF_OPERATION_COUNTS/g; s/GMPFRXX_MKII_BENCHMARK_MPF_INIT_COUNTER_HPP/GMPFRXX_MKII_BENCHMARK_MPF_OPERATION_COUNTER_HPP/g; s/Count mpf_init\\/mpf_init2\\/mpf_clear calls in instrumented GMP benchmarks/Count mpf_init, mpf_clear, and selected mpf arithmetic calls in instrumented GMP benchmarks/g' benchmarks/CMakeLists.txt benchmarks/common/mpf_operation_counter.hpp benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02.cpp benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03.cpp benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04.cpp STATUS.md`
- `rg -n "mpf_init_counter|benchmark_mpf_init_counter|GMPFRXX_MKII_BENCHMARK_COUNT_MPF_INIT|MPF_INIT_COUNTS|MFP_INIT|INIT_COUNTER" benchmarks STATUS.md`
- `sed -n '26,180p' benchmarks/common/mpf_operation_counter.hpp`
- `sed -n '26,54p' benchmarks/CMakeLists.txt`
- `perl -0pi -e 's/\\|mpf_init_counter//g' STATUS.md`
- `rg -n "mpf_init_counter|benchmark_mpf_init_counter|GMPFRXX_MKII_BENCHMARK_COUNT_MPF_INIT|MPF_INIT_COUNTS" benchmarks STATUS.md`
- `rg -n "mpf_operation_counter|benchmark_mpf_operation_counter|GMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS|MPF_OPERATION_COUNTS" benchmarks STATUS.md`
- `cmake -S . -B build_mpf_ops_count -DCMAKE_BUILD_TYPE=Debug -DGMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS=ON`
- `cmake --build build_mpf_ops_count -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_04_mkII Rdot_gmp_kernel_04_orig`
- `build_mpf_ops_count/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 1000 512`
- `build_mpf_ops_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04_mkII 1000 512`
- `build_mpf_ops_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04_orig 1000 512`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Old-name scan after cleanup: PASS, no matches.
- New-name scan: PASS.
- `cmake -S . -B build_mpf_ops_count -DCMAKE_BUILD_TYPE=Debug -DGMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS=ON`: PASS.
- Count-enabled benchmark build: PASS.
- `Rdot_gmp_C_native_01 1000 512`: PASS;
  `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
- `Rdot_gmp_kernel_04_mkII 1000 512`: PASS;
  `init=0 init2=2 total_init=2 clear=2 add=1000 mul=1000`.
- `Rdot_gmp_kernel_04_orig 1000 512`: PASS;
  `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- None.

Post-phase Rdot kernel 02 MPF operation counter check:
DONE

Implemented features:
- Added the optional benchmark kernel counter include and `begin_kernel` /
  `print_kernel` instrumentation to
  `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02.cpp`.
- Measured `Rdot_gmp_kernel_02` for both original `gmpxx.h` and mkII builds.

Tests added:
- None; this is benchmark instrumentation and measurement only.

Tests updated:
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '25,145p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02.cpp`
- `rg -n "Rdot_gmp_kernel_02" benchmarks/CMakeLists.txt benchmarks/gmp/CMakeLists.txt benchmarks -g 'CMakeLists.txt'`
- `cmake --build build_mpf_count -j --target Rdot_gmp_kernel_02_orig Rdot_gmp_kernel_02_mkII`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_orig 10 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII 10 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_orig 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII 1000 512`
- `cmake --build build -j --target Rdot_gmp_kernel_02_orig Rdot_gmp_kernel_02_mkII`
- `git diff --check`

Pass/fail result:
- Count-enabled Rdot kernel 02 build: PASS.
- `Rdot_gmp_kernel_02_orig 10 512`: PASS;
  `init=1 init2=10 total_init=11 clear=11 add=10 mul=10`.
- `Rdot_gmp_kernel_02_mkII 10 512`: PASS;
  `init=0 init2=11 total_init=11 clear=11 add=10 mul=10`.
- `Rdot_gmp_kernel_02_orig 1000 512`: PASS;
  `init=1 init2=1000 total_init=1001 clear=1001 add=1000 mul=1000`.
- `Rdot_gmp_kernel_02_mkII 1000 512`: PASS;
  `init=0 init2=1001 total_init=1001 clear=1001 add=1000 mul=1000`.
- Normal Rdot kernel 02 target build: PASS.
- `git diff --check`: PASS.

Known issues:
- Full CTest was not rerun for this benchmark-only instrumentation check.

Post-phase benchmark counter raw GMP wrapper safety:
DONE

Implemented features:
- Removed direct `__gmpf_*` calls from the benchmark MPF operation counter.
- Added `benchmark_mpf_operation_counter::raw::call_mpf_*` wrappers before the
  counter redefines the `mpf_*` macros.
- Counted wrappers now increment counters and call the saved raw wrappers
  instead of spelling GMP implementation symbols directly.

Tests added:
- None; this is benchmark instrumentation internals only.

Tests updated:
- `benchmarks/common/mpf_operation_counter.hpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,180p' benchmarks/common/mpf_operation_counter.hpp`
- `cmake --build build_mpf_count -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_04_mkII Rdot_gmp_kernel_04_orig`
- `rg -n "__gmpf_" benchmarks/common/mpf_operation_counter.hpp`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04_mkII 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04_orig 1000 512`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial raw wrapper attempt: FAIL because GMP's `mpf_*` macros expanded the
  raw wrapper function names. Fixed by naming the raw wrappers
  `call_mpf_*`.
- Count-enabled benchmark build after fix: PASS.
- `rg -n "__gmpf_" benchmarks/common/mpf_operation_counter.hpp`: PASS, no matches.
- `Rdot_gmp_C_native_01 1000 512`: PASS;
  `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
- `Rdot_gmp_kernel_04_mkII 1000 512`: PASS;
  `init=0 init2=2 total_init=2 clear=2 add=1000 mul=1000`.
- `Rdot_gmp_kernel_04_orig 1000 512`: PASS;
  `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- None.

Post-phase Rdot kernel 02/03/04 comparison:
DONE

Implemented features:
- Added optional benchmark kernel counter instrumentation to
  `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04.cpp`.
- Compared Rdot kernels 02, 03, and 04 for original `gmpxx.h` and mkII builds
  using kernel-only MPF operation counters.

Tests added:
- None; this is benchmark instrumentation and measurement only.

Tests updated:
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '25,145p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04.cpp`
- `rg -n "Rdot_gmp_kernel_04" benchmarks/CMakeLists.txt benchmarks -g 'CMakeLists.txt'`
- `cmake --build build_mpf_count -j --target Rdot_gmp_kernel_02_orig Rdot_gmp_kernel_02_mkII Rdot_gmp_kernel_03_orig Rdot_gmp_kernel_03_mkII Rdot_gmp_kernel_04_orig Rdot_gmp_kernel_04_mkII`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_orig 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_02_mkII 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_orig 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04_orig 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04_mkII 1000 512`
- `cmake --build build -j --target Rdot_gmp_kernel_04_orig Rdot_gmp_kernel_04_mkII`
- `git diff --check`

Pass/fail result:
- Count-enabled Rdot kernel 02/03/04 build: PASS.
- `Rdot_gmp_kernel_02_orig 1000 512`: PASS;
  `init=1 init2=1000 total_init=1001 clear=1001 add=1000 mul=1000`.
- `Rdot_gmp_kernel_02_mkII 1000 512`: PASS;
  `init=0 init2=1001 total_init=1001 clear=1001 add=1000 mul=1000`.
- `Rdot_gmp_kernel_03_orig 1000 512`: PASS;
  `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
- `Rdot_gmp_kernel_03_mkII 1000 512`: PASS;
  `init=0 init2=2 total_init=2 clear=2 add=1000 mul=1000`.
- `Rdot_gmp_kernel_04_orig 1000 512`: PASS;
  `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
- `Rdot_gmp_kernel_04_mkII 1000 512`: PASS;
  `init=0 init2=2 total_init=2 clear=2 add=1000 mul=1000`.
- Normal Rdot kernel 04 target build: PASS.
- `git diff --check`: PASS.

Known issues:
- Full CTest was not rerun for this benchmark-only instrumentation check.

Post-phase Rdot kernel 03 MPF operation counter check:
DONE

Implemented features:
- Added optional benchmark kernel counter instrumentation to
  `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03.cpp`.
- Measured `Rdot_gmp_kernel_03` for original `gmpxx.h` and mkII builds.

Tests added:
- None; this is benchmark instrumentation and measurement only.

Tests updated:
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '25,145p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03.cpp`
- `rg -n "Rdot_gmp_kernel_03" benchmarks/CMakeLists.txt benchmarks -g 'CMakeLists.txt'`
- `cmake --build build_mpf_count -j --target Rdot_gmp_kernel_03_orig Rdot_gmp_kernel_03_mkII`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_orig 10 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII 10 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_orig 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03_mkII 1000 512`
- `cmake --build build -j --target Rdot_gmp_kernel_03_orig Rdot_gmp_kernel_03_mkII`
- `git diff --check`

Pass/fail result:
- Count-enabled Rdot kernel 03 build: PASS.
- `Rdot_gmp_kernel_03_orig 10 512`: PASS;
  `init=2 init2=0 total_init=2 clear=2 add=10 mul=10`.
- `Rdot_gmp_kernel_03_mkII 10 512`: PASS;
  `init=0 init2=2 total_init=2 clear=2 add=10 mul=10`.
- `Rdot_gmp_kernel_03_orig 1000 512`: PASS;
  `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
- `Rdot_gmp_kernel_03_mkII 1000 512`: PASS;
  `init=0 init2=2 total_init=2 clear=2 add=1000 mul=1000`.
- Normal Rdot kernel 03 target build: PASS.
- `git diff --check`: PASS.

Known issues:
- Full CTest was not rerun for this benchmark-only instrumentation check.

Post-phase MPF compound assignment temporary reduction:
DONE

Implemented features:
- Optimized `gmpxx::mpf_class` compound assignment operators so the left-hand
  side is no longer materialized through `lhs = lhs op rhs`.
- Compound assignment now evaluates only the right-hand side to a temporary
  when needed, then applies the GMP C operation directly to the destination:
  `mpf_add/sub/mul/div(lhs, lhs, rhs_value)`.
- Direct `mpf_class` right-hand sides use no temporary; expression and scalar
  right-hand sides use a single destination-precision temporary.
- This reduces `temp += dx[i] * dy[i]` in the Rdot mkII benchmark from about
  `2N + 1` kernel `mpf_init*` calls to `N + 1`, matching `gmpxx.h` for the
  measured case.

Tests added:
- Added `dst += a * b` allocation coverage to `tests/test_mpf_alloc_count.cpp`.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `tests/test_mpf_alloc_count.cpp`
- `tests/test_mpf_scalar_alloc_count.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "operator\\+=|addmul|mpf_add|mpf_mul|assign_from_expression|eval.*mpf|compound" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/expr.hpp include/gmpfrxx_mkII/detail/eval_context.hpp tests -g '*.hpp' -g '*.cpp'`
- `sed -n '1380,1515p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1515,1895p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1,180p' tests/test_mpf_alloc_count.cpp`
- `sed -n '1,180p' tests/test_compound_assign.cpp`
- `cmake --build build -j --target test_mpf_alloc_count test_compound_assign`
- `cmake --build build_mpf_count -j --target Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_orig Rdot_gmp_C_native_01`
- `ctest --test-dir build -R 'test_mpf_alloc_count|test_compound_assign' --output-on-failure`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_orig 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII 1000 512`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `sed -n '68,108p' tests/test_mpf_scalar_alloc_count.cpp`
- `cmake --build build -j --target test_mpf_alloc_count test_mpf_scalar_alloc_count test_compound_assign`
- `ctest --test-dir build -R 'test_mpf_alloc_count|test_mpf_scalar_alloc_count|test_compound_assign' --output-on-failure`
- `cmake --build build -j`
- `cmake --build build_mpf_count -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_01_orig Rdot_gmp_kernel_01_mkII`
- `git diff --check`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_orig 1000 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII 1000 512`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial related tests: PASS.
- Initial Rdot count comparison after optimization:
  - native C: `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
  - orig `gmpxx.h`: `init=1 init2=1000 total_init=1001 clear=1001 add=1000 mul=1000`.
  - mkII: `init=0 init2=1001 total_init=1001 clear=1001 add=1000 mul=1000`.
- Initial full CTest: FAIL, `test_mpf_scalar_alloc_count` expected 2
  allocations for `dst += 5LL` but the optimized path now performs 1
  allocation.
- Updated `test_mpf_scalar_alloc_count` expectation for `dst += 5LL` to 1.
- Focused alloc/compound CTest: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- Final Rdot count comparison:
  - native C: `init=2 init2=0 total_init=2 clear=2 add=1000 mul=1000`.
  - orig `gmpxx.h`: `init=1 init2=1000 total_init=1001 clear=1001 add=1000 mul=1000`.
  - mkII: `init=0 init2=1001 total_init=1001 clear=1001 add=1000 mul=1000`.
- Final `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- None.

Post-phase benchmark kernel MPF operation counter:
DONE

Implemented features:
- Extended the optional benchmark MPF counter to wrap `mpf_add` and
  `mpf_mul` in addition to `mpf_init`, `mpf_init2`, and `mpf_clear`.
- Made benchmark counters atomic so OpenMP benchmark builds can be counted
  without data races in the counter itself.
- Added a kernel interval API:
  `benchmark_mpf_operation_counter::begin_kernel()` and
  `benchmark_mpf_operation_counter::print_kernel(label)`.
- Updated the instrumented Rdot native C and mkII kernel benchmarks to print
  kernel-only operation deltas with `MPF_KERNEL_COUNTS`.

Tests added:
- None; this is benchmark instrumentation only.

Tests updated:
- `benchmarks/common/mpf_operation_counter.hpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,240p' benchmarks/common/mpf_operation_counter.hpp`
- `rg -n "benchmark_mpf_operation_counter|MPF_OPERATION_COUNTS|mpf_add|mpf_mul" benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp benchmarks/CMakeLists.txt`
- `sed -n '25,45p' benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `sed -n '25,45p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp`
- `rg -n "mpf_add\\(|mpf_mul\\(|__gmpf_add|__gmpf_mul" include/gmpfrxx_mkII/detail benchmarks/gmp/00_Rdot -g '*.hpp' -g '*.cpp'`
- `cmake --build build_mpf_count -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_01_mkII`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 10 512`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII 10 512`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Count-enabled Rdot target build: PASS.
- `Rdot_gmp_C_native_01 10 512`: PASS; kernel counts were
  `init=2 init2=0 total_init=2 clear=2 add=10 mul=10`.
- `Rdot_gmp_kernel_01_mkII 10 512`: PASS; kernel counts were
  `init=0 init2=21 total_init=21 clear=21 add=10 mul=10`.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- None.

Post-phase benchmark strict-compatibility guard cleanup:
DONE

Implemented features:
- Removed benchmark-only `#if !defined ___GMPXX_STRICT_COMPATIBILITY___`
  guards around `using namespace gmpxx;`.
- Kept the benchmark source behavior unchanged for the local
  `gmpxx_mkII.h` path: `using namespace gmpxx;` is now unconditional when
  `ORIGINAL_GMPXX` is not selected.

Tests added:
- None; this is a benchmark source compatibility cleanup.

Tests updated:
- `benchmarks/gmp/00_Rdot/*.cpp`
- `benchmarks/gmp/01_Raxpy/*.cpp`
- `benchmarks/gmp/02_Rgemv/*.cpp`
- `benchmarks/gmp/03_Rgemm/*.cpp`
- `STATUS.md`

Exact commands run:
- `perl -0pi -e 's/#if !defined ___GMPXX_STRICT_COMPATIBILITY___\\nusing namespace gmpxx;\\n#endif/using namespace gmpxx;/g' benchmarks/gmp/00_Rdot/*.cpp benchmarks/gmp/01_Raxpy/*.cpp benchmarks/gmp/02_Rgemv/*.cpp benchmarks/gmp/03_Rgemm/*.cpp`
- `rg -n "___GMPXX_STRICT_COMPATIBILITY___" benchmarks`
- `rg -n "#include \\"gmpxx_mkII.h\\"|using namespace gmpxx;" benchmarks/gmp -g '*.cpp'`
- `git diff -- benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp benchmarks/gmp/03_Rgemm/Rgemm_gmp_kernel_openmp_03.cpp`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Guard removal command: PASS.
- Residual `___GMPXX_STRICT_COMPATIBILITY___` scan: PASS, no matches.
- Representative diff check: PASS.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- None.

Post-phase benchmark helper layout cleanup:
DONE

Implemented features:
- Removed obsolete micro benchmark sources:
  `benchmarks/bench_gmp_arithmetic.cpp` and
  `benchmarks/bench_mpfr_mpc_arithmetic.cpp`.
- Removed the corresponding CMake targets.
- Moved shared benchmark helpers to `benchmarks/common/`:
  `plot.py` and `run_benchmarks.sh`.
- Updated the runner default output directory to
  `benchmarks/gmp/results_raw/`.
- Added `.gitignore` entries for `build/` and
  `benchmarks/**/results_raw/`.
- Updated top-level and benchmark README instructions for the new helper
  location.

Tests added:
- None.

Tests updated:
- `.gitignore`
- `README.md`
- `benchmarks/CMakeLists.txt`
- `benchmarks/README.md`
- GMP benchmark README paths under `benchmarks/gmp/`
- `benchmarks/common/run_benchmarks.sh`
- `STATUS.md`

Exact commands run:
- `git rm benchmarks/bench_gmp_arithmetic.cpp benchmarks/bench_mpfr_mpc_arithmetic.cpp`
- `git mv benchmarks/plot.py benchmarks/common/plot.py`
- `git mv benchmarks/run_benchmarks.sh benchmarks/common/run_benchmarks.sh`
- `rg -n "run_benchmarks\\.sh|bench_gmp_arithmetic|bench_mpfr_mpc_arithmetic|benchmarks/results_raw|common/plot.py|plot.py" . -g '!build/**' -g '!benchmarks/**/results_raw/**'`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j`
- `bash benchmarks/common/run_benchmarks.sh build 128 8 8 4 4 3 3 3 /tmp/gmpfrxx_benchmark_common_smoke`
- `git check-ignore -v build benchmarks/results_raw benchmarks/gmp/results_raw`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j`: PASS.
- `bash benchmarks/common/run_benchmarks.sh build 128 8 8 4 4 3 3 3 /tmp/gmpfrxx_benchmark_common_smoke`: PASS; all reported correctness checks were `OK`.
- `git check-ignore -v build benchmarks/results_raw benchmarks/gmp/results_raw`: PASS for existing ignored `build/` and `benchmarks/results_raw/`; `benchmarks/gmp/results_raw/` is covered by the same ignore rule when created.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- Historical STATUS entries still mention the removed micro benchmark targets
  as part of earlier completed phases.

Post-phase Rdot MPF init counting:
DONE

Implemented features:
- Added optional benchmark-only `mpf_init` instrumentation via
  `benchmarks/common/mpf_operation_counter.hpp`.
- Added CMake option `GMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS`, default OFF, so
  normal benchmark timings are not polluted by counter increments.
- Instrumented `Rdot_gmp_C_native_01.cpp` and `Rdot_gmp_kernel_01.cpp` with
  `MPF_OPERATION_COUNTS` snapshots after setup, after the timed kernel, after the
  reference computation, after diff checking, and after manual cleanup.
- Fixed benchmark cleanup leaks in those files:
  `Rdot_gmp_C_native_01.cpp` now clears `_ans` and both files delete the
  `mpf_class` reference arrays.
- The counter wraps `mpf_init`, `mpf_init2`, and `mpf_clear`, and also catches
  inline `mpf_class` initialization paths when enabled before including
  `gmpxx_mkII.h` or upstream `gmpxx.h`.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/mpf_operation_counter.hpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp`
- `.gitignore`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake -S . -B build_mpf_count -DCMAKE_BUILD_TYPE=Debug -DGMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS=ON`
- `cmake --build build -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_01_mkII`
- `cmake --build build_mpf_count -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_orig`
- `build/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII 8 128`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 8 128`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_mkII 8 128`
- `build_mpf_count/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01_orig 8 128`
- `cmake --build build -j --target Rdot_gmp_C_native_01 Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_orig`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Normal `build/` output: PASS; no `MPF_OPERATION_COUNTS` lines are printed with
  counting disabled.
- Count-enabled `Rdot_gmp_C_native_01 8 128`: PASS; final snapshot
  after diff was `init=2 init2=71 total_init=73 clear=37`; after manual
  cleanup it was `init=2 init2=71 total_init=73 clear=71`.
- Count-enabled `Rdot_gmp_kernel_01_mkII 8 128`: PASS; final snapshot
  after diff was `init=0 init2=87 total_init=87 clear=50`; after manual
  cleanup it was `init=0 init2=87 total_init=87 clear=84`.
- Count-enabled `Rdot_gmp_kernel_01_orig 8 128`: PASS; final snapshot
  after diff was `init=19 init2=50 total_init=69 clear=34`; after manual
  cleanup it was `init=19 init2=50 total_init=69 clear=68`.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.

Known issues:
- Counter output is benchmark instrumentation only; it is intentionally kept
  behind `GMPFRXX_MKII_BENCHMARK_COUNT_MPF_OPERATIONS` to avoid changing normal
  benchmark timings.

Post-phase benchmark backend layout split:
DONE

Implemented features:
- Moved the existing MPLAPACK-derived GMP/MPF benchmark families under
  `benchmarks/gmp/`.
- Added `benchmarks/mpfr/README.md` as the reserved location for future
  MPFR/MPC benchmark families.
- Updated benchmark CMake source paths and runtime output directories to
  preserve target names while emitting executables under
  `build/benchmarks/gmp/...`.
- Updated `benchmarks/run_benchmarks.sh` and benchmark documentation for the
  new backend-first layout.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/README.md`
- `benchmarks/run_benchmarks.sh`
- GMP benchmark README paths under `benchmarks/gmp/`
- `STATUS.md`

Exact commands run:
- `git mv benchmarks/00_Rdot benchmarks/gmp/00_Rdot`
- `git mv benchmarks/01_Raxpy benchmarks/gmp/01_Raxpy`
- `git mv benchmarks/02_Rgemv benchmarks/gmp/02_Rgemv`
- `git mv benchmarks/03_Rgemm benchmarks/gmp/03_Rgemm`
- `rg -n "benchmarks/(00_Rdot|01_Raxpy|02_Rgemv|03_Rgemm)|build_bench_release/benchmarks/(00_Rdot|01_Raxpy|02_Rgemv|03_Rgemm)" benchmarks CMakeLists.txt STATUS.md -g '!benchmarks/results_raw/**'`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j`
- `bash benchmarks/run_benchmarks.sh build 128 8 8 4 4 3 3 3 /tmp/gmpfrxx_benchmark_layout_smoke`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j`: PASS.
- `bash benchmarks/run_benchmarks.sh build 128 8 8 4 4 3 3 3 /tmp/gmpfrxx_benchmark_layout_smoke`: PASS; executables were found under `build/benchmarks/gmp/...` and all reported correctness checks were `OK`.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.
- Stale-path scan excluding historical `benchmarks/results_raw/**`: PASS; no active benchmark path references the old top-level kernel directories.

Known issues:
- None.

Post-phase MPLAPACK-derived benchmark build:
DONE

Implemented features:
- Added build-tree namespace alias targets for `gmpxx_mkII`,
  `mpfrxx_mkII`, and `gmpfrxx_mkII`, so benchmark CMake can use the same
  target names as installed/package-style consumers.
- Restored the simple arithmetic benchmark targets while enabling the
  MPLAPACK-derived Rdot/Raxpy/Rgemv/Rgemm benchmark families.
- Kept `libgmpxx` usage local to `benchmarks/CMakeLists.txt` for the
  upstream `gmpxx.h` `orig` comparison variants; the library implementation
  still does not include `gmpxx.h` or link `libgmpxx`.
- Updated benchmark sources from the removed
  `gmpxx::gmpxx_defaults::set_initial_default_prec` call to
  `gmpxx::set_default_mpf_precision_bits`, guarded out for
  `USE_ORIGINAL_GMPXX`.
- Fixed shared-output OpenMP races in `Rgemv_gmp_kernel_openmp_02.cpp` and
  `Rgemm_gmp_C_native_openmp_02.cpp` that produced `Result NG` in smoke
  runs.  The final structure parallelizes Rgemv by output row `i` and Rgemm
  by output element `(i,j)`, so worker threads do not share-update the same
  output element.

Tests added:
- Tiny full-suite benchmark smoke via `benchmarks/run_benchmarks.sh`.

Tests updated:
- `CMakeLists.txt`
- `benchmarks/CMakeLists.txt`
- MPLAPACK-derived GMP benchmark sources under `benchmarks/gmp/00_Rdot`,
  `benchmarks/gmp/01_Raxpy`, `benchmarks/gmp/02_Rgemv`, and
  `benchmarks/gmp/03_Rgemm`.
- `STATUS.md`

Exact commands run:
- `ls benchmarks`
- `sed -n '1,260p' benchmarks/CMakeLists.txt`
- `rg -n "TODO|FIXME|gmpxx|mpfrxx|mpf_class|mpfr_class|mpc_class|mpfc_class|MPLAPACK|#include|main\\(" benchmarks`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target Rdot_gmp_kernel_01_mkII`
- `rg -n "gmpxx_defaults::set_initial_default_prec|set_initial_default_prec|set_default_mpf_precision_bits|mpf_set_default_prec" benchmarks`
- `cmake --build build -j --target bench_gmp_arithmetic bench_mpfr_mpc_arithmetic Rdot_gmp_C_native_01 Rdot_gmp_C_native_openmp_01 Rdot_gmp_kernel_01_orig Rdot_gmp_kernel_01_mkII Rdot_gmp_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_02_orig Rdot_gmp_kernel_02_mkII Rdot_gmp_kernel_02_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_03_orig Rdot_gmp_kernel_03_mkII Rdot_gmp_kernel_03_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_04_orig Rdot_gmp_kernel_04_mkII Rdot_gmp_kernel_04_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_01_orig Rdot_gmp_kernel_openmp_01_mkII Rdot_gmp_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rdot_gmp_kernel_openmp_02_orig Rdot_gmp_kernel_openmp_02_mkII Rdot_gmp_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH`
- `cmake --build build -j`
- `bash benchmarks/run_benchmarks.sh build 128 8 8 4 4 3 3 3 /tmp/gmpfrxx_benchmark_smoke`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `cmake --build build -j --target Rgemv_gmp_kernel_openmp_02_mkII Rgemv_gmp_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_gmp_kernel_openmp_02_orig Rgemm_gmp_C_native_openmp_02`
- `rg -n "critical|nowait|Compute alpha \\* A \\* B|Compute alpha \\* A \\* x" benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_02.cpp benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_openmp_02.cpp`

Pass/fail result:
- Initial configure/build exposed missing build-tree alias targets and
  obsolete benchmark precision setter usage.
- Initial tiny benchmark smoke built all variants but reported `Result NG`
  for `Rgemv_gmp_kernel_openmp_02_mkII` and
  `Rgemm_gmp_C_native_openmp_02` because of shared-output OpenMP races.
- `cmake --build build -j`: PASS after fixes.
- `bash benchmarks/run_benchmarks.sh build 128 8 8 4 4 3 3 3 /tmp/gmpfrxx_benchmark_smoke`: PASS; all reported benchmark correctness checks were `OK`.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.
- `git diff --check`: PASS.
- `rg -n "critical|nowait|Compute alpha \\* A \\* B|Compute alpha \\* A \\* x" benchmarks/gmp/02_Rgemv/Rgemv_gmp_kernel_openmp_02.cpp benchmarks/gmp/03_Rgemm/Rgemm_gmp_C_native_openmp_02.cpp`: PASS; no `critical` remains in the touched kernels.

Known issues:
- None.

Post-phase example15 hexadecimal digits port:
DONE

Implemented features:
- Ported `../gmpxx_mkII/examples/example16.cpp` as
  `examples/example15_mpf.cpp`, preserving the license and explanatory
  comment while adapting the include and MPF default precision setter.
- Added `examples/example15_mpfr.cpp` as the MPFR-backed counterpart with the
  same command-line surface and digit extraction algorithm.
- Registered both examples in `examples/CMakeLists.txt`.
- Added CTest coverage for both full-constant extraction and BBP-style
  offset extraction.

Tests added:
- `example15_mpf`
- `example15_mpfr`
- `example15_mpf_bbp`
- `example15_mpfr_bbp`

Tests updated:
- `examples/CMakeLists.txt`
- `STATUS.md`

Exact commands run:
- `ls examples ../gmpxx_mkII/examples`
- `sed -n '1,260p' ../gmpxx_mkII/examples/example16.cpp`
- `sed -n '261,620p' ../gmpxx_mkII/examples/example16.cpp`
- `sed -n '1,260p' examples/CMakeLists.txt`
- `rg -n "mpz_class\\(const mpfr_class|operator mpz|mpfr_get_z|class mpfr_class|const_log2|log_two|floor\\(" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp tests/test_mpfr_math.cpp examples/example*.cpp`
- `cp ../gmpxx_mkII/examples/example16.cpp examples/example15_mpf.cpp`
- `cp examples/example15_mpf.cpp examples/example15_mpfr.cpp`
- `rg -n "gmpxx::|mpf_class|mp_bitcnt_t|mpf_|get_mpf|floor|pow|log_two|const_pi|Example 16|gmpxx_mkII" examples/example15_mpfr.cpp`
- `rg -n "gmpxx::|mpf_class|mp_bitcnt_t|gmpxx_mkII|Example 16|log_two|const_log2|mpfr_get_z|hexadecimal_fraction" examples/example15_mpf.cpp examples/example15_mpfr.cpp`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target example15_mpf example15_mpfr`
- `ctest --test-dir build -R "example15" --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j --target example15_mpf example15_mpfr`: PASS.
- `ctest --test-dir build -R "example15" --output-on-failure`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 137/137 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase MPFC double-backed gamma removal:
DONE

Implemented features:
- Removed the `gmpxx::mpfc_class` complex `gamma` and `reciprocal_gamma`
  public helpers because the implementation converted through
  `std::complex<double>` and therefore could not honor MPF precision.
- Removed the internal MPFC `std::complex<double>` Lanczos helper and the
  conversion helpers used only by that path.

Tests added:
- None.

Tests updated:
- `tests/test_mpfc_math.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "gamma_lanczos|to_complex_double|from_complex_double|mpfc.*gamma|reciprocal_gamma\\(|gamma\\(" include/gmpfrxx_mkII/detail/math_mpfc.hpp tests examples STATUS.md`
- `sed -n '1,115p' include/gmpfrxx_mkII/detail/math_mpfc.hpp`
- `sed -n '140,180p' tests/test_mpfc_math.cpp`
- `rg -n "std::complex|complex<" include/gmpfrxx_mkII/detail/math_mpfc.hpp`
- `sed -n '450,490p' include/gmpfrxx_mkII/detail/math_mpfc.hpp`
- `rg -n "test_gamma_functions|test_.*\\(\\);" tests/test_mpfc_math.cpp`
- `cmake --build build -j --target test_mpfc_math`
- `ctest --test-dir build -R test_mpfc_math --output-on-failure`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfc_math`: PASS.
- `ctest --test-dir build -R test_mpfc_math --output-on-failure`: PASS, 1/1 test passed.
- `git diff --check`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 133/133 tests passed.

Known issues:
- MPFC complex gamma is now intentionally absent. Add it only with a
  high-precision implementation; do not restore the double-backed Lanczos
  path.

Post-phase example09 default-precision MPFC/MPC port:
DONE

Implemented features:
- Ported upstream `gmpxx_mkII/examples/example09.cpp` to
  `examples/example09_mpfc.cpp` using `gmpxx::mpf_class` and
  `gmpxx::mpfc_class`.
- Added `examples/example09_mpc.cpp` using `mpfrxx::mpfr_class` and
  `mpfrxx::mpc_class`.
- Removed the upstream multi-precision sweep and default-precision setters;
  both examples run at the active wrapper default precision.
- Replaced fixed `1e-35`/`1e-70` tolerances with
  `2^(-default_bits/2)`-style tolerances based on the active default
  precision.
- Registered both example09 variants in CMake and CTest.

Tests added:
- `example09_mpfc`
- `example09_mpc`

Tests updated:
- `examples/CMakeLists.txt`
- `examples/example09_mpfc.cpp`
- `examples/example09_mpc.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,380p' ../gmpxx_mkII/examples/example09.cpp`
- `cmake --build build -j`
- `./build/examples/example09_mpfc | head -n 20`
- `./build/examples/example09_mpc | head -n 20`
- `rg -n "set_default|set_initial_default|with_precision|bits_for_decimal_digits|mpf_class\\([^\\)]*, *(precision|prec)\\)|mpfr_class\\([^\\)]*, *(precision|prec)\\)|1e-35|1e-70" examples/example09_mpfc.cpp examples/example09_mpc.cpp`
- `ctest --test-dir build --output-on-failure -R "example09_mpfc|example09_mpc"`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial direct target build before CMake regeneration: FAIL because the new
  targets did not exist yet.
- `cmake --build build -j`: PASS.
- Example09 precision scan: PASS; no explicit precision setup or fixed
  upstream tolerance literals remain in the new example09 files.
- Focused example09 CTest: PASS, 2/2 tests passed.
- Full CTest: PASS, 123/123 tests passed.

Known issues:
- None.

Post-phase example09 license and explanation comments:
DONE

Implemented features:
- Expanded `examples/example09_mpfc.cpp` and `examples/example09_mpc.cpp`
  with the full BSD-style license block used by the neighboring examples.
- Added explanatory comments matching the style of `example08_*.cpp`,
  including the near-multiple-root conditioning summary and
  Wilkinson/Aberth-Ehrlich references.

Tests added:
- None; comment-only update.

Tests updated:
- `examples/example09_mpfc.cpp`
- `examples/example09_mpc.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,90p' examples/example08_mpfc.cpp`
- `sed -n '1,90p' examples/example08_mpc.cpp`
- `sed -n '1,90p' examples/example09_mpfc.cpp`
- `sed -n '1,90p' examples/example09_mpc.cpp`
- `git diff --check`

Pass/fail result:
- `git diff --check`: PASS.
- Build/CTest not rerun because the change is comment-only.

Known issues:
- None.

Post-phase example10 license comments:
DONE

Implemented features:
- Expanded `examples/example10_mpf.cpp` and `examples/example10_mpfr.cpp`
  with the full BSD-style license block used by the neighboring examples.

Tests added:
- None; comment-only update.

Tests updated:
- `examples/example10_mpf.cpp`
- `examples/example10_mpfr.cpp`
- `STATUS.md`

Exact commands run:
- `git diff --check`

Pass/fail result:
- `git diff --check`: PASS.
- Build/CTest not rerun because the change is comment-only.

Known issues:
- None.

Post-phase example11 MPF/MPFR port:
DONE

Implemented features:
- Added `examples/example11_mpf.cpp`, a `gmpxx::mpf_class` port of
  upstream `gmpxx_mkII/examples/example11.cpp`.
- Added `examples/example11_mpfr.cpp`, an `mpfrxx::mpfr_class` port with the
  same Muller's recurrence sample table and precision sweep.
- Preserved the upstream BSD-style license block and copied the explanatory
  Muller's recurrence comment, including the Muller/Rump/Goldberg references.
- Registered `example11_mpf` and `example11_mpfr` in CMake and CTest.
- No MPFC/MPC variants were added because this example is real-valued only.

Tests added:
- `example11_mpf`
- `example11_mpfr`

Tests updated:
- `examples/CMakeLists.txt`
- `examples/example11_mpf.cpp`
- `examples/example11_mpfr.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/examples/example11.cpp`
- `cmake --build build -j`
- `./build/examples/example11_mpf | head -n 35`
- `./build/examples/example11_mpfr | head -n 35`
- `ctest --test-dir build --output-on-failure -R "example11_mpf|example11_mpfr"`
- `git diff --check`

Pass/fail result:
- Initial `cmake --build build -j`: FAIL because the upstream
  `gmpxx::gmpxx_defaults::set_initial_default_prec` name is not public in
  this repo; replaced it with `gmpxx::set_default_mpf_precision_bits`.
- Final `cmake --build build -j`: PASS.
- Focused example11 CTest: PASS, 2/2 tests passed.
- `git diff --check`: PASS.

Known issues:
- Full CTest was not rerun for this phase; focused example11 CTest passed.

Post-phase example12 MPF/MPFR port:
DONE

Implemented features:
- Added `examples/example12_mpf.cpp`, a `gmpxx::mpf_class` port of
  upstream `gmpxx_mkII/examples/example12.cpp`.
- Added `examples/example12_mpfr.cpp`, an `mpfrxx::mpfr_class` port with the
  same integer-relation sweep at 100, 300, and 1000 working decimal digits.
- Preserved the upstream BSD-style license block and copied the explanatory
  integer-relation/PSLQ comment.
- Registered `example12_mpf` and `example12_mpfr` in CMake and CTest.
- No MPFC/MPC variants were added because this example is real-valued only.

Tests added:
- `example12_mpf`
- `example12_mpfr`

Tests updated:
- `examples/CMakeLists.txt`
- `examples/example12_mpf.cpp`
- `examples/example12_mpfr.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,360p' ../gmpxx_mkII/examples/example12.cpp`
- `cmake --build build -j`
- `./build/examples/example12_mpf`
- `./build/examples/example12_mpfr`
- `ctest --test-dir build --output-on-failure -R "example12_mpf|example12_mpfr"`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j`: PASS.
- `example12_mpf`: PASS; 100 digits fails, 300 digits recovers a candidate,
  and 1000 digits accepts the relation.
- `example12_mpfr`: PASS; 100 digits fails, 300 digits recovers a candidate,
  and 1000 digits accepts the relation.
- Focused example12 CTest: PASS, 2/2 tests passed.
- `git diff --check`: PASS.

Known issues:
- Full CTest was not rerun for this phase; focused example12 CTest passed.

Post-phase example13 MPF/MPFR port:
DONE

Implemented features:
- Added `examples/example13_mpf.cpp`, a `gmpxx::mpf_class` port of
  upstream `gmpxx_mkII/examples/example13.cpp`.
- Added `examples/example13_mpfr.cpp`, an `mpfrxx::mpfr_class` port with the
  same SIAM 100-Digit Challenge Problem 1 contour-transformed Simpson
  quadrature workflow and command-line options.
- Preserved the upstream BSD-style license block and explanatory problem
  comment.
- Registered `example13_mpf` and `example13_mpfr` in CMake and CTest.
- No MPFC/MPC variants were added because this port evaluates the transformed
  real integrals directly.

Tests added:
- `example13_mpf`
- `example13_mpfr`

Tests updated:
- `examples/CMakeLists.txt`
- `examples/example13_mpf.cpp`
- `examples/example13_mpfr.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,420p' ../gmpxx_mkII/examples/example13.cpp`
- `cmake --build build -j`
- `./build/examples/example13_mpf --digits 20 --first-panels 16 --max-panels 32 --reference-panels 64`
- `./build/examples/example13_mpfr --digits 20 --first-panels 16 --max-panels 32 --reference-panels 64`
- `ctest --test-dir build --output-on-failure -R "example13_mpf|example13_mpfr"`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j`: PASS.
- Short `example13_mpf` smoke run: PASS.
- Short `example13_mpfr` smoke run: PASS.
- Focused example13 CTest: PASS, 2/2 tests passed.
- `git diff --check`: PASS.

Known issues:
- Full CTest was not rerun for this phase; focused example13 CTest passed.

Post-phase example14 MPF/MPFR port:
DONE

Implemented features:
- Added `examples/example14_mpf.cpp`, a `gmpxx::mpf_class` port of
  upstream `gmpxx_mkII/examples/example14.cpp`.
- Added `examples/example14_mpfr.cpp`, an `mpfrxx::mpfr_class` port with the
  same NaCl Madelung theta-function lattice-sum transform and command-line
  options.
- Preserved the upstream BSD-style license block, explanatory Madelung
  constant comment, and the tail-correction note in the implementation.
- Registered `example14_mpf` and `example14_mpfr` in CMake and CTest.
- No MPFC/MPC variants were added because the theta-transform implementation
  is real-valued.

Tests added:
- `example14_mpf`
- `example14_mpfr`

Tests updated:
- `examples/CMakeLists.txt`
- `examples/example14_mpf.cpp`
- `examples/example14_mpfr.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,460p' ../gmpxx_mkII/examples/example14.cpp`
- `cmake --build build -j`
- `./build/examples/example14_mpf --digits 20 --first-panels 32 --max-panels 64 --reference-panels 128 --extent 6`
- `./build/examples/example14_mpfr --digits 20 --first-panels 32 --max-panels 64 --reference-panels 128 --extent 6`
- `ctest --test-dir build --output-on-failure -R "example14_mpf|example14_mpfr"`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j`: PASS.
- Short `example14_mpf` smoke run: PASS.
- Short `example14_mpfr` smoke run: PASS.
- Focused example14 CTest: PASS, 2/2 tests passed.
- `git diff --check`: PASS.

Known issues:
- Full CTest was not rerun for this phase; focused example14 CTest passed.

Post-phase examples default precision cleanup:
DONE

Implemented features:
- Removed unconditional explicit precision setup from example programs.
- Updated MPF/MPFR/MPFC/MPC examples to use ordinary constructors so default
  wrapper precision is used by default.
- Replaced fixed decimal tolerances in iterative examples with
  default-precision-derived thresholds of roughly `2^(-default_bits/2)`.
- Aligned `example04_mpf` and `example04_mpfr` printed fractional digits with
  the active default precision using `floor(default_bits * log10(2))`.
- Preserved command-line precision override support for Mandelbrot examples:
  `example07_mpfc --precision BITS` and `example07_mpc --precision BITS`
  still set wrapper defaults, while no option means no precision override.
- Added `example08_mpfc.cpp` and `example08_mpc.cpp` Wilkinson polynomial
  examples, both using default precision by default.

Tests added:
- Added CMake targets and CTest entries for `example08_mpfc` and
  `example08_mpc`.

Tests updated:
- `examples/CMakeLists.txt`
- `examples/example02_mpf.cpp`
- `examples/example02_mpfr.cpp`
- `examples/example02_mpfr_mpc.cpp`
- `examples/example03_mpf.cpp`
- `examples/example03_mpfc_math.cpp`
- `examples/example03_mpfr.cpp`
- `examples/example04_mpf.cpp`
- `examples/example04_mpfr.cpp`
- `examples/example05_mpf.cpp`
- `examples/example05_mpfr.cpp`
- `examples/example06_mpfc.cpp`
- `examples/example06_mpc.cpp`
- `examples/example07_mpfc.cpp`
- `examples/example07_mpc.cpp`
- `examples/example08_mpfc.cpp`
- `examples/example08_mpc.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "set_default|set_initial_default|with_precision|--precision|precision" examples`
- `cmake --build build -j`
- `rg -n "set_initial_default|with_precision|bits_for_decimal_digits|mpf_class\\([^\\)]*, *(precision|prec)\\)|mpfr_class\\([^\\)]*, *(precision|prec)\\)|const_pi\\((precision|prec)\\)|1e-50|1e-100|1e-45" examples`
- `git diff --check`
- `ctest --test-dir build --output-on-failure -R "example08|example07|example06|example05|example04|example03|example02"`
- `ctest --test-dir build --output-on-failure`
- `cmake --build build -j --target example04_mpf example04_mpfr`
- `./build/examples/example04_mpf | head -n 3`
- `./build/examples/example04_mpfr | head -n 3`
- `ctest --test-dir build --output-on-failure -R "example04_mpf|example04_mpfr"`

Pass/fail result:
- Initial build after adding `example08_mpc`: FAIL because MPC uses the free
  `mpfrxx::real(z)` helper rather than an `mpc_class::real()` member.
- `cmake --build build -j`: PASS after using `mpfrxx::real`.
- Precision/tolerance scan: PASS; remaining precision setters are only the
  command-line override paths in `example07_mpfc` and `example07_mpc`.
- `git diff --check`: PASS.
- Focused examples CTest: PASS, 16/16 tests passed.
- Full CTest: PASS, 121/121 tests passed.
- `cmake --build build -j --target example04_mpf example04_mpfr`: PASS.
- `ctest --test-dir build --output-on-failure -R "example04_mpf|example04_mpfr"`: PASS, 2/2 tests passed.

Known issues:
- None.

Post-phase upstream t-unary GMP adaptation:
DONE

Implemented features:
- Checked the upstream `gmpxx_mkII/cxx/t-unary.cc` unary-expression test
  against this repository's `gmpxx_mkII.h`.
- Covered upstream unary cases for `mpz_class`, `mpq_class`, and
  `mpf_class`, including unary plus, unary minus, nested unary minus, and
  `mpz_class` bitwise complement.
- No production-code changes were required.

Tests added:
- No permanent test file was added in this phase; the upstream test was
  compiled and executed as a temporary minimal-adaptation source.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-unary.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-unary.cc`
- `mkdir -p /tmp/t-unary-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-unary.cc /tmp/t-unary-gmpxx/t-unary-gmpxx-mkII.cc`
- `g++ -std=c++17 -Iinclude /tmp/t-unary-gmpxx/t-unary-gmpxx-mkII.cc -lgmp -o /tmp/t-unary-gmpxx/t-unary-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-unary.cc /tmp/t-unary-gmpxx/t-unary-gmpxx-mkII.cc`
- `/tmp/t-unary-gmpxx/t-unary-gmpxx-mkII`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial temporary compile without `using namespace gmpxx;`: FAIL, because
  this repository exposes GMP classes in namespace `gmpxx`.
- Temporary `t-unary-gmpxx-mkII.cc` compile after namespace adaptation: PASS.
- Temporary `t-unary-gmpxx-mkII` execution: PASS.
- Minimal adaptation diff: add `using namespace gmpxx;`.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- None.

Post-phase upstream t-ternary GMP adaptation:
DONE

Implemented features:
- Checked the upstream `gmpxx_mkII/cxx/t-ternary.cc` ternary-expression
  test against this repository's `gmpxx_mkII.h`.
- No production-code changes were required. The upstream file only contains
  active assertions for `mpz_class`; `check_mpq()` and `check_mpf()` remain
  empty upstream because there is no ternary MPQ/MPF operation in that test.

Tests added:
- No permanent test file was added in this phase; the upstream test was
  compiled and executed as a temporary minimal-adaptation source.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-ternary.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-ternary.cc`
- `sed -n '621,980p' ../gmpxx_mkII/cxx/t-ternary.cc`
- `mkdir -p /tmp/t-ternary-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-ternary.cc /tmp/t-ternary-gmpxx/t-ternary-gmpxx-mkII.cc`
- `g++ -std=c++17 -Iinclude /tmp/t-ternary-gmpxx/t-ternary-gmpxx-mkII.cc -lgmp -o /tmp/t-ternary-gmpxx/t-ternary-gmpxx-mkII`
- `/tmp/t-ternary-gmpxx/t-ternary-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-ternary.cc /tmp/t-ternary-gmpxx/t-ternary-gmpxx-mkII.cc`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Temporary `t-ternary-gmpxx-mkII.cc` compile: PASS.
- Temporary `t-ternary-gmpxx-mkII` execution: PASS.
- Minimal adaptation diff: harness-only include/assert/namespace changes.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- None.

Post-phase upstream t-misc GMP adaptation:
DONE

Implemented features:
- Added GMP-side `std::numeric_limits` specializations for
  `gmpxx::mpz_class`, `gmpxx::mpq_class`, and `gmpxx::mpf_class`.
- Kept the specialization intentionally narrow: `mpz_class` reports
  `is_integer == true`; `mpq_class` and `mpf_class` report
  `is_integer == false`.
- Left MPFR/MPC and MPFC `std::numeric_limits` unspecialized for the current
  public policy.
- Verified upstream `gmpxx_mkII/cxx/t-misc.cc` against this repo's
  `gmpxx_mkII.h` with minimal harness edits.

Tests added:
- None.

Tests updated:
- Updated `tests/test_abi_fingerprint.cpp` to reflect the GMP
  `std::numeric_limits` public API policy.

Exact commands run:
- `sed -n '1,280p' ../gmpxx_mkII/cxx/t-misc.cc`
- `sed -n '281,700p' ../gmpxx_mkII/cxx/t-misc.cc`
- `rg -n "numeric_limits|set_prec_raw|fits_sint_p|default_mpf_precision|mpf_get_default_prec" include tests STATUS.md`
- `sed -n '180,215p' tests/test_abi_fingerprint.cpp`
- `rg -n "namespace std|common_type" include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '1190,1262p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '536,610p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `g++ -std=c++17 -Iinclude /tmp/t-misc-gmpxx/t-misc-gmpxx-mkII.cc -lgmp -o /tmp/t-misc-gmpxx/t-misc-gmpxx-mkII`
- `/tmp/t-misc-gmpxx/t-misc-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-misc.cc /tmp/t-misc-gmpxx/t-misc-gmpxx-mkII.cc`
- `cmake --build build -j --target test_abi_fingerprint test_type_conversions test_mpf_precision_policy`
- `ctest --test-dir build --output-on-failure -R "test_abi_fingerprint|test_type_conversions|test_mpf_precision_policy"`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Minimal adapted upstream `t-misc.cc`: PASS.
- `cmake --build build -j --target test_abi_fingerprint test_type_conversions test_mpf_precision_policy`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_abi_fingerprint|test_type_conversions|test_mpf_precision_policy"`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- The upstream `t-misc.cc` assertion `f.get_prec() == mpf_get_default_prec()`
  is intentionally adapted to
  `f.get_prec() == gmpxx::default_mpf_precision_bits()`. This repo's MPF
  policy uses wrapper-owned default precision and does not call or mirror
  GMP's process-global `mpf_set_default_prec()` state.

Post-phase upstream t-mix GMP adaptation:
DONE

Implemented features:
- Made `gmpxx::mpf_class` construction from `gmpxx::mpz_class` and
  `gmpxx::mpq_class` implicit, matching upstream mixed conversion behavior
  used by `t-mix.cc`.
- Added `mpf_class` materialization from `mpz_class`/`mpq_class` expression
  nodes, so expressions such as `mpf_class(-z)` and implicit function
  argument conversion from `-z`/`-q` work without making arithmetic eager.
- Verified upstream `gmpxx_mkII/cxx/t-mix.cc` against this repo's
  `gmpxx_mkII.h` with minimal harness edits.

Tests added:
- Added construction-copy coverage for implicit `mpz_class`/`mpq_class` to
  `mpf_class` conversion.
- Added expression-node conversion coverage for unary `mpz_class` and
  `mpq_class` expressions materialized as `mpf_class`.

Tests updated:
- `tests/test_construction_copy.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-mix.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-mix.cc`
- `g++ -std=c++17 -Iinclude /tmp/t-mix-gmpxx/t-mix-gmpxx-mkII.cc -lgmp -o /tmp/t-mix-gmpxx/t-mix-gmpxx-mkII`
- `/tmp/t-mix-gmpxx/t-mix-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-mix.cc /tmp/t-mix-gmpxx/t-mix-gmpxx-mkII.cc`
- `cmake --build build -j --target test_construction_copy test_mixed_type_arithmetic test_gmpxx_mkII`
- `ctest --test-dir build --output-on-failure -R "test_construction_copy|test_mixed_type_arithmetic|test_gmpxx_mkII"`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Initial minimal adapted upstream `t-mix.cc`: FAIL before repo fix because
  `mpz_class`/`mpq_class` and their unary expression nodes did not implicitly
  convert to `mpf_class`.
- Minimal adapted upstream `t-mix.cc`: PASS after repo fix.
- `cmake --build build -j --target test_construction_copy test_mixed_type_arithmetic test_gmpxx_mkII`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_construction_copy|test_mixed_type_arithmetic|test_gmpxx_mkII"`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase upstream t-ops GMP adaptation:
DONE

Implemented features:
- Verified upstream `gmpxx_mkII/cxx/t-ops.cc` against this repo's
  `gmpxx_mkII.h`.
- No implementation changes were required for the upstream operator/function
  cases in `t-ops.cc`.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-ops.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-ops.cc`
- `sed -n '621,1040p' ../gmpxx_mkII/cxx/t-ops.cc`
- `g++ -std=c++17 -Iinclude /tmp/t-ops-gmpxx/t-ops-gmpxx-mkII.cc -lgmp -o /tmp/t-ops-gmpxx/t-ops-gmpxx-mkII`
- `/tmp/t-ops-gmpxx/t-ops-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-ops.cc /tmp/t-ops-gmpxx/t-ops-gmpxx-mkII.cc`
- `cmake --build build -j --target test_mpz_arithmetic test_mpq_arithmetic test_mpf_basic test_mixed_type_arithmetic test_gmpxx_mkII`
- `ctest --test-dir build --output-on-failure -R "test_mpz_arithmetic|test_mpq_arithmetic|test_mpf_basic|test_mixed_type_arithmetic|test_gmpxx_mkII"`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Minimal adapted upstream `t-ops.cc`: PASS.
- `cmake --build build -j --target test_mpz_arithmetic test_mpq_arithmetic test_mpf_basic test_mixed_type_arithmetic test_gmpxx_mkII`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_mpz_arithmetic|test_mpq_arithmetic|test_mpf_basic|test_mixed_type_arithmetic|test_gmpxx_mkII"`: PASS, 5/5 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- None.

Post-phase upstream t-ops2f GMP adaptation:
DONE

Implemented features:
- Verified upstream `gmpxx_mkII/cxx/t-ops2f.cc` against this repo's
  `gmpxx_mkII.h`.
- No implementation changes were required for the upstream MPF mixed
  operator/function cases in `t-ops2f.cc`.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-ops2f.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-ops2f.cc`
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-ops2.h`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-ops2.h`
- `g++ -std=c++17 -Iinclude -I/tmp/t-ops2f-gmpxx /tmp/t-ops2f-gmpxx/t-ops2f-gmpxx-mkII.cc -lgmp -o /tmp/t-ops2f-gmpxx/t-ops2f-gmpxx-mkII`
- `/tmp/t-ops2f-gmpxx/t-ops2f-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-ops2f.cc /tmp/t-ops2f-gmpxx/t-ops2f-gmpxx-mkII.cc`
- `diff -u ../gmpxx_mkII/cxx/t-ops2.h /tmp/t-ops2f-gmpxx/t-ops2.h`
- `cmake --build build -j --target test_mpf_basic test_mixed_type_arithmetic test_mixed_zq_mpf_promotion test_gmpxx_mkII`
- `ctest --test-dir build --output-on-failure -R "test_mpf_basic|test_mixed_type_arithmetic|test_mixed_zq_mpf_promotion|test_gmpxx_mkII"`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Minimal adapted upstream `t-ops2f.cc`: PASS.
- `diff -u ../gmpxx_mkII/cxx/t-ops2f.cc /tmp/t-ops2f-gmpxx/t-ops2f-gmpxx-mkII.cc`: PASS, no direct source diff.
- `cmake --build build -j --target test_mpf_basic test_mixed_type_arithmetic test_mixed_zq_mpf_promotion test_gmpxx_mkII`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_mpf_basic|test_mixed_type_arithmetic|test_mixed_zq_mpf_promotion|test_gmpxx_mkII"`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- The only minimal adaptation is in shared `t-ops2.h`: replace upstream
  local harness includes with `<gmpxx_mkII.h>`, `<cassert>`, `<cmath>`,
  `ASSERT_ALWAYS`, and `using namespace gmpxx;`.

Post-phase upstream t-ops2qf GMP adaptation:
DONE

Implemented features:
- Verified upstream `gmpxx_mkII/cxx/t-ops2qf.cc` against this repo's
  `gmpxx_mkII.h`.
- No implementation changes were required for the upstream MPQ/MPF mixed
  operator/function cases in `t-ops2qf.cc`.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-ops2qf.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-ops2qf.cc`
- `g++ -std=c++17 -Iinclude -I/tmp/t-ops2qf-gmpxx /tmp/t-ops2qf-gmpxx/t-ops2qf-gmpxx-mkII.cc -lgmp -o /tmp/t-ops2qf-gmpxx/t-ops2qf-gmpxx-mkII`
- `/tmp/t-ops2qf-gmpxx/t-ops2qf-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-ops2qf.cc /tmp/t-ops2qf-gmpxx/t-ops2qf-gmpxx-mkII.cc`
- `diff -u ../gmpxx_mkII/cxx/t-ops2.h /tmp/t-ops2qf-gmpxx/t-ops2.h`
- `cmake --build build -j --target test_mpq_arithmetic test_mpf_basic test_mixed_type_arithmetic test_mixed_zq_mpf_promotion test_gmpxx_mkII`
- `ctest --test-dir build --output-on-failure -R "test_mpq_arithmetic|test_mpf_basic|test_mixed_type_arithmetic|test_mixed_zq_mpf_promotion|test_gmpxx_mkII"`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial minimal adapted upstream `t-ops2qf.cc`: FAIL because upstream
  `mpf_set_default_prec()` does not affect this repo's wrapper-owned MPF
  default precision.
- Minimal adapted upstream `t-ops2qf.cc`: PASS after replacing the precision
  setup with `gmpxx::set_default_mpf_precision_bits(...)`.
- `cmake --build build -j --target test_mpq_arithmetic test_mpf_basic test_mixed_type_arithmetic test_mixed_zq_mpf_promotion test_gmpxx_mkII`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_mpq_arithmetic|test_mpf_basic|test_mixed_type_arithmetic|test_mixed_zq_mpf_promotion|test_gmpxx_mkII"`: PASS, 5/5 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- The shared `t-ops2.h` minimal adaptation replaces upstream local harness
  includes with `<gmpxx_mkII.h>`, `<cassert>`, `<cmath>`, `<limits>`,
  `ASSERT_ALWAYS`, and `using namespace gmpxx;`.
- The `t-ops2qf.cc` precision setup is intentionally adapted from
  `mpf_set_default_prec(...)` to `gmpxx::set_default_mpf_precision_bits(...)`
  because this repo's MPF class does not mirror GMP's process-global default
  precision.

Post-phase upstream t-ops2z GMP adaptation:
DONE

Implemented features:
- Verified upstream `gmpxx_mkII/cxx/t-ops2z.cc` against this repo's
  `gmpxx_mkII.h`.
- No implementation changes were required for the upstream MPZ operator,
  comparison, gcd/lcm, factorial, primorial, and fibonacci cases in
  `t-ops2z.cc`.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-ops2z.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-ops2z.cc`
- `g++ -std=c++17 -Iinclude -I/tmp/t-ops2z-gmpxx /tmp/t-ops2z-gmpxx/t-ops2z-gmpxx-mkII.cc -lgmp -o /tmp/t-ops2z-gmpxx/t-ops2z-gmpxx-mkII`
- `/tmp/t-ops2z-gmpxx/t-ops2z-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-ops2z.cc /tmp/t-ops2z-gmpxx/t-ops2z-gmpxx-mkII.cc`
- `diff -u ../gmpxx_mkII/cxx/t-ops2.h /tmp/t-ops2z-gmpxx/t-ops2.h`
- `cmake --build build -j --target test_mpz_arithmetic test_mpz_basic test_mixed_type_arithmetic test_gmpxx_mkII`
- `ctest --test-dir build --output-on-failure -R "test_mpz_arithmetic|test_mpz_basic|test_mixed_type_arithmetic|test_gmpxx_mkII"`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Minimal adapted upstream `t-ops2z.cc`: PASS.
- `diff -u ../gmpxx_mkII/cxx/t-ops2z.cc /tmp/t-ops2z-gmpxx/t-ops2z-gmpxx-mkII.cc`: PASS, no direct source diff.
- `cmake --build build -j --target test_mpz_arithmetic test_mpz_basic test_mixed_type_arithmetic test_gmpxx_mkII`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_mpz_arithmetic|test_mpz_basic|test_mixed_type_arithmetic|test_gmpxx_mkII"`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- The only minimal adaptation is in shared `t-ops2.h`: replace upstream local
  harness includes with `<gmpxx_mkII.h>`, `<cassert>`, `<cmath>`, `<limits>`,
  `ASSERT_ALWAYS`, and `using namespace gmpxx;`.

Post-phase upstream t-ops3 GMP adaptation:
DONE

Implemented features:
- Added `mpz_class` and `mpq_class` compound assignment support for
  `mpf_class` RHS values and MPF expression RHS values.
- The new compound assignment overloads materialize through `mpf_class` and
  assign back to the left-hand type; arithmetic operators themselves remain
  expression-template based.
- Verified upstream `gmpxx_mkII/cxx/t-ops3.cc` against this repo's
  `gmpxx_mkII.h` with minimal harness edits.

Tests added:
- Added `tests/test_compound_assign.cpp` coverage for `mpz_class` and
  `mpq_class` left-hand compound assignment with `mpf_class` RHS and unary MPF
  expression RHS.

Tests updated:
- `tests/test_compound_assign.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,280p' ../gmpxx_mkII/cxx/t-ops3.cc`
- `sed -n '281,700p' ../gmpxx_mkII/cxx/t-ops3.cc`
- `g++ -std=c++17 -Iinclude /tmp/t-ops3-gmpxx/t-ops3-gmpxx-mkII.cc -lgmp -o /tmp/t-ops3-gmpxx/t-ops3-gmpxx-mkII`
- `/tmp/t-ops3-gmpxx/t-ops3-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-ops3.cc /tmp/t-ops3-gmpxx/t-ops3-gmpxx-mkII.cc`
- `cmake --build build -j --target test_compound_assign test_mpz_arithmetic test_mpq_arithmetic test_mpf_basic test_mixed_type_arithmetic test_gmpxx_mkII`
- `ctest --test-dir build --output-on-failure -R "test_compound_assign|test_mpz_arithmetic|test_mpq_arithmetic|test_mpf_basic|test_mixed_type_arithmetic|test_gmpxx_mkII"`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial minimal adapted upstream `t-ops3.cc`: FAIL because `mpz_class` and
  `mpq_class` compound assignment did not accept `mpf_class` or MPF
  expression RHS values.
- Minimal adapted upstream `t-ops3.cc`: PASS after repo fix.
- `cmake --build build -j --target test_compound_assign test_mpz_arithmetic test_mpq_arithmetic test_mpf_basic test_mixed_type_arithmetic test_gmpxx_mkII`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_compound_assign|test_mpz_arithmetic|test_mpq_arithmetic|test_mpf_basic|test_mixed_type_arithmetic|test_gmpxx_mkII"`: PASS, 6/6 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- None.

Post-phase upstream t-ostream GMP adaptation:
DONE

Implemented features:
- Verified upstream `gmpxx_mkII/cxx/t-ostream.cc` against this repo's
  `gmpxx_mkII.h`.
- No implementation changes were required for the upstream GMP ostream
  formatting cases.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-ostream.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-ostream.cc`
- `g++ -std=c++17 -Iinclude /tmp/t-ostream-gmpxx/t-ostream-gmpxx-mkII.cc -lgmp -o /tmp/t-ostream-gmpxx/t-ostream-gmpxx-mkII`
- `/tmp/t-ostream-gmpxx/t-ostream-gmpxx-mkII`
- `/tmp/t-ostream-gmpxx/t-ostream-gmpxx-mkII -s`
- `diff -u ../gmpxx_mkII/cxx/t-ostream.cc /tmp/t-ostream-gmpxx/t-ostream-gmpxx-mkII.cc`
- `cmake --build build -j --target test_zq_string_io test_mpf_string_io test_gmpxx_mkII test_mpfc_io`
- `ctest --test-dir build --output-on-failure -R "test_zq_string_io|test_mpf_string_io|test_gmpxx_mkII|test_mpfc_io"`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Minimal adapted upstream `t-ostream.cc`: PASS.
- Minimal adapted upstream `t-ostream.cc -s`: PASS exit status; diagnostic
  differences were printed for standard `long`/`double` stream comparison
  cases where the C++ standard library formats zero/showpoint differently
  from GMP's expected wrapper output.
- `cmake --build build -j --target test_zq_string_io test_mpf_string_io test_gmpxx_mkII test_mpfc_io`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_zq_string_io|test_mpf_string_io|test_gmpxx_mkII|test_mpfc_io"`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- The minimal harness defines `MPZ_CHECK_FORMAT` as a no-op because it is an
  upstream internal limb-format assertion macro from `gmp-impl.h`, not part of
  this repo's public wrapper behavior.

Post-phase upstream t-prec GMP adaptation:
DONE

Implemented features:
- Verified upstream `gmpxx_mkII/cxx/t-prec.cc` against this repo's
  `gmpxx_mkII.h`.
- No implementation changes were required for the upstream MPF precision
  expression cases in `t-prec.cc`.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,280p' ../gmpxx_mkII/cxx/t-prec.cc`
- `sed -n '281,700p' ../gmpxx_mkII/cxx/t-prec.cc`
- `g++ -std=c++17 -Iinclude /tmp/t-prec-gmpxx/t-prec-gmpxx-mkII.cc -lgmp -o /tmp/t-prec-gmpxx/t-prec-gmpxx-mkII`
- `/tmp/t-prec-gmpxx/t-prec-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-prec.cc /tmp/t-prec-gmpxx/t-prec-gmpxx-mkII.cc`
- `cmake --build build -j --target test_mpf_precision_policy test_mpf_basic test_mixed_zq_mpf_promotion test_gmpxx_mkII`
- `ctest --test-dir build --output-on-failure -R "test_mpf_precision_policy|test_mpf_basic|test_mixed_zq_mpf_promotion|test_gmpxx_mkII"`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Minimal adapted upstream `t-prec.cc`: PASS.
- `cmake --build build -j --target test_mpf_precision_policy test_mpf_basic test_mixed_zq_mpf_promotion test_gmpxx_mkII`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_mpf_precision_policy|test_mpf_basic|test_mixed_zq_mpf_promotion|test_gmpxx_mkII"`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- None.

Post-phase upstream t-rand GMP adaptation:
DONE

Implemented features:
- Verified upstream `gmpxx_mkII/cxx/t-rand.cc` against this repo's
  `gmpxx_mkII.h`.
- No implementation changes were required for the upstream `gmp_randclass`,
  random `mpz_class`, or random `mpf_class` cases.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,280p' ../gmpxx_mkII/cxx/t-rand.cc`
- `sed -n '281,700p' ../gmpxx_mkII/cxx/t-rand.cc`
- `g++ -std=c++17 -Iinclude /tmp/t-rand-gmpxx/t-rand-gmpxx-mkII.cc -lgmp -o /tmp/t-rand-gmpxx/t-rand-gmpxx-mkII`
- `/tmp/t-rand-gmpxx/t-rand-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-rand.cc /tmp/t-rand-gmpxx/t-rand-gmpxx-mkII.cc`
- `cmake --build build -j --target test_random test_mpf_precision_policy test_gmpxx_mkII`
- `ctest --test-dir build --output-on-failure -R "test_random|test_mpf_precision_policy|test_gmpxx_mkII"`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial minimal adapted upstream `t-rand.cc`: FAIL because upstream
  `mpf_get_default_prec()` does not represent this repo's wrapper-owned MPF
  default precision.
- Minimal adapted upstream `t-rand.cc`: PASS after replacing
  `mpf_get_default_prec()` with `gmpxx::default_mpf_precision_bits()`.
- `cmake --build build -j --target test_random test_mpf_precision_policy test_gmpxx_mkII`: PASS.
- `ctest --test-dir build --output-on-failure -R "test_random|test_mpf_precision_policy|test_gmpxx_mkII"`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- The default precision assertions are intentionally adapted from GMP's
  process-global `mpf_get_default_prec()` to this repo's wrapper-owned
  `gmpxx::default_mpf_precision_bits()`.

Post-phase upstream t-cxx11 GMP parity check:
DONE

Implemented features:
- Re-exported `gmpxx::literals::operator"" _mpz`,
  `gmpxx::literals::operator"" _mpq`, and
  `gmpxx::literals::operator"" _mpf` into namespace `gmpxx`, so
  `using namespace gmpxx;` exposes GMP numeric/string UDLs.
- Made `gmpxx::mpf_class` integral construction non-explicit for upstream
  C++11 copy-initialization cases such as `mpf_class f = 3;`, while keeping
  `bool` construction deleted.
- Marked `gmpxx::mpz_class` default construction and
  `gmpxx::mpq_class::operator=(const mpz_class&)` as `noexcept`, matching the
  upstream `t-cxx11.cc` expectations.
- Removed `noexcept` from `gmpxx::mpq_class` and `gmpxx::mpf_class` move
  constructors so their `std::is_nothrow_move_constructible` traits match
  upstream GMP C++.
- Verified a minimally adapted copy of upstream
  `../gmpxx_mkII/cxx/t-cxx11.cc` against this repository's `gmpxx_mkII.h`.

Tests added:
- Added `using namespace gmpxx;` UDL visibility coverage to
  `tests/test_user_defined_literals.cpp`.
- Added compile-time coverage for `int -> gmpxx::mpf_class` convertibility,
  `gmpxx::mpz_class` nothrow default construction, and
  `gmpxx::mpq_class = gmpxx::mpz_class&&` noexcept behavior to
  `tests/test_construction_copy.cpp`.
- Updated compile-time coverage to assert that `gmpxx::mpq_class` and
  `gmpxx::mpf_class` move construction are not nothrow.

Tests updated:
- `tests/test_user_defined_literals.cpp`
- `tests/test_construction_copy.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-cxx11.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-cxx11.cc`
- `rg -n "using literals::operator\"\" _mpz|using literals::operator\"\" _mpq|using literals::operator\"\" _mpf|using literals::operator\"\" _mpfr|using literals::operator\"\" _mpc_i" include/gmpfrxx_mkII/detail`
- `g++ -std=c++17 -fext-numeric-literals -Iinclude /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII.cc -lgmp -o /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-cxx11.cc /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII.cc`
- `cmake --build build -j --target test_user_defined_literals test_construction_copy test_common_type test_abi_fingerprint test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_user_defined_literals|test_construction_copy|test_common_type|test_abi_fingerprint|test_gmpxx_mkII' --output-on-failure`
- `cmake --build build -j --target test_construction_copy test_user_defined_literals test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_construction_copy|test_user_defined_literals|test_gmpxx_mkII' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Initial minimally adapted upstream `t-cxx11.cc` build before wrapper changes:
  FAIL due GMP UDL namespace visibility, `mpf_class` integer copy-init, and
  selected `noexcept` differences.
- After wrapper changes, minimally adapted upstream `t-cxx11.cc` build: PASS.
- Temporary adapted `t-cxx11` run: PASS.
- After aligning `mpq_class` and `mpf_class` move-constructor noexcept traits,
  the temporary `t-cxx11.cc` diff only contains include/test-harness and
  namespace adaptation; no assertion changes are required.
- `cmake --build build -j --target test_user_defined_literals test_construction_copy test_common_type test_abi_fingerprint test_gmpxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_user_defined_literals|test_construction_copy|test_common_type|test_abi_fingerprint|test_gmpxx_mkII' --output-on-failure`: PASS, 5/5 tests passed.
- `cmake --build build -j --target test_construction_copy test_user_defined_literals test_gmpxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_construction_copy|test_user_defined_literals|test_gmpxx_mkII' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None for the GMP `t-cxx11.cc` parity check beyond local include/test-harness
  adaptation.

Post-phase upstream exception-smoke GMP header check:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream
  `../gmpxx_mkII/cxx/t-do-exceptions-work-at-all-with-this-compiler.cc`
  against this repository's `gmpxx_mkII.h`.
- No wrapper implementation changes were needed; including the GMP-only header
  with `mpz_class`, `mpq_class`, and `mpf_class` declarations present does not
  interfere with basic C++ exception handling.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,240p' ../gmpxx_mkII/cxx/t-do-exceptions-work-at-all-with-this-compiler.cc`
- `mkdir -p /tmp/t-exceptions-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-do-exceptions-work-at-all-with-this-compiler.cc /tmp/t-exceptions-gmpxx/t-do-exceptions-gmpxx-mkII.cc`
- Added `#include <gmpxx_mkII.h>` to the temporary copy.
- `g++ -std=c++17 -Iinclude /tmp/t-exceptions-gmpxx/t-do-exceptions-gmpxx-mkII.cc -lgmp -o /tmp/t-exceptions-gmpxx/t-do-exceptions-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-exceptions-gmpxx/t-do-exceptions-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-do-exceptions-work-at-all-with-this-compiler.cc /tmp/t-exceptions-gmpxx/t-do-exceptions-gmpxx-mkII.cc`

Pass/fail result:
- Temporary adapted exception-smoke build: PASS.
- Temporary adapted exception-smoke run: PASS.

Known issues:
- None.

Post-phase upstream t-headers GMP header check:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream
  `../gmpxx_mkII/cxx/t-headers.cc` against this repository's
  `gmpxx_mkII.h`.
- No wrapper implementation changes were needed; the GMP-only public header
  compiles standalone with `mpz_class`, `mpq_class`, and `mpf_class` exposed
  from namespace `gmpxx`.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-headers.cc`
- `mkdir -p /tmp/t-headers-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-headers.cc /tmp/t-headers-gmpxx/t-headers-gmpxx-mkII.cc`
- Replaced `#include "gmpxx_mkII.h"` with `#include <gmpxx_mkII.h>` in the
  temporary copy.
- `g++ -std=c++17 -Iinclude /tmp/t-headers-gmpxx/t-headers-gmpxx-mkII.cc -lgmp -o /tmp/t-headers-gmpxx/t-headers-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-headers-gmpxx/t-headers-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-headers.cc /tmp/t-headers-gmpxx/t-headers-gmpxx-mkII.cc`

Pass/fail result:
- Temporary adapted `t-headers` build: PASS.
- Temporary adapted `t-headers` run: PASS.

Known issues:
- None.

Post-phase upstream t-iostream GMP adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream
  `../gmpxx_mkII/cxx/t-iostream.cc` against this repository's
  `gmpxx_mkII.h`.
- Made `gmpxx::mpf_class(double)` non-explicit so upstream-style
  copy-initialization such as `mpf_class x = 1.5;` works, while keeping
  `bool` construction deleted.

Tests added:
- Added compile-time coverage that `double` is convertible to
  `gmpxx::mpf_class` in `tests/test_construction_copy.cpp`.

Tests updated:
- `tests/test_construction_copy.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,280p' ../gmpxx_mkII/cxx/t-iostream.cc`
- `sed -n '281,620p' ../gmpxx_mkII/cxx/t-iostream.cc`
- `mkdir -p /tmp/t-iostream-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-iostream.cc /tmp/t-iostream-gmpxx/t-iostream-gmpxx-mkII.cc`
- Applied include/test-harness and `using namespace gmpxx;` adaptation to the
  temporary copy.
- `g++ -std=c++17 -Iinclude /tmp/t-iostream-gmpxx/t-iostream-gmpxx-mkII.cc -lgmp -o /tmp/t-iostream-gmpxx/t-iostream-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-iostream-gmpxx/t-iostream-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-iostream.cc /tmp/t-iostream-gmpxx/t-iostream-gmpxx-mkII.cc`
- `cmake --build build -j --target test_construction_copy test_mpf_basic test_user_defined_literals test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_construction_copy|test_mpf_basic|test_user_defined_literals|test_gmpxx_mkII' --output-on-failure`

Pass/fail result:
- Initial temporary adapted `t-iostream` build: FAIL because
  `mpf_class x = 1.5;` required double copy-initialization support.
- Temporary adapted `t-iostream` build after wrapper change: PASS.
- Temporary adapted `t-iostream` run: PASS.
- `cmake --build build -j --target test_construction_copy test_mpf_basic test_user_defined_literals test_gmpxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_construction_copy|test_mpf_basic|test_user_defined_literals|test_gmpxx_mkII' --output-on-failure`: PASS, 4/4 tests passed.

Known issues:
- None for the basic GMP iostream smoke test beyond local include/test-harness
  adaptation.

Post-phase upstream t-istream GMP adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream
  `../gmpxx_mkII/cxx/t-istream.cc` against this repository's
  `gmpxx_mkII.h`.
- No wrapper implementation changes were needed; GMP `mpz_ptr`, `mpq_ptr`,
  and `mpf_ptr` stream extraction behavior already satisfies the upstream
  focused input cases, including the optional `-s` standard-stream comparison
  mode.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-istream.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-istream.cc`
- `mkdir -p /tmp/t-istream-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-istream.cc /tmp/t-istream-gmpxx/t-istream-gmpxx-mkII.cc`
- Applied include/test-harness, `numberof`, trace helper, and
  `using namespace gmpxx;` adaptation to the temporary copy.
- `g++ -std=c++17 -Iinclude /tmp/t-istream-gmpxx/t-istream-gmpxx-mkII.cc -lgmp -o /tmp/t-istream-gmpxx/t-istream-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-istream-gmpxx/t-istream-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-istream-gmpxx/t-istream-gmpxx-mkII -s`
- `diff -u ../gmpxx_mkII/cxx/t-istream.cc /tmp/t-istream-gmpxx/t-istream-gmpxx-mkII.cc`

Pass/fail result:
- Temporary adapted `t-istream` build: PASS.
- Temporary adapted `t-istream` run: PASS.
- Temporary adapted `t-istream -s` run: PASS.

Known issues:
- None for GMP istream extraction beyond local include/test-harness
  adaptation.

Post-phase upstream t-locale GMP adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream
  `../gmpxx_mkII/cxx/t-locale.cc` against this repository's
  `gmpxx_mkII.h`.
- No wrapper implementation changes were needed; GMP `mpf_t` stream input and
  output already respect the active C++ locale decimal point in the upstream
  focused locale smoke test.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,280p' ../gmpxx_mkII/cxx/t-locale.cc`
- `sed -n '281,620p' ../gmpxx_mkII/cxx/t-locale.cc`
- `mkdir -p /tmp/t-locale-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-locale.cc /tmp/t-locale-gmpxx/t-locale-gmpxx-mkII.cc`
- Applied include/test-harness macros and `using namespace gmpxx;` adaptation
  to the temporary copy.
- `g++ -std=c++17 -Iinclude /tmp/t-locale-gmpxx/t-locale-gmpxx-mkII.cc -lgmp -o /tmp/t-locale-gmpxx/t-locale-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-locale-gmpxx/t-locale-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-locale.cc /tmp/t-locale-gmpxx/t-locale-gmpxx-mkII.cc`

Pass/fail result:
- Temporary adapted `t-locale` build: PASS.
- Temporary adapted `t-locale` run: PASS.

Known issues:
- None for GMP locale stream behavior beyond local include/test-harness
  adaptation.

Post-phase upstream t-prec MPFR adaptation review:
DONE

Implemented features:
- Reviewed `../gmpxx_mkII/cxx/t-prec.cc` as a candidate for a minimal
  `mpfrxx_mkII.h` adaptation covering `mpfr_class`, `mpq_class`, and the
  shared exact classes exposed through the MPFR header.
- Built a temporary minimally adapted MPFR version at
  `/tmp/t-prec-mpfrxx/t-prec-mpfrxx-mkII.cc`.
- Decided not to pursue full line-by-line MPFR parity for this upstream
  test because the upstream assertions encode GMP `mpf_class` expression
  precision propagation policy.
- Kept MPFR grouped decimal string input support, because upstream precision
  tests use decimal strings with embedded spaces for readability and this is
  harmless for normal MPFR string construction, assignment, and `set_str`.

Tests added:
- Added MPFR string I/O regression coverage for grouped decimal input,
  grouped input through `set`, and grouped C99 hex-float input in
  `tests/test_mpfr_string_io.cpp`.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/test_mpfr_string_io.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-prec.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-prec.cc`
- `rg -n "ASSERT_ALWAYS_PREC|ASSERT_ALWAYS|mpf_|mpq_class|mpz_class|sqrt|hypot|mpf_class|main|check_" ../gmpxx_mkII/cxx/t-prec.cc`
- `sed -n '32,38p' ../gmpxx_mkII/cxx/t-prec.cc | tr -s ' ' | fold -w 160`
- `mkdir -p /tmp/t-prec-mpfrxx`
- `cp ../gmpxx_mkII/cxx/t-prec.cc /tmp/t-prec-mpfrxx/t-prec-mpfrxx-mkII.cc`
- Minimal source adaptation commands for the temporary MPFR version.
- `g++ -std=c++17 -Iinclude /tmp/t-prec-mpfrxx/t-prec-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-prec-mpfrxx/t-prec-mpfrxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-prec-mpfrxx/t-prec-mpfrxx-mkII`
- `cmake --build build -j --target test_mpfr_string_io`
- `./build/tests/test_mpfr_string_io`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `diff -u ../gmpxx_mkII/cxx/t-prec.cc /tmp/t-prec-mpfrxx/t-prec-mpfrxx-mkII.cc`

Pass/fail result:
- Temporary adapted `t-prec` build: PASS.
- First temporary adapted `t-prec` run: FAIL because MPFR string
  construction rejected upstream grouped decimal strings with embedded
  spaces.
- After adding grouped whitespace handling, temporary adapted `t-prec` run:
  FAIL at the `-(1 + f) / 3` precision assertion.
- The remaining failure is an intentional policy difference: upstream GMP
  `mpf_class` tests expect expression precision propagation behavior that
  should not be imposed on the MPFR wrapper.
- `cmake --build build -j --target test_mpfr_string_io`: PASS.
- `./build/tests/test_mpfr_string_io`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- `../gmpxx_mkII/cxx/t-prec.cc` is intentionally not fully ported to MPFR.
- MPFR precision policy remains explicit and MPFR-oriented instead of
  emulating GMP `mpf_class` expression precision propagation.

Post-phase upstream t-ostream MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted `../gmpxx_mkII/cxx/t-ostream.cc` against
  this repository's `mpfrxx_mkII.h`.
- The adapted check covers raw `mpz_t`, raw `mpq_t`, and raw `mpfr_t`
  stream insertion through the mpfrxx public header.
- Added MPFR ostream formatting support for non-decimal bases, including
  `std::hex`, `std::oct`, `std::showbase`, `std::showpoint`,
  `std::uppercase`, `std::fixed`, `std::scientific`, stream precision,
  width, fill, and `std::internal` padding.
- Matched the upstream MPF ostream precision-0 default/showpoint behavior
  for MPFR output.

Tests added:
- Added MPFR ostream regression coverage in `tests/test_mpfr_string_io.cpp`
  for showpoint zero with stream precision 0, hex internal padding, hex
  scientific output, oct scientific output, and raw `mpfr_t` hex output.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/test_mpfr_string_io.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,320p' ../gmpxx_mkII/cxx/t-ostream.cc`
- `sed -n '321,700p' ../gmpxx_mkII/cxx/t-ostream.cc`
- `mkdir -p /tmp/t-ostream-mpfrxx`
- `cp ../gmpxx_mkII/cxx/t-ostream.cc /tmp/t-ostream-mpfrxx/t-ostream-mpfrxx-mkII.cc`
- Minimal source adaptation commands for the temporary MPFR version.
- `g++ -std=c++17 -Iinclude /tmp/t-ostream-mpfrxx/t-ostream-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ostream-mpfrxx/t-ostream-mpfrxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-ostream-mpfrxx/t-ostream-mpfrxx-mkII`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake --build build -j --target test_mpfr_string_io`
- `./build/tests/test_mpfr_string_io`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `diff -u ../gmpxx_mkII/cxx/t-ostream.cc /tmp/t-ostream-mpfrxx/t-ostream-mpfrxx-mkII.cc`

Pass/fail result:
- Initial temporary build: FAIL because upstream's private
  `MPZ_CHECK_FORMAT` and `gmp-impl.h` were not available in the standalone
  adaptation.
- Initial temporary MPFR run: FAIL because MPFR ostream printed showpoint
  zero with stream precision 0 as `0.` instead of upstream MPF-compatible
  `0.00000`.
- Next temporary MPFR run: FAIL because MPFR ostream did not yet implement
  upstream-style hex/oct base formatting.
- Next temporary MPFR run: FAIL because `std::internal` padding placed fill
  before the base prefix (`-  0x1`) instead of after sign and prefix
  (`-0x  1`).
- Final temporary MPFR build/run: PASS.
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build build -j --target test_mpfr_string_io`: PASS.
- `./build/tests/test_mpfr_string_io`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None for the t-ostream MPFR adaptation.
- The temporary adaptation uses a local no-op `MPZ_CHECK_FORMAT` replacement
  only because upstream's private `gmp-impl.h` is not part of this repository.

Post-phase upstream t-rand MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted `../gmpxx_mkII/cxx/t-rand.cc` against this
  repository's `mpfrxx_mkII.h`.
- The adapted check covers `mpfrxx::gmp_randclass` initialization forms,
  `mpfrxx::mpz_class` seeding and random integer generation, and the MPFR
  random expression surface through `get_fr`.
- No wrapper implementation changes were needed; the existing MPFR random API
  already covers the upstream `gmp_randclass` surface once MPF-specific
  `get_f` calls are mapped to MPFR-specific `get_fr`.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-rand.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-rand.cc`
- `rg -n "class gmp_randclass|gmp_randclass|get_z_bits|get_z_range|get_f|get_fr|get_mpfr|rand" include tests`
- `sed -n '1,220p' tests/test_random.cpp`
- `sed -n '1,220p' tests/test_mpfr_random.cpp`
- `mkdir -p /tmp/t-rand-mpfrxx`
- `cp ../gmpxx_mkII/cxx/t-rand.cc /tmp/t-rand-mpfrxx/t-rand-mpfrxx-mkII.cc`
- Minimal source adaptation commands for the temporary MPFR version.
- `g++ -std=c++17 -Iinclude /tmp/t-rand-mpfrxx/t-rand-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-rand-mpfrxx/t-rand-mpfrxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-rand-mpfrxx/t-rand-mpfrxx-mkII`
- `cmake --build build -j --target test_mpfr_random test_random`
- `ctest --test-dir build -R 'test_mpfr_random|test_random' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `diff -u ../gmpxx_mkII/cxx/t-rand.cc /tmp/t-rand-mpfrxx/t-rand-mpfrxx-mkII.cc`

Pass/fail result:
- Temporary adapted `t-rand` build: PASS.
- Temporary adapted `t-rand` run: PASS.
- `cmake --build build -j --target test_mpfr_random test_random`: PASS.
- `ctest --test-dir build -R 'test_mpfr_random|test_random' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None for the t-rand MPFR adaptation.

Post-phase upstream t-ternary MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted `../gmpxx_mkII/cxx/t-ternary.cc` against
  this repository's `mpfrxx_mkII.h`.
- The adapted check covers the upstream `mpz_class` ternary/nested
  expression matrix through `mpfrxx::mpz_class`, which is an alias to the
  shared exact GMP-backed class.
- Kept `check_mpq()` empty as in upstream because the upstream test states
  there is currently no ternary `mpq` operation.
- Renamed the empty `check_mpf()` placeholder to `check_mpfr()` for the MPFR
  adaptation; no MPFR ternary-specific operation exists in this upstream
  test.
- No wrapper implementation changes were needed.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-ternary.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-ternary.cc`
- `rg -n "static void check_|check_mp|main|mpq_class|mpf_class|mpfr|mpz_class" ../gmpxx_mkII/cxx/t-ternary.cc`
- `tail -n 120 ../gmpxx_mkII/cxx/t-ternary.cc`
- `mkdir -p /tmp/t-ternary-mpfrxx`
- `cp ../gmpxx_mkII/cxx/t-ternary.cc /tmp/t-ternary-mpfrxx/t-ternary-mpfrxx-mkII.cc`
- Minimal source adaptation commands for the temporary MPFR version.
- `g++ -std=c++17 -Iinclude /tmp/t-ternary-mpfrxx/t-ternary-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ternary-mpfrxx/t-ternary-mpfrxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-ternary-mpfrxx/t-ternary-mpfrxx-mkII`
- `cmake --build build -j --target test_mpz_arithmetic test_mixed_zq_mpfr_promotion test_mpfrxx_mkII`
- `ctest --test-dir build -R 'test_mpz_arithmetic|test_mixed_zq_mpfr_promotion|test_mpfrxx_mkII' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `diff -u ../gmpxx_mkII/cxx/t-ternary.cc /tmp/t-ternary-mpfrxx/t-ternary-mpfrxx-mkII.cc`

Pass/fail result:
- Temporary adapted `t-ternary` build: PASS.
- Temporary adapted `t-ternary` run: PASS.
- `cmake --build build -j --target test_mpz_arithmetic test_mixed_zq_mpfr_promotion test_mpfrxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_mpz_arithmetic|test_mixed_zq_mpfr_promotion|test_mpfrxx_mkII' --output-on-failure`: PASS, 3/3 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None for the t-ternary MPFR adaptation.

Post-phase upstream t-unary MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted `../gmpxx_mkII/cxx/t-unary.cc` against this
  repository's `mpfrxx_mkII.h`.
- The adapted check covers unary `+` and `-` for `mpfrxx::mpz_class`,
  `mpfrxx::mpq_class`, and `mpfrxx::mpfr_class`, plus bitwise `~` for the
  shared exact `mpz_class`.
- No wrapper implementation changes were needed.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-unary.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-unary.cc`
- `mkdir -p /tmp/t-unary-mpfrxx`
- `cp ../gmpxx_mkII/cxx/t-unary.cc /tmp/t-unary-mpfrxx/t-unary-mpfrxx-mkII.cc`
- Minimal source adaptation commands for the temporary MPFR version.
- `g++ -std=c++17 -Iinclude /tmp/t-unary-mpfrxx/t-unary-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-unary-mpfrxx/t-unary-mpfrxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-unary-mpfrxx/t-unary-mpfrxx-mkII`
- `cmake --build build -j --target test_unary_minus_simplification test_mpz_arithmetic test_mpq_arithmetic test_mpfrxx_mkII`
- `ctest --test-dir build -R 'test_unary_minus_simplification|test_mpz_arithmetic|test_mpq_arithmetic|test_mpfrxx_mkII' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `diff -u ../gmpxx_mkII/cxx/t-unary.cc /tmp/t-unary-mpfrxx/t-unary-mpfrxx-mkII.cc`

Pass/fail result:
- Temporary adapted `t-unary` build: PASS.
- Temporary adapted `t-unary` run: PASS.
- `cmake --build build -j --target test_unary_minus_simplification test_mpz_arithmetic test_mpq_arithmetic test_mpfrxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_unary_minus_simplification|test_mpz_arithmetic|test_mpq_arithmetic|test_mpfrxx_mkII' --output-on-failure`: PASS, 4/4 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None for the t-unary MPFR adaptation.

Post-phase upstream t-assign GMP adaptation:
DONE

Implemented features:
- Verified a minimally adapted `../gmpxx_mkII/cxx/t-assign.cc` against this
  repository's `gmpxx_mkII.h`.
- The adapted check covers assignment and swap surfaces for `gmpxx::mpz_class`,
  `gmpxx::mpq_class`, and `gmpxx::mpf_class`, including signed/unsigned
  integral scalars, `int32_t`/`uint32_t`, `int64_t`/`uint64_t`, direct
  `__int128` assignment where available, `float`, `double`, string
  assignment, invalid string exceptions, member `swap`, explicit `::swap`,
  and ADL/std swap.
- No wrapper implementation changes were needed.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-assign.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-assign.cc`
- `sed -n '621,1040p' ../gmpxx_mkII/cxx/t-assign.cc`
- `mkdir -p /tmp/t-assign-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-assign.cc /tmp/t-assign-gmpxx/t-assign-gmpxx-mkII.cc`
- Minimal source adaptation commands for the temporary GMP version.
- `g++ -std=c++17 -Iinclude /tmp/t-assign-gmpxx/t-assign-gmpxx-mkII.cc -lgmp -o /tmp/t-assign-gmpxx/t-assign-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-assign-gmpxx/t-assign-gmpxx-mkII`
- `cmake --build build -j --target test_type_conversions test_mpf_basic test_mpq_basic test_mpz_basic test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_type_conversions|test_mpf_basic|test_mpq_basic|test_mpz_basic|test_gmpxx_mkII' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `diff -u ../gmpxx_mkII/cxx/t-assign.cc /tmp/t-assign-gmpxx/t-assign-gmpxx-mkII.cc`

Pass/fail result:
- Initial temporary adapted `t-assign` build: FAIL because this repository
  keeps GMP wrapper types in namespace `gmpxx` and does not expose global
  `mpz_class`, `mpq_class`, `mpf_class`, or `::swap`.
- Temporary adapted `t-assign` build after adding `using namespace gmpxx` and
  `using gmpxx::swap`: PASS.
- Temporary adapted `t-assign` run: PASS.
- `cmake --build build -j --target test_type_conversions test_mpf_basic test_mpq_basic test_mpz_basic test_gmpxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_type_conversions|test_mpf_basic|test_mpq_basic|test_mpz_basic|test_gmpxx_mkII' --output-on-failure`: PASS, 5/5 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None for the t-assign GMP adaptation.

Post-phase upstream t-binary GMP adaptation:
DONE

Implemented features:
- Verified a minimally adapted `../gmpxx_mkII/cxx/t-binary.cc` against this
  repository's `gmpxx_mkII.h`.
- The adapted check covers binary expression combinations for
  `gmpxx::mpz_class`, `gmpxx::mpq_class`, and `gmpxx::mpf_class`, including
  scalar mixed arithmetic, nested expression operands, shifts, exact
  `mpz`/`mpq`/`mpf` promotion paths, bitwise `mpz` operations, and the
  upstream large double bitwise case assigning
  `0x1000000000000000000000cafe`.
- No wrapper implementation changes were needed.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-binary.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-binary.cc`
- `mkdir -p /tmp/t-binary-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-binary.cc /tmp/t-binary-gmpxx/t-binary-gmpxx-mkII.cc`
- Minimal source adaptation commands for the temporary GMP version.
- `g++ -std=c++17 -Iinclude /tmp/t-binary-gmpxx/t-binary-gmpxx-mkII.cc -lgmp -o /tmp/t-binary-gmpxx/t-binary-gmpxx-mkII`
- `stdbuf -o0 -e0 /tmp/t-binary-gmpxx/t-binary-gmpxx-mkII`
- `cmake --build build -j --target test_mixed_type_arithmetic test_mpz_arithmetic test_mpq_arithmetic test_mpf_basic test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_mixed_type_arithmetic|test_mpz_arithmetic|test_mpq_arithmetic|test_mpf_basic|test_gmpxx_mkII' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`
- `diff -u ../gmpxx_mkII/cxx/t-binary.cc /tmp/t-binary-gmpxx/t-binary-gmpxx-mkII.cc`

Pass/fail result:
- Temporary adapted `t-binary` build: PASS.
- Temporary adapted `t-binary` run: PASS.
- `cmake --build build -j --target test_mixed_type_arithmetic test_mpz_arithmetic test_mpq_arithmetic test_mpf_basic test_gmpxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_mixed_type_arithmetic|test_mpz_arithmetic|test_mpq_arithmetic|test_mpf_basic|test_gmpxx_mkII' --output-on-failure`: PASS, 5/5 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None for the t-binary GMP adaptation.

Post-phase upstream t-cast GMP adaptation:
DONE

Implemented features:
- Verified upstream `../gmpxx_mkII/cxx/t-cast.cc` against this repository's
  `gmpxx_mkII.h`.
- The test is a GMP C macro warning-cleanliness check through the C++ wrapper
  header. It exercises GMP C API macro expansion such as `mpz_odd_p`,
  `mpz_even_p`, `mpz_cmp_si`, `mpz_cmp_ui`, `GMP_NUMB_*`, and low-level `mpn_*`
  declarations, rather than direct `mpz_class`/`mpq_class`/`mpf_class` object
  operations.
- No source adaptation was needed beyond copying the upstream test to `/tmp`.
- No wrapper implementation changes were needed.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-cast.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-cast.cc`
- `mkdir -p /tmp/t-cast-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-cast.cc /tmp/t-cast-gmpxx/t-cast-gmpxx-mkII.cc`
- `g++ -std=c++17 -Wold-style-cast -Werror=old-style-cast -Iinclude /tmp/t-cast-gmpxx/t-cast-gmpxx-mkII.cc -lgmp -o /tmp/t-cast-gmpxx/t-cast-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-cast.cc /tmp/t-cast-gmpxx/t-cast-gmpxx-mkII.cc`
- `stdbuf -o0 -e0 /tmp/t-cast-gmpxx/t-cast-gmpxx-mkII`
- `cmake --build build -j --target test_gmp_header_smoke test_header_boundaries test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_gmp_header_smoke|test_header_boundaries|test_gmpxx_mkII' --output-on-failure`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Temporary copied `t-cast` build with `-Wold-style-cast -Werror=old-style-cast`: PASS.
- Temporary copied `t-cast` run: PASS.
- Minimal diff against upstream: empty.
- `cmake --build build -j --target test_gmp_header_smoke test_header_boundaries test_gmpxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_gmp_header_smoke|test_header_boundaries|test_gmpxx_mkII' --output-on-failure`: PASS, 3/3 tests passed.
- `git diff --check`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- None for the t-cast GMP adaptation.

Post-phase upstream t-constr GMP adaptation:
DONE

Implemented features:
- Verified a minimally adapted `../gmpxx_mkII/cxx/t-constr.cc` against this
  repository's `gmpxx_mkII.h`.
- The adapted check covers constructor surfaces for `gmpxx::mpz_class`,
  `gmpxx::mpq_class`, and `gmpxx::mpf_class`, including signed/unsigned
  character, short, int, long, floating, string/base, raw GMP pointer,
  copy-construction, MPQ numerator/denominator construction, MPF precision
  constructors, and invalid string exception paths.
- Removed only the upstream bool-constructor assertions from the temporary
  adaptation. This repository intentionally deletes bool constructors as part
  of the scalar rejection policy, while upstream GMP C++ lets bool convert
  through int.
- No wrapper implementation changes were needed.

Tests added:
- None.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-constr.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-constr.cc`
- `sed -n '621,1040p' ../gmpxx_mkII/cxx/t-constr.cc`
- `mkdir -p /tmp/t-constr-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-constr.cc /tmp/t-constr-gmpxx/t-constr-gmpxx-mkII.cc`
- Minimal source adaptation commands for the temporary GMP version.
- `g++ -std=c++17 -Iinclude /tmp/t-constr-gmpxx/t-constr-gmpxx-mkII.cc -lgmp -o /tmp/t-constr-gmpxx/t-constr-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-constr.cc /tmp/t-constr-gmpxx/t-constr-gmpxx-mkII.cc`
- `stdbuf -o0 -e0 /tmp/t-constr-gmpxx/t-constr-gmpxx-mkII`
- `cmake --build build -j --target test_type_conversions test_mpf_basic test_mpq_basic test_mpz_basic test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_type_conversions|test_mpf_basic|test_mpq_basic|test_mpz_basic|test_gmpxx_mkII' --output-on-failure`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial temporary adapted `t-constr` build after namespace/config-only
  adaptation: FAIL because `mpz_class(bool)`, `mpq_class(bool)`,
  `mpf_class(bool)`, and `mpf_class(bool, precision)` are intentionally
  deleted.
- Temporary adapted `t-constr` build after removing only bool constructor
  assertions: PASS.
- Temporary adapted `t-constr` run: PASS.
- `cmake --build build -j --target test_type_conversions test_mpf_basic test_mpq_basic test_mpz_basic test_gmpxx_mkII`: PASS.
- `ctest --test-dir build -R 'test_type_conversions|test_mpf_basic|test_mpq_basic|test_mpz_basic|test_gmpxx_mkII' --output-on-failure`: PASS, 5/5 tests passed.
- `git diff --check`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- Intentional difference from upstream: bool constructors remain rejected.

Post-phase upstream t-cxx11 GMP adaptation and C++11 parity:
DONE

Implemented features:
- Added `std::common_type` specializations for expression nodes:
  `object_leaf`, `scalar_leaf`, `unary_expr`, and `binary_expr`.
- Added expression/expression `std::common_type` specializations so mixed
  expression nodes resolve through their public `result_type`, matching the
  upstream C++11 test intent for cases such as `decltype(-z)` with
  `decltype(z + z)` and `decltype(-q)` with `decltype(-f)`.
- Verified a minimally adapted `../gmpxx_mkII/cxx/t-cxx11.cc` against this
  repository's `gmpxx_mkII.h`, covering move construction/assignment,
  expression common_type, string UDLs, and explicit bool conversion for
  `mpz_class`, `mpq_class`, and `mpf_class`.

Tests added:
- Added expression-node `std::common_type` regression coverage to
  `tests/test_common_type.cpp`.

Tests updated:
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' ../gmpxx_mkII/cxx/t-cxx11.cc`
- `sed -n '261,620p' ../gmpxx_mkII/cxx/t-cxx11.cc`
- `rg -n "__GMPXX_USE_CXX11|tests_start|tests_end|ASSERT_ALWAYS|CHECK_COMMON_TYPE|check_" ../gmpxx_mkII/cxx/t-cxx11.cc`
- `rg -n "common_type|operator bool|explicit operator bool|operator\"\" _mpz|operator\"\" _mpq|operator\"\" _mpf|noexcept|move" include tests -g '*.hpp' -g '*.cpp'`
- `sed -n '1,170p' include/gmpfrxx_mkII/detail/expr.hpp`
- `sed -n '1188,1230p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '540,575p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '2548,2575p' include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `sed -n '1800,1825p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '3448,3475p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `rg -n "using namespace .*literals|operator\"\" _mpfr|_mpz" tests/test_user_defined_literals.cpp tests/test_mpfr_user_defined_literals.cpp tests/test_gmpxx_mkII.cpp`
- `mkdir -p /tmp/t-cxx11-gmpxx`
- `cp ../gmpxx_mkII/cxx/t-cxx11.cc /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII.cc`
- Minimal source adaptation commands for the temporary GMP version.
- `g++ -std=c++17 -Iinclude /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII.cc -lgmp -o /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII`
- `g++ -std=c++17 -fext-numeric-literals -Iinclude /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII.cc -lgmp -o /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII`
- `diff -u ../gmpxx_mkII/cxx/t-cxx11.cc /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII.cc`
- `stdbuf -o0 -e0 /tmp/t-cxx11-gmpxx/t-cxx11-gmpxx-mkII`
- `cmake --build build -j --target test_common_type test_user_defined_literals test_et_contract_zq_mpf test_mixed_type_arithmetic test_gmpxx_mkII`
- `cmake --build build -j --target test_common_type test_user_defined_literals test_gmpxx_mkII`
- `ctest --test-dir build -R 'test_common_type|test_user_defined_literals|test_gmpxx_mkII' --output-on-failure`
- `git diff --check`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Initial temporary adapted `t-cxx11` build before implementation: FAIL
  because expression-node `std::common_type` coverage was missing.
- Temporary adapted `t-cxx11` build after implementation and remaining policy
  adaptations: PASS.
- Temporary adapted `t-cxx11` run: PASS.
- `cmake --build build -j --target test_common_type test_user_defined_literals test_et_contract_zq_mpf test_mixed_type_arithmetic test_gmpxx_mkII`: PASS after the expression/expression common_type ambiguity fix.
- `ctest --test-dir build -R 'test_common_type|test_user_defined_literals|test_gmpxx_mkII' --output-on-failure`: PASS, 3/3 tests passed.
- `git diff --check`: PASS.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.

Known issues:
- Intentional differences from upstream remain in the temporary `t-cxx11`
  adaptation: this repository keeps bool scalar construction rejected and has
  different noexcept/default-construction policy details from upstream GMP C++.

Post-phase upstream t-ops MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream `gmpxx_mkII/cxx/t-ops.cc`
  against this repo's `mpfrxx_mkII.h` for mpz/mpq/mpfr operator and helper
  coverage.
- No wrapper implementation changes were needed; existing exact Z/Q operators
  and MPFR arithmetic/math/comparison helpers covered the adapted test.

Tests added:
- None.

Tests updated:
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/cxx/t-ops.cc
- sed -n '261,620p' ../gmpxx_mkII/cxx/t-ops.cc
- sed -n '621,980p' ../gmpxx_mkII/cxx/t-ops.cc
- cp ../gmpxx_mkII/cxx/t-ops.cc /tmp/t-ops-mpfrxx-mkII.cc
- perl -0pi -e '...' /tmp/t-ops-mpfrxx-mkII.cc
- g++ -std=c++17 -Iinclude /tmp/t-ops-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops-mpfrxx-mkII
- /tmp/t-ops-mpfrxx-mkII
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- g++ -std=c++17 -Iinclude /tmp/t-ops-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops-mpfrxx-mkII: PASS.
- /tmp/t-ops-mpfrxx-mkII: PASS.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 119/119 tests passed.

Known issues:
- The temporary adaptation maps upstream MPF operator coverage to
  `mpfrxx::mpfr_class`; GMP-only MPF-specific precision behavior is not part
  of this MPFR header check.

Post-phase upstream t-ops2f MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream `gmpxx_mkII/cxx/t-ops2f.cc`
  against this repo's `mpfrxx_mkII.h` for MPFR unary math, hypot, mixed
  MPFR/MPQ arithmetic, and MPFR/MPQ comparisons.
- Reused a minimally adapted temporary copy of upstream `t-ops2.h` so the
  upstream CHECK macro structure remained intact.
- No wrapper implementation changes were needed; existing MPFR math, mixed
  exact promotion, and comparison helpers covered the adapted test.

Tests added:
- None.

Tests updated:
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/cxx/t-ops2f.cc
- sed -n '261,620p' ../gmpxx_mkII/cxx/t-ops2f.cc
- sed -n '1,320p' ../gmpxx_mkII/cxx/t-ops2.h
- mkdir -p /tmp/t-ops2f-mpfrxx
- cp ../gmpxx_mkII/cxx/t-ops2f.cc /tmp/t-ops2f-mpfrxx/t-ops2f-mpfrxx-mkII.cc
- cp ../gmpxx_mkII/cxx/t-ops2.h /tmp/t-ops2f-mpfrxx/t-ops2.h
- perl -0pi -e '...' /tmp/t-ops2f-mpfrxx/t-ops2.h
- perl -0pi -e '...' /tmp/t-ops2f-mpfrxx/t-ops2f-mpfrxx-mkII.cc
- g++ -std=c++17 -Iinclude -I/tmp/t-ops2f-mpfrxx /tmp/t-ops2f-mpfrxx/t-ops2f-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops2f-mpfrxx/t-ops2f-mpfrxx-mkII
- /tmp/t-ops2f-mpfrxx/t-ops2f-mpfrxx-mkII
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- g++ -std=c++17 -Iinclude -I/tmp/t-ops2f-mpfrxx /tmp/t-ops2f-mpfrxx/t-ops2f-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops2f-mpfrxx/t-ops2f-mpfrxx-mkII: PASS.
- /tmp/t-ops2f-mpfrxx/t-ops2f-mpfrxx-mkII: PASS.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 119/119 tests passed.

Known issues:
- The temporary adaptation maps upstream MPF-specific default precision setup
  to `mpfrxx::set_default_precision_bits(DBL_MANT_DIG - DBL_MIN_EXP + 42)`.

Post-phase upstream t-ops2qf MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream `gmpxx_mkII/cxx/t-ops2qf.cc`
  against this repo's `mpfrxx_mkII.h` for mixed `mpq_class`/`mpfr_class`
  arithmetic, comparisons, shifts, unary helpers, and increment/decrement.
- Added rvalue pre/post increment and decrement overloads for
  `gmpxx::mpz_class`, `gmpxx::mpq_class`, `gmpxx::mpf_class`, and
  `mpfrxx::mpfr_class`, matching upstream-style expressions such as
  `++T(7)` and `T(7)++`.
- Kept the temporary adaptation limited to include/assertion setup,
  `mpf_class` to `mpfr_class`, and MPF default precision setup to
  `mpfrxx::set_default_precision_bits(...)`.

Tests added:
- Rvalue increment/decrement regression coverage in existing arithmetic tests.

Tests updated:
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- include/gmpfrxx_mkII/detail/mpf_impl.hpp
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/test_mpz_arithmetic.cpp
- tests/test_mpq_arithmetic.cpp
- tests/test_mpf_basic.cpp
- tests/test_mpfr_scalar_eval.cpp
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/cxx/t-ops2qf.cc
- sed -n '1,340p' ../gmpxx_mkII/cxx/t-ops2.h
- mkdir -p /tmp/t-ops2qf-mpfrxx
- cp ../gmpxx_mkII/cxx/t-ops2qf.cc /tmp/t-ops2qf-mpfrxx/t-ops2qf-mpfrxx-mkII.cc
- cp ../gmpxx_mkII/cxx/t-ops2.h /tmp/t-ops2qf-mpfrxx/t-ops2.h
- perl -0pi -e '...' /tmp/t-ops2qf-mpfrxx/t-ops2.h
- perl -0pi -e '...' /tmp/t-ops2qf-mpfrxx/t-ops2qf-mpfrxx-mkII.cc
- g++ -std=c++17 -Iinclude -I/tmp/t-ops2qf-mpfrxx /tmp/t-ops2qf-mpfrxx/t-ops2qf-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops2qf-mpfrxx/t-ops2qf-mpfrxx-mkII
- /tmp/t-ops2qf-mpfrxx/t-ops2qf-mpfrxx-mkII
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial g++ build of the temporary adaptation: FAIL because rvalue
  increment/decrement overloads were missing for `mpq_class` and `mpfr_class`.
- g++ -std=c++17 -Iinclude -I/tmp/t-ops2qf-mpfrxx /tmp/t-ops2qf-mpfrxx/t-ops2qf-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops2qf-mpfrxx/t-ops2qf-mpfrxx-mkII after adding rvalue overloads: PASS.
- /tmp/t-ops2qf-mpfrxx/t-ops2qf-mpfrxx-mkII: PASS.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j: PASS.
- Initial ctest --test-dir build --output-on-failure: FAIL because the new
  MPF regression compared requested precision to the exact literal `160`
  instead of GMP's actual rounded precision.
- ctest --test-dir build --output-on-failure after adjusting the MPF
  regression to compare against actual `get_prec()`: PASS, 119/119 tests
  passed.

Known issues:
- None for the t-ops2qf MPFR adaptation.

Post-phase upstream t-ops2z MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream `gmpxx_mkII/cxx/t-ops2z.cc`
  against this repo's `mpfrxx_mkII.h` for `mpz_class` arithmetic,
  comparisons, bit operations, shifts, unary helpers, integer helpers, and
  exceptional helper paths.
- Adjusted Z/Q expression-template division so `mpz_class / mpz_class`
  materializes as truncating integer `mpz_class`, matching upstream gmpxx
  behavior. Mixed rational division remains `mpq_class`.
- Added floating scalar `%` overloads for `mpz_class` so upstream macro cases
  such as `mpz_class % double` and `double % mpz_class` resolve directly.
- Added `mpz_class::factorial`, `mpz_class::primorial`, and
  `mpz_class::fibonacci` static helpers forwarding to the existing free
  helpers.
- Matched upstream helper overflow behavior for factorial/primorial/fibonacci
  by throwing `std::bad_alloc` when the input does not fit `unsigned long`.

Tests added:
- Regression coverage in `tests/test_mpz_arithmetic.cpp` for truncating
  `mpz/mpz` division, floating scalar `%`, and static integer helper forms.

Tests updated:
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- tests/test_mpz_arithmetic.cpp
- STATUS.md

Exact commands run:
- sed -n '1,280p' ../gmpxx_mkII/cxx/t-ops2z.cc
- sed -n '1,120p' ../gmpxx_mkII/cxx/t-ops2.h
- mkdir -p /tmp/t-ops2z-mpfrxx
- cp ../gmpxx_mkII/cxx/t-ops2z.cc /tmp/t-ops2z-mpfrxx/t-ops2z-mpfrxx-mkII.cc
- cp ../gmpxx_mkII/cxx/t-ops2.h /tmp/t-ops2z-mpfrxx/t-ops2.h
- perl -0pi -e '...' /tmp/t-ops2z-mpfrxx/t-ops2.h
- g++ -std=c++17 -Iinclude -I/tmp/t-ops2z-mpfrxx /tmp/t-ops2z-mpfrxx/t-ops2z-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops2z-mpfrxx/t-ops2z-mpfrxx-mkII
- /tmp/t-ops2z-mpfrxx/t-ops2z-mpfrxx-mkII
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial g++ build of the temporary adaptation: FAIL because floating scalar
  `%` and static integer helpers were missing.
- Initial run after adding those helpers: FAIL because `mpz/mpz` division was
  exact rational rather than upstream truncating integer division.
- g++ -std=c++17 -Iinclude -I/tmp/t-ops2z-mpfrxx /tmp/t-ops2z-mpfrxx/t-ops2z-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops2z-mpfrxx/t-ops2z-mpfrxx-mkII after fixes: PASS.
- /tmp/t-ops2z-mpfrxx/t-ops2z-mpfrxx-mkII: PASS.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j: PASS.
- Initial ctest --test-dir build --output-on-failure: FAIL because a new
  `test_mpz_arithmetic` regression asserted a fixed quotient for every
  parameterized input pair.
- ctest --test-dir build --output-on-failure after fixing that regression:
  PASS, 119/119 tests passed.

Known issues:
- None for the t-ops2z MPFR adaptation.

Post-phase upstream t-ops3 MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream `gmpxx_mkII/cxx/t-ops3.cc`
  against this repo's `mpfrxx_mkII.h` for compound assignment coverage across
  `mpz_class`, `mpq_class`, and `mpfr_class`.
- Made `mpfrxx::mpfr_class(double)` non-explicit so upstream-style
  copy-initialization such as `mpfr_class d = 3.375;` is accepted.
- Updated the common-type policy to allow
  `std::common_type_t<mpfrxx::mpfr_class, double>` as `mpfrxx::mpfr_class`.
- Added MPFR-header-only compound assignment overloads for exact Z/Q lvalues
  with MPFR RHS values and expressions; the result is materialized through
  MPFR and converted back to the exact lvalue type.
- Added `mpz_class` bitwise compound assignment overloads for `&=`, `|=`, and
  `^=` with integer/Z expression operands.

Tests added:
- Regression coverage in `tests/test_mpfr_compound_assign.cpp` for
  `mpfr_class d = 3.375;` and Z/Q lvalue compound assignment with MPFR RHS.
- Regression coverage in `tests/test_mpz_arithmetic.cpp` for `mpz_class`
  bitwise compound assignment.

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- tests/test_mpfr_compound_assign.cpp
- tests/test_mpz_arithmetic.cpp
- tests/test_common_type.cpp
- tests/test_abi_fingerprint.cpp
- STATUS.md

Exact commands run:
- sed -n '1,320p' ../gmpxx_mkII/cxx/t-ops3.cc
- mkdir -p /tmp/t-ops3-mpfrxx
- cp ../gmpxx_mkII/cxx/t-ops3.cc /tmp/t-ops3-mpfrxx/t-ops3-mpfrxx-mkII.cc
- perl -0pi -e '...' /tmp/t-ops3-mpfrxx/t-ops3-mpfrxx-mkII.cc
- g++ -std=c++17 -Iinclude /tmp/t-ops3-mpfrxx/t-ops3-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops3-mpfrxx/t-ops3-mpfrxx-mkII
- /tmp/t-ops3-mpfrxx/t-ops3-mpfrxx-mkII
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial g++ build of the temporary adaptation: FAIL because namespace setup
  was missing in the temporary file.
- Second g++ build: FAIL because `mpfr_class(double)` was explicit, Z/Q
  lvalue compound assignment with MPFR RHS was missing, and `mpz_class`
  bitwise compound assignment was missing.
- g++ -std=c++17 -Iinclude /tmp/t-ops3-mpfrxx/t-ops3-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-ops3-mpfrxx/t-ops3-mpfrxx-mkII after fixes: PASS.
- /tmp/t-ops3-mpfrxx/t-ops3-mpfrxx-mkII: PASS.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Initial cmake --build build -j after making `mpfr_class(double)`
  non-explicit: FAIL because common-type policy tests still expected no
  `mpfr_class`/`double` common type.
- cmake --build build -j after updating common-type tests: PASS.
- ctest --test-dir build --output-on-failure: PASS, 119/119 tests passed.

Known issues:
- The MPFR header now intentionally accepts implicit `double` construction for
  `mpfr_class` to match upstream-style `mpf_class` usage.

Post-phase upstream t-mix MPFR adaptation:
DONE

Implemented features:
- Verified a minimally adapted copy of upstream `gmpxx_mkII/cxx/t-mix.cc`
  against this repo's `mpfrxx_mkII.h` for mpz/mpq/mpfr conversion legality.
- Made `mpfrxx::mpfr_class` naturally constructible from mpz/mpq exact values,
  exact expression nodes, and supported integral scalars in the same overload
  contexts as upstream MPF promotion.
- Added explicit `mpfr_class` conversion operators to `gmpxx::mpz_class` and
  `gmpxx::mpq_class`, so MPFR-to-exact conversions remain opt-in.
- Moved exact `mpz_class(mpq_class)` and assignment definitions from the MPF
  implementation path into `zq_impl.hpp`, allowing `mpfrxx_mkII.h` to use
  exact Z/Q conversions without including MPF implementation.
- Split mpz expression construction so mpz-result expressions remain implicit
  while mpq-result expressions are explicit, avoiding upstream t-mix overload
  ambiguity for `-q`.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_type_conversions.cpp
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/cxx/t-mix.cc
- rg -n "mixed|mpq_class|mpz_class|mpfr_class|operator[+*/%-]|common_type" tests include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp
- cp ../gmpxx_mkII/cxx/t-mix.cc /tmp/t-mix-mpfrxx-mkII.cc
- perl -0pi -e '...' /tmp/t-mix-mpfrxx-mkII.cc
- g++ -std=c++17 -Iinclude /tmp/t-mix-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-mix-mpfrxx-mkII
- /tmp/t-mix-mpfrxx-mkII
- cmake --build build -j --target test_mpfr_type_conversions
- ctest --test-dir build -R test_mpfr_type_conversions --output-on-failure
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- cmake --build build -j --target test_mpz_basic test_mpfr_type_conversions
- ctest --test-dir build -R 'test_mpz_basic|test_mpfr_type_conversions' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check
- diff -u ../gmpxx_mkII/cxx/t-mix.cc /tmp/t-mix-mpfrxx-mkII.cc

Pass/fail result:
- Initial t-mix MPFR adaptation build: FAIL, exact-to-MPFR promotion and
  MPFR-to-exact explicit conversion surface were incomplete; exact
  `mpz_class(mpq_class)` also linked only through the MPF implementation path.
- t-mix MPFR adaptation after wrapper changes: PASS.
- cmake --build build -j --target test_mpfr_type_conversions: PASS.
- ctest --test-dir build -R test_mpfr_type_conversions --output-on-failure:
  PASS, 1/1 tests passed.
- First full cmake --build build -j after t-mix changes: FAIL, `abs(-mpz)`
  and `sgn(-mpz)` became ambiguous after mpq expression construction accepted
  mpz-result expressions.
- Added expression-node `abs` and `sgn` overloads keyed by result_type.
- cmake --build build -j --target test_mpz_basic test_mpfr_type_conversions:
  PASS.
- ctest --test-dir build -R 'test_mpz_basic|test_mpfr_type_conversions' --output-on-failure:
  PASS, 2/2 tests passed.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Final cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 119/119 tests passed.
- git diff --check: PASS.
- diff -u ../gmpxx_mkII/cxx/t-mix.cc /tmp/t-mix-mpfrxx-mkII.cc:
  PASS, expected nonzero diff status because the adapted file differs.

Known issues:
- The temporary adaptation uses explicit casts for MPFR-to-exact conversions
  (`static_cast<mpz_class>(mpfr_class(...))` and `static_cast<mpq_class>(...)`)
  because this repo keeps MPFR-to-exact conversion opt-in rather than implicit.

Post-phase MPFR default_prec alias:
DONE

Implemented features:
- Added `mpfrxx::default_prec()` as a source-compatible alias for
  `mpfrxx::default_precision_bits()`.
- Kept `default_precision_bits()` unchanged; the new name is an inline
  header-only API addition and does not remove or rename the existing API.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_defaults.cpp
- STATUS.md

Exact commands run:
- rg -n "default_precision_bits|default_prec|get_default_prec|set_default_precision|default_rounding" include tests examples STATUS.md
- sed -n '200,275p' include/gmpfrxx_mkII/detail/environment.hpp
- sed -n '1,120p' tests/test_mpfr_defaults.cpp
- cmake --build build -j --target test_mpfr_defaults test_mpfr_type_conversions
- ctest --test-dir build -R 'test_mpfr_defaults|test_mpfr_type_conversions' --output-on-failure
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpfr_defaults test_mpfr_type_conversions: PASS.
- ctest --test-dir build -R 'test_mpfr_defaults|test_mpfr_type_conversions' --output-on-failure: PASS, 2/2 tests passed.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 119/119 tests passed.

Known issues:
- `mpfrxx::set_default_precision_bits()` remains the setter name for now;
  no `set_default_prec()` alias was added in this phase.

Post-phase upstream t-misc MPFR adaptation:
DONE

Implemented features:
- Added GMP-compatible MPFR accessor/fits members:
  `mpfr_class::get_d`, `get_si`, `get_ui`, `fits_sint_p`,
  `fits_uint_p`, `fits_slong_p`, `fits_ulong_p`, `fits_sshort_p`, and
  `fits_ushort_p`.
- These wrappers delegate to the corresponding MPFR C APIs using the current
  mpfrxx default rounding policy.
- Verified a minimally adapted copy of upstream `gmpxx_mkII/cxx/t-misc.cc`
  against this repo's `mpfrxx_mkII.h` for mpz/mpq/mpfr coverage.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_type_conversions.cpp
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/cxx/t-misc.cc
- sed -n '261,620p' ../gmpxx_mkII/cxx/t-misc.cc
- rg -n "fits_|get_str|get_d|mpfr_fits|operator!|sgn|swap|canonical|misc" include tests STATUS.md
- sed -n '250,470p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "numeric_limits<|fits_sint_p|fits_uint_p|fits_slong_p|fits_ulong_p|fits_sshort_p|fits_ushort_p|set_prec|get_prec|get_ui|get_si" include/gmpfrxx_mkII/detail include/*.h
- cp ../gmpxx_mkII/cxx/t-misc.cc /tmp/t-misc-mpfrxx-mkII.cc
- perl -0pi -e '...' /tmp/t-misc-mpfrxx-mkII.cc
- g++ -std=c++17 -Iinclude /tmp/t-misc-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-misc-mpfrxx-mkII
- /tmp/t-misc-mpfrxx-mkII
- cmake --build build -j --target test_mpfr_type_conversions
- ctest --test-dir build -R test_mpfr_type_conversions --output-on-failure
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check
- diff -u ../gmpxx_mkII/cxx/t-misc.cc /tmp/t-misc-mpfrxx-mkII.cc

Pass/fail result:
- First t-misc MPFR adaptation build: FAIL, `mpfr_class` lacked
  GMP-compatible `fits_*` and `get_d/get_si/get_ui` members.
- t-misc MPFR adaptation after adding members: PASS.
- cmake --build build -j --target test_mpfr_type_conversions: PASS.
- ctest --test-dir build -R test_mpfr_type_conversions --output-on-failure:
  PASS, 1/1 tests passed.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 119/119 tests passed.
- git diff --check: PASS.
- diff -u ../gmpxx_mkII/cxx/t-misc.cc /tmp/t-misc-mpfrxx-mkII.cc:
  PASS, expected nonzero diff status because the adapted file differs.

Known issues:
- The temporary t-misc adaptation keeps this repo's ABI policy that
  `std::numeric_limits` is not specialized for wrapper classes; this differs
  from upstream GMP's historical t-misc expectations.
- MPFR `get_str(exp, base, 0)` returns precision-dependent digits, so the
  temporary MPFR adaptation uses explicit `n_digits` for the upstream MPF
  `get_str` assertions.

Post-phase MPFR raw stream extraction/insertion:
DONE

Implemented features:
- Added global `std::istream& operator>>(std::istream&, mpfr_ptr)` for raw
  initialized MPFR values, matching the GMP raw pointer extraction style.
- Added global `std::ostream& operator<<(std::ostream&, mpfr_srcptr)` for raw
  initialized MPFR values, matching the GMP raw pointer insertion style.
- Raw MPFR extraction preserves the destination precision, uses the current
  mpfrxx rounding/exponent context when committing a parsed value, and leaves
  the destination unchanged on failed input.
- Verified the upstream `../gmpxx_mkII/cxx/t-istream.cc` intent against
  `mpfrxx_mkII.h` with the MPF section adapted to raw `mpfr_t`.
- Verified the upstream `../gmpxx_mkII/cxx/t-locale.cc` intent against
  `mpfrxx_mkII.h` with the MPF section adapted to raw `mpfr_t`.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_string_io.cpp
- STATUS.md

Exact commands run:
- sed -n '720,785p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1085,1130p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,140p' tests/test_mpfr_string_io.cpp
- sed -n '1130,1165p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '140,260p' tests/test_mpfr_string_io.cpp
- cmake --build build -j --target test_mpfr_string_io
- ctest --test-dir build -R test_mpfr_string_io --output-on-failure
- cp ../gmpxx_mkII/cxx/t-istream.cc /tmp/t-istream-mpfrxx-mkII.cc
- g++ -std=c++17 -Iinclude /tmp/t-istream-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-istream-mpfrxx-mkII
- /tmp/t-istream-mpfrxx-mkII
- diff -u ../gmpxx_mkII/cxx/t-istream.cc /tmp/t-istream-mpfrxx-mkII.cc
- rg -n "operator<<\\(std::ostream&.*mpfr|mpfr_srcptr|operator>>\\(std::istream&.*mpfr_ptr|print_mpfr" include tests
- cp ../gmpxx_mkII/cxx/t-locale.cc /tmp/t-locale-mpfrxx-mkII.cc
- g++ -std=c++17 -Iinclude /tmp/t-locale-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-locale-mpfrxx-mkII
- /tmp/t-locale-mpfrxx-mkII
- diff -u ../gmpxx_mkII/cxx/t-locale.cc /tmp/t-locale-mpfrxx-mkII.cc
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check
- git status --short

Pass/fail result:
- cmake --build build -j --target test_mpfr_string_io: PASS.
- ctest --test-dir build -R test_mpfr_string_io --output-on-failure: PASS, 1/1 test passed.
- g++ -std=c++17 -Iinclude /tmp/t-istream-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-istream-mpfrxx-mkII: PASS.
- /tmp/t-istream-mpfrxx-mkII: PASS.
- g++ -std=c++17 -Iinclude /tmp/t-locale-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-locale-mpfrxx-mkII: PASS.
- /tmp/t-locale-mpfrxx-mkII: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 119/119 tests passed.
- git diff --check: PASS.

Known issues:
- Raw `mpfr_ptr` extraction and `mpfr_srcptr` insertion require initialized
  `mpfr_t` values, as with MPFR C API usage generally.

Post-phase std::common_type promotion parity:
DONE

Implemented features:
- Added public `std::common_type` specializations for GMP promotion:
  `mpz_class -> mpq_class -> mpf_class -> mpfc_class`.
- Added public `std::common_type` specializations for MPFR/MPC promotion:
  `mpz_class/mpq_class -> mpfr_class -> mpc_class`.
- Kept forbidden cross-domain promotion absent:
  `gmpxx::mpf_class` / `gmpxx::mpfc_class` do not gain common types with
  `mpfrxx::mpfr_class` / `mpfrxx::mpc_class`.
- Updated ABI fingerprint expectations from "no common_type" to the new
  promotion policy.

Tests added:
- tests/test_common_type.cpp

Tests updated:
- tests/CMakeLists.txt
- tests/test_abi_fingerprint.cpp
- STATUS.md

Exact commands run:
- sed -n '1,130p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1,140p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,150p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '70,225p' tests/test_abi_fingerprint.cpp
- sed -n '1,90p' include/gmpxx_mkII.h
- sed -n '1,90p' include/mpfrxx_mkII.h
- sed -n '1,90p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '1,110p' tests/CMakeLists.txt
- rg -n "} // namespace gmpxx|} // namespace mpfrxx|class mpfc_class|class mpc_class|class mpq_class|class mpz_class" include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '900,980p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '250,340p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '160,245p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- sed -n '780,835p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '160,215p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '445,500p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "mpf_class\\s*\\{|private:|mpf_t value_|};" include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '505,545p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1138,1165p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target test_common_type test_abi_fingerprint
- ctest --test-dir build -R "test_common_type|test_abi_fingerprint" --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check
- git status --short

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j --target test_common_type test_abi_fingerprint: PASS.
- ctest --test-dir build -R "test_common_type|test_abi_fingerprint" --output-on-failure: PASS, 2/2 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 119/119 tests passed.
- git diff --check: PASS.

Known issues:
- `std::common_type` is intentionally not provided for forbidden GMP/MPFR
  cross-domain pairs such as `gmpxx::mpf_class` with `mpfrxx::mpfr_class`.

Post-phase upstream t-binary auto-base assignment and MPFR shift parity:
DONE

Implemented features:
- Updated `mpz_class` and `mpq_class` string assignment from `const char*`
  and `std::string` to use GMP base 0 parsing, so assignment such as
  `want = "0x1000000000000000000000cafe";` works without adapting the
  upstream `t-binary.cc` assertion.
- Updated `mpf_class` string assignment to use `mpf_set_str(..., 0)` and
  report invalid inputs through `std::invalid_argument`.
- Updated `mpfr_class` string assignment to use MPFR base 0 parsing, including
  C99-style hexadecimal floating input accepted by MPFR such as `0x1p+5`.
- Added MPFR `operator<<`, `operator>>`, `operator<<=`, and `operator>>=`
  as expression-template power-of-two scaling operations backed by
  `mpfr_mul_2ui` / `mpfr_div_2ui`.
- Verified minimally adapted upstream `../gmpxx_mkII/cxx/t-binary.cc` against
  both `gmpxx_mkII.h` and `mpfrxx_mkII.h`. The GMP-header adaptation keeps
  the upstream `want = "0x1000000000000000000000cafe";` line unchanged.

Tests added:
- None.

Tests updated:
- tests/test_zq_string_io.cpp
- tests/test_mpf_string_io.cpp
- tests/test_mpfr_string_io.cpp
- tests/test_mpfr_power_of_two_fusion.cpp
- STATUS.md

Exact commands run:
- rg -n "operator<<|operator>>|shl_op|shr_op|mpfr_apply|operator\\+=|namespace mpfrxx" include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "operator<<|operator>>|shl_op|shr_op" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/expr.hpp
- rg -n "power_of_two|shl|shr|<<|>>" tests/test_mpfr_power_of_two_fusion.cpp tests/test_mpfr_scalar_eval.cpp tests/test_et_contract_mpfr.cpp
- sed -n '1240,1385p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1510,1745p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1428,1625p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1390,1445p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,180p' tests/test_mpfr_power_of_two_fusion.cpp
- cmake --build build -j --target test_mpfr_power_of_two_fusion test_zq_string_io test_mpf_string_io test_mpfr_string_io
- ctest --test-dir build -R "test_mpfr_power_of_two_fusion|test_zq_string_io|test_mpf_string_io|test_mpfr_string_io" --output-on-failure
- g++ -std=c++17 -Iinclude /tmp/t-binary-gmpxx-mkII.cc -lgmp -o /tmp/t-binary-gmpxx-mkII
- g++ -std=c++17 -Iinclude /tmp/t-binary-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-binary-mpfrxx-mkII
- /tmp/t-binary-gmpxx-mkII
- /tmp/t-binary-mpfrxx-mkII
- diff -u ../gmpxx_mkII/cxx/t-binary.cc /tmp/t-binary-gmpxx-mkII.cc
- diff -u ../gmpxx_mkII/cxx/t-binary.cc /tmp/t-binary-mpfrxx-mkII.cc
- git diff -- include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_zq_string_io.cpp tests/test_mpf_string_io.cpp tests/test_mpfr_string_io.cpp tests/test_mpfr_power_of_two_fusion.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake --build build -j --target test_mpfr_power_of_two_fusion test_zq_string_io test_mpf_string_io test_mpfr_string_io: PASS.
- ctest --test-dir build -R "test_mpfr_power_of_two_fusion|test_zq_string_io|test_mpf_string_io|test_mpfr_string_io" --output-on-failure: PASS, 4/4 tests passed.
- g++ -std=c++17 -Iinclude /tmp/t-binary-gmpxx-mkII.cc -lgmp -o /tmp/t-binary-gmpxx-mkII: PASS.
- g++ -std=c++17 -Iinclude /tmp/t-binary-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-binary-mpfrxx-mkII: PASS.
- /tmp/t-binary-gmpxx-mkII: PASS.
- /tmp/t-binary-mpfrxx-mkII: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 118/118 tests passed.
- git diff --check: PASS.

Known issues:
- `mpf_class` string assignment follows GMP `mpf_set_str(..., 0)` semantics;
  unlike MPFR, GMP MPF does not accept C99 hexadecimal floating literals such
  as `0x1p+5`.

Post-phase MPFR nextafter alias:
DONE

Implemented features:
- Added `mpfrxx::nextafter(x, y)` as a thin public alias for
  `mpfrxx::nexttoward(x, y)`, matching the standard `nextafter` naming while
  preserving MPFR `mpfr_nexttoward` semantics.
- The result keeps the source operand precision because the implementation
  delegates to the existing `nexttoward` wrapper.

Tests added:
- None.

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/test_mpfr_comparisons.cpp
- STATUS.md

Exact commands run:
- sed -n '1938,1982p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '135,205p' tests/test_mpfr_comparisons.cpp
- cmake --build build -j --target test_mpfr_comparisons
- ctest --test-dir build -R test_mpfr_comparisons --output-on-failure
- cmake --build build -j
- git diff --check
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpfr_comparisons: PASS.
- ctest --test-dir build -R test_mpfr_comparisons --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- git diff --check: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for this phase.

Post-phase MPFR round-to-integer helpers:
DONE

Implemented features:
- Added MPFR-native `mpfrxx::ceil`, `mpfrxx::floor`, and `mpfrxx::trunc`
  wrappers that delegate to `mpfr_ceil`, `mpfr_floor`, and `mpfr_trunc`.
- The wrappers accept MPFR objects and MPFR expression operands, materialize at
  the operand precision, and intentionally do not add an MPFR `epsilon` helper.
- Documented that MPF `set_epsilon()` remains GMP/upstream compatibility
  surface; MPFR should use MPFR-native next/rounding helpers instead.

Tests added:
- None.

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- rg -n "ceil|floor|trunc|mpfr_ceil|mpfr_floor|mpfr_trunc|epsilon|set_epsilon" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp tests/test_mpfr_comparisons.cpp STATUS.md
- rg -n "mpfr_(ceil|floor|trunc)" /usr/include/mpfr.h /usr/local/include/mpfr.h
- cmake --build build -j --target test_mpfr_math
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- cmake --build build -j
- git diff --check
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpfr_math: PASS.
- ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- git diff --check: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for `ceil`/`floor`/`trunc`. The broader `test_mpf_math_functions.cpp`
  MPFR adaptation row still tracks remainder/scaling/exact-helper policy
  decisions separately.

Post-phase MPFR remainder and scaling helpers:
DONE

Implemented features:
- Added MPFR-native remainder wrappers:
  - `mpfrxx::remainder(x, y)` delegating to `mpfr_remainder`.
  - `mpfrxx::fmod(x, y)` delegating to `mpfr_fmod`.
  - `mpfrxx::remquo(x, y)` delegating to `mpfr_remquo` and returning
    `std::pair<mpfr_class, long>`.
- Added MPFR-native power-of-two scaling wrappers:
  - `mpfrxx::mul_2ui`
  - `mpfrxx::div_2ui`
  - `mpfrxx::mul_2si`
  - `mpfrxx::div_2si`
- Kept `ldexp` out of the public surface for now; the MPFR C API names are the
  canonical policy surface.
- Added exact-helper style checks for expression operands and destination
  precision preservation without adding an MPFR `epsilon` helper.

Tests added:
- None.

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- rg -n "remainder|fmod|remquo|mul_2|div_2|ldexp|mpfr_remainder|mpfr_fmod|mpfr_remquo|mpfr_mul_2|mpfr_div_2" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_mpf_math_functions.cpp
- rg -n "mpfr_(remainder|fmod|remquo|mul_2ui|div_2ui|mul_2si|div_2si)" /usr/include/mpfr.h
- cmake --build build -j --target test_mpfr_math
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- cmake --build build -j --target test_mpfr_math
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- cmake --build build -j
- git diff --check
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpfr_math: PASS.
- ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- git diff --check: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for MPFR remainder/scaling. Remaining MPFR adaptation work for
  `test_mpf_math_functions.cpp` is limited to deciding whether any additional
  exact-helper aliases such as a public `neg` wrapper are worth adding, since
  the natural MPFR APIs now cover ceil/floor/trunc, remainder, scaling, sign,
  sqrt/sqr, hypot, expression operands, and precision preservation.

Post-phase MPFR extended transcendent focused coverage:
DONE

Implemented features:
- Added focused MPFR coverage mirroring the upstream MPF extended transcendent
  assertion intent where it naturally maps to MPFR:
  - `const_e`, `const_log10`, `pi_over_two`, and derived constant identities;
  - inverse trigonometric identities;
  - hyperbolic and inverse hyperbolic identities;
  - `exp2`/`exp10` base-variant identities;
  - `gamma`, `lngamma`, `reciprocal_gamma`, and gamma recurrence checks;
  - expression-operand checks for the extended unary function family;
  - result precision checks for the same family.
- Added MPFR convenience wrappers requested for MPF parity:
  - `mpfrxx::const_e`
  - `mpfrxx::const_log10`
  - `mpfrxx::pi_over_two`
  - `mpfrxx::reciprocal_gamma`
- Added deterministic random double smoke coverage for MPFR transcendental
  wrappers, checking generated values against `std::` double functions with a
  `1e-15` scaled tolerance.

Tests added:
- None.

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- rg -n "const_e|const_log10|pi_over_two|reciprocal_gamma|const_pi|const_log2|const_catalan|const_euler|gamma" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp include/gmpfrxx_mkII/detail/math_mpf.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1700,1795p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,260p' tests/test_mpf_extended_transcendent_functions.cpp
- sed -n '260,380p' tests/test_mpf_extended_transcendent_functions.cpp
- rg -n "const_|asin|acos|atan|sinh|cosh|tanh|asinh|acosh|atanh|exp|gamma|lngamma|precision|expression|check_unary|test_" tests/test_mpfr_math.cpp
- rg -n "reciprocal_gamma|recip|gamma" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp /usr/include/mpfr.h
- cmake --build build -j --target test_mpfr_math
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- cmake --build build -j
- git diff --check
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpfr_math: PASS.
- ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- git diff --check: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for the natural MPFR adaptation of the upstream MPF extended
  transcendent assertion list. MPFR `epsilon` remains intentionally absent.

Post-phase MPFR neg helper:
DONE

Implemented features:
- Added `mpfrxx::neg(expr)` as a direct MPFR-backed helper using `mpfr_neg`.
- Accepted MPFR objects and MPFR expression operands through the same focused
  MPFR unary helper path as the surrounding math wrappers.
- Preserved the materialized operand precision for expression inputs.

Tests added:
- None.

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- rg -n "test_mpf_math_functions|Post-phase MPFR|MPFR neg|remaining|Remaining|test_mpf_transcendent" STATUS.md
- git diff -- include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp
- cmake --build build -j --target test_mpfr_math && ctest --test-dir build -R test_mpfr_math --output-on-failure
- cmake --build build -j
- git diff --check
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpfr_math and focused CTest: PASS,
  1/1 test passed.
- cmake --build build -j: PASS.
- git diff --check: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for `mpfrxx::neg`.

Post-phase non-canonical mpq stream extraction:
DONE

Implemented features:
- Changed `mpq_class` stream extraction to preserve the rational read by
  `mpq_set_str` instead of routing through `mpq_class::set_str`, which
  canonicalizes.
- Changed raw `mpq_ptr` stream extraction the same way.
- Kept constructor and explicit `set_str` behavior canonicalized; only
  formatted input now preserves non-canonical rationals.
- Verified upstream `cxx/t-istream.cc` with minimal local edits against both
  `gmpxx_mkII.h` and `mpfrxx_mkII.h`.

Tests added:
- None.

Tests updated:
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- tests/test_zq_string_io.cpp
- STATUS.md

Exact commands run:
- rg -n "class mpq_class|set_str\\(|canonical|operator>>\\(std::istream& in, mpq|mpq_set_str|mpq_canonicalize" include/gmpfrxx_mkII/detail/zq_impl.hpp tests/test_zq_string_io.cpp tests/test_mpq_canonicalization.cpp
- cmake --build build -j --target test_zq_string_io
- ctest --test-dir build -R test_zq_string_io --output-on-failure
- gdb -batch -ex run -ex bt --args build/tests/test_zq_string_io
- g++ -std=c++17 -Iinclude /tmp/t-istream-gmpxx-mkII.cc -lgmp -o /tmp/t-istream-gmpxx-mkII && /tmp/t-istream-gmpxx-mkII && /tmp/t-istream-gmpxx-mkII -s
- g++ -std=c++17 -Iinclude /tmp/t-istream-mpfrxx-mkII.cc -lgmp -lmpfr -lmpc -o /tmp/t-istream-mpfrxx-mkII && /tmp/t-istream-mpfrxx-mkII && /tmp/t-istream-mpfrxx-mkII -s
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial test_zq_string_io after preserving non-canonical rationals: FAIL,
  expected decimal `10/20` for hex input instead of the correct non-canonical
  decimal value `16/32`.
- test_zq_string_io after expectation update: PASS.
- Minimal upstream `t-istream.cc` adaptation for `gmpxx_mkII.h`: PASS,
  including `-s`.
- Minimal upstream `t-istream.cc` adaptation for `mpfrxx_mkII.h`: PASS,
  including `-s`.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 118/118 tests passed.

Known issues:
- None for stream extraction. Non-canonical rationals can now be held after
  formatted input; callers can still call `canonicalize()` explicitly.

Post-phase MPF transcendent test rename:
DONE

Implemented features:
- Renamed the local MPF transcendental focused test from
  `tests/test_mpf_compute_log.cpp` to
  `tests/test_mpf_transcendent_functions.cpp` so the repository test name
  matches the upstream source test name.
- Renamed the CTest/CMake target from `test_mpf_compute_log` to
  `test_mpf_transcendent_functions`.

Tests added:
- None.

Tests updated:
- tests/test_mpf_transcendent_functions.cpp
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- rg -n "test_mpf_compute_log|mpf_compute_log" .
- sed -n '1,90p' tests/CMakeLists.txt
- sed -n '2658,2682p' STATUS.md
- sed -n '2864,2870p' STATUS.md
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target test_mpf_transcendent_functions
- ctest --test-dir build -R test_mpf_transcendent_functions --output-on-failure
- cmake --build build -j
- rg -n "test_mpf_compute_log" tests CMakeLists.txt STATUS.md
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j --target test_mpf_transcendent_functions: PASS.
- ctest --test-dir build -R test_mpf_transcendent_functions --output-on-failure:
  PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for the rename.

Post-phase MPFR transcendent focused test:
DONE

Implemented features:
- Added `tests/test_mpfr_transcendent_functions.cpp` as the MPFR-side focused
  counterpart to `tests/test_mpf_transcendent_functions.cpp`.
- Added deterministic random smoke coverage comparing MPFR wrapper results
  against the corresponding `std::` double functions with a `1e-15` scaled
  tolerance.
- Covered trigonometric, unit/pi trigonometric, hyperbolic, log/exp, power,
  root/hypot, gamma, reciprocal-gamma, and erf/erfc wrapper families.
- Registered the new `test_mpfr_transcendent_functions` CTest target.

Tests added:
- tests/test_mpfr_transcendent_functions.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- rg -n "double.*smoke|mt19937|1e-15|assert_double_close|std::(sin|cos|exp|log|pow|gamma)|test_double" tests/test_mpfr_math.cpp tests/test_mpf_transcendent_functions.cpp tests/CMakeLists.txt
- sed -n '1100,1235p' tests/test_mpfr_math.cpp
- rg -n "double to_double|to_double\\(|get_d\\(" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr*.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target test_mpfr_transcendent_functions
- ctest --test-dir build -R test_mpfr_transcendent_functions --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial ctest --test-dir build -R test_mpfr_transcendent_functions
  --output-on-failure: FAIL, `tanpi` random inputs included a high-condition
  tangent region that exceeded `1e-15`.
- After narrowing the `tanpi` smoke input range:
  - cmake --build build -j --target test_mpfr_transcendent_functions: PASS.
  - ctest --test-dir build -R test_mpfr_transcendent_functions
    --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 118/118 tests passed.

Known issues:
- None for the MPFR transcendent focused smoke test.

Post-phase scalar allocation fast path:
DONE

Implemented features:
- Added MPF integer scalar evaluation fast paths that use `mpf_set_si` when
  `long` covers int64_t and `mpf_set_ui` when `unsigned long` covers uint64_t.
- Added MPFR integer scalar evaluation fast paths that use `mpfr_set_si` and
  `mpfr_set_ui` under the same ABI-width conditions.
- Kept the existing `gmpxx::mpz_class` fallback on narrower ABIs so full
  int64_t/uint64_t exactness is preserved.
- Updated scalar allocation-count tests to document the reduced LP64 counts:
  direct integer scalar expressions are allocation-free, nested scalar
  expressions allocate one temporary, and compound aliasing expressions allocate
  two temporaries.
- Added explicit int64_t-min and uint64_t-max allocation checks on ABIs where
  direct long/unsigned long fast paths are exact.
- Updated the upstream migration table row for `test_scalar_alloc_count.cpp`
  from TODO/current-policy to Done.

Tests added:
- None.

Tests updated:
- tests/test_mpf_scalar_alloc_count.cpp
- tests/test_mpfr_scalar_alloc_count.cpp
- include/gmpfrxx_mkII/detail/mpf_impl.hpp
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- STATUS.md

Exact commands run:
- sed -n '1,260p' tests/test_mpf_scalar_alloc_count.cpp
- sed -n '1,280p' tests/test_mpfr_scalar_alloc_count.cpp
- sed -n '1280,1345p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1160,1235p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '28,55p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '28,55p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "#include <limits>" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build -j --target test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count
- ctest --test-dir build -R 'test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count' --output-on-failure
- cmake --build build -j --target test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count
- ctest --test-dir build -R 'test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count' --output-on-failure
- cmake --build build -j --target test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count
- ctest --test-dir build -R 'test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count' --output-on-failure
- cmake --build build -j --target test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count
- ctest --test-dir build -R 'test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count' --output-on-failure
- tail -n 80 STATUS.md
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- Initial focused CTest after fast path: FAIL because direct scalar count
  dropped from 1 to 0; test expectation updated.
- Second focused CTest: FAIL because nested scalar count dropped from 2 to 1;
  test expectation updated.
- Third focused CTest: FAIL because compound scalar count dropped from 3 to 2;
  test expectation updated.
- Final focused CTest: PASS, 2/2 tests passed.
- Full build: PASS.
- Full CTest: PASS, 117/117 tests passed.
- git diff --check: PASS.

Known issues:
- None for this phase.

Post-phase scalar arithmetic focused migration:
DONE

Implemented features:
- Completed the focused migration row for `test_scalar_arithmetic.cpp`.
- Added `gmpxx::mpf_class` pre/post increment and decrement operators that
  preserve destination precision.
- Added `mpfrxx::mpfr_class` pre/post increment and decrement operators as the
  natural MPFR adaptation, also preserving destination precision.
- Extended MPF scalar arithmetic coverage to mirror the upstream matrix:
  signed and unsigned integer edge values, `uint64_t` max, float/double scalar
  values including tiny and huge exponents, scalar-left and scalar-right
  arithmetic, destination assignment precision preservation, expression plus
  scalar shapes, and inc-dec precision preservation.
- Extended MPFR scalar arithmetic coverage with the same natural matrix using
  direct MPFR C API references.
- Updated the upstream migration table row for `test_scalar_arithmetic.cpp` to
  Done for current policy.

Tests added:
- None.

Tests updated:
- tests/test_mpf_basic.cpp
- tests/test_mpfr_scalar_eval.cpp
- include/gmpfrxx_mkII/detail/mpf_impl.hpp
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- STATUS.md

Exact commands run:
- sed -n '1,360p' ../gmpxx_mkII/tests/test_scalar_arithmetic.cpp
- sed -n '1,360p' tests/test_mpf_basic.cpp
- sed -n '1,360p' tests/test_mpfr_scalar_eval.cpp
- rg -n "operator\\+\\+\\(mpf|operator--\\(mpf|operator\\+\\+\\(mpfr|operator--\\(mpfr|scalar_normalize|normalized_mpf_scalar|normalized_mpfr_scalar|operator\\+=\\(mpf|operator\\+=\\(mpfr" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/*.hpp
- sed -n '1110,1265p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1000,1085p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1265,1715p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1085,1575p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1715,1755p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1575,1605p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build -j --target test_mpf_basic test_mpfr_scalar_eval
- ctest --test-dir build -R 'test_mpf_basic|test_mpfr_scalar_eval' --output-on-failure
- tail -n 80 STATUS.md
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpf_basic test_mpfr_scalar_eval: PASS.
- ctest --test-dir build -R 'test_mpf_basic|test_mpfr_scalar_eval'
  --output-on-failure: PASS, 2/2 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for this phase.

Post-phase precision policy focused migration:
DONE

Implemented features:
- Completed the focused migration row for `test_precision_policy.cpp`.
- Added `.eval()` materialization helpers to arithmetic unary and binary
  expression nodes.
- Added `gmpxx::mpf_class::set_prec()` and `set_prec_raw()` wrappers matching
  GMP's `mpf_set_prec` and `mpf_set_prec_raw` behavior.
- Added `mpfrxx::mpfr_class::set_prec()` as the natural MPFR adaptation using
  `mpfr_prec_round` with the current wrapper rounding/exponent context.
- Extended MPF precision-policy tests for expression construction precision,
  destination assignment precision preservation, `.eval()` materialization,
  compound-assignment precision preservation, `set_prec`, and `set_prec_raw`
  against direct GMP C API references.
- Extended MPFR precision-policy tests with the natural MPFR counterparts for
  expression construction precision, destination assignment precision
  preservation, `.eval()` materialization, compound-assignment precision
  preservation, and `set_prec` against direct MPFR C API references.
- Updated the upstream migration table row for `test_precision_policy.cpp` to
  Done for current policy.

Tests added:
- None.

Tests updated:
- tests/test_mpf_precision_policy.cpp
- tests/test_mpfr_precision_policy.cpp
- STATUS.md

Exact commands run:
- sed -n '1,320p' ../gmpxx_mkII/tests/test_precision_policy.cpp
- sed -n '1,320p' tests/test_mpf_precision_policy.cpp
- sed -n '1,340p' tests/test_mpfr_precision_policy.cpp
- rg -n "set_prec|set_prec_raw|eval\\(|operator\\+=|precision\\(\\)|get_prec" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpf_precision_policy.cpp tests/test_mpfr_precision_policy.cpp
- rg -n "struct .*expr|eval\\(" include/gmpfrxx_mkII/detail/expr.hpp include/gmpfrxx_mkII/detail/*.hpp
- sed -n '1,150p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '300,340p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '270,310p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "set_prec|set_prec_raw" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build -j --target test_mpf_precision_policy test_mpfr_precision_policy
- ctest --test-dir build -R 'test_mpf_precision_policy|test_mpfr_precision_policy' --output-on-failure
- tail -n 90 STATUS.md
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j --target test_mpf_precision_policy
  test_mpfr_precision_policy: PASS.
- ctest --test-dir build -R
  'test_mpf_precision_policy|test_mpfr_precision_policy'
  --output-on-failure: PASS, 2/2 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for this phase.

Post-phase exact arithmetic matrix focused migration:
DONE

Implemented features:
- Completed the focused migration rows for `test_mixed_type_arithmetic.cpp`,
  `test_mpq_arithmetic.cpp`, and `test_mpz_arithmetic.cpp`.
- Extended exact Z/Q scalar expression support to `float`/`double` while
  keeping `long double`, `bool`, and `__int128` rejected as expression scalar
  leaves by policy.
- Removed eager `mpz_class op double` binary overloads so exact double scalar
  arithmetic now forms expression nodes and materializes only at construction,
  assignment, comparison, or explicit evaluation boundaries.
- Evaluates exact floating scalar leaves as truncated integers for MPZ-result
  expressions and as exact GMP rationals for MPQ-result expressions and exact
  Z/Q comparisons.
- Added upstream-style MPZ coverage for double scalar ET, bitwise double
  operands, shifts, inc/dec, nested expression shapes, and integer helper
  functions.
- Added upstream-style MPQ coverage for double scalar ET, shifts, inc/dec,
  mixed exact promotion, accessors, nested expressions, compound assignment,
  and unary signs.
- Added upstream-style mixed MPF/MPFR coverage for exact operands, negative
  rational cases, exact Z/Q double scalar leaves, DBL edge smoke checks,
  precision-preserving assignment, and power-of-two shifts.

Tests added:
- None.

Tests updated:
- tests/test_mixed_type_arithmetic.cpp
- tests/test_mpq_arithmetic.cpp
- tests/test_mpz_arithmetic.cpp
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/tests/test_mixed_type_arithmetic.cpp
- sed -n '1,320p' ../gmpxx_mkII/tests/test_mpq_arithmetic.cpp
- sed -n '1,360p' ../gmpxx_mkII/tests/test_mpz_arithmetic.cpp
- sed -n '1,320p' tests/test_mixed_type_arithmetic.cpp
- sed -n '1,360p' tests/test_mpq_arithmetic.cpp
- sed -n '1,420p' tests/test_mpz_arithmetic.cpp
- rg -n "gcd\\(|lcm\\(|factorial|primorial|fibonacci|operator%|operator&|operator\\^|operator\\||operator<<|operator>>|operator\\+\\+|operator--" include/gmpfrxx_mkII/detail/zq_impl.hpp tests
- rg -n "double|long double|is_supported_expression_integral|is_zq_scalar|is_zq.*scalar" include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/type_traits.hpp tests/compile_fail
- sed -n '1210,1460p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1460,1820p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '2180,2280p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "operator[=!<>]=?\\(.*double|double.*operator[=!<>]|cmp\\(.*double|double.*cmp" include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp
- rg -n "comparison|cmp\\(|operator==|is_mpf_comparison" include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1560,1628p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '2140,2205p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "mpz_class\\(const Expr|mpz_class\\(.*Expr|template <.*Expr" include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '380,405p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1908,1925p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "static mpz_class (factorial|primorial|fibonacci)|class mpz_class" include/gmpfrxx_mkII/detail/zq_impl.hpp
- cmake --build build -j --target test_mpz_arithmetic test_mpq_arithmetic test_mixed_type_arithmetic
- cmake --build build -j --target test_mpz_arithmetic test_mpq_arithmetic test_mixed_type_arithmetic
- ctest --test-dir build -R 'test_mpz_arithmetic|test_mpq_arithmetic|test_mixed_type_arithmetic' --output-on-failure
- tail -n 110 STATUS.md
- cmake --build build -j
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial focused build: FAIL because helper code used nonexistent member
  `mpz_class::sgn()`; changed it to `mpz_sgn`.
- Second focused build: FAIL because MPQ unary sign tests used nonexistent
  member `mpq_class::sgn()`; changed them to `gmpxx::sgn`.
- cmake --build build -j --target test_mpz_arithmetic test_mpq_arithmetic
  test_mixed_type_arithmetic: PASS.
- ctest --test-dir build -R
  'test_mpz_arithmetic|test_mpq_arithmetic|test_mixed_type_arithmetic'
  --output-on-failure: PASS, 3/3 tests passed.
- Initial full build: FAIL because `test_abi_fingerprint.cpp` still expected
  `double` not to be an exact Z/Q expression scalar; updated the ABI
  fingerprint to the new ET policy.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for this phase.

Post-phase type conversion and comparison focused migration:
DONE

Implemented features:
- Added focused GMP/MPF type-conversion coverage for raw `mpz_srcptr` and
  `mpq_srcptr` construction, string assignment, invalid-input preservation,
  exact int64_t/uint64_t and int128 paths, wrapper-to-wrapper conversions,
  explicit bool conversion, fit/accessor queries, and MPF destination precision
  preservation.
- Added `mpz_class`/`mpq_class` raw pointer constructors, string assignment
  operators, explicit bool conversion, and wrapper conversion assignment
  operators needed by the policy-compatible upstream conversion matrix.
- Added MPF precision constructors from `mpz_class` and `mpq_class`.
- Added MPFR natural conversion coverage for exact integer/rational aliases,
  raw `mpfr_srcptr` construction, string assignment with invalid-input
  preservation, destination precision preservation, and explicit bool
  conversion.
- Added `mpfr_class` construction and assignment from `gmpxx::mpz_class` and
  `gmpxx::mpq_class`, plus explicit bool conversion.
- Extended GMP and MPFR comparison tests with conversion-backed exact integer,
  rational, MPF, and MPFR cases, including int64_t/uint64_t edge values.
- Updated the upstream migration table rows for `test_type_conversions.cpp` and
  `test_comparisons.cpp` from Partial/follow-up to Done for current policy.

Missing features:
- None for the current type-conversion and comparison migration policy.
- Intentional differences remain documented in the migration table: bool
  construction is rejected, `mpfrxx_mkII.h` does not expose `gmpxx::mpf_class`,
  and MPFR does not mirror GMP-only MPF conversion forms.

Tests added:
- tests/test_type_conversions.cpp
- tests/test_mpfr_type_conversions.cpp

Tests updated:
- tests/CMakeLists.txt
- tests/test_comparisons.cpp
- tests/test_mpfr_comparisons.cpp
- STATUS.md

Exact commands run:
- rg -n "class mpfr_class|explicit operator bool|mpfr_srcptr|mpfr_data|default_rounding_mode|get_str|add_phase0_test\\(test_.*mpfr|test_type_conversions|test_comparisons" include tests STATUS.md
- git diff -- include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_type_conversions.cpp tests/test_comparisons.cpp tests/CMakeLists.txt
- rg -n "\\| test_.*\\|.*(Partial|TODO|未|partial|todo)|TODO|Partial|未完|follow-up" STATUS.md
- git status --short
- sed -n '1,260p' tests/test_type_conversions.cpp
- sed -n '1,280p' tests/test_comparisons.cpp
- sed -n '1,120p' tests/CMakeLists.txt
- sed -n '2260,2345p' STATUS.md
- sed -n '260,560p' tests/test_type_conversions.cpp
- sed -n '40,240p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "operator==|cmp\\(|is_mpfr_comparison|mpfr_cmp|comparison" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_comparisons.cpp
- rg -n "get_d\\(|get_ui\\(|get_si\\(|fits_.*_p|set\\(const char|set_str|operator=\\(const char|operator=\\(const std::string" include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '80,160p' tests/test_mpfr_comparisons.cpp
- sed -n '232,310p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "static mpfr_prec_t default_precision|default_precision\\(" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/environment.hpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target test_type_conversions test_mpfr_type_conversions test_comparisons test_mpfr_comparisons
- sed -n '488,512p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build -j --target test_type_conversions test_mpfr_type_conversions test_comparisons test_mpfr_comparisons
- ctest --test-dir build -R 'test_type_conversions|test_mpfr_type_conversions|test_comparisons|test_mpfr_comparisons' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- tail -n 80 STATUS.md

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Initial focused build: FAIL because `mpfr_class` did not yet expose explicit
  bool conversion; added it and rebuilt.
- cmake --build build -j --target test_type_conversions
  test_mpfr_type_conversions test_comparisons test_mpfr_comparisons: PASS.
- ctest --test-dir build -R
  'test_type_conversions|test_mpfr_type_conversions|test_comparisons|test_mpfr_comparisons'
  --output-on-failure: PASS, 4/4 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 117/117 tests passed.

Known issues:
- None for this phase.

Post-phase arithmetic matrix ET coverage:
DONE

Implemented features:
- Added `mpz_class` and `mpq_class` pre/post increment and decrement support.
- Added focused upstream-style arithmetic coverage for integer binary,
  integral scalar, remainder, bitwise, shift, increment/decrement, and nested
  expression shapes.
- Added focused rational arithmetic coverage for binary, integral scalar,
  shift, increment/decrement, and mixed `mpz/mpq` promotion.
- Added focused mixed-type coverage for MPF and MPFR interactions with
  `mpz_class`, `mpq_class`, and scalar leaves, including precision-preserving
  materialization and power-of-two shifts.
- Added compile-time ET contract assertions proving these public expression
  forming operators return expression nodes instead of eager public numeric
  objects.

Missing features:
- Exact `mpz_class`/`mpq_class` double-scalar parity with upstream
  `gmpxx_mkII` remains intentionally outside this phase because the current
  project ET scalar policy for exact Z/Q expressions is integral-only; MPF and
  MPFR double scalar leaves are covered.

Tests added:
- tests/test_mpz_arithmetic.cpp
- tests/test_mpq_arithmetic.cpp
- tests/test_mixed_type_arithmetic.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/tests/test_mpz_arithmetic.cpp
- sed -n '260,560p' ../gmpxx_mkII/tests/test_mpz_arithmetic.cpp
- sed -n '1,300p' ../gmpxx_mkII/tests/test_mpq_arithmetic.cpp
- sed -n '1,320p' ../gmpxx_mkII/tests/test_mixed_type_arithmetic.cpp
- sed -n '1,260p' tests/test_mpq_basic.cpp
- sed -n '1,300p' tests/test_mixed_zq_mpf_promotion.cpp
- sed -n '1,300p' tests/test_mixed_zq_mpfr_promotion.cpp
- rg -n "operator\\+\\+|operator--|operator%|operator%=|gcd\\(|lcm\\(" include/gmpfrxx_mkII/detail/zq_impl.hpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target test_mpz_arithmetic test_mpq_arithmetic test_mixed_type_arithmetic
- ctest --test-dir build -R 'test_mpz_arithmetic|test_mpq_arithmetic|test_mixed_type_arithmetic' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j --target test_mpz_arithmetic test_mpq_arithmetic test_mixed_type_arithmetic: PASS.
- ctest --test-dir build -R 'test_mpz_arithmetic|test_mpq_arithmetic|test_mixed_type_arithmetic' --output-on-failure: PASS, 3/3 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 115/115 tests passed.

Known issues:
- Exact Z/Q double-scalar upstream parity remains to be resolved as a separate
  API-policy/ET design decision.

Post-phase ZQ/MPF bitwise and shift ET audit:
DONE

Implemented features:
- Converted newly added `mpz_class` bitwise operators (`~`, `&`, `|`, `^`) to
  expression-template nodes instead of eager public `mpz_class` return values.
- Converted `mpz_class`, `mpq_class`, and `mpf_class` power-of-two shift
  operators (`<<`, `>>`) to expression-template nodes; compound assignments
  materialize through assignment, which is an allowed materialization point.
- Added evaluator support for MPZ bitwise nodes and MPZ/MPQ/MPF shift nodes.

Missing features:
- Broader upstream split migration for `test_mixed_type_arithmetic.cpp`,
  `test_mpq_arithmetic.cpp`, and `test_mpz_arithmetic.cpp` is still ongoing;
  this phase specifically fixed the ET contract for the bitwise/shift area
  before continuing the matrix expansion.

Tests added:
- None.

Tests updated:
- tests/test_mpz_basic.cpp
- STATUS.md

Exact commands run:
- rg -n "operator[&|^]|operator<<|operator>>|bit_|shl|shr|mpf_class operator<<|mpq_class operator<<|mpz_class operator<<" include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_mpz_basic.cpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '250,520p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1880,2140p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '520,760p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1740,1888p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1440,1645p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '1,260p' tests/test_mpz_basic.cpp
- rg -n "is_zq_expression_operand|make_zq_operand|zq_binary_result|mpz_evaluate|mpq_evaluate" include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1180,1445p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1280,1445p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j: PASS after fixing evaluator control flow.
- ctest --test-dir build --output-on-failure: PASS, 112/112 tests passed.
- git diff --check: PASS.

Known issues:
- None for the bitwise/shift ET conversion.

Post-phase header scan and MPF long-width dispatch migration:
DONE

Implemented features:
- Compared `../gmpxx_mkII/tests/test_headers.cpp` with local coverage.
  Upstream only includes `gmpxx_mkII.h`; local `test_header_boundaries.cpp`
  now also checks this repository's stricter header roles.
- Extended header source scans to assert:
  - `gmpxx_mkII.h` includes GMP and excludes MPFR/MPC headers and symbols.
  - `mpfrxx_mkII.h` includes GMP/MPFR/MPC but does not bridge through
    `gmpxx_mkII.h` or expose GMP-only MPF/MPFC implementation headers.
  - `gmpfrxx_mkII.h` aggregates only `gmpxx_mkII.h` and `mpfrxx_mkII.h`.
  - Public/implementation headers do not include `gmpxx.h`.
  - GMP-only MPF/MPFC implementation paths do not contain MPFR/MPC C API
    symbols.
- Added `tests/test_mpf_long_width_dispatch.cpp`, mirroring the existing
  MPFR long-width test for MPF.
- Updated the upstream migration table rows for `test_headers.cpp` and
  `test_long_width_dispatch.cpp` to Done for current policy.

Tests added:
- tests/test_mpf_long_width_dispatch.cpp

Tests updated:
- tests/test_header_boundaries.cpp
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/tests/test_headers.cpp
- sed -n '1,260p' ../gmpxx_mkII/tests/test_long_width_dispatch.cpp
- sed -n '1,260p' tests/test_header_boundaries.cpp
- sed -n '1,220p' tests/test_mpfr_long_width_dispatch.cpp
- sed -n '1,140p' tests/CMakeLists.txt
- sed -n '1,120p' include/gmpxx_mkII.h
- sed -n '1,120p' include/mpfrxx_mkII.h
- sed -n '1,120p' include/gmpfrxx_mkII.h
- rg -n "mpf_data|get_mpf_t|mpf_cmp_z|mpf_cmp" include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_mpf* tests/test_gmpxx_mkII.cpp
- rg -n "mpz_data\\(|mpq_data\\(" include/gmpfrxx_mkII/detail/zq_impl.hpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target test_header_boundaries test_mpf_long_width_dispatch test_mpfr_long_width_dispatch
- ctest --test-dir build -R 'test_header_boundaries|test_mpf_long_width_dispatch|test_mpfr_long_width_dispatch' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j --target test_header_boundaries test_mpf_long_width_dispatch test_mpfr_long_width_dispatch: PASS.
- ctest --test-dir build -R 'test_header_boundaries|test_mpf_long_width_dispatch|test_mpfr_long_width_dispatch' --output-on-failure: PASS, 3/3 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 112/112 tests passed.
- git diff --check: PASS.

Known issues:
- None for these two migration rows.

Post-phase MPC constructor adoption scan:
DONE

Implemented features:
- Scanned the repository for remaining `mpc_set_fr_fr` usage after adding
  `mpfrxx::mpc_class(const mpfr_class&, const mpfr_class&)`.
- Replaced additional wrapper-side uses that no longer need direct
  `mpc_set_fr_fr`:
  - `mpfrxx::operator>>(std::istream&, mpc_class&)`
  - string `_mpc_i` user-defined literal
- Left the single direct `mpc_set_fr_fr` call in the new pair constructor as
  the central implementation point.

Tests added:
- None.

Tests updated:
- STATUS.md

Exact commands run:
- rg -n "mpc_set_fr_fr" .
- sed -n '1070,1135p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- rg -n "mpc_set_fr_fr" include examples tests
- rg -n "_mpc_i|mpc_class.*>>|operator>>|test_mpc_io|literal" tests examples include/gmpfrxx_mkII/detail/mpc_impl.hpp
- cmake --build build -j --target test_mpc_io test_mpfr_user_defined_literals test_mpc_basic
- ctest --test-dir build -R 'test_mpc_io|test_mpfr_user_defined_literals|test_mpc_basic' --output-on-failure
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- rg -n "mpc_set_fr_fr" include examples tests: PASS, only `include/gmpfrxx_mkII/detail/mpc_impl.hpp` constructor use remains.
- cmake --build build -j --target test_mpc_io test_mpfr_user_defined_literals test_mpc_basic: PASS.
- ctest --test-dir build -R 'test_mpc_io|test_mpfr_user_defined_literals|test_mpc_basic' --output-on-failure: PASS, 3/3 tests passed.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 111/111 tests passed.
- git diff --check: PASS.

Known issues:
- Reference MPC manual files still mention `mpc_set_fr_fr`; these are upstream
  reference artifacts, not implementation usage.

Post-phase generated Git commit hash API:
DONE

Implemented features:
- Added CMake Git detection and configure-time generation of build/generated/gmpfrxx_mkII/detail/version.hpp.
- Added include/gmpfrxx_mkII/detail/version.hpp.in as the source template for GMPFRXX_MKII_GIT_COMMIT_HASH.
- Added generated include directory to gmpxx_mkII, mpfrxx_mkII, and gmpfrxx_mkII interface targets.
- Added direct-include fallback in config.hpp so non-CMake source-tree includes report "unknown" when no generated version header is present.
- Added gmpxx::git_commit_hash(), mpfrxx::git_commit_hash(), gmpxx::print_git_commit_hash(std::ostream&), and mpfrxx::print_git_commit_hash(std::ostream&).

Tests added:
- tests/test_version_info.cpp

Tests updated:
- CMakeLists.txt
- tests/CMakeLists.txt
- include/gmpfrxx_mkII/detail/config.hpp
- include/gmpfrxx_mkII/detail/version.hpp.in
- STATUS.md

Exact commands run:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_version_info -j
- ctest --test-dir build -R test_version_info --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_version_info: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 68/68 tests passed.

Known issues:
- The generated Git hash is captured at CMake configure time; rerun cmake -S . -B build to refresh it after new commits.

Post-phase MPF allocation-count parity test:
DONE

Implemented features:
- Added an MPF allocation-count regression test parallel to the existing MPFR allocation-count test.
- Covered non-aliasing ET assignment fast paths for simple addition chains.
- Covered the current one-temporary behavior for compound two-sided expressions.

Tests added:
- tests/test_mpf_alloc_count.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_mpf_alloc_count -j
- ctest --test-dir build -R test_mpf_alloc_count --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_mpf_alloc_count: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 69/69 tests passed.

Known issues:
- This test intentionally mirrors the existing MPFR allocation-count surface first; later scalar allocation tests cover the current scalar policy, while aliasing, IO, parser, and transcendent-function allocation behavior remain outside the current allocation-count surface.

Post-phase test suite catalog:
DONE

Implemented features:
- Added a consolidated catalog of the tests under tests/.
- Documented what each test verifies and how it verifies it.
- Split normal compile-and-run tests from expected compile-fail contract tests.

Normal tests:

| Test | Target | What it tests | How it tests |
| --- | --- | --- | --- |
| test_gmp_header_smoke | gmpxx_mkII | GMP-only public header basics | Includes gmpxx_mkII.h and static-asserts default constructibility of mpz_class, mpq_class, and mpf_class. |
| test_mpfr_header_smoke | mpfrxx_mkII | MPFR/MPC public header basics | Includes mpfrxx_mkII.h and static-asserts default constructibility of mpz/mpq aliases, mpfr_class, and mpc_class. |
| test_aggregator_header_smoke | gmpfrxx_mkII | Full aggregator header basics | Includes gmpfrxx_mkII.h and static-asserts the combined GMP and MPFR/MPC public types. |
| test_namespace_aliases | mpfrxx_mkII | mpfrxx exact-type alias policy | Static-asserts mpfrxx::mpz_class and mpfrxx::mpq_class are the same types as gmpxx::mpz_class and gmpxx::mpq_class. |
| test_header_boundaries | gmpfrxx_mkII | Header dependency boundaries | Scans source headers at runtime for forbidden includes and forbidden MPFR/MPC symbols in GMP-only paths. |
| test_version_info | gmpfrxx_mkII | Generated Git commit hash API | Checks hash API return types, non-empty values, macro consistency, and stream-print helpers. |
| test_integer_model_diagnostics | gmpxx_mkII | Platform integer assumptions | Checks standard integer widths and signedness assumptions used by exact integer conversion paths. |
| test_abi_fingerprint | gmpfrxx_mkII | Internal ET/scalar ABI shape | Static-asserts normalized scalar leaf types, supported operand traits, expression node shape, operation tags, and result type traits. |
| test_alias_safety | gmpfrxx_mkII | Cross-type alias-safe assignment | Compares aliased MPF and MPFR assignments against independently computed expected values while preserving destination precision. |
| test_random | gmpxx_mkII | GMP random state API | Checks gmp_randclass construction, seeding, integer/float generation, precision behavior, error paths, and mean/variance smoke statistics. |
| test_et_contract_mpfr | mpfrxx_mkII | MPFR expression-template contract | Static-asserts arithmetic operators return expression nodes, not eager mpfr_class values. |
| test_mpfr_expression_eval | mpfrxx_mkII | MPFR expression evaluation | Evaluates nested arithmetic expressions and compares materialized double values. |
| test_mpfr_precision_policy | mpfrxx_mkII | MPFR expression precision policy | Checks new materialization uses max leaf precision and assignment preserves destination precision. |
| test_mpfr_aliasing | mpfrxx_mkII | Basic MPFR self-assignment alias safety | Assigns expressions involving the destination back into itself and checks numeric results. |
| test_mpfr_random | mpfrxx_mkII | MPFR random state API | Checks MPFR random state construction, seeding, integer/uniform/normal/exponential generation, precision behavior, error paths, and mean/variance smoke statistics. |
| test_et_contract_mpfr_scalar | mpfrxx_mkII | MPFR scalar ET operands | Static-asserts supported scalar leaves, rejected scalar leaves, and expression node result types. |
| test_mpfr_scalar_eval | mpfrxx_mkII | MPFR scalar expression evaluation | Evaluates MPFR expressions mixed with integral and floating scalar operands. |
| test_mpfr_long_width_dispatch | mpfrxx_mkII | Exact 64-bit integer conversion into MPFR | Verifies uint64 max, int64 min, and int64 max are represented exactly through the integer conversion path. |
| test_mpfr_alloc_count | mpfrxx_mkII | MPFR non-aliasing allocation fast paths | Installs GMP allocation hooks and checks addition chains allocate zero times while a two-sided compound expression allocates one temporary. |
| test_mpfr_defaults | mpfrxx_mkII | MPFR built-in defaults and runtime setters | Clears environment, checks 512-bit default precision, runtime precision setter, and rounding behavior. |
| test_mpfr_environment | mpfrxx_mkII | Valid MPFR environment configuration | Sets MPFRXX_* variables, reloads defaults, checks precision/exponent/rounding values and rounding behavior. |
| test_mpfr_environment_invalid | mpfrxx_mkII | Invalid MPFR environment fallback | Sets invalid MPFRXX_* variables and checks fallback defaults, then checks valid prefixed rounding strings. |
| test_mpfr_string_io | mpfrxx_mkII | MPFR string and stream I/O | Checks set_str/get_str, invalid input preservation, base parsing, stream formatting, expression output, and stream extraction. |
| test_mpfr_transcendent_functions | mpfrxx_mkII | MPFR transcendental functions | Checks trigonometric, hyperbolic, log/exp, pow, gamma, and erf/erfc wrappers against deterministic random double smoke references at `1e-15` scaled tolerance. |
| test_et_contract_mpf | gmpxx_mkII | MPF expression-template contract | Static-asserts MPF arithmetic returns expression nodes with expected result types. |
| test_mpf_basic | gmpxx_mkII | Basic MPF arithmetic evaluation | Evaluates MPF arithmetic with object and scalar operands and compares double results. |
| test_mpf_precision_policy | gmpxx_mkII | MPF default and expression precision policy | Checks MPFXX_DEFAULT_PREC_BITS, runtime default setter, max leaf precision materialization, and assignment precision preservation. |
| test_mpf_aliasing | gmpxx_mkII | Basic MPF self-assignment alias safety | Assigns expressions involving the destination back into itself and checks numeric results. |
| test_mpf_string_io | gmpxx_mkII | MPF string and stream I/O | Checks set/get string APIs, formatting flags, stream extraction, invalid input handling, and expression output. |
| test_mpf_alloc_count | gmpxx_mkII | MPF non-aliasing allocation fast paths | Installs GMP allocation hooks and mirrors MPFR allocation expectations for addition chains and one compound expression. |
| test_mpf_pi | gmpxx_mkII | MPF pi/log_two constants and caches | Checks precision, decimal prefixes, hardcoded 512/1024/2048-bit constants, default precision behavior, and cache regression across precision changes. |
| test_mpf_transcendent_functions | gmpxx_mkII | MPF logarithm and transcendental functions | Checks log/log2/log10/log1p/exp family, trig/hyperbolic/inverse functions, pow, gamma, and reciprocal gamma with deterministic and random smoke comparisons. |
| test_construction_copy | gmpfrxx_mkII | Construction, copy, move, assignment APIs | Static-asserts constructors and assignment traits, then checks values and precision preservation for GMP exact, MPF, and MPFR classes. |
| test_et_contract_mpfc | gmpxx_mkII | MPFC expression-template contract | Static-asserts MPFC arithmetic returns expression nodes and keeps MPFC separate from MPC. |
| test_mpfc_basic | gmpxx_mkII | Basic MPFC arithmetic evaluation | Evaluates complex MPF-backed arithmetic and compares real/imaginary double values. |
| test_mpfc_precision_policy | gmpxx_mkII | MPFC precision propagation | Checks real/imag precision propagation, assignment preservation, and mixed MPF/MPFC precision rules. |
| test_mpfc_header_boundary | gmpxx_mkII | MPFC availability in GMP-only header | Constructs mpfc_class through gmpxx_mkII.h and checks real/imag precision without MPFR/MPC exposure. |
| test_mpfc_math | gmpxx_mkII | MPFC math functions | Checks sqrt, exp, log, trig, hyperbolic, pow, and expression materialization against double-level expectations. |
| test_et_contract_zq_mpf | gmpxx_mkII | z/q/MPF mixed ET contract | Static-asserts mpz/mpq/mpf mixed arithmetic returns expression nodes with the required promoted result types. |
| test_mixed_zq_mpf_promotion | gmpxx_mkII | z/q to MPF promotion behavior | Evaluates mixed exact/MPF expressions and checks promoted MPF numeric results. |
| test_et_contract_zq_mpfr | mpfrxx_mkII | z/q/MPFR mixed ET contract | Static-asserts mpz/mpq/mpfr mixed arithmetic returns expression nodes with MPFR result types. |
| test_mpz_basic | gmpxx_mkII | Basic mpz arithmetic | Evaluates integer addition, subtraction, multiplication, division, and nested expressions against GMP integer results. |
| test_mpq_basic | gmpxx_mkII | Basic mpq arithmetic | Evaluates rational arithmetic and checks numerator/denominator results. |
| test_mpq_canonicalization | gmpxx_mkII | mpq canonical form | Checks reduction, sign normalization, and arithmetic canonicalization. |
| test_zq_string_io | gmpxx_mkII | mpz/mpq string and stream I/O | Checks set/get string APIs, base parsing, stream formatting, stream extraction, invalid input preservation, and zero-denominator rejection. |
| test_mixed_zq_mpfr_promotion | mpfrxx_mkII | z/q to MPFR promotion behavior | Evaluates mixed exact/MPFR expressions and checks promoted MPFR numeric results. |
| test_mixed_zq_mpfr_exact_subexpression | mpfrxx_mkII | Exact subexpression before MPFR promotion | Verifies exact mpz/mpq subexpressions are evaluated before conversion to MPFR. |
| test_et_contract_mpc | mpfrxx_mkII | MPC expression-template contract | Static-asserts MPC arithmetic and mixed MPC/MPFR/exact/scalar operands produce expression nodes with MPC results. |
| test_mpc_basic | mpfrxx_mkII | Basic MPC arithmetic evaluation | Evaluates complex arithmetic and compares real/imaginary double values. |
| test_mpc_aliasing | mpfrxx_mkII | Basic MPC self-assignment alias safety | Assigns expressions involving the destination back into itself and checks real/imaginary results. |
| test_mpc_defaults | mpfrxx_mkII | MPC built-in defaults and rounding inheritance | Checks MPC default precision/rounding behavior and inheritance from MPFR defaults. |
| test_mpc_environment | mpfrxx_mkII | Valid MPC environment configuration | Sets MPC-related environment variables and checks real/imag precision plus rounding behavior. |
| test_mpc_environment_invalid | mpfrxx_mkII | Invalid MPC environment fallback | Sets invalid MPC environment values and checks fallback/inheritance behavior. |
| test_mpc_precision_policy | mpfrxx_mkII | MPC precision propagation | Checks real/imag precision propagation, mixed operand precision, and assignment precision preservation. |

Expected compile-fail tests:

| Test | Header/target | What must fail | How it tests |
| --- | --- | --- | --- |
| compile_fail_mpfr_header_must_not_expose_mpf | mpfrxx_mkII | mpfrxx_mkII.h must not expose gmpxx::mpf_class | Attempts to name gmpxx::mpf_class after including only mpfrxx_mkII.h. |
| compile_fail_mpfr_header_must_not_expose_mpfc | mpfrxx_mkII | mpfrxx_mkII.h must not expose gmpxx::mpfc_class | Attempts to name gmpxx::mpfc_class after including only mpfrxx_mkII.h. |
| compile_fail_gmp_header_must_not_expose_mpfr_or_mpc | gmpxx_mkII | gmpxx_mkII.h must not expose MPFR/MPC public types | Attempts to name mpfrxx::mpfr_class and mpfrxx::mpc_class after including only gmpxx_mkII.h. |
| compile_fail_test_bool_scalar | mpfrxx_mkII | bool scalar leaves are rejected | Attempts to form an MPFR expression with x + true. |
| compile_fail_test_long_double_scalar | mpfrxx_mkII | long double scalar leaves are rejected | Attempts to form an MPFR expression with x + 1.0L. |
| compile_fail_test_int128_scalar | mpfrxx_mkII | __int128 scalar leaves are rejected | Attempts to form an MPFR expression with x + __int128. |
| compile_fail_test_scalar_scalar_et_operator | mpfrxx_mkII | scalar/scalar ET operators are not public API | Attempts to call the detail operator on two scalar operands. |
| compile_fail_test_mpf_plus_mpfr | gmpfrxx_mkII | MPF and MPFR must not mix | Attempts gmpxx::mpf_class + mpfrxx::mpfr_class. |
| compile_fail_test_mpfr_plus_mpf | gmpfrxx_mkII | MPFR and MPF must not mix | Attempts mpfrxx::mpfr_class + gmpxx::mpf_class. |
| compile_fail_test_mpf_plus_mpc | gmpfrxx_mkII | MPF and MPC must not mix | Attempts gmpxx::mpf_class + mpfrxx::mpc_class. |
| compile_fail_test_mpc_plus_mpf | gmpfrxx_mkII | MPC and MPF must not mix | Attempts mpfrxx::mpc_class + gmpxx::mpf_class. |
| compile_fail_test_mpfc_plus_mpfr | gmpfrxx_mkII | MPFC and MPFR must not mix | Attempts gmpxx::mpfc_class + mpfrxx::mpfr_class. |
| compile_fail_test_mpfr_plus_mpfc | gmpfrxx_mkII | MPFR and MPFC must not mix | Attempts mpfrxx::mpfr_class + gmpxx::mpfc_class. |
| compile_fail_test_mpfc_plus_mpc | gmpfrxx_mkII | MPFC and MPC must not mix | Attempts gmpxx::mpfc_class + mpfrxx::mpc_class. |
| compile_fail_test_mpc_plus_mpfc | gmpfrxx_mkII | MPC and MPFC must not mix | Attempts mpfrxx::mpc_class + gmpxx::mpfc_class. |

Test execution model:
- Normal tests are registered with add_phase0_test(), built as standalone executables, linked to the relevant interface target, and run by CTest.
- Compile-fail tests invoke the C++ compiler directly under CTest and are marked WILL_FAIL TRUE.
- test_mpfr_alloc_count and test_mpf_alloc_count are marked RUN_SERIAL because GMP memory allocators are process-global.

Tests updated:
- STATUS.md

Exact commands run:
- rg --files tests
- sed -n '1,220p' tests/CMakeLists.txt
- ls tests
- for f in tests/*.cpp; do ... assertion scan ...; done
- for f in tests/compile_fail/*.cpp; do ... source scan ...; done
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 71/71 tests passed.

Known issues:
- The catalog summarizes test intent at the file level; it does not enumerate every individual assertion inside large smoke tests.

Post-phase GMP random API:
DONE

Implemented features:
- Added gmpxx::gmp_randclass as a GMP-only RAII wrapper around gmp_randstate_t.
- Added default, gmp_randinit_default, gmp_randinit_mt, gmp_randinit_lc_2exp_size, gmp_randinit_lc_2exp, and obsolete gmp_randalg_t constructor forms.
- Added seed(unsigned long) and seed(const gmpxx::mpz_class&).
- Added get_z_bits(mp_bitcnt_t), get_z_bits(const gmpxx::mpz_class&), and get_z_range(const gmpxx::mpz_class&).
- Added gmpxx::random_mpf_expr and get_f(), get_f(mp_bitcnt_t), get_f(const gmpxx::mpf_class&).
- Integrated random_mpf_expr with MPF expression materialization so construction uses the requested/default precision and assignment preserves the destination precision.
- Kept the implementation in the GMP-only path; no MPFR/MPC dependency was introduced.

Design notes:
- The API follows the GMP C++ binding shape documented for gmp_randclass: state object, seed overloads, get_z_bits, get_z_range, and get_f.
- The implementation uses the GMP C API underneath: gmp_randinit_*, gmp_randseed*, mpz_urandomb, mpz_urandomm, and mpf_urandomb.
- get_f() returns a small expression node so that assignment can generate at the destination precision, matching GMP C++ binding semantics.

Tests added:
- tests/test_random.cpp

Tests updated:
- include/gmpfrxx_mkII/detail/mpf_impl.hpp
- tests/CMakeLists.txt
- STATUS.md

Test coverage:
- Compile-time surface checks for construction and non-copyable/non-movable state ownership.
- Deterministic sequence checks for unsigned-long and mpz_class seeds.
- Range checks for get_z_bits, get_z_range, and get_f.
- Precision behavior checks for get_f(), get_f(precision), get_f(prototype), construction, and assignment.
- Constructor coverage for default, MT, LC size, LC parameter, and obsolete gmp_randalg_t forms.
- Error checks for negative bit counts, non-positive range limits, and too-large LC table size.
- Statistical smoke checks for get_z_range(1000) and get_f(256): sample mean and variance are checked against uniform-distribution expectations.

Exact commands run:
- GMP manual review: C++ Interface Random Numbers, Random Number Functions, Integer Random Numbers, Random State Initialization, Random State Seeding, and Miscellaneous Float Functions.
- sed -n '1,260p' ../gmpxx_mkII/tests/test_random.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_random -j
- ctest --test-dir build -R test_random --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- test_random: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 70/70 tests passed.

Known issues:
- Statistical checks are smoke tests with conservative tolerances; they are intended to catch gross range/distribution wiring errors, not certify random quality.

Post-phase exact/MPFC upstream test migration:
DONE

Implemented features:
- Added exact scalar leaves for mpz/mpq expression-template operands, with integral scalars materialized through the existing exact integer conversion policy.
- Added exact unary expression support for mpz/mpq operands.
- Added mpz/mpq arithmetic compound assignment support for supported exact expression operands.
- Added direct `mpz_addmul`/`mpz_submul` compound-assignment overloads for `mpz_class += mpz*mpz`, `mpz_class -= mpz*mpz`, and object/scalar variants that can use `mpz_addmul_ui` or `mpz_submul_ui`.
- Added `gmpxx::mpfc_class` stream extraction for `(real,imag)` values, preserving old value and component precision on parse failure.
- Added `gmpxx::mpfc_class` expression stream insertion by materializing the expression for output.
- Updated MPF stream insertion to honor the active C++ locale decimal point, which also makes MPFC output locale-aware through its components.

Tests added:
- tests/test_mpfc_io.cpp
- tests/test_mpz_addmul_fusion.cpp
- tests/test_mpz_addmul_alloc_count.cpp
- tests/test_mpz_mpq_alloc_count.cpp

Tests updated:
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- include/gmpfrxx_mkII/detail/mpf_impl.hpp
- include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- tests/test_abi_fingerprint.cpp
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- nl -ba tests/test_mpz_addmul_alloc_count.cpp
- rg -n "mpz_addmul|operator\\+=|is_mpz_addmul" include/gmpfrxx_mkII/detail/zq_impl.hpp
- nl -ba include/gmpfrxx_mkII/detail/zq_impl.hpp | sed -n '1280,1468p'
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_mpfc_io|test_mpz_addmul_alloc_count|test_mpz_addmul_fusion|test_mpz_mpq_alloc_count"
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial focused CTest: FAIL because generic `operator+=` was preferred for `mpz_class += b*c`; fixed by excluding direct addmul expressions from the generic compound overload.
- Focused CTest after fix: PASS, 4/4 tests passed.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 81/81 tests passed.

Known issues:
- `test_mpz_mpq_alloc_count.cpp` is migrated as functional exact ET/compound coverage. The upstream-style wrapper instrumentation counters are not present in this repository, so strict wrapper allocation-count parity is not claimed.
- `test_mpz_addmul_fusion.cpp` verifies expression shape, behavior, alias cases, and direct overload routing through allocator-count regression coverage, but there is no public diagnostic fusion-counter API.

Post-phase upstream gmpxx_mkII test migration tracker:
IN PROGRESS

Scope:
- Track every C++ test currently present in ../gmpxx_mkII/tests.
- Port the GMP-facing coverage to this repository's gmpxx::mpf_class/mpz_class/mpq_class/mpfc_class APIs.
- Add natural mpfrxx::mpfr_class adaptations where the test exercises behavior that should also exist for MPFR.
- Add missing public APIs when the upstream test exposes a real compatibility or behavior gap.
- Keep MPFR/MPC dependencies out of gmpxx_mkII.h.

Migration table:

| Upstream test | Current local coverage | MPF/GMP status | MPFR adaptation status | Missing API / risk | Next action |
| --- | --- | --- | --- | --- | --- |
| test_abi_fingerprint.cpp | tests/test_abi_fingerprint.cpp | Done for current ABI policy | Done for current ABI policy | Upstream has std::common_type/numeric_limits specializations and legacy concept names; this repo intentionally keeps C++17 local detail traits, rejects bool/long double scalar leaves, and has no std trait specializations. | Keep current-policy ABI fingerprint; revisit only if public standard trait specializations are added. |
| test_alias_safety.cpp | tests/test_alias_safety.cpp, tests/test_mpf_aliasing.cpp, tests/test_mpfr_aliasing.cpp | Done for upstream assertion-list parity | Done for MPFR natural adaptation | The upstream MPF alias case list has been compared line-by-line; local coverage includes all seven upstream alias stress cases plus matching MPFR adaptations. | Keep migrated; add only if upstream adds new alias stress cases. |
| test_alloc_count.cpp | tests/test_mpf_alloc_count.cpp, tests/test_mpfr_alloc_count.cpp | Done | Done | None for current object-object ET fast path; scalar allocation behavior is tracked separately in test_scalar_alloc_count.cpp. | Keep as migrated; add new allocation TODOs only if alias, parser, IO, or transcendent-function allocation counts become public policy. |
| test_comparisons.cpp | tests/test_comparisons.cpp, tests/test_mpfr_comparisons.cpp | Done | Done | Upstream C++20 concept checks were rewritten as C++17 detection/static_assert checks; bool and long double remain rejected by policy. Conversion-backed exact integer, rational, MPF, and MPFR comparison cases now cover the newly supported conversion APIs, including int64_t/uint64_t edge values. | Keep migrated; add only if future conversion APIs introduce new comparison operand families. |
| test_compound_assign.cpp | tests/test_compound_assign.cpp, tests/test_mpfr_compound_assign.cpp | Done for MPF arithmetic compound surface | Done for MPFR arithmetic compound surface | Upstream legacy exact-type bitwise/shift compound surface is not included in the MPF/MPFR adaptation yet. | Track exact-type legacy operators under mpz/mpq arithmetic rows. |
| test_construction_copy.cpp | tests/test_construction_copy.cpp | Done for current policy | Done | Upstream expects bool constructors; this repo intentionally rejects bool. | Keep migrated; add only missing constructor forms discovered by other tests. |
| test_defaults_policy.cpp | tests/test_mpf_precision_policy.cpp, tests/test_mpfr_defaults.cpp, tests/test_mpfr_environment*.cpp, tests/test_mpf_thread_safety.cpp, tests/test_mpfr_thread_safety.cpp | Done for current defaults policy | Done for current defaults policy | Upstream thread-local/default snapshot and default-base APIs are intentionally not mirrored; this repo uses process-global wrapper defaults, explicit reload/set APIs, and fixed string parse bases unless a base is specified. | Keep current-policy defaults coverage; revisit only if thread-local defaults or default-base APIs become policy. |
| test_exception_support.cpp | tests/test_exception_support.cpp, tests/test_mpfr_exception_support.cpp | Done | Done | MPFC/MPC stream invalid-input preservation is covered in their I/O tests; direct parser/string-constructor exception APIs are intentionally limited. | Keep migrated; add direct MPFC/MPC parser exception cases only if public string-constructor/parser APIs are added. |
| test_gmpxx_mkII.cpp | tests/test_gmpxx_mkII.cpp plus focused local tests | Monolithic smoke migrated and passing; focused split restored for previous TODO assertions | tests/test_mpfrxx_mkII.cpp provides MPFR/MPC adaptation and is passing | The previous local TODO blocks for MPF copy-assignment precision, legacy helper materialization, and mpz compound assignment with double have been restored under current policy. Broader focused split remains tracked by the specific upstream rows below. | Keep monolithic smoke as regression coverage; continue moving broad exact/scalar/conversion matrices into focused tests under their dedicated rows. |
| test_headers.cpp | tests/test_header_boundaries.cpp, compile_fail header tests | Done for current header-role policy | Done for current header-role policy | Upstream is an include-only smoke test; local coverage now adds public header include expectations, `gmpxx.h` exclusion, GMP-only source scans, MPFR/MPC header boundary checks, and compile-fail public namespace boundary tests. | Keep current-policy header scan coverage; extend only if header roles change. |
| test_io_and_strings.cpp | tests/test_zq_string_io.cpp, tests/test_mpf_string_io.cpp, tests/test_mpfr_string_io.cpp, tests/test_mpfc_io.cpp, tests/test_mpc_io.cpp | Done for current GMP I/O surface | Done for current MPFR/MPC policy | Upstream class-API string/I/O cases have been compared and the natural missing cases were added, including MPF non-decimal ostream base formatting and raw GMP pointer I/O helpers. MPFR uses MPFR formatting primitives intentionally. | Keep current focused coverage; extend only if upstream adds new I/O cases or MPFR/MPC policy changes. |
| test_long_width_dispatch.cpp | tests/test_mpf_long_width_dispatch.cpp, tests/test_mpfr_long_width_dispatch.cpp, construction tests | Done | Done for MPFR scalar eval | MPF and MPFR now both cover exact uint64 max, int64 min, and int64 max expression-scalar dispatch through the full-width integer conversion path. | Keep migrated; extend exact types only if upstream adds additional long-width cases. |
| test_mixed_type_arithmetic.cpp | tests/test_mixed_type_arithmetic.cpp, tests/test_mixed_zq_mpf_promotion.cpp, tests/test_mixed_zq_mpfr_promotion.cpp | Done | Done | Upstream mixed mpz/mpq/mpf scalar matrix is now mirrored for current policy, including MPF with exact operands, exact Z/Q with double scalar leaves, denormal/DBL edge smoke checks, precision-preserving assignment, and power-of-two shifts. MPFR has the natural mpz/mpq/scalar adaptation for the MPFR expression surface. | Keep migrated; extend only if upstream adds new mixed operand families. |
| test_mpf_extended_transcendent_functions.cpp | tests/test_mpf_extended_transcendent_functions.cpp, tests/test_mpf_transcendent_functions.cpp | Done for upstream assertion-list parity | Done for natural MPFR adaptation | Upstream extended MPF constants, inverse trig, hyperbolic, inverse hyperbolic, exp-base variants, gamma, expression-overload, and precision-policy assertions are mirrored in focused C++17 tests. MPFR natural adaptation is covered in `tests/test_mpfr_math.cpp` with `const_e`/`const_log10`/`pi_over_two`, inverse trig/hyperbolic/exp/gamma/reciprocal-gamma identities, expression operands, result precision checks, and deterministic random double smoke tests at `1e-15` scaled tolerance. MPFR `epsilon` remains intentionally absent. | Keep migrated; add only if upstream or MPFR policy adds new extended-function surface. |
| test_mpf_math_functions.cpp | tests/test_mpf_math_functions.cpp, tests/test_mpf_pi.cpp, tests/test_mpf_transcendent_functions.cpp | Done for upstream assertion-list parity | Done for natural MPFR adaptation | Upstream MPF sqrt/abs/neg/ceil/floor/trunc/remainder/epsilon/hypot/scaling/sign/exact helper assertions are now mirrored in a focused C++17 test. MPFR `neg`, `ceil`/`floor`/`trunc`, `remainder`/`fmod`/`remquo`, and `mul_2ui`/`div_2ui`/`mul_2si`/`div_2si` now delegate to direct MPFR C APIs and are covered against direct MPFR references; MPFR `epsilon` is intentionally not added because MPF `set_epsilon()` is a GMP/upstream compatibility helper rather than a natural MPFR API. | Keep migrated; add only if upstream or MPFR policy adds a new exact-helper alias. |
| test_mpf_transcendent_functions.cpp | tests/test_mpf_transcendent_functions.cpp | Done for upstream assertion-list parity | Done for natural MPFR adaptation | Upstream assertion list is now mirrored in the focused MPF transcendent test, adapted to C++17 and local helper names. MPFR wrapper math parity is covered by `tests/test_mpfr_transcendent_functions.cpp`, which compares deterministic random wrapper results against double `std::` references at `1e-15` scaled tolerance. | Keep migrated; extend only if upstream adds new transcendental families or MPFR policy adds new public wrappers. |
| test_mpfc_arithmetic.cpp | tests/test_mpfc_basic.cpp, tests/test_et_contract_mpfc.cpp | Done for upstream arithmetic/precision focused coverage | N/A for MPFR; MPC analog exists separately | None known for the GMP-only MPFC arithmetic surface; upstream compound, swap/precision, mixed real operand, and random arithmetic matrix cases are now mirrored locally. | Keep as migrated; add MPC analogs only as separate MPC policy work. |
| test_mpfc_io.cpp | tests/test_mpfc_io.cpp, tests/test_mpc_io.cpp | Done | MPC analog covered separately | None known for the current local complex `(real,imag)` stream format; this is local-policy parity, not a direct upstream MPC test. | Keep migrated; add only if upstream gains an MPC-specific I/O parity matrix or local complex-format policy changes. |
| test_mpfc_transcendent_functions.cpp | tests/test_mpfc_math.cpp, tests/test_mpc_math.cpp | Done for upstream complex math focused coverage except complex gamma | MPC analog covered separately with MPC 1.3.1 C API references | MPFC gamma/reciprocal_gamma are intentionally not exposed because the previous implementation converted through `std::complex<double>`. MPC wrappers cover MPC 1.3.1 sqrt/exp/log/log10/trig/hyperbolic/inverse/pow functions against direct `mpc_*` references; MPC gamma is not part of MPC 1.3.1. | Add MPFC complex gamma only with a real high-precision implementation; do not restore the double-backed Lanczos helper. |
| test_mpq_arithmetic.cpp | tests/test_mpq_arithmetic.cpp, tests/test_mpq_basic.cpp, tests/test_mpq_canonicalization.cpp | Done | mpfrxx aliases use same exact types | Upstream rational binary/scalar/shift, mixed exact promotion, accessor, nested expression, compound assignment, unary, and ET contract cases are covered under the shared exact type. | Keep migrated; add only if upstream expands rational-specific arithmetic coverage. |
| test_mpz_addmul_alloc_count.cpp | tests/test_mpz_addmul_alloc_count.cpp | Done for direct mpz addmul/submul paths | N/A | No upstream wrapper instrumentation counters; local test checks GMP allocator count for preallocated direct paths. | Keep as direct-path regression coverage. |
| test_mpz_addmul_fusion.cpp | tests/test_mpz_addmul_fusion.cpp | Done | N/A | No diagnostic fusion counter API; behavior and direct overload routing are covered. | Keep migrated; add counters only if instrumentation becomes public policy. |
| test_mpz_arithmetic.cpp | tests/test_mpz_arithmetic.cpp, tests/test_mpz_basic.cpp | Done | mpfrxx aliases use same exact types | Upstream integer binary/scalar arithmetic, double scalar ET, bit ops, shifts, inc/dec, nested addmul-style expression shapes, and gcd/lcm/factorial/primorial/fibonacci helper cases are covered under the shared exact type. | Keep migrated; add only if upstream expands integer-specific arithmetic coverage. |
| test_mpz_mpq_alloc_count.cpp | tests/test_mpz_mpq_alloc_count.cpp | Done for functional exact ET/compound coverage | N/A | Exact wrapper allocation-count parity is not available without upstream instrumentation hooks. | Keep functional coverage; add allocator-specific assertions only for stable no-allocation paths. |
| test_numeric_equivalence.cpp | tests/test_mpf_numeric_equivalence.cpp, tests/test_mpfr_numeric_equivalence.cpp | Done | Done | None known for arithmetic expression value/precision equivalence against GMP/MPFR C API. | Keep migrated; extend if scalar arithmetic matrix exposes additional expression shapes. |
| test_power_of_two_fusion.cpp | tests/test_mpf_power_of_two_fusion.cpp, tests/test_mpfr_power_of_two_fusion.cpp | Done for behavior | Done for natural MPFR behavior | No diagnostic fusion counter API; tests verify power-of-two scalar multiply/divide behavior and destination precision preservation. | Add counters only if optimizer instrumentation becomes public policy. |
| test_precision_policy.cpp | tests/test_mpf_precision_policy.cpp, tests/test_mpfr_precision_policy.cpp | Done | Done for natural MPFR adaptation | Upstream MPF precision-policy assertions are now mirrored, including `set_prec`, `set_prec_raw`, expression construction precision, assignment precision preservation, `.eval()` materialization, compound-assignment precision preservation, and value comparison against direct C API references. MPFR has the natural adaptation with `set_prec` implemented via `mpfr_prec_round`; there is no MPFR `set_prec_raw` wrapper because raw precision mutation is not part of the current MPFR policy. | Keep migrated; revisit only if MPFR raw-precision mutation becomes public policy. |
| test_random.cpp | tests/test_random.cpp, tests/test_mpfr_random.cpp | Done | Done | Statistical tests are smoke checks only; no deep distribution or high-precision randomness quality test is currently defined as policy. | Keep migrated; add stronger statistical/high-precision random tests only if the random API policy expands. |
| test_scalar_alloc_count.cpp | tests/test_mpf_scalar_alloc_count.cpp, tests/test_mpfr_scalar_alloc_count.cpp | Done | Done | MPF/MPFR integer scalar leaves now use direct GMP/MPFR C API fast paths when `long`/`unsigned long` cover int64_t/uint64_t, avoiding the previous `mpz` allocation on LP64 while preserving the mpz fallback on narrower ABIs. Direct scalar assignment is allocation-free; nested and compound expression temporaries remain documented by the focused tests. | Keep migrated; revisit only if expression alias evaluation gains scalar-temporary elision. |
| test_scalar_arithmetic.cpp | tests/test_mpf_basic.cpp, tests/test_mpfr_scalar_eval.cpp | Done | Done for natural MPFR adaptation | Upstream scalar matrix is now covered for MPF and naturally adapted to MPFR: signed/unsigned integer edges, uint64 max, float/double extremes, assignment precision preservation, scalar-left/right arithmetic, expression-plus-scalar shapes, and pre/post inc-dec precision preservation. Exact Z/Q inc-dec and exact scalar interactions are covered under `test_mpz_arithmetic.cpp`, `test_mpq_arithmetic.cpp`, and `test_mixed_type_arithmetic.cpp`. | Keep migrated; extend only if upstream adds new scalar families. |
| test_thread_safety.cpp | tests/test_mpf_thread_safety.cpp, tests/test_mpfr_thread_safety.cpp | Done for current global-default policy | Done for current global-default policy | This repo exposes process-global wrapper defaults, not upstream thread-snapshot default semantics. Tests cover concurrent default construction, isolation from GMP/MPFR global defaults, and parallel expression materialization with per-thread objects. | Keep current-policy tests; revisit only if defaults become thread-local. |
| test_type_conversions.cpp | tests/test_type_conversions.cpp, tests/test_mpfr_type_conversions.cpp, tests/test_gmpxx_mkII.cpp, tests/test_mpfrxx_mkII.cpp | Done for policy-compatible focused matrix | Done for natural MPFR adaptation | Focused tests now cover raw GMP/MPFR pointer construction, exact int64_t/uint64_t and int128 construction/assignment, string assignment and invalid-input preservation, wrapper-to-wrapper conversions, explicit bool conversion, accessors, fit predicates, and destination-precision preservation. Upstream APIs that conflict with local policy remain intentional differences: bool construction is rejected, `mpfrxx_mkII.h` does not expose `gmpxx::mpf_class`, and MPFR does not mirror GMP-only MPF conversion forms. | Keep migrated; extend only if upstream adds a new policy-compatible conversion assertion or local conversion policy changes. |
| test_unary_minus_simplification.cpp | tests/test_unary_minus_simplification.cpp | Done for GMP plus MPFR adaptation | Done for MPFR unary expression adaptation | None known for current unary simplification policy. Double-negative MPF/MPFR expressions now simplify to `pos_op`; MPZ/MPQ behavior and MPZ complement are covered. | Keep migrated; extend only if MPC/MPFC unary node-shape simplification becomes policy. |
| test_user_defined_literals.cpp | tests/test_user_defined_literals.cpp, tests/test_mpfr_user_defined_literals.cpp | Done for GMP literal surface plus MPFC imaginary literal extension | Done for MPFR `_mpfr` and MPC imaginary literal extensions | GMP `_mpz/_mpq/_mpf` literals are covered, including exact auto-base string/raw integer parsing. MPFC `_mpfc_i` follows `std::complex_literals` as an imaginary-only suffix and covers decimal plus prefixed hex/binary string inputs. MPFR `_mpfr` and MPC `_mpc_i` are public under `mpfrxx::literals` and covered for floating, string, auto-base/default-precision behavior where applicable. | Keep migrated; add real-only complex literals only if a future API policy explicitly wants them. |

Execution policy:
- Do not copy upstream C++20 concept tests verbatim; rewrite to C++17 and this repository's public namespaces.
- When upstream behavior conflicts with this repository's policy, keep this repository's policy and document the divergence in this table.
- For every migrated source test, add or update a local CTest target.
- For every MPF-facing migration, add a matching MPFR adaptation when the behavior naturally applies to mpfrxx::mpfr_class.
- Run at least the focused test target and then full CTest before marking a row Done.

Tests updated:
- STATUS.md
- tests/test_comparisons.cpp
- tests/test_mpfr_comparisons.cpp
- tests/test_compound_assign.cpp
- tests/test_mpfr_compound_assign.cpp
- tests/test_exception_support.cpp
- tests/test_mpfr_exception_support.cpp
- tests/CMakeLists.txt

Exact commands run:
- ls ../gmpxx_mkII/tests/*cpp
- ls tests/*cpp
- sed -n '1,220p' tests/CMakeLists.txt
- for f in ../gmpxx_mkII/tests/*cpp; do ... assertion scan ...; done
- for f in tests/*cpp; do ... assertion scan ...; done
- sed -n '1,260p' ../gmpxx_mkII/tests/test_comparisons.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_comparisons|test_mpfr_comparisons"
- ctest --test-dir build --output-on-failure
- sed -n '1,280p' ../gmpxx_mkII/tests/test_compound_assign.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_compound_assign|test_mpfr_compound_assign"
- ctest --test-dir build --output-on-failure
- sed -n '1,320p' ../gmpxx_mkII/tests/test_exception_support.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure -R "test_exception_support|test_mpfr_exception_support"
- ctest --test-dir build --output-on-failure

Pass/fail result:
- PASS: cmake --build build -j
- PASS: ctest --test-dir build --output-on-failure -R "test_comparisons|test_mpfr_comparisons" (2/2)
- PASS: ctest --test-dir build --output-on-failure -R "test_compound_assign|test_mpfr_compound_assign" (2/2)
- PASS: ctest --test-dir build --output-on-failure -R "test_exception_support|test_mpfr_exception_support" (2/2)
- PASS: ctest --test-dir build --output-on-failure (77/77)

Known issues:
- The table is intentionally conservative: several rows marked Partial may already have some behavior covered by smaller local tests, but remain open until compared line-by-line with the upstream source test.

Comparison migration details:
- Added strict mpz/mpq comparison support through cmp() and relational operators using exact mpq materialization and mpq_cmp.
- Added gmpxx::mpf_class comparison support for mpf/exact/scalar/expression operands, preserving exact-only comparisons on the mpq_cmp path.
- Added mpfrxx::mpfr_class comparison support for mpfr/exact/scalar/expression operands using MPFR evaluation context and mpfr_cmp.
- Added tests/test_comparisons.cpp as the C++17 MPF/GMP adaptation of upstream test_comparisons.cpp.
- Added tests/test_mpfr_comparisons.cpp as the natural MPFR adaptation.
- Added gmpxx::mpf_class and mpfrxx::mpfr_class operator+=, operator-=, operator*=, and operator/= for supported expression/scalar operands.
- Added tests/test_compound_assign.cpp and tests/test_mpfr_compound_assign.cpp for destination-precision preservation, scalar RHS, wider RHS precision, exact RHS, self-aliasing, and expression RHS.
- Added centralized exception support tests for exact string construction, MPF/MPFR invalid string construction and assignment, random argument validation, MPF domain errors, and MPFC/MPC component exception propagation.
- Changed mpfrxx::mpfr_class string construction and string assignment to throw std::invalid_argument on invalid input, matching MPF behavior; set_str() remains nonthrowing and preserves the old value on failure.
- Changed gmpxx::mpq_class string construction and set_str() to reject zero denominators before canonicalization.

Post-phase MPFR random API:
DONE

Implemented features:
- Added mpfrxx::gmp_randclass as an MPFR-side RAII wrapper around gmp_randstate_t.
- Added default, gmp_randinit_default, gmp_randinit_mt, gmp_randinit_lc_2exp_size, gmp_randinit_lc_2exp, and obsolete gmp_randalg_t constructor forms.
- Added seed(unsigned long) and seed(const gmpxx::mpz_class&).
- Added get_z_bits(mp_bitcnt_t), get_z_bits(const gmpxx::mpz_class&), and get_z_range(const gmpxx::mpz_class&), returning mpfrxx::mpz_class aliases.
- Added mpfrxx::random_mpfr_expr and get_fr(), get_fr(mpfr_prec_t), get_fr(const mpfrxx::mpfr_class&).
- Added MPFR-specific random distributions: get_fr_uniform(), get_fr_normal(), and get_fr_exponential(), each with default and explicit precision overloads.
- Added MPFR C API name aliases: get_fr_urandomb(), get_fr_urandom(), get_fr_nrandom(), and get_fr_erandom().
- Integrated random_mpfr_expr with MPFR expression materialization so construction uses the requested/default precision and assignment preserves destination precision.

Design notes:
- MPFR random generation uses GMP random states, as documented by MPFR.
- get_fr() maps to mpfr_urandomb for the closest MPF-style [0, 1) random float behavior.
- get_fr_uniform()/get_fr_urandom() maps to mpfr_urandom, get_fr_normal()/get_fr_nrandom() maps to mpfr_nrandom, and get_fr_exponential()/get_fr_erandom() maps to mpfr_erandom.
- mpfr_urandom/mpfr_nrandom/mpfr_erandom return ternary rounding information; non-zero return values are not treated as failures.

Tests added:
- tests/test_mpfr_random.cpp

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/CMakeLists.txt
- STATUS.md

Test coverage:
- Compile-time surface checks for construction and non-copyable/non-movable state ownership.
- Deterministic sequence checks for unsigned-long and mpz_class seeds.
- Range checks for get_z_bits, get_z_range, get_fr, and get_fr_uniform.
- Deterministic alias checks for get_fr_urandomb, get_fr_urandom, get_fr_nrandom, and get_fr_erandom.
- Precision behavior checks for get_fr(), get_fr(precision), get_fr(prototype), construction, and assignment.
- Constructor coverage for default, MT, LC size, LC parameter, and obsolete gmp_randalg_t forms.
- Error checks for negative bit counts, non-positive range limits, and too-large LC table size.
- Statistical smoke checks for get_z_range(1000), get_fr(256), get_fr_normal(256), and get_fr_exponential(256): sample mean and variance are checked against the expected distributions.

Exact commands run:
- MPFR manual review: current MPFR random functions covering mpfr_urandomb, mpfr_urandom, mpfr_nrandom, mpfr_grandom, and mpfr_erandom.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_mpfr_random -j
- ctest --test-dir build -R test_mpfr_random --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial test_mpfr_random build: FAIL because implementation used mpz_class before the mpfrxx alias declaration; fixed by using gmpxx::mpz_class internally.
- Initial test_mpfr_random run: FAIL because mpfr_urandom non-zero ternary return was treated as failure; fixed to treat it as rounding information.
- test_mpfr_random: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 71/71 tests passed.

Known issues:
- Statistical checks are smoke tests with conservative tolerances; they are intended to catch gross range/distribution wiring errors, not certify random quality.

Post-phase MPFR adaptations for addmul/allocation and complex I/O:
DONE

Implemented features:
- Made tests/test_mpz_addmul_fusion.cpp build through mpfrxx_mkII.h and use mpfrxx::mpz_class aliases, confirming the mpfrxx header exposes the exact integer ET/addmul behavior through aliases.
- Made tests/test_mpz_addmul_alloc_count.cpp build through mpfrxx_mkII.h and link against mpfrxx_mkII while preserving the zero-allocation fused addmul checks.
- Made tests/test_mpz_mpq_alloc_count.cpp build through mpfrxx_mkII.h and use mpfrxx::mpz_class/mpq_class aliases.
- Added stream insertion and extraction for mpfrxx::mpc_class, including expression output, precision-preserving input, failure preserving old values, and locale decimal-point handling for complex component output/input.

Missing features:
- gmpxx::mpfc_class remains GMP/MPF-only and is intentionally not exposed by mpfrxx_mkII.h.
- mpfrxx::mpfr_class standalone output still has its existing locale behavior; the new locale adaptation is scoped to mpc_class complex formatting.

Tests added:
- tests/test_mpc_io.cpp

Tests updated:
- include/gmpfrxx_mkII/detail/mpc_impl.hpp
- tests/CMakeLists.txt
- tests/test_mpz_addmul_fusion.cpp
- tests/test_mpz_addmul_alloc_count.cpp
- tests/test_mpz_mpq_alloc_count.cpp
- STATUS.md

Exact commands run:
- rg --files
- git status --short
- rg -n "test_mpfc_io|test_mpz_addmul_alloc_count|test_mpz_addmul_fusion|test_mpz_mpq_alloc_count|mpfc|addmul|alloc_count" .
- sed -n '1,220p' tests/test_mpfc_io.cpp
- sed -n '1,240p' tests/test_mpz_addmul_alloc_count.cpp
- sed -n '1,260p' tests/test_mpz_addmul_fusion.cpp
- sed -n '1,260p' tests/test_mpz_mpq_alloc_count.cpp
- sed -n '1,260p' tests/test_mpfr_string_io.cpp
- sed -n '1,260p' tests/test_mpc_basic.cpp
- sed -n '1,260p' tests/CMakeLists.txt
- rg -n "operator<<|operator>>|mpc_class|mpfr_class|addmul|fused|compound" include/gmpfrxx_mkII/detail include/mpfrxx_mkII.h include/gmpfrxx_mkII.h tests
- sed -n '1,240p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '240,720p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '540,670p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '200,260p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- rg -n "gmpxx::|#include <gmpxx_mkII.h>" tests/test_mpz_addmul_fusion.cpp tests/test_mpz_addmul_alloc_count.cpp tests/test_mpz_mpq_alloc_count.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial ctest --test-dir build --output-on-failure: FAIL, test_mpc_io locale output assertion failed.
- Fixed mpc_class component formatting to apply the stream locale decimal point.
- Final cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 82/82 tests passed.

Known issues:
- The mpc_class stream format intentionally mirrors gmpxx::mpfc_class as "(real,imag)" for the local wrapper API.

Post-phase MPFR real I/O alignment with MPF:
DONE

Implemented features:
- Aligned mpfrxx::mpfr_class stream output more closely with gmpxx::mpf_class output.
- Added an MPFR-owned allocated string RAII helper so mpfr_asprintf results are freed on all paths.
- Added locale decimal-point replacement to mpfrxx::mpfr_class output, matching the existing gmpxx::mpf_class behavior.
- Kept existing MPFR-specific formatting via mpfr_asprintf and %R conversions.

Missing features:
- No remaining known mpf_class/mpfr_class stream I/O behavior gap from the currently tested surface.

Tests added:
- Added mpfrxx::mpfr_class locale decimal-point output coverage to tests/test_mpfr_string_io.cpp.
- Added mpfrxx::mpfr_class expression locale output coverage to tests/test_mpfr_string_io.cpp.
- Added mpfrxx::mpfr_class locale decimal-point input coverage to tests/test_mpfr_string_io.cpp.

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/test_mpfr_string_io.cpp
- STATUS.md

Exact commands run:
- sed -n '1,220p' tests/test_mpf_string_io.cpp
- sed -n '1,220p' tests/test_mpfr_string_io.cpp
- sed -n '421,520p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '563,650p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '28,50p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "class gmp_allocated_string|struct gmp_allocated_string|gmp_stream_decimal_point" include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp
- sed -n '45,75p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '75,105p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- cmake --build build --target test_mpfr_string_io -j
- ctest --test-dir build -R test_mpfr_string_io --output-on-failure
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Focused cmake --build build --target test_mpfr_string_io -j: PASS.
- Focused ctest --test-dir build -R test_mpfr_string_io --output-on-failure: PASS.
- Final cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 82/82 tests passed.

Known issues:
- mpfrxx::mpfr_class still uses MPFR formatting primitives, not GMP MPF primitives; this is intentional for MPFR correctness.

Post-phase upstream test_io_and_strings.cpp comparison:
DONE

Implemented features:
- Compared ../gmpxx_mkII/tests/test_io_and_strings.cpp against the current focused I/O tests.
- Added upstream exact-type stream formatting regression cases for hex showbase zero, uppercase rational output, and internal padding with hex prefixes.
- Added upstream legacy exact input prefix/spacing cases for mpz_class and mpq_class, including partial decimal consumption, noskipws failure preservation, automatic-base negative hex, rational whitespace boundaries, and automatic-base hex rationals.
- Added upstream MPF parser edge cases for partial float consumption and invalid mantissa/exponent rollback.
- Added MPF locale decimal-point input/output table coverage and matching MPFR adaptations.
- Added matching MPFR parser edge cases for partial float consumption and invalid mantissa/exponent rollback.
- Adjusted exact integer/rational hex showbase formatting so zero is formatted as 0x0/0X0, matching the upstream regression case.

Intentional deltas:
- Raw pointer stream APIs and print helpers from upstream were initially left out, then added in the later raw GMP pointer I/O phase.

Tests added:
- Expanded tests/test_zq_string_io.cpp.
- Expanded tests/test_mpf_string_io.cpp.
- Expanded tests/test_mpfr_string_io.cpp.

Tests updated:
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- tests/test_zq_string_io.cpp
- tests/test_mpf_string_io.cpp
- tests/test_mpfr_string_io.cpp
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/tests/test_io_and_strings.cpp
- sed -n '260,560p' ../gmpxx_mkII/tests/test_io_and_strings.cpp
- sed -n '560,920p' ../gmpxx_mkII/tests/test_io_and_strings.cpp
- sed -n '1,180p' tests/test_zq_string_io.cpp
- sed -n '1,180p' tests/test_mpf_string_io.cpp
- wc -l ../gmpxx_mkII/tests/test_io_and_strings.cpp tests/test_zq_string_io.cpp tests/test_mpf_string_io.cpp tests/test_mpfr_string_io.cpp tests/test_mpfc_io.cpp tests/test_mpc_io.cpp
- rg -n "print_mpz|print_mpq|print_mpf|operator<<\\(std::ostream&.*mpz_srcptr|operator>>\\(std::istream&.*mpz_ptr|operator<<\\(std::ostream&.*mpq_srcptr|operator>>\\(std::istream&.*mpq_ptr|operator<<\\(std::ostream&.*mpf_srcptr|operator>>\\(std::istream&.*mpf_ptr" include/gmpfrxx_mkII/detail
- sed -n '760,855p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '180,360p' tests/test_zq_string_io.cpp
- sed -n '690,760p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- cmake --build build --target test_zq_string_io test_mpf_string_io test_mpfr_string_io -j
- ctest --test-dir build -R "test_zq_string_io|test_mpf_string_io|test_mpfr_string_io" --output-on-failure
- ./build/tests/test_zq_string_io
- gdb -batch -ex run -ex bt --args ./build/tests/test_zq_string_io
- nl -ba tests/test_zq_string_io.cpp | sed -n '100,120p'
- cmake --build build --target test_zq_string_io -j && ctest --test-dir build -R test_zq_string_io --output-on-failure
- gdb -batch -ex run -ex bt --args ./build/tests/test_zq_string_io
- nl -ba tests/test_zq_string_io.cpp | sed -n '120,132p'
- ctest --test-dir build -R "test_zq_string_io|test_mpf_string_io|test_mpfr_string_io|test_mpfc_io|test_mpc_io" --output-on-failure

Pass/fail result:
- Initial focused I/O run: FAIL in test_zq_string_io because hex showbase zero did not match upstream; fixed in zq_impl.hpp.
- Second focused zq run: FAIL due test stream flags carrying uppercase into the internal-padding case; fixed test setup.
- Final focused ctest --test-dir build -R "test_zq_string_io|test_mpf_string_io|test_mpfr_string_io|test_mpfc_io|test_mpc_io" --output-on-failure: PASS, 5/5 tests passed.
- Final cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 82/82 tests passed.

Known issues:
- None known for the current GMP class and raw pointer I/O cases covered by upstream test_io_and_strings.cpp.

Post-phase MPF non-decimal stream formatting:
DONE

Implemented features:
- Ported the upstream GMP-only MPF non-decimal ostream formatter into gmpxx::mpf_class.
- Added std::hex and std::oct MPF output for defaultfloat, fixed, and scientific modes.
- Added showbase prefixes for hex/octal MPF output, uppercase conversion, sign handling, and exponent marker selection matching the upstream policy.
- Aligned decimal precision-zero defaultfloat/showpoint behavior with upstream test_io_and_strings.cpp.

Tests added:
- Added std::hex/std::oct MPF stream output regression cases to tests/test_mpf_string_io.cpp, including zero, showbase, scientific hex exponent marker, fixed octal fractions, and precision-zero cases.

Tests updated:
- include/gmpfrxx_mkII/detail/mpf_impl.hpp
- tests/test_mpf_string_io.cpp
- STATUS.md

Exact commands run:
- rg -n "mpf_stream_string|format_mpf|print_mpf|showbase.*mpf|oct_showbase|hex_showbase" ../gmpxx_mkII/include/gmpxx_mkII.h.in ../gmpxx_mkII/tests/test_io_and_strings.cpp include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_mpf_string_io.cpp
- sed -n '4100,4195p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '210,330p' ../gmpxx_mkII/tests/test_io_and_strings.cpp
- sed -n '2280,2345p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '2640,2665p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '2345,2645p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- cmake --build build --target test_mpf_string_io -j
- ctest --test-dir build -R test_mpf_string_io --output-on-failure
- gdb -batch -ex run -ex bt --args ./build/tests/test_mpf_string_io
- nl -ba tests/test_mpf_string_io.cpp | sed -n '116,130p'
- ctest --test-dir build -R "test_zq_string_io|test_mpf_string_io|test_mpfr_string_io|test_mpfc_io|test_mpc_io" --output-on-failure
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial focused test_mpf_string_io run: FAIL because a reused stream still had std::hex set for a decimal zero/showpoint assertion; fixed test setup.
- Focused ctest --test-dir build -R "test_zq_string_io|test_mpf_string_io|test_mpfr_string_io|test_mpfc_io|test_mpc_io" --output-on-failure: PASS, 5/5 tests passed.
- Final cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 82/82 tests passed.

Known issues:
- None known for MPF non-decimal stream formatting.

Post-phase raw GMP pointer I/O helpers:
DONE

Implemented features:
- Added print_mpz, print_mpq, and print_mpf helpers.
- Added stream insertion for mpz_srcptr, mpq_srcptr, and mpf_srcptr.
- Added stream extraction for mpz_ptr, mpq_ptr, and mpf_ptr.
- Reused existing class-formatting and token-parsing paths so raw pointer I/O follows the same base, showbase, uppercase, locale, precision, rollback, and failbit behavior as the wrapper classes.
- Kept wrapper class stream insertion routed through the same print helpers.

Tests added:
- Added compile-time signature checks for raw pointer stream insertion/extraction and print helpers.
- Added runtime print/helper and raw stream insertion cases for mpz, mpq, and mpf.
- Added runtime raw stream extraction cases including base autodetection, hex float input, precision preservation, and invalid-input value preservation.

Tests updated:
- include/gmpfrxx_mkII/detail/zq_impl.hpp
- include/gmpfrxx_mkII/detail/mpf_impl.hpp
- tests/test_zq_string_io.cpp
- tests/test_mpf_string_io.cpp
- STATUS.md

Exact commands run:
- rg -n "print_mpz|print_mpq|print_mpf|operator<<\\(std::ostream&.*mp[zqf]_srcptr|operator>>\\(std::istream&.*mp[zqf]_ptr|mpz_ptr|mpq_ptr|mpf_ptr" ../gmpxx_mkII/include ../gmpxx_mkII/tests/test_io_and_strings.cpp
- sed -n '1,260p' ../gmpxx_mkII/tests/test_io_and_strings.cpp
- sed -n '4140,4305p' ../gmpxx_mkII/include/gmpxx_mkII.h.in
- sed -n '720,880p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '650,790p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- cmake --build build --target test_zq_string_io test_mpf_string_io -j
- ctest --test-dir build -R "test_zq_string_io|test_mpf_string_io" --output-on-failure
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial focused ctest --test-dir build -R "test_zq_string_io|test_mpf_string_io" --output-on-failure: FAIL because a new raw mpq print assertion inherited std::hex/showbase stream flags; fixed test setup.
- Focused cmake --build build --target test_zq_string_io test_mpf_string_io -j: PASS.
- Focused ctest --test-dir build -R "test_zq_string_io|test_mpf_string_io" --output-on-failure: PASS, 2/2 tests passed.
- Final cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 82/82 tests passed.

Known issues:
- None known for raw GMP pointer I/O helpers.

Post-phase MPF/MPFR numeric, power-of-two, scalar allocation, and thread tests:
DONE

Implemented features:
- Added MPF numeric equivalence tests against direct GMP C API arithmetic for unary, binary, nested, mixed-precision, and assignment-preserves-destination-precision expression cases.
- Added MPFR numeric equivalence tests against direct MPFR C API arithmetic for the natural MPFR adaptation of the same expression families.
- Added MPF power-of-two scalar behavior tests against mpf_mul_2exp/mpf_div_2exp and related GMP C API references.
- Added MPFR power-of-two scalar behavior tests against mpfr_mul_2ui/mpfr_div_2ui and related MPFR C API references.
- Added MPF and MPFR scalar allocation-count tests for the current exact scalar policy.
- Added MPF and MPFR current-policy thread tests for default construction visibility, isolation from GMP/MPFR global defaults, and parallel expression materialization.

Tests added:
- tests/test_mpf_numeric_equivalence.cpp
- tests/test_mpfr_numeric_equivalence.cpp
- tests/test_mpf_power_of_two_fusion.cpp
- tests/test_mpfr_power_of_two_fusion.cpp
- tests/test_mpf_scalar_alloc_count.cpp
- tests/test_mpfr_scalar_alloc_count.cpp
- tests/test_mpf_thread_safety.cpp
- tests/test_mpfr_thread_safety.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- ls ../gmpxx_mkII/tests
- sed -n '1,260p' ../gmpxx_mkII/tests/test_numeric_equivalence.cpp
- sed -n '1,260p' ../gmpxx_mkII/tests/test_power_of_two_fusion.cpp
- sed -n '1,280p' ../gmpxx_mkII/tests/test_scalar_alloc_count.cpp
- sed -n '1,280p' ../gmpxx_mkII/tests/test_thread_safety.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_mpf_numeric_equivalence test_mpfr_numeric_equivalence test_mpf_power_of_two_fusion test_mpfr_power_of_two_fusion test_mpf_scalar_alloc_count test_mpfr_scalar_alloc_count test_mpf_thread_safety test_mpfr_thread_safety -j
- ctest --test-dir build -R "test_mpf_numeric_equivalence|test_mpfr_numeric_equivalence|test_mpf_power_of_two_fusion|test_mpfr_power_of_two_fusion|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|test_mpf_thread_safety|test_mpfr_thread_safety" --output-on-failure
- gdb -batch -ex run -ex bt --args ./build/tests/test_mpf_thread_safety
- gdb -batch -ex run -ex bt --args ./build/tests/test_mpf_scalar_alloc_count
- gdb -batch -ex run -ex bt --args ./build/tests/test_mpfr_scalar_alloc_count
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial focused 8-test run: FAIL in scalar alloc-count tests because current exact integer scalar paths allocate through mpz, and FAIL in MPF thread test due GMP effective precision rounding/shared object assumptions.
- Final focused ctest --test-dir build -R "test_mpf_numeric_equivalence|test_mpfr_numeric_equivalence|test_mpf_power_of_two_fusion|test_mpfr_power_of_two_fusion|test_mpf_scalar_alloc_count|test_mpfr_scalar_alloc_count|test_mpf_thread_safety|test_mpfr_thread_safety" --output-on-failure: PASS, 8/8 tests passed.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 90/90 tests passed.

Known issues:
- Scalar allocation-count tests document current policy. Integer scalar leaves allocate through mpz to preserve full int64_t/uint64_t exactness; exact fit-based fast paths remain a TODO.

Post-phase ABI fingerprint and defaults policy completion:
DONE

Implemented features:
- Expanded the ABI fingerprint test to document current C++17 detail traits for MPF, MPFR, MPC, and MPFC operands.
- Added current-policy ABI assertions that std::numeric_limits and std::common_type are not specialized for wrapper types.
- Added MPF defaults policy coverage for invalid environment precision strings and invalid explicit precision setters.
- Added MPFR defaults policy coverage for environment precision, rounding aliases, exponent ranges, invalid environment values, and invalid explicit precision setters.

Tests updated:
- tests/test_abi_fingerprint.cpp
- tests/test_mpf_precision_policy.cpp
- tests/test_mpfr_defaults.cpp
- STATUS.md

Exact commands run:
- sed -n '1,280p' ../gmpxx_mkII/tests/test_abi_fingerprint.cpp
- sed -n '1,320p' ../gmpxx_mkII/tests/test_defaults_policy.cpp
- sed -n '1,260p' tests/test_abi_fingerprint.cpp
- sed -n '1,220p' tests/test_mpf_precision_policy.cpp
- sed -n '1,220p' tests/test_mpfr_defaults.cpp
- rg -n "numeric_limits|common_type|set_default|reload.*defaults|default_.*base|MPFRXX|MPFXX|setenv|unsetenv|default_mpf_precision_bits|default_precision_bits" include tests/test_abi_fingerprint.cpp tests/test_mpf_precision_policy.cpp tests/test_mpfr_defaults.cpp tests/test_mpf_thread_safety.cpp tests/test_mpfr_thread_safety.cpp
- cmake --build build --target test_abi_fingerprint test_mpf_precision_policy test_mpfr_defaults -j
- ctest --test-dir build -R "test_abi_fingerprint|test_mpf_precision_policy|test_mpfr_defaults" --output-on-failure
- gdb -batch -ex run -ex bt --args ./build/tests/test_mpfr_defaults
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial focused run: FAIL in test_mpfr_defaults because MPFR_PREC_MIN is 1, so precision 1 is valid under MPFR; changed invalid precision checks to 0.
- Final focused ctest --test-dir build -R "test_abi_fingerprint|test_mpf_precision_policy|test_mpfr_defaults" --output-on-failure: PASS, 3/3 tests passed.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 90/90 tests passed.

Known issues:
- Defaults coverage follows this repository's process-global wrapper default policy, not upstream thread-local snapshot/default-base APIs.
- std::numeric_limits/std::common_type specializations remain intentionally absent unless they become public ABI policy.

Post-phase migration TODO audit:
DONE

Implemented features:
- Audited the upstream migration table for stale TODOs and under-specified residual risks.
- Clarified that MPF log10, exp2, exp10, and expm1 are no longer pending, while upstream high-precision transcendental case-list parity remains open.
- Updated allocation-count notes to reflect the later scalar allocation-count tests and to keep alias/parser/IO/transcendent allocation counters as conditional policy work.
- Made alias-safety, conversion-backed comparison, MPFC/MPC I/O, exception/parser, and random-test follow-up TODOs explicit where they are still useful.

Tests updated:
- STATUS.md

Exact commands run:
- rg -n "test_(alias_safety|alloc_count|comparisons|exception_support|mpfc_io|random|power_of_two_fusion|scalar_alloc_count|type_conversions|unary_minus|user_defined_literals)|does not yet cover scalar|log10, exp2|remaining high-precision|TODO|Partial|Known issues" STATUS.md
- rg -n "log10|exp2|exp10|expm1|transcendent|random|mpf_class" tests include/gmpfrxx_mkII/detail/math_mpf.hpp
- git status --short
- sed -n '1408,1430p' STATUS.md
- sed -n '1856,1872p' STATUS.md
- sed -n '2084,2122p' STATUS.md
- tail -n 80 STATUS.md
- git diff -- STATUS.md
- git diff --check
- ctest --test-dir build --output-on-failure

Pass/fail result:
- git diff --check: PASS.
- ctest --test-dir build --output-on-failure: PASS, 90/90 tests passed.

Known issues:
- The migration table still intentionally marks broad upstream suites Partial/TODO until each upstream assertion is mapped line-by-line to an existing local test, a new local test, or an intentional policy difference.

Post-phase monolithic smoke migration:
DONE

Implemented features:
- Added tests/test_gmpxx_mkII.cpp as a full upstream monolithic smoke migration based on ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp.
- Added the compatibility API needed by that smoke test where it is natural public GMP-wrapper surface:
  - mpf_class string constructors, get_d/get_ui/get_si, fits_* predicates, div_2exp/mul_2exp, set_epsilon, and _mpf literals.
  - mpz_class double and 128-bit conversion support, fits_* predicates, get_d/get_ui/get_si, modulo operators, integer math helpers, and _mpz literals.
  - mpq_class canonicalize, get_d, numerator/denominator accessors, and _mpq literals.
  - MPF helper functions for neg, ceil, floor, trunc, sgn, hypot, and mpf_remainder.
- Added tests/test_mpfrxx_mkII.cpp as the MPFR/MPC counterpart structured after the upstream monolithic smoke categories. MPF-specific groups are adapted to mpfr_class behavior, exact-type groups are covered through mpfrxx mpz/mpq aliases, and complex groups are covered through mpc_class.
- Updated tests/test_mpfrxx_mkII.cpp to mirror the upstream monolithic smoke output style: each test function reports a `... passed.` line and `main()` prints a version-style banner plus `All tests passed.`.

Tests added:
- tests/test_gmpxx_mkII.cpp
- tests/test_mpfrxx_mkII.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- wc -l ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- sed -n '1,260p' ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- sed -n '260,620p' ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- sed -n '620,980p' ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- rg -n "^(void|int|bool|std::string|template|mpf_class|mpz_class|mpq_class).*\) ?\{|^int main" ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- rg -n "namespace .*literals|operator\"" include tests
- rg -n "get_default_prec|get_initial_default_prec|set_initial_default_prec|gmpxx_defaults|fits_|get_ui\(|get_si\(|get_d\(|factorial|primorial|fibonacci|const_pi|const_log2|div2exp|mul2exp|atan2|gamma|reciprocal" include/gmpfrxx_mkII include/*.h
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_gmpxx_mkII -j
- ctest --test-dir build -R test_gmpxx_mkII --output-on-failure
- rg -n "inline mpfr_class (sqrt|abs|ceil|floor|trunc|exp|log|log2|log10|sin|cos|tan|atan|atan2|pow|const_pi|const_log2|sgn|neg)" include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build --target test_mpfrxx_mkII -j
- ctest --test-dir build -R "test_gmpxx_mkII|test_mpfrxx_mkII" --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- cmake --build build --target test_mpfrxx_mkII -j
- ./build/tests/test_mpfrxx_mkII
- ctest --test-dir build -R "test_gmpxx_mkII|test_mpfrxx_mkII" --output-on-failure

Pass/fail result:
- test_gmpxx_mkII: PASS.
- test_mpfrxx_mkII: PASS after adapting MPC compound operations to assignment from expressions and using current MPC precision policy checks.
- Focused ctest --test-dir build -R "test_gmpxx_mkII|test_mpfrxx_mkII" --output-on-failure: PASS, 2/2 tests passed.
- Initial full cmake --build build -j: FAIL because mpf_class(double) was temporarily made non-explicit for upstream list initialization, which violated the existing ABI fingerprint/common_type policy, and because bool could convert through new exact-type constructors.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 92/92 tests passed.
- test_mpfrxx_mkII direct execution: PASS and now prints per-function progress output.
- Final focused ctest --test-dir build -R "test_gmpxx_mkII|test_mpfrxx_mkII" --output-on-failure after output alignment: PASS, 2/2 tests passed.

Known issues:
- test_mpfrxx_mkII.cpp is intentionally an MPFR/MPC adaptation of the upstream monolithic function structure, not a literal line-by-line copy. MPFR math groups use MPFR C API calls as references because mpfr_class math wrapper free functions are not yet exposed.
- Some upstream GMP legacy behaviors remain adapted to this repository's policy, including expression-template materialization and process-global wrapper defaults.

Post-phase mpfrxx monolithic source alignment:
DONE

Implemented features:
- Expanded tests/test_mpfrxx_mkII.cpp to follow the upstream tests/test_gmpxx_mkII.cpp function layout much more closely.
- Added MPFR counterparts for the upstream MPF groups covering double mixed arithmetic, ceil/floor/hypot/sgn, get_d/get_ui/get_si, constructor-with-raw-value coverage, literals-by-string construction, swap, get_str/set_str, trunc, fits_* predicates, const_log2, div2exp/mul2exp, log, exp, remainder, precision mixing, casts, and integer-width construction/assignment policy checks.
- Added mpfrxx exact-alias sections for upstream mpz_class and mpq_class groups, using mpfrxx::mpz_class/mpfrxx::mpq_class aliases while preserving the mpfrxx_mkII.h header boundary.
- Kept MPFR-specific adaptations explicit where upstream MPF behavior has no direct wrapper member in this header: conversion from mpfr_class to mpz_class uses mpfr_get_z in the test, mpfr_class to exact values uses mpfr_set_z/mpfr_set_q, and MPFR math functions use the MPFR C API reference path.

Tests updated:
- tests/test_mpfrxx_mkII.cpp
- STATUS.md

Exact commands run:
- rg -n "^void test|^int main|passed|MPFRXX_MKII|GMPXX_MKII" tests/test_mpfrxx_mkII.cpp
- rg -n "^void test|^int main|passed|GMPXX_MKII|MPFRXX_MKII" ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- sed -n '1,260p' tests/test_mpfrxx_mkII.cpp
- sed -n '260,620p' tests/test_mpfrxx_mkII.cpp
- cmake --build build --target test_mpfrxx_mkII -j
- ./build/tests/test_mpfrxx_mkII
- wc -l tests/test_mpfrxx_mkII.cpp ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp tests/test_gmpxx_mkII.cpp
- cmake --build build -j
- ctest --test-dir build -R "test_gmpxx_mkII|test_mpfrxx_mkII" --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build --target test_mpfrxx_mkII -j: PASS.
- ./build/tests/test_mpfrxx_mkII: PASS and prints a per-function progress line matching the upstream smoke style.
- cmake --build build -j: PASS.
- focused ctest --test-dir build -R "test_gmpxx_mkII|test_mpfrxx_mkII" --output-on-failure: PASS, 2/2 tests passed.
- full ctest --test-dir build --output-on-failure: PASS, 92/92 tests passed.

Known issues:
- test_mpfrxx_mkII.cpp is still an MPFR/MPC adaptation rather than a literal copy. Remaining intentional differences come from this repository's public API boundaries: mpfrxx_mkII.h does not expose gmpxx::mpf_class, mpfr_class does not currently expose every MPF-style math helper as a C++ free function, and mpz/mpq are aliases under mpfrxx rather than separate MPFR-owned classes.

Post-phase mpfrxx monolithic output alignment:
DONE

Implemented features:
- Updated tests/test_mpfrxx_mkII.cpp to include upstream-style detailed rule-of-five progress output for mpfr_class, mpz_class alias, and mpq_class alias tests.
- Updated int128 constructor and assignment tests to print the upstream-style Testing/Expected/Actual/result lines and to use the same large 128-bit values as the upstream monolithic smoke test.
- Reordered int128 constructor before assignment in main() to match upstream output order.

Tests updated:
- tests/test_mpfrxx_mkII.cpp
- STATUS.md

Exact commands run:
- sed -n '292,320p' ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- sed -n '954,982p' ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- sed -n '3387,3465p' ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- cmake --build build --target test_mpfrxx_mkII -j
- ./build/tests/test_mpfrxx_mkII
- ./build/tests/test_mpfrxx_mkII | tail -n 35
- ctest --test-dir build -R test_mpfrxx_mkII --output-on-failure

Pass/fail result:
- cmake --build build --target test_mpfrxx_mkII -j: PASS.
- ./build/tests/test_mpfrxx_mkII: PASS and prints detailed rule-of-five and int128 progress lines.
- ctest --test-dir build -R test_mpfrxx_mkII --output-on-failure: PASS, 1/1 test passed.

Post-phase mpfrxx string comparison alignment:
DONE

Implemented features:
- Replaced double-tolerance based MPFR/MPC test comparisons in tests/test_mpfrxx_mkII.cpp with string-based Expected/Actual comparisons.
- Added shared string comparison helpers that print Expected and Actual before asserting, normalize trailing decimal zeroes, and use the expected decimal precision for MPFR string rendering.
- Updated MPFR math checks to compare MPFR-rendered strings rather than comparing through to_double() or std::math double references.
- Updated MPC checks to compare stringified real/imaginary MPFR components rather than real_to_double()/imag_to_double().

Tests updated:
- tests/test_mpfrxx_mkII.cpp
- STATUS.md

Exact commands run:
- rg -n "require_(mpfr|mpc|)_?close|to_double\\(|real_to_double|imag_to_double|Expected:|Actual" tests/test_mpfrxx_mkII.cpp
- sed -n '1,140p' tests/test_mpfrxx_mkII.cpp
- rg -n "require_close|to_double\\(\\)|real_to_double|imag_to_double|std::(cos|sin|tan|exp|atan|atan2|log)" tests/test_mpfrxx_mkII.cpp
- cmake --build build --target test_mpfrxx_mkII -j
- ./build/tests/test_mpfrxx_mkII
- ctest --test-dir build -R test_mpfrxx_mkII --output-on-failure
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build --target test_mpfrxx_mkII -j: PASS.
- ./build/tests/test_mpfrxx_mkII: PASS and prints string Expected/Actual lines for MPFR/MPC comparisons.
- ctest --test-dir build -R test_mpfrxx_mkII --output-on-failure: PASS, 1/1 test passed.
- ctest --test-dir build --output-on-failure: PASS, 92/92 tests passed.

Post-phase current test status audit:
DONE

Implemented features:
- Updated the upstream migration table to reflect the current monolithic smoke test status:
  - tests/test_gmpxx_mkII.cpp is now present and passing as the GMP monolithic smoke migration.
  - tests/test_mpfrxx_mkII.cpp is now present and passing as the MPFR/MPC adaptation with upstream-style progress output.
  - test_type_conversions.cpp is no longer a blank TODO in practice; monolithic smoke tests cover a broad subset, while the dedicated upstream conversion matrix remains partial.
- Audited remaining TODO comments in test sources. At that point, TODO comments were only in tests/test_gmpxx_mkII.cpp:
  - line 775: review legacy copy-assignment precision expectation.
  - lines 1293 and 1304: restore legacy helper checks after adapting them to expression nodes.
  - lines 1508, 1527, 1543, 1559, 1614, 1632, 1707, 1728, and 1830: restore mpz_class compound assignment with double cases after that API is supported.
- Confirmed tests/test_mpfrxx_mkII.cpp has no TODO/FIXME/XXX comments and no double-based MPFR/MPC comparison helpers.
- Later phase "Post-phase alias safety and monolithic TODO restoration" removed these tests/test_gmpxx_mkII.cpp TODO blocks.

Tests updated:
- STATUS.md

Exact commands run:
- sed -n '2076,2122p' STATUS.md
- sed -n '2640,2735p' STATUS.md
- rg -n "TODO|FIXME|XXX" tests/test_gmpxx_mkII.cpp tests/test_mpfrxx_mkII.cpp
- rg -n "TODO|FIXME|XXX|Partial|not yet|remaining|Known issues|missing|intentional" include tests STATUS.md
- git status --short

Pass/fail result:
- No code or test-source behavior changed in this phase.
- Latest full ctest remains: ctest --test-dir build --output-on-failure: PASS, 92/92 tests passed.

Known issues:
- The STATUS migration table remains conservative. Several upstream rows still say Partial/TODO because dedicated focused tests have not yet been ported line-by-line, even where tests/test_gmpxx_mkII.cpp or tests/test_mpfrxx_mkII.cpp now provide broad smoke coverage.

Post-phase MPF transcendent assertion parity:
DONE

Implemented features:
- Completed upstream assertion-list parity for ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp inside tests/test_mpf_compute_log.cpp.
- Added missing focused assertion groups for:
  - expression overload value parity between expression inputs and materialized mpf_class values.
  - exact special values, including pow(two, -3) == 0.125.
  - functional identities: exp(log(x)), log(exp(x)), log1p(x), expm1(x), log2/log10 identities, and trigonometric identities.
  - near-zero and near-one paths for log1p/expm1/exp/log.
  - trig argument reduction around pi/2 and sin^2 + cos^2 identity.
  - pow integer cases and pow(0, negative) domain error.
  - 512/1024 precision-doubling cases that were missing for several unary functions and pow.
- Kept the assertions in the existing tests/test_mpf_compute_log.cpp target instead of adding a separate test file, matching the current local test organization.

Tests updated:
- tests/test_mpf_compute_log.cpp
- STATUS.md

Exact commands run:
- sed -n '1,560p' ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp
- sed -n '1,940p' tests/test_mpf_compute_log.cpp
- rg -n "^void (test_|require_)|^int main|assert_within_ulp|assert_close|static_assert" ../gmpxx_mkII/tests/test_mpf_transcendent_functions.cpp
- rg -n "^void (test_|require_)|^int main|require_within_ulp|require_close_bits|static_assert" tests/test_mpf_compute_log.cpp
- cmake --build build --target test_mpf_compute_log -j
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure

Pass/fail result:
- cmake --build build --target test_mpf_compute_log -j: PASS.
- ctest --test-dir build -R test_mpf_compute_log --output-on-failure: PASS, 1/1 test passed.

Known issues:
- This completes the MPF focused assertion-list parity for the upstream transcendent test. MPFR equivalents are still intentionally not treated as complete until MPFR public math wrapper APIs are defined as policy.

Post-phase MPFC arithmetic and transcendent assertion parity:
DONE

Implemented features:
- Replaced the local MPFC arithmetic smoke with the upstream `test_mpfc_arithmetic.cpp` focused case list in tests/test_mpfc_basic.cpp.
- Replaced the local MPFC math smoke with the upstream `test_mpfc_transcendent_functions.cpp` focused case list in tests/test_mpfc_math.cpp.
- Added MPFC API support needed by the upstream arithmetic assertions:
  - `mpfc_class::get_prec()`.
  - MPFC compound assignment operators for supported MPFC/MPF/scalar expression operands.
  - MPFC `swap()` now swaps component `mpf_t` values directly so component precision moves with the value.
- Added missing MPFC complex math surface:
  - asin, acos, atan, asinh, acosh, atanh.
  - reciprocal_gamma.
  - scalar-base `pow(scalar, mpfc_class)`.
  - expression overloads for the new MPFC math functions.
- Updated the upstream migration table rows for test_mpfc_arithmetic.cpp and test_mpfc_transcendent_functions.cpp from Partial to Done.

Tests updated:
- tests/test_mpfc_basic.cpp
- tests/test_mpfc_math.cpp
- STATUS.md

Exact commands run:
- git status --short
- rg -n "get_prec|precision\\(|real_precision|imag_precision|operator\\+=|operator-=|operator\\*=|operator/=|pow\\(" include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp tests/test_mpfc_basic.cpp tests/test_mpfc_math.cpp
- sed -n '1,240p' tests/test_mpfc_basic.cpp
- sed -n '240,360p' tests/test_mpfc_basic.cpp
- sed -n '1,380p' tests/test_mpfc_math.cpp
- rg -n "operator\\+=|operator-=|operator\\*=|operator/=" include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '120,210p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- sed -n '620,760p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- rg -n "is_mpfc_expression_operand|make_mpfc_operand|is_supported_mpf_scalar" include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- sed -n '220,310p' include/gmpfrxx_mkII/detail/math_mpfc.hpp
- cmake --build build --target test_mpfc_basic test_mpfc_math -j
- ctest --test-dir build -R "test_mpfc_basic|test_mpfc_math" --output-on-failure
- rg -n "mpfc|MPFC|test_mpfc|transcendent|arithmetic matrix|STATUS|TODO|Partial|Missing|未" STATUS.md
- git diff -- include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp tests/test_mpfc_basic.cpp tests/test_mpfc_math.cpp STATUS.md
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial cmake --build build --target test_mpfc_basic test_mpfc_math -j: FAIL before API additions, because MPFC lacked compound assignment, get_prec, inverse complex math, reciprocal_gamma, and scalar-base pow.
- Final cmake --build build --target test_mpfc_basic test_mpfc_math -j: PASS.
- Final ctest --test-dir build -R "test_mpfc_basic|test_mpfc_math" --output-on-failure: PASS, 2/2 tests passed.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 92/92 tests passed.

Known issues:
- Superseded by the later MPFC double-backed gamma removal phase:
  MPFC gamma and reciprocal_gamma are no longer exposed until a
  high-precision implementation is available.

Post-phase unary simplification and user-defined literals:
DONE

Implemented features:
- Added generic double-negative expression simplification so `-(-expr)` materializes as a `pos_op` expression node.
- Tightened MPFR unary operator overloads to MPFR object/node operands, avoiding ambiguity with exact MPZ/MPQ expression nodes when using the combined header.
- Added `gmpxx::mpz_class` bitwise complement operator `~`.
- Switched GMP exact user-defined literals to raw/auto-base parsing for `_mpz` and `_mpq`, covering large raw hex literals that do not fit `unsigned long long`.
- Added focused coverage for:
  - MPZ/MPQ/MPF unary plus/minus, double-negative, triple-negative, and MPZ complement behavior.
  - MPF and MPFR double-negative expression-node shape and assignment precision preservation.
  - `_mpz`, `_mpq`, and `_mpf` literals, including large hex `_mpz`, rational canonicalization, MPF floating literals, and auto-base exact string literals.
- Updated the upstream migration table rows for test_unary_minus_simplification.cpp and test_user_defined_literals.cpp from TODO to Done for current policy.

Tests added:
- tests/test_unary_minus_simplification.cpp
- tests/test_user_defined_literals.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/tests/test_unary_minus_simplification.cpp
- sed -n '1,260p' ../gmpxx_mkII/tests/test_user_defined_literals.cpp
- rg -n "operator\\\"\\\"|unary|neg_op|pos_op|literals|add_test|test_unary|test_user_defined" include tests CMakeLists.txt tests/CMakeLists.txt
- sed -n '1,130p' tests/CMakeLists.txt
- sed -n '1,130p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '1990,2030p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1600,1635p' include/gmpfrxx_mkII/detail/mpf_impl.hpp
- rg -n "mpz_class\\(const char|mpq_class\\(const char|detect|base|set_default_base|get_default_base|mpz_set_str|mpq_set_str" include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "operator~|com_op|bit" include/gmpfrxx_mkII/detail/zq_impl.hpp tests/test_mpz_basic.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_unary_minus_simplification test_user_defined_literals -j
- ctest --test-dir build -R "test_unary_minus_simplification|test_user_defined_literals" --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial focused build before CMake reconfigure: FAIL because the new targets were not yet registered.
- Initial focused build after adding generic simplification: FAIL due MPFR unary overload ambiguity with exact MPZ/MPQ nodes under the combined header.
- Initial literal focused test: FAIL because large raw hex `_mpz` selected the `unsigned long long` literal overload and overflowed.
- Final cmake --build build --target test_unary_minus_simplification test_user_defined_literals -j: PASS.
- Final ctest --test-dir build -R "test_unary_minus_simplification|test_user_defined_literals" --output-on-failure: PASS, 2/2 tests passed.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 94/94 tests passed.

Known issues:
- Complex imaginary literals were added later as `_mpfc_i` and `_mpc_i`; real-only complex literals remain intentionally outside the current public API.

Post-phase MPFR user-defined literal:
DONE

Implemented features:
- Added `mpfrxx::literals::operator"" _mpfr`.
- `_mpfr` supports floating literals through the wrapper default precision.
- `_mpfr` supports string and raw numeric literals through MPFR auto-base parsing.
- Added focused tests for decimal floating literals, string literals, hex and binary auto-base strings, and wrapper default precision.
- Updated the upstream migration table row for test_user_defined_literals.cpp to mark the MPFR adaptation as Done.

Tests added:
- tests/test_mpfr_user_defined_literals.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- rg -n "namespace literals|mpfr_class\\(|precision\\(|mpfr_class\\(const char|mpfr_class\\(const std::string|operator\\\"\\\"" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_user_defined_literals.cpp tests/test_mpfrxx_mkII.cpp
- sed -n '80,180p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tail -n 90 include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_mpfr_user_defined_literals test_user_defined_literals -j
- ctest --test-dir build -R "test_mpfr_user_defined_literals|test_user_defined_literals" --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build --target test_mpfr_user_defined_literals test_user_defined_literals -j: PASS.
- ctest --test-dir build -R "test_mpfr_user_defined_literals|test_user_defined_literals" --output-on-failure: PASS, 2/2 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 95/95 tests passed.

Known issues:
- MPC imaginary literals were added later as `_mpc_i`; real-only `_mpc` remains intentionally outside the current public API.

Post-phase imaginary complex user-defined literals:
DONE

Implemented features:
- Added `gmpxx::literals::operator"" _mpfc_i` as an imaginary-only MPFC literal aligned with `std::complex_literals`.
- Added `mpfrxx::literals::operator"" _mpc_i` as an imaginary-only MPC literal aligned with `std::complex_literals`.
- `_mpfc_i` supports floating literals, decimal string literals, and prefixed hex/binary string literals normalized to GMP MPF base-specific parsing.
- `_mpc_i` supports floating literals and MPFR auto-base string literals using the existing MPFR literal/string policy.
- Added tests showing `real + imaginary_literal` construction for both MPFC and MPC.
- Added signed and scientific-notation imaginary literal cases using binary-finite values:
  - `-3.25i` style negative literals.
  - `-9.765625e-4i`, which is exactly `-1/1024`.
  - `5.36870912e+8i`, which is exactly `2^29`.
- Updated the upstream migration table row for test_user_defined_literals.cpp to record MPFC/MPC imaginary literal coverage.

Tests updated:
- tests/test_user_defined_literals.cpp
- tests/test_mpfr_user_defined_literals.cpp
- STATUS.md

Exact commands run:
- rg -n "class mpc_class|mpc_class\\(|real_precision|imag_precision|operator\\\"\\\"|namespace literals|mpfc_class\\(" include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpf_impl.hpp tests/test_mpfr_user_defined_literals.cpp tests/test_user_defined_literals.cpp
- sed -n '50,155p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '50,120p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- sed -n '110,160p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- tail -n 90 include/gmpfrxx_mkII/detail/mpc_impl.hpp
- tail -n 80 include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- cmake --build build --target test_user_defined_literals test_mpfr_user_defined_literals -j
- ctest --test-dir build -R "test_mpfr_user_defined_literals|test_user_defined_literals" --output-on-failure

Pass/fail result:
- Initial focused ctest failed because `_mpfc_i` string parsing tried MPF base 0 for a hex floating literal; `_mpfc_i` was corrected to match existing MPF decimal string policy.
- Final cmake --build build --target test_user_defined_literals test_mpfr_user_defined_literals -j: PASS.
- Final ctest --test-dir build -R "test_mpfr_user_defined_literals|test_user_defined_literals" --output-on-failure: PASS, 2/2 tests passed.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 95/95 tests passed.

Known issues:
- Real-only `_mpfc` and `_mpc` literals are intentionally not implemented; complex literals follow the standard-library-style imaginary-only suffix design.

Post-phase MPF extended/math function assertion parity:
DONE

Implemented features:
- Added the upstream MPF extended transcendent function focused coverage as a C++17 local test.
- Added public MPF helpers needed by the upstream extended list:
  - `gmpxx::e`, `const_e`, `inv_log_two`, `log_ten`, `const_log10`, `pi_over_two`, `pi_over_four`, and `two_pi`.
- Added the upstream MPF math function focused coverage as a C++17 local test.
- Added `gmpxx::mpf_remainder(lhs, rhs)` overload in addition to the quotient-pointer form.
- Aligned `mpf_remainder` quotient behavior with the upstream floor-quotient expectations for negative dividends.
- Aligned `mpf_class::set_epsilon()` with the upstream `2^(1-precision)` expectation.
- Added scalar mixed `gmpxx::hypot(mpf_class, scalar)` and `gmpxx::hypot(scalar, mpf_class)` overloads for supported MPF scalar leaves.
- Updated the upstream migration table rows for test_mpf_extended_transcendent_functions.cpp and test_mpf_math_functions.cpp from Partial to Done for MPF assertion-list parity.

Tests added:
- tests/test_mpf_extended_transcendent_functions.cpp
- tests/test_mpf_math_functions.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- git status --short
- rg -n "test_mpf_(compute_log|pi|extended|math)|add_phase0_test" tests/CMakeLists.txt
- sed -n '1,260p' ../gmpxx_mkII/tests/test_mpf_math_functions.cpp
- sed -n '260,520p' ../gmpxx_mkII/tests/test_mpf_math_functions.cpp
- sed -n '1,260p' tests/test_mpf_extended_transcendent_functions.cpp
- sed -n '1300,1375p' include/gmpfrxx_mkII/detail/math_mpf.hpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_mpf_extended_transcendent_functions test_mpf_math_functions -j
- ctest --test-dir build -R "test_mpf_extended_transcendent_functions|test_mpf_math_functions" --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial focused build: FAIL because `gmpxx::mpf_remainder(lhs, rhs)` was missing.
- Initial focused ctest after adding the overload: FAIL because `mpf_remainder` used truncating quotient conversion, while the upstream negative-dividend cases expect floor quotient.
- Final cmake --build build --target test_mpf_extended_transcendent_functions test_mpf_math_functions -j: PASS.
- Final ctest --test-dir build -R "test_mpf_extended_transcendent_functions|test_mpf_math_functions" --output-on-failure: PASS, 2/2 tests passed.
- Final cmake --build build -j: PASS.
- Final ctest --test-dir build --output-on-failure: PASS, 97/97 tests passed.

Known issues:
- MPFR equivalents for these GMP-only upstream MPF math files are intentionally left as separate policy work until public MPFR math wrapper APIs are defined.

Post-phase MPF/MPFC counterpart audit:
DONE

Implemented features:
- Audited STATUS.md MPF and MPFC test rows against actual tests/CMakeLists.txt registrations and source files.
- Confirmed that the following MPF-focused tests have MPFR counterparts or natural MPFR extensions:
  - `test_et_contract_mpf.cpp` -> `test_et_contract_mpfr.cpp`
  - `test_mpf_basic.cpp` -> `test_mpfr_expression_eval.cpp`, `test_mpfr_scalar_eval.cpp`, and `test_mpfrxx_mkII.cpp`
  - `test_mpf_precision_policy.cpp` -> `test_mpfr_precision_policy.cpp`, plus MPFR defaults/environment tests
  - `test_mpf_aliasing.cpp` -> `test_mpfr_aliasing.cpp`
  - `test_mpf_string_io.cpp` -> `test_mpfr_string_io.cpp`
  - `test_mpf_alloc_count.cpp` -> `test_mpfr_alloc_count.cpp`
  - `test_mpf_scalar_alloc_count.cpp` -> `test_mpfr_scalar_alloc_count.cpp`
  - `test_mpf_numeric_equivalence.cpp` -> `test_mpfr_numeric_equivalence.cpp`
  - `test_mpf_power_of_two_fusion.cpp` -> `test_mpfr_power_of_two_fusion.cpp`
  - `test_mpf_thread_safety.cpp` -> `test_mpfr_thread_safety.cpp`
  - `test_mixed_zq_mpf_promotion.cpp` -> `test_mixed_zq_mpfr_promotion.cpp` and `test_mixed_zq_mpfr_exact_subexpression.cpp`
  - GMP/MPF user-defined literal coverage -> `test_mpfr_user_defined_literals.cpp`
- Confirmed that the following MPFC-focused tests have MPC counterparts or natural MPC extensions:
  - `test_et_contract_mpfc.cpp` -> `test_et_contract_mpc.cpp`
  - `test_mpfc_basic.cpp` -> `test_mpc_basic.cpp`, `test_mpc_aliasing.cpp`, and `test_mpc_precision_policy.cpp`
  - `test_mpfc_precision_policy.cpp` -> `test_mpc_precision_policy.cpp`
  - `test_mpfc_io.cpp` -> `test_mpc_io.cpp`
  - MPFC imaginary literal coverage -> MPC imaginary literal coverage in `test_mpfr_user_defined_literals.cpp`

Tests added:
- None.

Tests updated:
- STATUS.md

Exact commands run:
- rg -n "mpf|mpfc|mpfr|mpc|Migration table|test_" STATUS.md
- rg --files tests
- sed -n '2073,2122p' STATUS.md
- sed -n '1,120p' tests/CMakeLists.txt
- rg -n "^(void|template|int main)|assert\\(|static_assert|gamma|sin|cos|tan|exp|log|sqrt|pow|asin|acos|atan|sinh|cosh|tanh|hypot|remainder|epsilon|precision|alias|swap|operator" tests/test_mpf_math_functions.cpp tests/test_mpf_extended_transcendent_functions.cpp tests/test_mpf_compute_log.cpp tests/test_mpfrxx_mkII.cpp tests/test_mpfr_*.cpp
- rg -n "^(void|template|int main)|assert\\(|static_assert|gamma|sin|cos|tan|exp|log|sqrt|pow|asin|acos|atan|sinh|cosh|tanh|precision|alias|swap|operator|real|imag" tests/test_mpfc_basic.cpp tests/test_mpfc_math.cpp tests/test_mpfc_io.cpp tests/test_mpc_*.cpp tests/test_mpfrxx_mkII.cpp
- rg --files include/gmpfrxx_mkII/detail
- sed -n '1388,1532p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp

Pass/fail result:
- Audit only; no build or CTest run was needed because no production or test source code changed.

Known issues:
- MPF math/transcendent focused tests do not yet have true MPFR focused counterparts. `test_mpfrxx_mkII.cpp` has MPFR C API smoke coverage for log/exp/trig/pow-related behavior, but this repository currently does not expose MPFR free-function wrappers parallel to `gmpxx::log`, `gmpxx::exp`, `gmpxx::sin`, `gmpxx::gamma`, etc.
- This audit was later updated by "Post-phase MPC math wrappers": MPC now has `tests/test_mpc_math.cpp` covering MPC 1.3.1 sqrt/exp/log/log10/trig/hyperbolic/inverse/pow wrappers against direct `mpc_*` C API references.
- `test_mpfc_basic.cpp` is broader than `test_mpc_basic.cpp`; the MPC side covers basic arithmetic and aliasing, but does not mirror the full MPFC arithmetic matrix, swap/accessor/helper surface, and std::complex comparison matrix line-by-line.

Post-phase alias safety and monolithic TODO restoration:
DONE

Implemented features:
- Compared `../gmpxx_mkII/tests/test_alias_safety.cpp` against `tests/test_alias_safety.cpp`.
- Confirmed local alias coverage includes every upstream MPF alias stress case and matching MPFR adaptations.
- Restored the disabled `tests/test_gmpxx_mkII.cpp` MPF copy-assignment precision assertion using this repository's wrapper-owned default precision policy.
- Adapted the legacy `test_func(f * h, g)` helper to expression-template materialization so the MPF and MPZ monolithic smoke assertions can run safely.
- Added `gmpxx::mpz_class` compound assignment overloads for `double` to match existing legacy binary `mpz_class`/`double` arithmetic operators.
- Restored the disabled monolithic `mpz_class` compound assignment with `double` assertions.
- Updated the upstream migration table rows for `test_alias_safety.cpp` and `test_gmpxx_mkII.cpp`.

Tests added:
- None.

Tests updated:
- tests/test_gmpxx_mkII.cpp
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/tests/test_alias_safety.cpp
- sed -n '1,320p' tests/test_alias_safety.cpp
- rg -n "TODO|FIXME|XXX|disabled|legacy|expression-node|compound assignment|double|TODO" tests/test_gmpxx_mkII.cpp STATUS.md
- rg -n "^void |^template|^int main|assert\\(|static_assert|passed|TODO" ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp tests/test_gmpxx_mkII.cpp
- sed -n '740,790p' tests/test_gmpxx_mkII.cpp
- sed -n '1260,1320p' tests/test_gmpxx_mkII.cpp
- sed -n '1480,1568p' tests/test_gmpxx_mkII.cpp
- sed -n '1588,1840p' tests/test_gmpxx_mkII.cpp
- rg -n "operator\\+=|operator-=|operator\\*=|operator/=|is_zq_expression_operand|supported.*scalar|double" include/gmpfrxx_mkII/detail/zq_impl.hpp include/gmpfrxx_mkII/detail/integer_conversion.hpp
- sed -n '1140,1225p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1690,1790p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- sed -n '1888,1955p' include/gmpfrxx_mkII/detail/zq_impl.hpp
- rg -n "test_func\\(" tests/test_gmpxx_mkII.cpp ../gmpxx_mkII/tests/test_gmpxx_mkII.cpp
- sed -n '1,180p' include/gmpfrxx_mkII/detail/expr.hpp
- sed -n '1,60p' tests/test_gmpxx_mkII.cpp
- cmake --build build --target test_alias_safety test_gmpxx_mkII -j
- ctest --test-dir build -R "test_alias_safety|test_gmpxx_mkII" --output-on-failure
- rg -n "TODO|FIXME|XXX" tests/test_gmpxx_mkII.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build --target test_alias_safety test_gmpxx_mkII -j: PASS.
- ctest --test-dir build -R "test_alias_safety|test_gmpxx_mkII" --output-on-failure: PASS, 2/2 tests passed.
- rg -n "TODO|FIXME|XXX" tests/test_gmpxx_mkII.cpp: PASS, no matches.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 97/97 tests passed.

Known issues:
- The monolithic smoke is no longer carrying local TODO blocks. Broader focused split work remains tracked under dedicated rows such as `test_mpz_arithmetic.cpp`, `test_mpq_arithmetic.cpp`, `test_scalar_arithmetic.cpp`, and `test_type_conversions.cpp`.

Post-phase gmpxx/mpfrxx monolithic parallel alignment:
DONE

Implemented features:
- Aligned `tests/test_mpfrxx_mkII.cpp` with the newly restored `tests/test_gmpxx_mkII.cpp` monolithic coverage shape.
- Added MPFR-side copy-construction/default-assignment precision checks parallel to the MPF copy/default-assignment checks.
- Added an expression-template materializing `test_func(f * h, g)` helper to the MPFR/MPC monolithic smoke.
- Added MPFR `mpfr_class` extension coverage using the same `test_func(f * h, g)` expression shape as the MPF monolithic smoke.
- Added exact-alias `mpfrxx::mpz_class` extension coverage using the same `test_func(f * h, g)` expression shape as the GMP monolithic smoke.
- Added exact-alias `mpfrxx::mpz_class` double compound assignment checks parallel to the restored GMP monolithic `mpz_class` double compound checks.

Tests added:
- None.

Tests updated:
- tests/test_mpfrxx_mkII.cpp
- STATUS.md

Exact commands run:
- rg -n "test_.*precision|test_func|extention|compound|double|operator|TODO|FIXME|XXX|mpz_class_addition|mpz_class_subtraction|mpz_class_multiplication|mpz_class_division" tests/test_gmpxx_mkII.cpp tests/test_mpfrxx_mkII.cpp
- sed -n '720,820p' tests/test_gmpxx_mkII.cpp
- sed -n '720,850p' tests/test_mpfrxx_mkII.cpp
- sed -n '1270,1320p' tests/test_gmpxx_mkII.cpp
- sed -n '900,980p' tests/test_mpfrxx_mkII.cpp
- sed -n '1,60p' tests/test_mpfrxx_mkII.cpp
- sed -n '560,590p' tests/test_mpfrxx_mkII.cpp
- sed -n '1320,1350p' tests/test_mpfrxx_mkII.cpp
- sed -n '860,910p' tests/test_mpfrxx_mkII.cpp
- sed -n '930,980p' tests/test_mpfrxx_mkII.cpp
- cmake --build build --target test_gmpxx_mkII test_mpfrxx_mkII -j
- ctest --test-dir build -R "test_gmpxx_mkII|test_mpfrxx_mkII" --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake --build build --target test_gmpxx_mkII test_mpfrxx_mkII -j: PASS.
- ctest --test-dir build -R "test_gmpxx_mkII|test_mpfrxx_mkII" --output-on-failure: PASS, 2/2 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 97/97 tests passed.

Known issues:
- `tests/test_mpfrxx_mkII.cpp` remains an MPFR/MPC adaptation rather than a literal copy because `mpfrxx_mkII.h` intentionally does not expose `gmpxx::mpf_class`, and MPFR math free-function wrappers parallel to GMP-only MPF helpers are not yet public API.

Post-phase MPC math wrappers:
DONE

Implemented features:
- Read GNU MPC 1.3.1 manual sections for:
  - GNU MPC basics, rounding modes, return values, branch cuts, and complex-function precision semantics.
  - Power functions and logarithm.
  - Trigonometric, hyperbolic, inverse trigonometric, and inverse hyperbolic functions.
- Cross-checked the installed MPC 1.3.1 headers for available C API functions.
- Added public `mpfrxx::mpc_class` math wrappers:
  - `sqrt`
  - `exp`
  - `log`
  - `log10`
  - `sin`
  - `cos`
  - `tan`
  - `sinh`
  - `cosh`
  - `tanh`
  - `asin`
  - `acos`
  - `atan`
  - `asinh`
  - `acosh`
  - `atanh`
  - `pow`
- Added expression-input overload coverage for unary MPC math and `pow`.
- Added `tests/test_mpc_math.cpp`, comparing each wrapper directly against the corresponding MPC 1.3.1 `mpc_*` C API at the same destination precision and rounding mode.
- Updated the upstream migration table row for `test_mpfc_transcendent_functions.cpp` to record the separate MPC analog.

Tests added:
- tests/test_mpc_math.cpp

Tests updated:
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- rg -n "mpc_(sqrt|exp|log|sin|cos|tan|sinh|cosh|tanh|asin|acos|atan|asinh|acosh|atanh|pow|gamma|root|proj|conj|norm|abs)" /usr/include /usr/local/include
- rg -n "mpc_(sqrt|exp|log|sin|cos|tan|sinh|cosh|tanh|asin|acos|atan|asinh|acosh|atanh|pow)" include tests
- find /usr/share/doc /usr/local/share/doc -iname '*mpc*' -o -iname '*libmpc*'
- dpkg -l | rg "libmpc|mpc"
- sed -n '1,220p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '620,725p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '360,545p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '545,625p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_mpc_math -j
- ctest --test-dir build -R test_mpc_math --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check
- rg -n "TODO|FIXME|XXX" tests/test_mpc_math.cpp include/gmpfrxx_mkII/detail/mpc_impl.hpp STATUS.md
- git status --short
- tail -n 120 STATUS.md
- rg -n "test_mpfc_transcendent_functions|test_mpc_math|MPC complex math|test_mpfc_math|test_mpc_basic|Post-phase MPF/MPFC counterpart audit" STATUS.md

Pass/fail result:
- cmake --build build --target test_mpc_math -j: PASS.
- ctest --test-dir build -R test_mpc_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 98/98 tests passed.
- git diff --check: PASS.
- rg -n "TODO|FIXME|XXX" tests/test_mpc_math.cpp include/gmpfrxx_mkII/detail/mpc_impl.hpp STATUS.md: PASS, no matches in the touched source/test files.

Known issues:
- MPC 1.3.1 does not provide `mpc_gamma` or `mpc_rgamma`, so gamma/reciprocal_gamma remain MPFC-only in this repository.
- The new MPC math wrappers are eager public math helpers, matching the existing MPF/MPFC math-helper pattern rather than expression-template arithmetic operators.

Post-phase MPC additional math wrappers:
DONE

Implemented features:
- Re-scanned the installed MPC 1.3.1 header for math-like APIs beyond sqrt/exp/log/trig/hyperbolic/inverse/pow.
- Added public `mpfrxx::mpc_class` math/helper wrappers for the natural next API group:
  - `conj`
  - `real`
  - `imag`
  - `abs`
  - `norm`
  - `arg`
  - `proj`
  - `sqr`
  - `fma`
  - `agm`
  - `rootofunity`
  - `sin_cos`
- `real`, `imag`, `abs`, `norm`, and `arg` return `mpfrxx::mpfr_class`; the others return `mpfrxx::mpc_class` except `sin_cos`, which returns `std::pair<mpc_class, mpc_class>`.
- Added object and expression-input coverage for the added wrappers where applicable.

Tests added:
- None.

Tests updated:
- tests/test_mpc_math.cpp
- STATUS.md

Exact commands run:
- rg -n "^__MPC_DECLSPEC.*mpc_[a-zA-Z0-9_]+|^int mpc_[a-zA-Z0-9_]+|mpc_[a-zA-Z0-9_]+ \\(" /usr/include/mpc.h
- rg -n "mpc_(sqrt|exp|log|log10|sin|cos|tan|sinh|cosh|tanh|asin|acos|atan|asinh|acosh|atanh|pow|arg|abs|norm|conj|proj|rootofunity|mul_i|div|add|sub|mul|fma|sqr|rec|neg)" /usr/include/mpc.h include/gmpfrxx_mkII/detail/mpc_impl.hpp tests/test_mpc_math.cpp
- sed -n '1,220p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '80,180p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '1,230p' tests/test_mpc_math.cpp
- rg -n "with_precision|mpfr_data|default_rounding_mode" include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '220,385p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '560,805p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- rg -n "\\b(abs|norm|arg|real|imag|conj|proj|sqr|fma|agm|rootofunity|sin_cos|sum|dot)\\b" include tests
- cmake --build build --target test_mpc_math -j
- ctest --test-dir build -R test_mpc_math --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake --build build --target test_mpc_math -j: PASS.
- ctest --test-dir build -R test_mpc_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 98/98 tests passed.
- git diff --check: PASS.

Known issues:
- `mpc_sum` and `mpc_dot` remain unwrapped. They are vector/array reduction APIs rather than scalar unary/binary math helpers, so they need a separate public container/API policy.
- Specialized `mpc_pow_fr`, `mpc_pow_d`, `mpc_pow_si`, `mpc_pow_ui`, and `mpc_pow_z` remain unwrapped as fast paths. Current public `pow` materializes operands to `mpc_class` and calls `mpc_pow`.
- `mpc_mul_i`, `mpc_mul_2ui`, `mpc_div_2ui`, `mpc_mul_2si`, and `mpc_div_2si` remain covered by general arithmetic/scalar expression routes rather than public named wrappers.

Post-phase MPFR initial math wrappers:
DONE

Implemented features:
- Redirected the initial MPFR API parity work to `mpfrxx::mpfr_class`; no new `gmpxx::mpf_class` math APIs were kept in this phase.
- Added public eager MPFR math wrappers:
  - `sqrt`
  - `sqr`
  - `sqrt_ui`
  - `rec_sqrt`
  - `agm`
  - `fma`
  - `sin_cos`
  - `sinh_cosh`
- Object and expression operands are supported where the API accepts MPFR operands.
- Result precision policy:
  - Unary functions preserve operand precision.
  - `sqrt_ui(value, precision)` uses the explicit destination precision; `sqrt_ui(value)` uses the wrapper default precision.
  - Binary and ternary functions use the maximum operand precision.
  - Pair-return functions return both values at the materialized operand precision.

Tests added:
- tests/test_mpfr_math.cpp

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/CMakeLists.txt
- STATUS.md

Exact commands run:
- git status --short
- git diff -- include/gmpfrxx_mkII/detail/math_mpf.hpp tests/test_mpf_math_functions.cpp
- rg -n "math_mpfr|mpfr_class.*sqrt|mpfr_(sqrt|sqr|agm|sin_cos|sinh_cosh|fma)|namespace mpfrxx" include tests
- sed -n '610,960p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1380,1565p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "is_mpfr_object|object_or_node|mpfr_expression_precision\\(" include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1018,1120p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,120p' tests/CMakeLists.txt
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target test_mpfr_math -j
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake --build build --target test_mpfr_math -j: PASS.
- ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 99/99 tests passed.
- git diff --check: PASS.

Known issues:
- Remaining MPFR math API candidates include `fms`, `fmma`, `fmms`, `min`, `max`, `dim`, `cbrt`, root variants, log/exp variants, trig `pi`/`u` variants, special functions, and remainder/rounding variants.
- MPFR vector APIs such as `mpfr_sum` and `mpfr_dot` remain unwrapped pending a public container/API policy.

Post-phase MPFR trig and hyperbolic wrappers:
DONE

Implemented features:
- Added public `mpfrxx::mpfr_class` trig/hyperbolic wrappers:
  - `sin`
  - `cos`
  - `tan`
  - `asin`
  - `acos`
  - `atan`
  - `atan2`
  - `sinh`
  - `cosh`
  - `tanh`
  - `asinh`
  - `acosh`
  - `atanh`
  - `sec`
  - `csc`
  - `cot`
  - `sech`
  - `csch`
  - `coth`
  - `sinu`
  - `cosu`
  - `tanu`
  - `asinu`
  - `acosu`
  - `atanu`
  - `atan2u`
  - `sinpi`
  - `cospi`
  - `tanpi`
  - `asinpi`
  - `acospi`
  - `atanpi`
  - `atan2pi`
- Object and expression operands are supported where the API accepts MPFR operands.
- Unary result precision preserves the materialized operand precision; binary result precision uses the maximum operand precision.
- Extended `tests/test_mpfr_math.cpp` to compare each wrapper against the corresponding MPFR 4.2.2 `mpfr_*` C API at the same precision and rounding mode.

Tests added:
- None.

Tests updated:
- include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- sed -n '34,48p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1560,1710p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,260p' tests/test_mpfr_math.cpp
- cmake --build build --target test_mpfr_math -j
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- rg -n "inline mpfr_class (sin|cos|tan|asin|acos|atan|atan2|sinh|cosh|tanh|asinh|acosh|atanh|sec|csc|cot|sech|csch|coth|sinu|cosu|tanu|asinu|acosu|atanu|atan2u|sinpi|cospi|tanpi|asinpi|acospi|atanpi|atan2pi)\\b|mpfr_(sin|cos|tan|asin|acos|atan|atan2|sinh|cosh|tanh|asinh|acosh|atanh|sec|csc|cot|sech|csch|coth|sinu|cosu|tanu|asinu|acosu|atanu|atan2u|sinpi|cospi|tanpi|asinpi|acospi|atanpi|atan2pi)" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check
- rg -n "gmpfrxx_mKII|TODO|FIXME|XXX" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp

Pass/fail result:
- cmake --build build --target test_mpfr_math -j: PASS.
- ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 99/99 tests passed.
- git diff --check: PASS.
- rg -n "gmpfrxx_mKII|TODO|FIXME|XXX" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp: PASS, no matches.

Known issues:
- Remaining MPFR math API candidates now exclude the trig/hyperbolic group covered in this phase.
- Non-trig remaining candidates include `fms`, `fmma`, `fmms`, `min`, `max`, `dim`, `hypot`, `cbrt`, root variants, log/exp variants, pow variants, special functions, and remainder/rounding variants.

Post-phase MPFR double trig/hyperbolic smoke:
DONE

Implemented features:
- Added deterministic random smoke coverage comparing `mpfrxx` trig/hyperbolic wrappers on `mpfr_class(double, 192)` inputs against double formulas for 64 generated cases.
- Covered:
  - `sin`
  - `cos`
  - `tan`
  - `asin`
  - `acos`
  - `atan`
  - `atan2`
  - `sinh`
  - `cosh`
  - `tanh`
  - `asinh`
  - `acosh`
  - `atanh`
  - `sec`
  - `csc`
  - `cot`
  - `sech`
  - `csch`
  - `coth`
  - `sinu`
  - `cosu`
  - `tanu`
  - `asinu`
  - `acosu`
  - `atanu`
  - `atan2u`
  - `sinpi`
  - `cospi`
  - `tanpi`
  - `asinpi`
  - `acospi`
  - `atanpi`
  - `atan2pi`
- Kept this as a smoke test, separate from the exact MPFR C API parity checks.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- rg -n "mpfrxx::(sin|cos|tan|asin|acos|atan|atan2|sinh|cosh|tanh|asinh|acosh|atanh|sec|csc|cot|sech|csch|coth|sinu|cosu|tanu|asinu|acosu|atanu|atan2u|sinpi|cospi|tanpi|asinpi|acospi|atanpi|atan2pi)\\([^\\n]*(double|0\\.|1\\.|2\\.|3\\.|4\\.|5\\.|6\\.|7\\.|8\\.|9\\.|[0-9]+\\))|double .*mpfrxx::|static_cast<double|mpfr_class\\([^,]+\\.[0-9]" tests/test_mpfr_math.cpp tests/test_mpfr*.cpp
- sed -n '90,230p' tests/test_mpfr_math.cpp
- cmake --build build --target test_mpfr_math -j
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- rg -n "test_double_sin_random_smoke|std::sin|mt19937" tests/test_mpfr_math.cpp
- rg -n "test_double_trig_hyperbolic_random_smoke|assert_double_close|std::(sin|cos|tan|asin|acos|atan|atan2|sinh|cosh|tanh|asinh|acosh|atanh)" tests/test_mpfr_math.cpp
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake --build build --target test_mpfr_math -j: PASS.
- ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 99/99 tests passed.
- git diff --check: PASS.

Known issues:
- This smoke test checks numerical agreement with platform `std::*` functions and simple reciprocal/unit/pi formulas within a double tolerance; it is not a substitute for the existing exact MPFR C API parity checks.

Post-phase MPFR power/root and log/exp wrappers:
DONE

Implemented features:
- Added `mpfrxx::mpfr_class` wrappers for MPFR power/root APIs:
  - `pow`
  - `powr`
  - `pow_si`
  - `pow_ui`
  - `pow_z`
  - `pow_sj`
  - `pow_uj`
  - `ui_pow`
  - `ui_pow_ui`
  - `compound_si`
  - `cbrt`
  - `rootn_ui`
  - `rootn_si`
- Added `mpfrxx::mpfr_class` wrappers for MPFR log/exp APIs:
  - `log`
  - `log2`
  - `log10`
  - `log1p`
  - `log2p1`
  - `log10p1`
  - `log_ui`
  - `exp`
  - `exp2`
  - `exp10`
  - `expm1`
  - `exp2m1`
  - `exp10m1`
  - `eint`
  - `li2`
- Result precision follows the existing math-wrapper policy: unary wrappers preserve operand precision, binary wrappers use max operand precision, and scalar-only constructors such as `ui_pow_ui` and `log_ui` provide explicit-precision and default-precision overloads.
- `mpfr_root` was intentionally not exposed because it is deprecated in MPFR; `rootn_ui` and `rootn_si` are the supported root wrappers.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- rg -n "MPFRXX_DEFINE|sqrt_ui|rec_sqrt|agm|fma|sin_cos|test_mpfr|log2p1|powr" include tests STATUS.md
- sed -n '1540,1885p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,260p' tests/test_mpfr_math.cpp
- sed -n '260,470p' tests/test_mpfr_math.cpp
- rg -n "mpfr_(powr|compound_si|rootn_si|log2p1|exp2m1)" /usr/include/mpfr.h
- tail -n 90 STATUS.md
- rg -n "mpz_data\\(|mpq_data\\(|class mpz_class|class mpfr_class|is_mpfr_expression_operand_v|is_mpfr_object_or_node_v" include/gmpfrxx_mkII/detail include/*.h
- rg -n "mpfr_(pow|pow_ui|pow_si|pow_z|pow_sj|pow_uj|ui_pow|ui_pow_ui|root|rootn_ui|cbrt|log|exp|li2)" /usr/include/mpfr.h
- sed -n '1,80p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- rg -n "mpfr_eint" /usr/include/mpfr.h
- cmake --build build --target test_mpfr_math -j
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- git diff -- include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp
- rg -n "powr|pow_si|pow_ui|pow_z|pow_sj|pow_uj|ui_pow|ui_pow_ui|compound_si|cbrt|rootn|log2p1|exp2m1|eint|li2" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp
- cmake --build build -j
- rg -n "\\broot\\(|mpfr_root\\b|rootn_ui|rootn_si" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp
- git diff --check
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- Initial cmake --build build --target test_mpfr_math -j: FAIL because `pow_z` used the `mpfrxx::mpz_class` alias before the alias declaration; fixed by using `::gmpxx::mpz_class` in the implementation.
- Focused cmake --build build --target test_mpfr_math -j: PASS.
- Focused ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- git diff --check: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 99/99 tests passed.

Known issues:
- `mpfr_root` is intentionally not wrapped because MPFR marks it deprecated; use `rootn_ui` or `rootn_si`.
- Remaining MPFR math API candidates include fused/misc arithmetic (`fms`, `fmma`, `fmms`, `min`, `max`, `dim`, `hypot`), special functions, constants, rounding/remainder helpers, scaling/sign/next/comparison helpers, and vector-like helpers (`sum`, `dot`).

Post-phase MPFR fused and misc arithmetic wrappers:
DONE

Implemented features:
- Added `mpfrxx::mpfr_class` wrappers for MPFR fused arithmetic APIs:
  - `fms`
  - `fmma`
  - `fmms`
- Added `mpfrxx::mpfr_class` wrappers for MPFR misc arithmetic APIs:
  - `min`
  - `max`
  - `dim`
  - `hypot`
- Added an internal four-operand MPFR math helper for `fmma` and `fmms`.
- Result precision follows the existing math-wrapper policy: binary wrappers use max operand precision, ternary wrappers use max of three operands, and four-operand wrappers use max of four operands.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- rg -n "ternary_mpfr_math|fma\\(|check_ternary|check_binary|test_binary_and_ternary|mpfr_fm" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp /usr/include/mpfr.h
- sed -n '1540,2045p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '1,470p' tests/test_mpfr_math.cpp
- sed -n '796,812p' /usr/include/mpfr.h
- cmake --build build --target test_mpfr_math -j
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- tail -n 95 STATUS.md
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- Focused cmake --build build --target test_mpfr_math -j: PASS.
- Focused ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 99/99 tests passed.
- git diff --check: PASS.

Known issues:
- Remaining MPFR math API candidates now exclude fused/misc arithmetic. Remaining groups include special functions, constants, rounding/remainder helpers, scaling/sign/next/comparison helpers, and vector-like helpers (`sum`, `dot`).

Post-phase MPFR constants wrappers:
DONE

Implemented features:
- Added `mpfrxx::mpfr_class` wrappers for MPFR constants:
  - `const_pi`
  - `const_log2`
  - `const_euler`
  - `const_catalan`
- Each constant has explicit-precision and default-precision overloads.
- Implementations delegate directly to the corresponding MPFR C APIs:
  - `mpfr_const_pi`
  - `mpfr_const_log2`
  - `mpfr_const_euler`
  - `mpfr_const_catalan`

Tests added:
- None.

Tests updated:
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- rg -n "const_pi|const_log2|mpfr_const|sqrt_ui|log_ui|ui_pow_ui|test_mpfr_class_const" include tests STATUS.md /usr/include/mpfr.h
- sed -n '1588,1710p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- sed -n '220,470p' tests/test_mpfr_math.cpp
- cmake --build build --target test_mpfr_math -j
- ctest --test-dir build -R test_mpfr_math --output-on-failure
- rg -n "const_pi|const_log2|const_euler|const_catalan|test_constants_against_mpfr" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests/test_mpfr_math.cpp STATUS.md
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake --build build --target test_mpfr_math -j: PASS.
- ctest --test-dir build -R test_mpfr_math --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 99/99 tests passed.
- git diff --check: PASS.

Known issues:
- Remaining MPFR math API candidates now exclude constants. Remaining groups include special functions, rounding/remainder helpers, scaling/sign/next/comparison helpers, and vector-like helpers (`sum`, `dot`).

Post-phase example02 MPF/MPFR port:
DONE

Implemented features:
- Ported `../gmpxx_mkII/examples/example02.cpp` into this repository as:
  - `examples/example02_mpf.cpp`
  - `examples/example02_mpfr.cpp`
- Kept the MPF version faithful to the upstream example body, using `gmpxx::mpf_class`, 256-bit precision, and `gmpxx::sqrt(two)`.
- Added a parallel MPFR version with minimal changes: `mpfrxx::mpfr_class`, `mpfr_prec_t`, `mpfrxx::sqrt(two)`, and `mpfrxx_mkII.h`.
- Registered both examples in `examples/CMakeLists.txt` and CTest.

Tests added:
- example02_mpf
- example02_mpfr

Tests updated:
- examples/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,240p' ../gmpxx_mkII/examples/example02.cpp
- ls -la examples
- sed -n '1,220p' examples/CMakeLists.txt
- rg -n "example02|mpf|mpfr|example" examples CMakeLists.txt STATUS.md
- sed -n '1,100p' examples/example01.cpp
- sed -n '1,100p' examples/example02_mpfr_mpc.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build --target example02_mpf example02_mpfr -j
- ctest --test-dir build -R "example02_mpf|example02_mpfr" --output-on-failure
- ./build/examples/example02_mpf
- ./build/examples/example02_mpfr
- cmake --build build -j
- ctest --test-dir build --output-on-failure

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build --target example02_mpf example02_mpfr -j: PASS.
- ctest --test-dir build -R "example02_mpf|example02_mpfr" --output-on-failure: PASS, 3/3 tests passed because the regex also matched the existing `example02_mpfr_mpc`.
- Direct `./build/examples/example02_mpf`: PASS, printed `sqrt(2)` to 50 digits.
- Direct `./build/examples/example02_mpfr`: PASS, printed matching `sqrt(2)` output to 50 digits.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 101/101 tests passed.

Known issues:
- None for the example02 MPF/MPFR port.

Post-phase MPFR sign/next/predicate/comparison helpers:
DONE

Implemented features:
- Added MPFR sign helpers:
  - `mpfrxx::abs`
  - `mpfrxx::signbit`
  - `mpfrxx::sgn`
  - `mpfrxx::setsign`
  - `mpfrxx::copysign`
- Added MPFR next-representable helpers:
  - `mpfrxx::nextabove`
  - `mpfrxx::nextbelow`
  - `mpfrxx::nexttoward`
- Added MPFR predicate helpers:
  - `mpfrxx::nan_p`
  - `mpfrxx::inf_p`
  - `mpfrxx::number_p`
  - `mpfrxx::integer_p`
  - `mpfrxx::zero_p`
  - `mpfrxx::regular_p`
- Added MPFR comparison helpers:
  - `mpfrxx::cmpabs`
  - `mpfrxx::cmpabs_ui`
  - `mpfrxx::reldiff`
  - `mpfrxx::eq`
  - `mpfrxx::greater_p`
  - `mpfrxx::greaterequal_p`
  - `mpfrxx::less_p`
  - `mpfrxx::lessequal_p`
  - `mpfrxx::lessgreater_p`
  - `mpfrxx::equal_p`
  - `mpfrxx::unordered_p`
- Helpers accept MPFR wrapper values and expression operands where natural, then delegate to the corresponding MPFR C APIs.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_comparisons.cpp
- STATUS.md

Exact commands run:
- cmake --build build -j --target test_mpfr_comparisons
- ctest --test-dir build -R '^test_mpfr_comparisons$' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake --build build -j --target test_mpfr_comparisons: PASS.
- ctest --test-dir build -R '^test_mpfr_comparisons$' --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 101/101 tests passed.
- git diff --check: PASS.

Known issues:
- Remaining MPFR math API candidates still include special functions, rounding/remainder helpers, scaling helpers, and vector-like helpers (`sum`, `dot`).

Post-phase MPFR special functions:
DONE

Implemented features:
- Added MPFR unary special-function wrappers:
  - `mpfrxx::erf`
  - `mpfrxx::erfc`
  - `mpfrxx::gamma`
  - `mpfrxx::lngamma`
  - `mpfrxx::digamma`
  - `mpfrxx::zeta`
  - `mpfrxx::j0`
  - `mpfrxx::j1`
  - `mpfrxx::y0`
  - `mpfrxx::y1`
  - `mpfrxx::ai`
- Added MPFR special-function wrappers with additional arguments:
  - `mpfrxx::lgamma`, returning `std::pair<mpfr_class, int>` for value plus sign.
  - `mpfrxx::zeta_ui`, with default-precision and explicit-precision overloads.
  - `mpfrxx::fac_ui`, with default-precision and explicit-precision overloads.
  - `mpfrxx::jn`
  - `mpfrxx::yn`
  - `mpfrxx::gamma_inc`
  - `mpfrxx::beta`
- Wrappers accept MPFR wrapper values and expression operands where natural, preserve operand/max precision policy, and delegate to the corresponding MPFR C APIs.

Tests added:
- None.

Tests updated:
- tests/test_mpfr_math.cpp
- STATUS.md

Exact commands run:
- sed -n '1,220p' tests/test_mpfr_math.cpp
- sed -n '220,760p' tests/test_mpfr_math.cpp
- sed -n '1600,2495p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp
- cmake --build build -j --target test_mpfr_math
- ctest --test-dir build -R '^test_mpfr_math$' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake --build build -j --target test_mpfr_math: PASS.
- ctest --test-dir build -R '^test_mpfr_math$' --output-on-failure: PASS, 1/1 test passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 101/101 tests passed.
- git diff --check: PASS.

Known issues:
- Remaining MPFR math API candidates now exclude special functions. Remaining groups include rounding/remainder helpers, scaling helpers, additional comparison/conversion helpers, and vector-like helpers (`sum`, `dot`).

Post-phase example03 MPF/MPFR port:
DONE

Implemented features:
- Ported `../gmpxx_mkII/examples/example03.cpp` into this repository as:
  - `examples/example03_mpf.cpp`
  - `examples/example03_mpfr.cpp`
- Kept the MPF version faithful to the upstream Newton/Heron iteration example, using `gmpxx::mpf_class`, `gmpxx::abs`, `gmpxx::sqrt`, and wrapper default precision setup.
- Added a parallel MPFR version with minimal changes: `mpfrxx::mpfr_class`, `mpfr_prec_t`, `mpfrxx::abs`, `mpfrxx::sqrt`, and `mpfrxx_mkII.h`.
- Registered both examples in `examples/CMakeLists.txt` and CTest.

Tests added:
- example03_mpf
- example03_mpfr

Tests updated:
- examples/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,260p' ../gmpxx_mkII/examples/example03.cpp
- sed -n '1,220p' examples/CMakeLists.txt
- ls -la examples
- rg -n "set_initial_default_prec|set_initial_default_precision|default.*precision|gmpxx_defaults|mpfrxx_defaults" include examples tests
- sed -n '1,140p' examples/example02_mpf.cpp
- sed -n '1,140p' examples/example02_mpfr.cpp
- cmake --build build -j --target example03_mpf example03_mpfr
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target example03_mpf example03_mpfr
- ctest --test-dir build -R 'example03_mpf|example03_mpfr' --output-on-failure
- ./build/examples/example03_mpf
- ./build/examples/example03_mpfr
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- Initial cmake --build build -j --target example03_mpf example03_mpfr: FAIL because the build tree had not been reconfigured after adding new targets.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j --target example03_mpf example03_mpfr: PASS.
- ctest --test-dir build -R 'example03_mpf|example03_mpfr' --output-on-failure: PASS, 3/3 tests passed because the regex also matched existing `example03_mpfc_math`.
- Direct `./build/examples/example03_mpf`: PASS, printed Newton iteration for sqrt(2) and matching `sqrt()` result to 50 digits.
- Direct `./build/examples/example03_mpfr`: PASS, printed matching Newton iteration and `sqrt()` result to 50 digits.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 103/103 tests passed.
- git diff --check: PASS.

Known issues:
- None for the example03 MPF/MPFR port.

Post-phase example04 MPF/MPFR port:
DONE

Implemented features:
- Ported `../gmpxx_mkII/examples/example04.cpp` into this repository as:
  - `examples/example04_mpf.cpp`
  - `examples/example04_mpfr.cpp`
- Kept the MPF version faithful to the upstream Gauss-Legendre / Brent-Salamin pi example, using `gmpxx::mpf_class`, `gmpxx::sqrt`, `gmpxx::abs`, `gmpxx::const_pi`, and wrapper default precision setup.
- Added a parallel MPFR version with minimal changes: `mpfrxx::mpfr_class`, `mpfr_prec_t`, `mpfrxx::sqrt`, `mpfrxx::abs`, `mpfrxx::const_pi`, and `mpfrxx_mkII.h`.
- Registered both examples in `examples/CMakeLists.txt` and CTest.

Tests added:
- example04_mpf
- example04_mpfr

Tests updated:
- examples/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,280p' ../gmpxx_mkII/examples/example04.cpp
- sed -n '1,260p' examples/CMakeLists.txt
- ls -la examples
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target example04_mpf example04_mpfr
- ctest --test-dir build -R 'example04_mpf|example04_mpfr' --output-on-failure
- ./build/examples/example04_mpf
- ./build/examples/example04_mpfr
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j --target example04_mpf example04_mpfr: PASS.
- ctest --test-dir build -R 'example04_mpf|example04_mpfr' --output-on-failure: PASS, 2/2 tests passed.
- Direct `./build/examples/example04_mpf`: PASS, printed Gauss-Legendre iteration for pi and matching `const_pi()` result to 100 digits.
- Direct `./build/examples/example04_mpfr`: PASS, printed matching Gauss-Legendre iteration and `const_pi()` result to 100 digits.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 105/105 tests passed.
- git diff --check: PASS.

Known issues:
- None for the example04 MPF/MPFR port.

Post-phase example05 MPF/MPFR port:
DONE

Implemented features:
- Ported `../gmpxx_mkII/examples/example05.cpp` into this repository as:
  - `examples/example05_mpf.cpp`
  - `examples/example05_mpfr.cpp`
- Kept the MPF version faithful to the upstream Aberth-Ehrlich root-finding example, including the local real-only complex type built from `gmpxx::mpf_class`, Cauchy-radius initialization, Horner evaluation, simultaneous correction update, and residual printing.
- Added a parallel MPFR version with minimal changes: `mpfrxx::mpfr_class`, `mpfr_prec_t`, `mpfrxx::sqrt`, `mpfrxx::abs`, `mpfrxx::sin`, `mpfrxx::cos`, `mpfrxx::const_pi`, and `mpfrxx_mkII.h`.
- Registered both examples in `examples/CMakeLists.txt` and CTest.

Tests added:
- example05_mpf
- example05_mpfr

Tests updated:
- examples/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,320p' ../gmpxx_mkII/examples/example05.cpp
- sed -n '1,280p' examples/CMakeLists.txt
- ls -la examples
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target example05_mpf example05_mpfr
- ctest --test-dir build -R 'example05_mpf|example05_mpfr' --output-on-failure
- ./build/examples/example05_mpf
- ./build/examples/example05_mpfr
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j --target example05_mpf example05_mpfr: PASS.
- ctest --test-dir build -R 'example05_mpf|example05_mpfr' --output-on-failure: PASS, 2/2 tests passed.
- Direct `./build/examples/example05_mpf`: PASS, printed Aberth iteration, roots, and residuals.
- Direct `./build/examples/example05_mpfr`: PASS, printed matching Aberth iteration/root structure and small residuals.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 107/107 tests passed.
- git diff --check: PASS.

Known issues:
- None for the example05 MPF/MPFR port.

Post-phase MPC compound assignment and example06 MPFC/MPC port:
DONE

Implemented features:
- Added `mpfrxx::mpc_class` compound assignment operators:
  - `operator+=`
  - `operator-=`
  - `operator*=`
  - `operator/=`
- Compound assignment delegates through existing expression assignment, preserving destination real/imag precision.
- Ported `../gmpxx_mkII/examples/example06.cpp` into this repository as:
  - `examples/example06_mpfc.cpp`
  - `examples/example06_mpc.cpp`
- Kept the MPFC version faithful to the upstream Aberth-Ehrlich `gmpxx::mpfc_class` example, with only local include/default-precision API adjustments.
- Added the MPC version as the MPFR/MPC counterpart, using `mpfrxx::mpfr_class`, `mpfrxx::mpc_class`, `mpfr_prec_t`, `mpfrxx::abs`, `mpfrxx::sin`, `mpfrxx::cos`, `mpfrxx::const_pi`, and `mpfrxx_mkII.h`.
- Registered both examples in `examples/CMakeLists.txt` and CTest.

Tests added:
- example06_mpfc
- example06_mpc

Tests updated:
- tests/test_mpc_basic.cpp
- examples/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,360p' ../gmpxx_mkII/examples/example06.cpp
- sed -n '1,320p' examples/CMakeLists.txt
- rg -n "example06|gamma|beta|erf|zeta|fac_ui|const_" include examples tests
- sed -n '1,180p' examples/example03_mpfc_math.cpp
- rg -n "class mpc_class|mpc_class\\(|real\\(|imag\\(|abs\\(|operator\\+=|operator/=|mpc_data|get_mpc" include/gmpfrxx_mkII/detail/mpc_impl.hpp include/gmpfrxx_mkII/detail/math_mpc.hpp tests/test_mpc_math.cpp tests/test_mpc_basic.cpp
- rg -n "mpfc_class\\(|real\\(|imag\\(|abs\\(|operator\\+=|operator/=|mpfc_data" include/gmpfrxx_mkII/detail/mpfc_impl.hpp include/gmpfrxx_mkII/detail/math_mpfc.hpp tests/test_mpfc_math.cpp tests/test_mpfc_basic.cpp
- sed -n '49,190p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '1040,1105p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '640,725p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp
- sed -n '620,760p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '1,260p' tests/test_mpc_basic.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target example06_mpfc example06_mpc
- cmake --build build -j --target test_mpc_basic example06_mpc example06_mpfc
- ctest --test-dir build -R 'test_mpc_basic|example06_mpc|example06_mpfc' --output-on-failure
- ./build/examples/example06_mpfc
- ./build/examples/example06_mpc
- cmake --build build -j --target test_mpc_basic example06_mpc example06_mpfc
- ctest --test-dir build -R 'test_mpc_basic|example06_mpc|example06_mpfc' --output-on-failure
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- Initial cmake --build build -j --target example06_mpfc example06_mpc: FAIL because `mpc_class` did not yet provide compound assignment operators.
- cmake --build build -j --target test_mpc_basic example06_mpc example06_mpfc after adding compound assignment: PASS.
- ctest --test-dir build -R 'test_mpc_basic|example06_mpc|example06_mpfc' --output-on-failure: PASS, 3/3 tests passed.
- Direct `./build/examples/example06_mpfc`: PASS, printed Aberth iteration, roots, and residuals.
- Direct `./build/examples/example06_mpc`: PASS, printed matching Aberth iteration/root structure and small residuals.
- cmake --build build -j --target test_mpc_basic example06_mpc example06_mpfc after restoring compound syntax in the MPC example: PASS.
- ctest --test-dir build -R 'test_mpc_basic|example06_mpc|example06_mpfc' --output-on-failure after restoring compound syntax: PASS, 3/3 tests passed.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 109/109 tests passed.
- git diff --check: PASS.

Known issues:
- None for MPC compound assignment or the example06 MPFC/MPC port.

Post-phase example07 MPFC/MPC port and MPC pair constructor:
DONE

Implemented features:
- Added `mpfrxx::mpc_class(const mpfr_class&, const mpfr_class&)`.
- The constructor initializes real and imaginary precision from the two
  `mpfr_class` operands independently and sets the value through
  `mpc_set_fr_fr`.
- Ported `../gmpxx_mkII/examples/example07.cpp` into this repository as:
  - `examples/example07_mpfc.cpp`
  - `examples/example07_mpc.cpp`
- Kept the MPFC version faithful to the upstream Mandelbrot escape-time
  renderer, with only local include/default-precision API adjustments.
- Added the MPC version as the MPFR/MPC counterpart, using
  `mpfrxx::mpfr_class`, `mpfrxx::mpc_class`, `mpfr_prec_t`,
  `mpfrxx::norm`, `mpfrxx_mkII.h`, and the new pair constructor.
- Registered both examples in `examples/CMakeLists.txt` and CTest with
  reduced render dimensions for fast test execution.

Tests added:
- example07_mpfc
- example07_mpc

Tests updated:
- tests/test_mpc_basic.cpp
- examples/CMakeLists.txt
- STATUS.md

Exact commands run:
- sed -n '1,360p' ../gmpxx_mkII/examples/example07.cpp
- sed -n '361,760p' ../gmpxx_mkII/examples/example07.cpp
- sed -n '1,120p' examples/CMakeLists.txt
- ls -la examples
- sed -n '1,360p' examples/example06_mpc.cpp
- rg -n "get_mpfr|mpfr_data|mpfr_ptr|real_precision|imag_precision|with_precision" include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpc_impl.hpp
- cp ../gmpxx_mkII/examples/example07.cpp examples/example07_mpfc.cpp
- cp ../gmpxx_mkII/examples/example07.cpp examples/example07_mpc.cpp
- sed -n '1,230p' include/gmpfrxx_mkII/detail/mpc_impl.hpp
- sed -n '1,150p' tests/test_mpc_basic.cpp
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j --target test_mpc_basic example07_mpfc example07_mpc
- ctest --test-dir build -R 'test_mpc_basic|example07_mpfc|example07_mpc' --output-on-failure
- ./build/examples/example07_mpfc --width 24 --height 10 --iterations 24
- ./build/examples/example07_mpc --width 24 --height 10 --iterations 24
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j --target test_mpc_basic example07_mpfc example07_mpc: PASS.
- ctest --test-dir build -R 'test_mpc_basic|example07_mpfc|example07_mpc' --output-on-failure: PASS, 3/3 tests passed.
- Direct `./build/examples/example07_mpfc --width 24 --height 10 --iterations 24`: PASS, printed ASCII Mandelbrot output.
- Direct `./build/examples/example07_mpc --width 24 --height 10 --iterations 24`: PASS, printed matching ASCII Mandelbrot output.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 111/111 tests passed.
- git diff --check: PASS.

Known issues:
- None for the example07 MPFC/MPC port or the MPC pair constructor.

Post-phase example06 MPC constructor cleanup:
DONE

Implemented features:
- Updated `examples/example06_mpc.cpp` to use
  `mpfrxx::mpc_class(const mpfr_class&, const mpfr_class&)` in its
  `make_complex` helpers.
- Removed raw `mpc_set_fr_fr` usage from the example06/example07 MPC example
  sources; direct MPC C API construction now stays in the wrapper
  implementation.

Tests added:
- None.

Tests updated:
- STATUS.md

Exact commands run:
- rg -n "mpc_set_fr_fr|make_complex" examples/example06_mpc.cpp examples/example07_mpc.cpp
- sed -n '60,105p' examples/example06_mpc.cpp
- cmake --build build -j --target example06_mpc example07_mpc test_mpc_basic
- ctest --test-dir build -R 'example06_mpc|example07_mpc|test_mpc_basic' --output-on-failure
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
- cmake --build build -j
- ctest --test-dir build --output-on-failure
- git diff --check

Pass/fail result:
- rg -n "mpc_set_fr_fr" examples/example06_mpc.cpp examples/example07_mpc.cpp: PASS, no matches.
- cmake --build build -j --target example06_mpc example07_mpc test_mpc_basic: PASS.
- ctest --test-dir build -R 'example06_mpc|example07_mpc|test_mpc_basic' --output-on-failure: PASS, 3/3 tests passed.
- cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug: PASS.
- cmake --build build -j: PASS.
- ctest --test-dir build --output-on-failure: PASS, 111/111 tests passed.
- git diff --check: PASS.

Known issues:
- None.

Post-phase GMP builtin scalar common_type parity:
DONE

Implemented features:
- Added explicit `std::common_type` specializations for supported builtin
  scalar types with `gmpxx::mpz_class`, `gmpxx::mpq_class`,
  `gmpxx::mpf_class`, and `gmpxx::mpfc_class`.
- Kept the scalar set aligned with expression scalar policy and MPFR behavior:
  ordinary integral types plus `float` and `double` are accepted; `bool` and
  `long double` remain unsupported.
- Used concrete builtin-type specializations rather than a broad partial
  specialization, so expression-node `std::common_type` remains unambiguous.

Tests added:
- Added GMP builtin scalar `std::common_type` coverage to
  `tests/test_common_type.cpp`.
- Added ABI fingerprint coverage for
  `std::common_type_t<gmpxx::mpf_class, double>`.

Tests updated:
- `tests/test_common_type.cpp`
- `tests/test_abi_fingerprint.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '520,590p' include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `sed -n '560,610p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1,130p' tests/test_abi_fingerprint.cpp`
- `sed -n '190,225p' tests/test_abi_fingerprint.cpp`
- `rg -n "is_supported_mpf_scalar|normalized_mpf_scalar|is_supported_mpfr_scalar|normalized_mpfr_scalar|is_zq_scalar_operand" include/gmpfrxx_mkII/detail`
- `cmake --build build -j --target test_common_type test_abi_fingerprint`
- `ctest --test-dir build -R 'test_common_type|test_abi_fingerprint' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- Initial generic partial specialization build: FAIL due ambiguity with
  expression-node `std::common_type` specializations.
- `cmake --build build -j --target test_common_type test_abi_fingerprint`: PASS after switching to explicit builtin-type specializations.
- `ctest --test-dir build -R 'test_common_type|test_abi_fingerprint' --output-on-failure`: PASS, 2/2 tests passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- Namespace-level GMP UDL re-export remains out of scope for this phase.

Post-phase MPFR UDL namespace visibility parity:
DONE

Implemented features:
- Re-exported `mpfrxx::literals::operator"" _mpfr` into namespace
  `mpfrxx`.
- Re-exported `mpfrxx::literals::operator"" _mpc_i` into namespace
  `mpfrxx`.
- `using namespace mpfrxx;` now exposes MPFR real and MPC imaginary UDLs,
  matching the GMP-side namespace-level visibility policy.

Tests added:
- Added `using namespace mpfrxx;` visibility coverage to
  `tests/test_mpfr_user_defined_literals.cpp` for numeric `_mpfr`, string
  `_mpfr`, and `_mpc_i`.

Tests updated:
- `tests/test_mpfr_user_defined_literals.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "operator\"\" _mpz|operator\"\" _mpq|operator\"\" _mpf|operator\"\" _mpfr|operator\"\" _mpc_i|operator\"\" _mpfc_i|using literals::operator" include/gmpfrxx_mkII/detail tests/test_user_defined_literals.cpp tests/test_mpfr_user_defined_literals.cpp`
- `sed -n '3425,3480p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1135,1170p' include/gmpfrxx_mkII/detail/mpc_impl.hpp`
- `sed -n '1,220p' tests/test_mpfr_user_defined_literals.cpp`
- `cmake --build build -j --target test_mpfr_user_defined_literals`
- `ctest --test-dir build -R 'test_mpfr_user_defined_literals' --output-on-failure`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `git diff --check`

Pass/fail result:
- `cmake --build build -j --target test_mpfr_user_defined_literals`: PASS.
- `ctest --test-dir build -R 'test_mpfr_user_defined_literals' --output-on-failure`: PASS, 1/1 test passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 119/119 tests passed.
- `git diff --check`: PASS.

Known issues:
- None.

Post-phase GMP/MPFR scaled-full benchmark tier timing:
DONE

Implemented features:
- Added `scaled-full` benchmark tier documentation for GMP and MPFR.
- Documented measured wall time on the Ryzen Threadripper 3970X benchmark
  host for GMP, MPFR, and sequential GMP+MPFR scaled-full runs.

Tests added:
- None.

Tests updated:
- `benchmarks/README.md`
- `STATUS.md`

Exact commands run:
- `mkdir -p benchmarks/gmp/results_raw/scaled_full_20260510`
- `mkdir -p benchmarks/mpfr/results_raw/scaled_full_20260510`
- `/usr/bin/time -f 'TOTAL_WALL_SECONDS %e' benchmarks/common/run_benchmarks.sh build_bench_clean_release 512 10000000 10000000 1000 1000 200 200 200 benchmarks/gmp/results_raw/scaled_full_20260510 10`
- `/usr/bin/time -f 'TOTAL_WALL_SECONDS %e' benchmarks/common/run_mpfr_benchmarks.sh build_bench_clean_release 512 10000000 benchmarks/mpfr/results_raw/scaled_full_20260510 10 10000000 1000 1000 200 200 200`

Pass/fail result:
- GMP scaled-full benchmark: PASS, `TOTAL_WALL_SECONDS 2258.85`.
- MPFR scaled-full benchmark: PASS, `TOTAL_WALL_SECONDS 2736.24`.
- Sequential GMP+MPFR scaled-full total: `4995.09` seconds.
- Result logs and plots were written under
  `benchmarks/gmp/results_raw/scaled_full_20260510/` and
  `benchmarks/mpfr/results_raw/scaled_full_20260510/`.

Known issues:
- None.

Post-phase MPFR NaN comparison semantics:
DONE

Implemented features:
- Added NaN-aware MPFR comparison evaluation for boolean comparison
  operators.
- `mpfrxx::mpfr_class` comparisons now treat unordered operands with
  IEEE/MPFR semantics: equality is false, inequality is true, and ordered
  comparisons are false when either side is NaN.
- Kept the existing `mpfrxx::cmp` three-way helper unchanged for source
  compatibility.

Tests added:
- Added MPFR NaN comparison coverage for object/object, object/scalar,
  scalar/object, and expression comparisons.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/test_mpfr_comparisons.cpp`
- `STATUS.md`

Exact commands run:
- `rg -n "inline int cmp|operator==|operator!=|operator<=|operator>=|test_mpfr_comparisons|mpfr_cmp|mpfr_nan_p" include/gmpfrxx_mkII/detail/mpfr_impl.hpp tests STATUS.md`
- `git status --short`
- `sed -n '2325,2420p' include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `sed -n '1,340p' tests/test_mpfr_comparisons.cpp`
- `cmake --build build -j --target test_mpfr_comparisons`
- `ctest --test-dir build -R test_mpfr_comparisons --output-on-failure`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfr_comparisons`: PASS.
- `ctest --test-dir build -R test_mpfr_comparisons --output-on-failure`: PASS, 1/1 test passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 152/152 tests passed.

Known issues:
- `mpfrxx::cmp` intentionally remains a three-way helper backed by
  `mpfr_cmp`; callers needing unordered status should use the boolean
  comparison operators or the NaN-aware comparison result helper.

Post-phase MPQ stream extraction canonicalization comment:
DONE

Implemented features:
- Documented that `mpq_class` and raw `mpq_ptr` stream extraction
  intentionally use `mpq_set_str` directly, matching `gmpxx.h` semantics.
- Kept the existing non-canonicalizing extraction behavior unchanged.

Tests added:
- None.

Tests updated:
- `include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `STATUS.md`

Exact commands run:
- `rg -n "operator>>\\(.*mpq|set_str\\(|mpq_set_str|mpq_canonicalize|print_mpq|read.*mpq" include/gmpfrxx_mkII/detail tests README.md STATUS.md`
- `git status --short`
- `nl -ba include/gmpfrxx_mkII/detail/zq_impl.hpp | sed -n '1218,1245p;1428,1442p'`
- `cmake --build build -j --target test_zq_string_io`
- `git diff --check`
- `ctest --test-dir build -R '^test_zq_string_io$' --output-on-failure`
- `cmake --build build -j`
- `git diff -- include/gmpfrxx_mkII/detail/zq_impl.hpp`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_zq_string_io`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build -R '^test_zq_string_io$' --output-on-failure`: PASS, 1/1 test passed.
- `cmake --build build -j`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 152/152 tests passed.

Known issues:
- None.

Post-phase scalar precision contribution policy:
DONE

Implemented features:
- Changed MPF, MPFR, and MPFC scalar expression leaves to contribute zero
  precision, matching exact integer and rational leaves.
- Added MPF expression-constructor fallback so expression materialization
  uses the wrapper default precision only when no operand contributes a
  precision.
- Added MPFC real/imaginary materialization precision helpers with the same
  zero-precision fallback behavior.
- Low-precision floating leaves mixed with scalar operands now preserve the
  floating leaf precision instead of being promoted to the default precision.

Tests added:
- Added MPF, MPFR, and MPFC materialization checks for low-precision
  floating leaf plus scalar expressions.

Tests updated:
- `include/gmpfrxx_mkII/detail/mpf_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `tests/test_mpf_fixed_precision_materialization.cpp`
- `tests/test_mpfr_fixed_precision_materialization.cpp`
- `tests/test_mpfc_precision_policy.cpp`
- `STATUS.md`

Exact commands run:
- `sed -n '1,190p' tests/test_mpfc_precision_policy.cpp`
- `sed -n '1,90p' tests/test_mpc_precision_policy.cpp`
- `git status --short`
- `sed -n '1,230p' include/gmpfrxx_mkII/detail/mpfc_impl.hpp`
- `cmake --build build -j --target test_mpf_fixed_precision_materialization test_mpfr_fixed_precision_materialization test_mpfc_precision_policy test_mpf_basic test_mpfr_scalar_eval`
- `ctest --test-dir build -R 'test_mpf_fixed_precision_materialization|test_mpfr_fixed_precision_materialization|test_mpfc_precision_policy|test_mpf_basic|test_mpfr_scalar_eval' --output-on-failure`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `git diff -- include/gmpfrxx_mkII/detail/mpf_impl.hpp include/gmpfrxx_mkII/detail/mpfr_impl.hpp include/gmpfrxx_mkII/detail/mpfc_impl.hpp tests/test_mpf_fixed_precision_materialization.cpp tests/test_mpfr_fixed_precision_materialization.cpp tests/test_mpfc_precision_policy.cpp`
- `git status --short`

Pass/fail result:
- Targeted build: PASS.
- Targeted CTest: PASS, 5/5 tests passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 152/152 tests passed.

Known issues:
- MPC scalar precision policy remains unchanged in this phase.

Post-phase MPFR per-thread default initialization:
DONE

Implemented features:
- Replaced process-once MPFR environment default initialization with
  per-thread initialization.
- MPFR default precision, rounding mode, and exponent range are now applied
  to libmpfr-owned TLS state once per thread on first wrapper default access.
- `reload_mpfr_defaults_from_environment()` reloads the environment defaults
  for the calling thread's MPFR TLS state.
- Documented that MPC defaults intentionally share MPFR's libmpfr-owned TLS
  default state and do not keep separate wrapper-side default storage.

Tests added:
- Added MPFR thread-safety coverage proving a worker thread receives the
  wrapper 512-bit default on first use even after the main thread changes its
  own MPFR default precision.

Tests updated:
- `include/gmpfrxx_mkII/detail/environment.hpp`
- `include/gmpfrxx_mkII/detail/mpc_environment.hpp`
- `tests/test_mpfr_thread_safety.cpp`
- `STATUS.md`

Exact commands run:
- `nl -ba include/gmpfrxx_mkII/detail/environment.hpp | sed -n '190,300p'`
- `nl -ba include/gmpfrxx_mkII/detail/mpc_environment.hpp | sed -n '95,205p'`
- `nl -ba include/gmpfrxx_mkII/detail/config.hpp | sed -n '70,95p'`
- `git status --short`
- `rg -n "mpfr_buildopt_tls|mpfr_buildopt_tls_p|GMPFRXX_MKII_MPFR|initialize_mpfr_defaults_once|reload_mpfr_defaults_from_environment|test_mpfr_thread|test_mpfr_defaults|environment" CMakeLists.txt cmake include tests`
- `sed -n '1,260p' tests/test_mpfr_thread_safety.cpp`
- `sed -n '1,180p' tests/test_mpfr_defaults.cpp`
- `sed -n '1,60p' include/gmpfrxx_mkII/detail/environment.hpp`
- `git diff -- include/gmpfrxx_mkII/detail/environment.hpp include/gmpfrxx_mkII/detail/mpc_environment.hpp tests/test_mpfr_thread_safety.cpp`
- `cmake --build build -j --target test_mpfr_thread_safety`
- `ctest --test-dir build -R '^test_mpfr_thread_safety$' --output-on-failure`
- `cmake --build build -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `cmake --build build -j --target test_mpfr_thread_safety`: PASS.
- `ctest --test-dir build -R '^test_mpfr_thread_safety$' --output-on-failure`: PASS, 1/1 test passed.
- `cmake --build build -j`: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 152/152 tests passed.

Known issues:
- None.

Post-phase GMP MPF default-context specification:
DONE

Implemented features:
- Documented that GMP `mpf_set_default_prec()` / `mpf_get_default_prec()` are
  process-global ambient state, not TLS default state.
- Documented that `gmpxx::mpf_class` default construction uses the wrapper
  default precision policy and must not rely on GMP's ambient global default.
- Documented the two GMP default-context modes:
  frozen-env header-only mode and external-provider mode.
- Documented that mutable thread-specific MPF defaults require external-provider
  mode and exactly one provider implementation linked into the process.
- Documented that provider objects must not be embedded into multiple shared
  libraries, and that participating DSOs should observe the same provider token.

Tests added:
- None. Documentation-only phase.

Tests updated:
- `SPECIFICATIONS.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' SPECIFICATIONS.md`
- `sed -n '1,260p' include/gmpfrxx_mkII/detail/gmp_default_context.hpp`
- `tail -n 80 STATUS.md`
- `git diff --check`
- `git status --short`
- `tail -n 55 STATUS.md`
- `rg -n "Post-phase GMP MPF default-context specification|Post-phase MPFR/MPC default-state specification|Post-phase MPFR per-thread" STATUS.md`
- `ctest --test-dir build -R '^test_mpfr_thread_safety$' --output-on-failure`

Pass/fail result:
- Documentation update completed.
- `git diff --check`: PASS.
- `ctest --test-dir build -R '^test_mpfr_thread_safety$' --output-on-failure`: PASS, 1/1 test passed.

Known issues:
- None.

Post-phase GMP Rdot 10M repeat10 OpenMP 01-04 benchmark refresh:
DONE

Implemented features:
- Re-ran the GMP Rdot benchmark at `N=10000000`, 512-bit precision,
  `repeat=10`, and `OMP_NUM_THREADS=32` after aligning OpenMP 01/02/03/04
  kernel source shapes with the serial 01/02/03/04 kernels.
- Added a new raw log, summary CSV, and serial/OpenMP plots under
  `benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513`.
- Updated `benchmarks/gmp/00_Rdot/README.md` so the recorded sample,
  artifact links, analysis, and hotpath summary all refer to the new current
  OpenMP 01/02/03/04 split.
- Documented that OpenMP 01/02 remain allocation-bound while OpenMP 03/04
  reuse one product object per thread and reach the C native OpenMP range.

Tests added:
- None. Benchmark artifact and documentation phase.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513/benchmark_rdot_n1e7_512_openmp_01_04_repeat10.log`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513/summary_rdot_n1e7_512_openmp_01_04_repeat10.csv`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513/benchmark_rdot_n1e7_512_openmp_01_04_repeat10_Linux_Ryzen_3970X_32-Core_serial_Rdot.png`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513/benchmark_rdot_n1e7_512_openmp_01_04_repeat10_Linux_Ryzen_3970X_32-Core_serial_summary.png`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513/benchmark_rdot_n1e7_512_openmp_01_04_repeat10_Linux_Ryzen_3970X_32-Core_openmp_Rdot.png`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513/benchmark_rdot_n1e7_512_openmp_01_04_repeat10_Linux_Ryzen_3970X_32-Core_openmp_summary.png`
- `STATUS.md`

Exact commands run:
- `pgrep -af 'Rdot_gmp_|benchmark_rdot_n1e8|run_benchmarks'`
- `git status --short`
- `mkdir -p benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513`
- `OMP_NUM_THREADS=32 /bin/bash -lc 'set -u; out="benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513/benchmark_rdot_n1e7_512_openmp_01_04_repeat10.log"; ... run Rdot 01/02/03/04 variants 10 times ...'`
- `python3 benchmarks/common/plot.py benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513/benchmark_rdot_n1e7_512_openmp_01_04_repeat10.log --output-dir benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513 --backend GMP`
- `find benchmarks/gmp/results_raw/rdot_n1e7_openmp_01_04_20260513 -maxdepth 1 -type f -printf '%f %s bytes\n' | sort`
- `python3 - <<'PY' ... parse Rdot repeat10 log and write summary CSV ... PY`
- `sed -n '1,330p' benchmarks/gmp/00_Rdot/README.md`
- `sed -n '60,145p' benchmarks/gmp/00_Rdot/README.md && sed -n '250,340p' benchmarks/gmp/00_Rdot/README.md`
- `tail -n 90 STATUS.md`
- `rg -n "Post-phase GMP Rdot OpenMP|Post-phase GMP Rdot README|Post-phase GMP Rgemm" STATUS.md`
- `wc -l STATUS.md && sed -n '1840,1905p' STATUS.md`
- `tail -n 120 STATUS.md | head -n 120`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- `pgrep`: PASS, no stale 100M benchmark process remained.
- Rdot benchmark run: PASS, all 26 variants completed 10/10 runs and all runs
  reported `OK`.
- Plot generation: PASS, serial/OpenMP plot and summary images written.
- Summary CSV generation: PASS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- The interrupted 100M benchmark left an untracked partial artifact directory
  under `benchmarks/gmp/results_raw/rdot_n1e8_openmp_01_04_20260513`; it is
  not part of this benchmark refresh.

Post-phase GMP Rdot OpenMP 03 hotpath documentation:
DONE

Implemented features:
- Updated `benchmarks/gmp/00_Rdot/README.md` with a release-binary
  disassembly comparison for `C_native_openmp_01`, `kernel_openmp_03_orig`,
  `kernel_openmp_03_mkII`, and
  `kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH`.
- Documented that these OpenMP 03 variants have the same timed inner-loop
  shape: one `__gmpf_mul`, one `__gmpf_add`, pointer increments, and the loop
  branch.
- Documented that the small max-MFLOPS ordering differences in the repeat-10
  run should be treated as OpenMP run-to-run variance, not as a different
  arithmetic kernel.

Tests added:
- None. Documentation-only phase.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '120,340p' benchmarks/gmp/00_Rdot/README.md`
- `tail -80 STATUS.md`
- `git status --short`
- `objdump -d --demangle --no-show-raw-insn --start-address=0x3430 --stop-address=0x35a0 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_openmp_01`
- `objdump -d --demangle --no-show-raw-insn --start-address=0x2f70 --stop-address=0x3130 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_03_orig`
- `objdump -d --demangle --no-show-raw-insn --start-address=0x3400 --stop-address=0x35d0 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_03_mkII`
- `objdump -d --demangle --no-show-raw-insn --start-address=0x34a0 --stop-address=0x3670 build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_03_mkII_FIXED_PRECISION_FASTPATH`
- `git diff -- benchmarks/gmp/00_Rdot/README.md`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Documentation update completed.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- Existing untracked benchmark artifacts remain outside this documentation
  update.

Post-phase GMP Rdot accumulator-unroll 05/06 benchmark:
DONE

Implemented features:
- Added GMP Rdot `kernel_05` as a four-way accumulator unroll with
  `acc0..acc3` and one reused product temporary.
- Added GMP Rdot `kernel_06` as a four-way accumulator unroll with
  `acc0..acc3` and four reused product temporaries.
- Added matching OpenMP variants `kernel_openmp_05` and `kernel_openmp_06`
  with the same per-thread source shapes.
- Registered serial and OpenMP 05/06 benchmark targets for upstream
  `gmpxx.h`, `gmpxx_mkII`, and `gmpxx_mkII_FIXED_PRECISION_FASTPATH`.
- Re-ran focused 05/06 GMP Rdot benchmarks at `N=10000000`, 512-bit
  precision, `repeat=10`, and `OMP_NUM_THREADS=32`.
- Added raw log, summary CSV, and serial/OpenMP plots under
  `benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513`.
- Updated `benchmarks/gmp/00_Rdot/README.md` with the 05/06 kernel shapes,
  inline plots, result table, and analysis.

Tests added:
- None. Benchmark source and artifact phase.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_05.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_06.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_05.cpp`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_06.cpp`
- `benchmarks/gmp/00_Rdot/go.sh`
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_05_06_repeat10.log`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/summary_rdot_n1e7_512_05_06_repeat10.csv`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_05_06_repeat10_Linux_Ryzen_3970X_32-Core_serial_Rdot.png`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_05_06_repeat10_Linux_Ryzen_3970X_32-Core_serial_summary.png`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_05_06_repeat10_Linux_Ryzen_3970X_32-Core_openmp_Rdot.png`
- `benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_05_06_repeat10_Linux_Ryzen_3970X_32-Core_openmp_summary.png`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/00_Rdot/CMakeLists.txt`
- `sed -n '1,220p' benchmarks/gmp/00_Rdot/go.sh`
- `ls benchmarks/gmp/00_Rdot`
- `git status --short`
- `rg -n "Rdot_gmp_kernel_04|Rdot_gmp_kernel_openmp_04|Rdot_gmp_C_native_openmp" -S .`
- `sed -n '1,220p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_03.cpp`
- `sed -n '1,220p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_03.cpp`
- `sed -n '110,175p' benchmarks/CMakeLists.txt`
- `sed -n '1,110p' benchmarks/CMakeLists.txt`
- `sed -n '35,75p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_04.cpp`
- `sed -n '35,85p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_openmp_04.cpp`
- `sed -n '70,120p' benchmarks/common/run_benchmarks.sh`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rdot_gmp_kernel_05_orig ... Rdot_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH`
- `OMP_NUM_THREADS=4 /bin/bash -lc 'set -e; for exe in Rdot_gmp_kernel_05_orig ... Rdot_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH; do ... 1000 128 ...; done'`
- `mkdir -p benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513`
- `OMP_NUM_THREADS=32 /bin/bash -lc 'set -u; out="benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_openmp_03_06_repeat10.log"; ...'`
- `pgrep -af 'Rdot_gmp_|benchmark_rdot_n1e7_512_openmp_03_06'`
- `kill 1317968 1318399 1318400`
- `OMP_NUM_THREADS=32 /bin/bash -lc 'set -u; out="benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_05_06_repeat10.log"; ... focused 05/06 repeat10 ...'`
- `python3 benchmarks/common/plot.py benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_05_06_repeat10.log --output-dir benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513 --backend GMP`
- `python3 - <<'PY' ... write summary_rdot_n1e7_512_05_06_repeat10.csv ... PY`
- `perl -0pi -e 's/COMMAND Rdot 05_/COMMAND Rdot kernel_05_/g; ...' benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/benchmark_rdot_n1e7_512_05_06_repeat10.log`
- `cat benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/summary_rdot_n1e7_512_05_06_repeat10.csv`
- `find benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513 -maxdepth 1 -type f -printf '%f %s bytes\n' | sort`
- `cmake --build build -j --target Rdot_gmp_kernel_05_orig ... Rdot_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Release build of all new 05/06 targets: PASS.
- Smoke run at `N=1000`, 128-bit precision, `OMP_NUM_THREADS=4`: PASS, all
  12 new variants reported `OK`.
- Focused 05/06 benchmark run: PASS, all 12 variants completed 10/10 runs and
  reported `OK`.
- Best focused serial max: `kernel_06_orig`, 33.786 MFLOPS.
- Best focused serial average: `kernel_06_orig`, 32.910 MFLOPS.
- Best focused OpenMP max: `kernel_openmp_05_mkII_FIXED_PRECISION_FASTPATH`,
  591.045 MFLOPS.
- Best focused OpenMP average: `kernel_openmp_06_mkII`, 529.473 MFLOPS.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- A partial aborted all-baseline 03-06 log remains untracked in the new result
  directory and is not part of the focused 05/06 analysis.
- Pre-existing untracked benchmark artifacts remain outside this phase.

Post-phase GMP Rdot C native read/add/mul microbench:
DONE

Implemented features:
- Added `Rdot_gmp_C_native_microbench.cpp` with compile-time modes for:
  `07_readonly`, `08_addonly`, `09_mulonly`, and `10_muladd`.
- Registered four C native microbench targets:
  `Rdot_gmp_C_native_07_readonly`, `Rdot_gmp_C_native_08_addonly`,
  `Rdot_gmp_C_native_09_mulonly`, and `Rdot_gmp_C_native_10_muladd`.
- Ran the microbench at `N=10000000`, 512-bit precision, `repeat=10`.
- Added raw log and summary CSV under
  `benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513`.
- Updated `benchmarks/gmp/00_Rdot/README.md` with the microbench result table
  and interpretation.

Tests added:
- None. Benchmark source and artifact phase.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_microbench.cpp`
- `benchmarks/gmp/00_Rdot/README.md`
- `benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513/benchmark_rdot_c_native_microbench_n1e7_512_repeat10.log`
- `benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513/summary_rdot_c_native_microbench_n1e7_512_repeat10.csv`
- `STATUS.md`

Exact commands run:
- `sed -n '1,190p' benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `sed -n '135,165p' benchmarks/CMakeLists.txt`
- `git status --short`
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rdot_gmp_C_native_07_readonly Rdot_gmp_C_native_08_addonly Rdot_gmp_C_native_09_mulonly Rdot_gmp_C_native_10_muladd`
- `/bin/bash -lc 'set -e; for exe in Rdot_gmp_C_native_07_readonly ... Rdot_gmp_C_native_10_muladd; do ... 1000 128 ...; done'`
- `mkdir -p benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513`
- `/bin/bash -lc 'set -u; out="benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513/benchmark_rdot_c_native_microbench_n1e7_512_repeat10.log"; ... repeat10 microbench ...'`
- `python3 - <<'PY' ... write summary_rdot_c_native_microbench_n1e7_512_repeat10.csv ... PY`
- `cat benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513/summary_rdot_c_native_microbench_n1e7_512_repeat10.csv`
- `find benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513 -maxdepth 1 -type f -printf '%f %s bytes\n' | sort`
- `tail -40 benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513/benchmark_rdot_c_native_microbench_n1e7_512_repeat10.log`
- `cmake --build build -j --target Rdot_gmp_C_native_07_readonly Rdot_gmp_C_native_08_addonly Rdot_gmp_C_native_09_mulonly Rdot_gmp_C_native_10_muladd`

Pass/fail result:
- Release build of all C native microbench targets: PASS.
- Debug build of all C native microbench targets: PASS.
- Smoke run at `N=1000`, 128-bit precision: PASS, all four variants reported
  `OK`.
- Focused microbench run: PASS, all four variants completed 10/10 runs and
  reported `OK`.
- Average elapsed times at `N=10000000`, 512-bit precision:
  `07_readonly` 0.079478 s, `08_addonly` 0.166463 s, `09_mulonly`
  0.447660 s, and `10_muladd` 0.625428 s.
- The results support arithmetic-limited behavior: `09_mulonly` accounts for
  about 72% of the full `10_muladd` time, and `08_addonly + 09_mulonly`
  nearly matches `10_muladd`.

Known issues:
- `07_readonly` reads only metadata and `_mp_d[0]`; it is a traversal
  lower-bound, not a full limb-stream bandwidth test.
- Pre-existing untracked benchmark artifacts remain outside this phase.

Post-phase GMP Rdot 1024-bit 01-06 benchmark:
DONE

Implemented features:
- Ran GMP Rdot 01-06 serial and OpenMP benchmarks at `N=10000000`,
  1024-bit precision, `repeat=10`, and `OMP_NUM_THREADS=32`.
- Used `MPFXX_DEFAULT_PREC_BITS=1024` for the benchmark processes so mkII's
  frozen-env default precision path actually constructs 1024-bit `mpf_class`
  objects.
- Added raw log and summary CSV under
  `benchmarks/gmp/results_raw/rdot_n1e7_1024_01_06_env1024_20260513`.

Tests added:
- None. Benchmark artifact phase.

Tests updated:
- `benchmarks/gmp/results_raw/rdot_n1e7_1024_01_06_env1024_20260513/benchmark_rdot_n1e7_1024_01_06_env1024_repeat10.log`
- `benchmarks/gmp/results_raw/rdot_n1e7_1024_01_06_env1024_20260513/summary_rdot_n1e7_1024_01_06_env1024_repeat10.csv`
- `STATUS.md`

Exact commands run:
- `cmake --build build_bench_release -j --target Rdot_gmp_C_native_01 Rdot_gmp_C_native_openmp_01 ... Rdot_gmp_kernel_openmp_06_mkII_FIXED_PRECISION_FASTPATH`
- `./build_bench_release/benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01 1000 1024`
- `/bin/bash -lc '... OMP_NUM_THREADS=32 "$exe" 10000000 1024 ...'`
- `sed -n '1,150p' benchmarks/gmp/00_Rdot/Rdot_gmp_kernel_01.cpp`
- `sed -n '1,140p' benchmarks/gmp/00_Rdot/Rdot_gmp_C_native_01.cpp`
- `sed -n '1,220p' benchmarks/gmp/00_Rdot/Rdot.hpp`
- `rg -n "set_default_mpf_precision_bits|default_mpf_precision_bits|MPFXX_DEFAULT" include -S`
- `sed -n '60,230p' include/gmpfrxx_mkII/detail/gmp_default_context.hpp`
- `kill 1323082 1323092 1323093 1323549`
- `/bin/bash -lc '... OMP_NUM_THREADS=32 MPFXX_DEFAULT_PREC_BITS=1024 "$exe" 10000000 1024 ...'`
- `/bin/bash -lc '... awk ... summary_rdot_n1e7_1024_01_06_env1024_repeat10.csv ...'`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- Release build of all Rdot 01-06 serial/OpenMP targets: PASS.
- Smoke run of `Rdot_gmp_C_native_01 1000 1024`: PASS.
- Full 1024-bit benchmark run: PASS, all 38 variants completed 10/10 runs and
  reported `OK`.
- Best serial max: `Rdot_gmp_kernel_03_mkII`, 12.254 MFLOPS.
- Best serial average: `Rdot_gmp_kernel_05_mkII`, 11.946 MFLOPS.
- Best OpenMP max: `Rdot_gmp_kernel_openmp_06_mkII`, 326.111 MFLOPS.
- Best OpenMP average: `Rdot_gmp_kernel_openmp_06_mkII`, 292.364 MFLOPS.
- `Rdot_gmp_C_native_openmp_01` reached 321.162 MFLOPS max and
  277.527 MFLOPS average.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- An initial run without `MPFXX_DEFAULT_PREC_BITS=1024` was stopped because
  mkII stayed on its frozen 512-bit default precision despite the benchmark's
  call to `gmpxx::set_default_mpf_precision_bits(1024)`. That partial log is
  not part of the reported 1024-bit results.
- OpenMP results show large run-to-run variance at this problem size.

Post-phase GMP Rdot 1024-bit README analysis:
DONE

Implemented features:
- Updated `benchmarks/gmp/00_Rdot/README.md` with the 1024-bit Rdot 01-06
  benchmark table.
- Added analysis of required memory bandwidth, 512-bit versus 1024-bit limb
  work, and OpenMP speedup relative to serial.
- Documented that 1024-bit mkII benchmark processes must set
  `MPFXX_DEFAULT_PREC_BITS=1024` in frozen-env mode.

Tests added:
- None. Documentation and benchmark-analysis phase.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `git status --short`
- `tail -n 180 benchmarks/gmp/00_Rdot/README.md`
- `cat benchmarks/gmp/results_raw/rdot_n1e7_1024_01_06_env1024_20260513/summary_rdot_n1e7_1024_01_06_env1024_repeat10.csv`
- `cat benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513/summary_rdot_n1e7_512_05_06_repeat10.csv`
- `find benchmarks/gmp/results_raw/rdot_n1e7_1024_01_06_env1024_20260513 -maxdepth 1 -type f -printf '%f %s bytes\n' | sort`
- `find benchmarks/gmp/results_raw/rdot_n1e7_openmp_03_06_20260513 -maxdepth 1 -type f -printf '%f %s bytes\n' | sort`
- `find benchmarks/gmp/results_raw/rdot_c_native_microbench_20260513 -maxdepth 1 -type f -printf '%f %s bytes\n' | sort`
- `git diff --stat`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- README updated with 1024-bit data and analysis.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- The bandwidth estimates are lower bounds for streaming input data.  Product
  temporaries and accumulators are treated as local cache traffic, not required
  DRAM bandwidth.

Post-phase GMP Rgemm partial step-7 sweep plot:
DONE

Implemented features:
- Generated a partial plot for the in-progress GMP Rgemm OpenMP 02/04/05/06
  step-7 plus powers-of-two sweep at 512-bit precision.
- Preserved the interrupted partial CSV and log for the run completed so far.

Tests added:
- None. Benchmark artifact and plotting phase.

Tests updated:
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512.csv`
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512.log`
- `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512_partial.png`
- `STATUS.md`

Exact commands run:
- `pgrep -af "rgemm_gmp_openmp_02_04_05_06_step7_powers|Rgemm_gmp_"`
- `git status --short`
- `wc -l benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512.csv benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_1024.csv 2>/dev/null || true`
- `awk -F, 'NR>1 {rows++; if($8=="OK") ok++; else bad++; if($2+0>maxn) maxn=$2+0; seen[$2]=1; if($7+0>best){best=$7+0; bestline=$0}} END{for (n in seen) sizes++; print "rows=" rows, "ok=" ok+0, "bad=" bad+0, "sizes=" sizes, "maxN=" maxn; print "best=" bestline}' benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512.csv`
- `python3 - <<'PY' ... PY`
- `git diff --stat`
- `ls -lh benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512.csv benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512.log benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512_partial.png`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- No benchmark process was left running.
- Partial CSV contains 1752 result rows, covering 146 sizes up to `N=974`.
- All recorded rows report `Result OK`.
- Current best row is `Rgemm_gmp_C_native_openmp_06` at `N=799`,
  886.892 MFLOPS.
- Generated
  `benchmarks/results_raw/rgemm_gmp_openmp_02_04_05_06_step7_powers_core32_512_partial.png`.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- This is an intentionally partial artifact. The full requested sweep was
  stopped before 1024-bit runs because the benchmark executables also run
  reference checks and some variants run an additional serial measurement after
  the timed loop, making the complete 1..2049 dense sweep impractically long in
  the current harness.

Post-phase GMP Rdot lessons learned documentation:
DONE

Implemented features:
- Added a `Lessons Learned` section to `benchmarks/gmp/00_Rdot/README.md`.
- Documented why the current GMP Rdot wrapper kernels are effectively
  exhausted under the existing `mpf_class`-compatible API.
- Recorded the practical stopping rule: once the hot loop is one `mpf_mul`
  plus one `mpf_add`, remaining differences are dominated by GMP arithmetic,
  streaming/reduction effects, and OpenMP variance rather than wrapper
  dispatch.

Tests added:
- None. Documentation-only phase.

Tests updated:
- `benchmarks/gmp/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/gmp/00_Rdot/README.md`
- `sed -n '260,620p' benchmarks/gmp/00_Rdot/README.md`
- `tail -n 80 STATUS.md`
- `git status --short`
- `pgrep -af "rg -n|ctest|cmake|Rdot|Rgemm"`
- `git diff -- benchmarks/gmp/00_Rdot/README.md STATUS.md`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `git diff --stat`

Pass/fail result:
- README updated with a GMP Rdot lessons-learned summary.
- `kernel_05` and `kernel_06` are documented as retained experimental
  negative-result benchmarks, while `kernel_03` remains the recommended
  optimized wrapper shape.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- No new known issues.  The section intentionally recommends moving further
  work to MPFR Rdot unless the GMP benchmark changes its kernel contract or
  data layout.

Post-phase MPFR Rdot GMP-shape alignment:
DONE

Implemented features:
- Aligned MPFR Rdot wrapper kernels `01` through `06` with the corresponding
  GMP Rdot source shapes.
- Removed raw `mpfr_mul` / `mpfr_fma` calls from MPFR wrapper kernels `05` and
  `06`; wrapper FMA behavior now comes from the existing expression-template
  FMA build option.
- Split MPFR C native FMA and non-FMA benchmarks into separate source files
  for both serial and OpenMP targets.
- Updated the MPFR Rdot README to describe the new source-shape policy and to
  mark the previous repeat-10 results as a historical baseline.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01_FMA.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_openmp_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_openmp_01_FMA.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_02.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_03.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_04.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_06.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_01.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_02.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_03.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_04.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_05.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_06.cpp`
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `rg -n "mpfr_mul|mpfr_fma|MPFRXX_ENABLE_FMA|with_precision|mpfr_set_zero|_Rdot" benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_*.cpp`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05.cpp`
- `sed -n '1,240p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_06.cpp`
- `sed -n '1,260p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_01.cpp`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_02.cpp`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_03.cpp`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_04.cpp`
- `sed -n '1,240p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_05.cpp`
- `sed -n '1,250p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_06.cpp`
- `rg -n "Rdot_mpfr_C_native|FMA|kernel_0[1-6]|add_executable|target_compile_definitions" benchmarks/mpfr/00_Rdot CMakeLists.txt benchmarks -g 'CMakeLists.txt' -g '*.cmake'`
- `rg --files benchmarks/mpfr/00_Rdot`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01.cpp`
- `sed -n '1,220p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01_FMA.cpp`
- `sed -n '1,240p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_openmp_01.cpp`
- `sed -n '1,240p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_openmp_01_FMA.cpp`
- `sed -n '1,180p' benchmarks/CMakeLists.txt`
- `rg -n "mpfr_mul|mpfr_fma|MPFRXX_ENABLE_FMA|MPFR_C_NATIVE_USE_FMA|#include \"Rdot_mpfr_C_native" benchmarks/mpfr/00_Rdot`
- `rg -n "with_precision|mpfr_set_zero|mpfr_get_default_rounding_mode" benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_*.cpp`
- `sed -n '1,130p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_05.cpp`
- `sed -n '1,140p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_05.cpp`
- `git status --short`
- `rg -n "MPFR_C_NATIVE_USE_FMA|#include \"Rdot_mpfr_C_native|mpfr_mul|mpfr_fma|MPFRXX_ENABLE_FMA" benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_*.cpp benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native*.cpp`
- `ls -d build build_bench_release 2>/dev/null`
- `cmake --build build_bench_release -j --target Rdot_mpfr_C_native_01 Rdot_mpfr_C_native_01_FMA Rdot_mpfr_C_native_openmp_01 Rdot_mpfr_C_native_openmp_01_FMA ... Rdot_mpfr_kernel_openmp_06_mkII_STABLE_ROUNDING_FMA`
- `build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01 1000 512`
- `build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_C_native_01_FMA 1000 512`
- `build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01_mkII_FMA 1000 512`
- `OMP_NUM_THREADS=4 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_06_mkII_STABLE_ROUNDING_FMA 1000 512`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`

Pass/fail result:
- MPFR Rdot native and wrapper benchmark target build: PASS.
- Source scan: wrapper kernels no longer contain raw `mpfr_mul`, `mpfr_fma`,
  `MPFRXX_ENABLE_FMA`, or explicit `with_precision` / `mpfr_set_zero`
  benchmark-loop setup.
- Native source split: PASS.  Only C native source files contain raw
  `mpfr_mul` / `mpfr_fma`; no `MPFR_C_NATIVE_USE_FMA` source include bridge
  remains.
- Smoke runs: `Rdot_mpfr_C_native_01`, `Rdot_mpfr_C_native_01_FMA`,
  `Rdot_mpfr_kernel_01_mkII_FMA`, and
  `Rdot_mpfr_kernel_openmp_06_mkII_STABLE_ROUNDING_FMA` all reported `DIFF OK`.
- Disassembly check: `Rdot_mpfr_kernel_01_mkII_FMA` hot `_Rdot` loop calls
  `mpfr_fma@plt`; `Rdot_mpfr_kernel_01_mkII` hot `_Rdot` loop calls
  `mpfr_mul@plt` followed by `mpfr_add@plt`.
- `git diff --check`: PASS.
- `ctest --test-dir build --output-on-failure`: PASS, 156/156 tests passed.

Known issues:
- The committed repeat-10 MPFR Rdot result table predates this source-shape
  alignment and should be regenerated before it is used for current ranking.

## Phase: MPFR Rdot Hotpath Disassembly Notes

Implemented features:
- Added MPFR Rdot hotpath disassembly notes to
  `benchmarks/mpfr/00_Rdot/README.md`.
- Documented the C native FMA baseline, the closest mkII stable-rounding FMA
  path, the allocation-heavy unfused `kernel_01_mkII` path, and the
  reusable-product `kernel_03_mkII_STABLE_ROUNDING` path.
- Documented that `kernel_06_mkII_STABLE_ROUNDING_FMA` is built with the FMA
  option but remains a four-way unrolled `mpfr_mul` plus `mpfr_add` hot loop
  because the source materializes product temporaries before accumulation.

Missing features:
- None for this documentation update.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,320p' benchmarks/mpfr/00_Rdot/README.md`
- `find benchmarks/mpfr/results_raw -maxdepth 2 -type f | sort | tail -40`
- `sed -n '320,760p' benchmarks/mpfr/00_Rdot/README.md`
- `sed -n '1,80p' benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514/summary_rdot_n1e7_512_aligned_repeat10_omp32.csv`
- `du -h benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514/*`
- `sed -n '100,280p' benchmarks/mpfr/00_Rdot/README.md`
- `git diff -- benchmarks/mpfr/00_Rdot/README.md`
- `sed -n '114,146p' benchmarks/mpfr/00_Rdot/README.md`
- `tail -80 STATUS.md`
- `git diff --stat`
- `git diff --check`
- `git add benchmarks/mpfr/00_Rdot/README.md STATUS.md benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514`
- `git diff --cached --stat`
- `git diff --cached --check`
- `perl -0pi -e 's/\r\n/\n/g; s/\n+\z/\n/' benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514/benchmark_rdot_n1e7_512_aligned_repeat10_omp32.log benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514/raw_rdot_n1e7_512_aligned_repeat10_omp32.csv benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514/summary_rdot_n1e7_512_aligned_repeat10_omp32.csv`
- `git add benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514/benchmark_rdot_n1e7_512_aligned_repeat10_omp32.log benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514/raw_rdot_n1e7_512_aligned_repeat10_omp32.csv benchmarks/mpfr/results_raw/rdot_n1e7_512_aligned_repeat10_omp32_20260514/summary_rdot_n1e7_512_aligned_repeat10_omp32.csv`
- `git diff --cached --check`
- `git diff --cached --stat`

Pass/fail result:
- README hotpath note review: PASS.
- `git diff --check`: PASS.
- `git diff --cached --check`: PASS.

Known issues:
- None for this documentation update.

## Phase: MPFR Rdot Kernel 07

Implemented features:
- Added `Rdot_mpfr_kernel_07.cpp` as a four-way unrolled,
  FMA-preserving wrapper Rdot shape using `acc0..acc3 += dx[i] * dy[i]`.
- Added `Rdot_mpfr_kernel_openmp_07.cpp` with the same source shape per
  OpenMP thread.
- Added serial and OpenMP kernel 07 variants to the benchmark CMake targets
  and the MPFR benchmark runner.
- Documented kernel 07 in the MPFR Rdot README as the unrolled counterpart to
  `kernel_01` that should expose four `mpfr_fma` calls in FMA builds.
- Ran the full MPFR Rdot 01-07 sweep at `N=100000000`, precision 512,
  repeat 10, `OMP_NUM_THREADS=32`.
- Added raw log, plots, raw CSV, and summary CSV for the 01-07 sweep under
  `benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/`.
- Updated the MPFR Rdot README with the 01-07 sweep results and analysis.

Missing features:
- None.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07.cpp`
- `benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07.cpp`
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `benchmarks/mpfr/00_Rdot/README.md`
- `benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/benchmark_rdot_n1e8_512_01_07_repeat10_omp32.log`
- `benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/raw_rdot_n1e8_512_01_07_repeat10_omp32.csv`
- `benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/summary_rdot_n1e8_512_01_07_repeat10_omp32.csv`
- `benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/benchmark_rdot_n1e8_512_01_07_repeat10_omp32_Linux_Ryzen_3970X_32-Core_serial_Rdot.png`
- `benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/benchmark_rdot_n1e8_512_01_07_repeat10_omp32_Linux_Ryzen_3970X_32-Core_serial_summary.png`
- `benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/benchmark_rdot_n1e8_512_01_07_repeat10_omp32_Linux_Ryzen_3970X_32-Core_openmp_Rdot.png`
- `benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/benchmark_rdot_n1e8_512_01_07_repeat10_omp32_Linux_Ryzen_3970X_32-Core_openmp_summary.png`
- `STATUS.md`

Exact commands run:
- `free -h`
- `git status --short`
- `rg -n "Rdot_mpfr_kernel(_openmp)?_0[1-6]|Rdot_mpfr_C_native|mpfr.*Rdot" benchmarks CMakeLists.txt -g 'CMakeLists.txt' -g '*.cmake' -g '*.sh'`
- `ls benchmarks/mpfr/00_Rdot`
- `sed -n '1,260p' benchmarks/common/run_mpfr_benchmarks.sh`
- `nl -ba benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_06.cpp | sed -n '1,180p'`
- `sed -n '1,120p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_01.cpp`
- `sed -n '1,120p' benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_01.cpp`
- `sed -n '1,120p' benchmarks/CMakeLists.txt`
- `sed -n '120,230p' benchmarks/CMakeLists.txt`
- `cmake --build build_bench_release -j --target Rdot_mpfr_kernel_07_mkII Rdot_mpfr_kernel_07_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_07_mkII_FMA Rdot_mpfr_kernel_07_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_openmp_07_mkII Rdot_mpfr_kernel_openmp_07_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_openmp_07_mkII_FMA Rdot_mpfr_kernel_openmp_07_mkII_STABLE_ROUNDING_FMA`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Rdot_mpfr_kernel_07_mkII Rdot_mpfr_kernel_07_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_07_mkII_FMA Rdot_mpfr_kernel_07_mkII_STABLE_ROUNDING_FMA Rdot_mpfr_kernel_openmp_07_mkII Rdot_mpfr_kernel_openmp_07_mkII_STABLE_ROUNDING Rdot_mpfr_kernel_openmp_07_mkII_FMA Rdot_mpfr_kernel_openmp_07_mkII_STABLE_ROUNDING_FMA`
- `build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_mkII_STABLE_ROUNDING_FMA 1000 512`
- `OMP_NUM_THREADS=4 build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_openmp_07_mkII_STABLE_ROUNDING_FMA 1000 512`
- `objdump -d --demangle build_bench_release/benchmarks/mpfr/00_Rdot/Rdot_mpfr_kernel_07_mkII_STABLE_ROUNDING_FMA | rg -n "mpfr_fma|mpfr_mul|mpfr_add" -C 2`
- `OMP_NUM_THREADS=32 /bin/bash -lc '<Rdot-only N=100000000 repeat=10 sweep for 60 MPFR Rdot variants>'`
- `python3 <parse benchmark_rdot_n1e8_512_01_07_repeat10_omp32.log into raw and summary CSV>`
- `find benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514 -maxdepth 1 -type f -printf '%f\n' | sort`
- `du -h benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/*`
- `tail -20 benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/benchmark_rdot_n1e8_512_01_07_repeat10_omp32.log`
- `sed -n '1,80p' benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/summary_rdot_n1e8_512_01_07_repeat10_omp32.csv`
- `wc -l benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/raw_rdot_n1e8_512_01_07_repeat10_omp32.csv benchmarks/mpfr/results_raw/rdot_n1e8_512_01_07_repeat10_omp32_20260514/summary_rdot_n1e8_512_01_07_repeat10_omp32.csv`
- `git diff --check`
- `cmake --build build_bench_release -j`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Initial target build before reconfigure: FAIL, build tree did not yet know
  the new kernel 07 targets.
- Reconfigure: PASS.
- Kernel 07 target build after reconfigure: PASS.
- Serial smoke run:
  `Rdot_mpfr_kernel_07_mkII_STABLE_ROUNDING_FMA 1000 512`: PASS, `DIFF OK`.
- OpenMP smoke run:
  `Rdot_mpfr_kernel_openmp_07_mkII_STABLE_ROUNDING_FMA 1000 512`: PASS,
  `DIFF OK`.
- Disassembly check: PASS.  `Rdot_mpfr_kernel_07_mkII_STABLE_ROUNDING_FMA`
  contains the expected unrolled `mpfr_fma@plt` calls in the hot path.
- Full Rdot 01-07 sweep: PASS.  600 records, 60 variants, all runs reported
  `DIFF OK`.
- Generated CSV line counts: PASS.  Raw CSV has 600 records plus header;
  summary CSV has 60 variants plus header.
- `git diff --check`: PASS before the final build/CTest pass.
- Full release build: PASS.
- CTest: PASS.  156/156 tests passed.
- Best serial average in the sweep:
  `kernel_06_mkII_STABLE_ROUNDING_FMA`, 23.616 MFLOPS, but this is still an
  unrolled `mpfr_mul` plus `mpfr_add` source shape rather than true FMA.
- Best serial true FMA wrapper average:
  `kernel_07_mkII_STABLE_ROUNDING_FMA`, 23.223 MFLOPS.
- Best OpenMP wrapper average:
  `kernel_openmp_03_mkII_STABLE_ROUNDING`, 634.274 MFLOPS.
- Best OpenMP true FMA wrapper average:
  `kernel_openmp_07_mkII_FMA`, 608.243 MFLOPS.

Known issues:
- OpenMP timed-loop MFLOPS still has visible run-to-run variance even at
  `N=100000000`.

## Phase: Benchmark OpenMP Affinity Defaults

Implemented features:
- Added OpenMP affinity defaults to the common GMP and MPFR benchmark runners:
  `OMP_NUM_THREADS=32`, `OMP_PLACES=cores`, and `OMP_PROC_BIND=spread`.
- The runners preserve caller-provided OpenMP affinity variables, so close/spread
  and thread-count comparisons can still be run from the command line.
- Added `BENCH_COMMAND_PREFIX` support to run benchmarks under wrappers such as
  `numactl --interleave=all`.
- Logged OpenMP affinity and command-prefix settings in benchmark logs.
- Documented the affinity defaults and override examples in
  `benchmarks/README.md`.

Missing features:
- Per-benchmark first-touch initialization is not changed in this phase.

Tests added:
- None.

Tests updated:
- `benchmarks/common/run_benchmarks.sh`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `benchmarks/README.md`
- `STATUS.md`

Exact commands run:
- `rg --files | rg '(^|/)go\.sh$|run_.*benchmarks\.sh$|Rdot.*\.sh$'`
- `rg -n "OMP_NUM_THREADS|OMP_PLACES|OMP_PROC_BIND|numactl|go\.sh|run_mpfr_benchmarks" benchmarks scripts . -g '*.sh' -g 'README.md'`
- `sed -n '1,260p' benchmarks/common/run_mpfr_benchmarks.sh`
- `sed -n '1,220p' benchmarks/common/run_benchmarks.sh`
- `sed -n '1,160p' benchmarks/gmp/00_Rdot/go.sh`
- `sed -n '220,420p' benchmarks/common/run_benchmarks.sh`
- `sed -n '1,130p' benchmarks/README.md`
- `tail -80 STATUS.md`
- `bash -n benchmarks/common/run_benchmarks.sh benchmarks/common/run_mpfr_benchmarks.sh`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`
- `OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close benchmarks/common/run_mpfr_benchmarks.sh build_bench_release 128 8 /tmp/gmpfrxx_mkII_mpfr_affinity_smoke 1 8 2 2 2 2 2`

Pass/fail result:
- Shell syntax check: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.
- Tiny MPFR runner smoke: PASS.  The log records
  `OPENMP_AFFINITY OMP_NUM_THREADS=2 OMP_PLACES=cores OMP_PROC_BIND=close`,
  confirming caller-provided affinity overrides are preserved.

Known issues:
- OpenMP affinity reduces run-to-run migration noise, but it does not by itself
  fix NUMA first-touch placement if benchmark data initialization remains
  serial.

## Phase: STREAM-like OpenMP Bandwidth Helper

Implemented features:
- Added `benchmarks/common/stream_like_omp.cpp`, a small OpenMP Copy, Scale,
  Add, and Triad bandwidth helper for host memory bandwidth checks.
- Added `benchmarks/common/run_stream_like.sh`, which builds the helper with
  `-O3 -march=native -fopenmp`, records CPU topology, and runs spread/close
  affinity comparisons.
- Documented the STREAM-like helper in `benchmarks/README.md`.

Missing features:
- This is a STREAM-like diagnostic helper, not an official STREAM submission.

Tests added:
- None.

Tests updated:
- `benchmarks/common/stream_like_omp.cpp`
- `benchmarks/common/run_stream_like.sh`
- `benchmarks/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' /tmp/gmpfrxx_stream_omp.cpp`
- `chmod +x benchmarks/common/run_stream_like.sh`
- `tail -80 STATUS.md`
- `bash -n benchmarks/common/run_stream_like.sh benchmarks/common/run_benchmarks.sh benchmarks/common/run_mpfr_benchmarks.sh`
- `g++ -O3 -march=native -fopenmp benchmarks/common/stream_like_omp.cpp -o /tmp/gmpfrxx_stream_like_omp_check`
- `git diff --check`
- `RUN_STREAM_SMT=0 benchmarks/common/run_stream_like.sh 1024 1 1 /tmp/gmpfrxx_stream_like_smoke /tmp/gmpfrxx_stream_like_omp_smoke`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Shell syntax check: PASS.
- Standalone helper compile: PASS.
- `git diff --check`: PASS.
- Tiny runner smoke: PASS.  The helper ran spread and close affinity cases and
  wrote `/tmp/gmpfrxx_stream_like_smoke/stream_like_20260515_133616.log`.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The helper reports decimal `GB/s`; compare carefully against GiB/s numbers
  from other tools.

## Phase: Top README Header Roles Refresh

Implemented features:
- Updated the top-level README header-role table to match the current split:
  `gmpxx_mkII.h` is GMP-only, `mpfrxx_mkII.h` is GMP + MPFR, `mpcxx_mkII.h`
  adds MPC, and `gmpfrxx_mkII.h` is the combined aggregator.
- Updated public type and example sections so `mpfrxx::mpc_class` is documented
  as coming from `mpcxx_mkII.h` or `gmpfrxx_mkII.h`, not from
  `mpfrxx_mkII.h`.
- Updated CMake target documentation to include `mpcxx_mkII` and to mark
  `mpfrxx_mkII` as GMP + MPFR only.
- Updated benchmark wording to mention both GMP and MPFR benchmark families.

Missing features:
- None.

Tests added:
- None.

Tests updated:
- `README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' README.md`
- `rg -n "Header Roles|mpfrxx_mkII|gmpfrxx_mkII|mpc|MPC|Dependencies|Public API" README.md CMakeLists.txt include -g 'README.md' -g 'CMakeLists.txt' -g '*.h' -g '*.hpp'`
- `sed -n '260,380p' README.md`
- `rg -n "mpfrxx_mkII.*MPC|mpfrxx_mkII\\s+-> GMP \\+ MPFR \\+ MPC|mpfrxx::mpc_class.*mpfrxx_mkII|MPFR/MPC|mpcxx_mkII|Header Roles" README.md`
- `sed -n '1,340p' README.md`
- `tail -60 STATUS.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- README consistency search: PASS. Remaining `mpfrxx::mpc_class` references
  point to `mpcxx_mkII.h` or `gmpfrxx_mkII.h`, not `mpfrxx_mkII.h`.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- None.

## Phase: MPFR Raxpy Kernel Shape Parity

Implemented features:
- Added MPFR Raxpy `kernel_03`, `kernel_04`, and `kernel_openmp_03` so the
  MPFR benchmark has the same source-shape ladder as GMP Raxpy for the
  currently useful stages.
- Normalized MPFR OpenMP Raxpy scheduling to `schedule(static)` to match the
  GMP Raxpy benchmark intent.
- Registered the new MPFR Raxpy kernels in CMake and in the common MPFR
  benchmark runner, including stable-rounding and FMA build variants.
- Added `benchmarks/mpfr/01_Raxpy/README.md` documenting the MPFR kernel
  shapes and how they map to GMP Raxpy.

Missing features:
- No MPFR Raxpy `kernel_05`/`kernel_06` unroll variants yet.  They can be added
  later if measurements show value.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_03.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_04.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_03.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_01.cpp`
- `benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_02.cpp`
- `benchmarks/mpfr/01_Raxpy/README.md`
- `benchmarks/CMakeLists.txt`
- `benchmarks/common/run_mpfr_benchmarks.sh`
- `STATUS.md`

Exact commands run:
- `git status --short`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j --target Raxpy_mpfr_kernel_03_mkII Raxpy_mpfr_kernel_03_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_03_mkII_FMA Raxpy_mpfr_kernel_03_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_04_mkII Raxpy_mpfr_kernel_04_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_04_mkII_FMA Raxpy_mpfr_kernel_04_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_openmp_03_mkII Raxpy_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_openmp_03_mkII_FMA Raxpy_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_openmp_01_mkII Raxpy_mpfr_kernel_openmp_02_mkII`
- `/bin/bash -lc 'set -euo pipefail; for exe in Raxpy_mpfr_kernel_03_mkII Raxpy_mpfr_kernel_04_mkII Raxpy_mpfr_kernel_openmp_03_mkII Raxpy_mpfr_kernel_03_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_03_mkII_FMA Raxpy_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING_FMA; do echo "=== $exe ==="; OMP_NUM_THREADS=4 build_bench_release/benchmarks/mpfr/01_Raxpy/$exe 1000 256 | tail -8; done'`
- `bash -n benchmarks/common/run_mpfr_benchmarks.sh`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Release reconfigure: PASS.
- New MPFR Raxpy target build: PASS.
- New-kernel smoke run: PASS.  All sampled executables reported `Result OK`.
- Runner shell syntax: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- The FMA suffix only proves the build option is enabled.  Disassembly is still
  required to confirm whether a specific source shape lowers to `mpfr_fma`.

## Phase: MPFR Raxpy N=10000000 Repeat-10 Benchmark

Implemented features:
- Ran the MPFR Raxpy benchmark matrix at `N=10000000`, 512-bit precision, and
  repeat count 10.
- Covered C native, C native FMA, C native OpenMP, C native OpenMP FMA, wrapper
  serial kernels 01-04, and wrapper OpenMP kernels 01-03.
- Parsed the benchmark log into raw and summary CSV files.

Missing features:
- No plots were generated for this focused Raxpy-only run.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_20260515_153432/benchmark_raxpy_n10000000_p512_repeat10.log`
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_20260515_153432/raw_raxpy_n10000000_p512_repeat10.csv`
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_20260515_153432/summary_raxpy_n10000000_p512_repeat10.csv`
- `STATUS.md`

Exact commands run:
- `cmake --build build_bench_release -j --target Raxpy_mpfr_C_native_01 Raxpy_mpfr_C_native_01_FMA Raxpy_mpfr_C_native_openmp_01 Raxpy_mpfr_C_native_openmp_01_FMA Raxpy_mpfr_kernel_01_mkII Raxpy_mpfr_kernel_01_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_01_mkII_FMA Raxpy_mpfr_kernel_01_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_02_mkII Raxpy_mpfr_kernel_02_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_02_mkII_FMA Raxpy_mpfr_kernel_02_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_03_mkII Raxpy_mpfr_kernel_03_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_03_mkII_FMA Raxpy_mpfr_kernel_03_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_04_mkII Raxpy_mpfr_kernel_04_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_04_mkII_FMA Raxpy_mpfr_kernel_04_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_openmp_01_mkII Raxpy_mpfr_kernel_openmp_01_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_openmp_01_mkII_FMA Raxpy_mpfr_kernel_openmp_01_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_openmp_02_mkII Raxpy_mpfr_kernel_openmp_02_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_openmp_02_mkII_FMA Raxpy_mpfr_kernel_openmp_02_mkII_STABLE_ROUNDING_FMA Raxpy_mpfr_kernel_openmp_03_mkII Raxpy_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING Raxpy_mpfr_kernel_openmp_03_mkII_FMA Raxpy_mpfr_kernel_openmp_03_mkII_STABLE_ROUNDING_FMA`
- `/bin/bash -lc 'set -euo pipefail; outdir="benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_$(date +%Y%m%d_%H%M%S)"; mkdir -p "$outdir"; log="$outdir/benchmark_raxpy_n10000000_p512_repeat10.log"; exe_dir="build_bench_release/benchmarks/mpfr/01_Raxpy"; ...; for exe in "${executables[@]}"; do for run in $(seq 1 10); do /usr/bin/time -f "WALL_SECONDS %e" "${exe_dir}/${exe}" 10000000 512; done; done'`
- `python3 -c 'import csv, re, statistics, pathlib; ...'`
- `python3 -c 'import csv, pathlib; ...'`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Target build: PASS.
- Benchmark run: PASS.  320/320 timed runs reported `Result OK`.
- Raw CSV and summary CSV generation: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- OpenMP measurements contain normal run-to-run variation; use max and average
  separately when comparing variants.

## Phase: MPFR Raxpy Result Documentation

Implemented features:
- Added the repeat-10 MPFR Raxpy result summary to
  `benchmarks/mpfr/01_Raxpy/README.md`.
- Added an inline summary plot for the `N=10000000`, 512-bit precision run.
- Added hotpath disassembly notes comparing C native, C native FMA, wrapper
  stable FMA, wrapper reusable-product, and OpenMP outlined loops.
- Added MPFR Raxpy lessons learned, including the source-shape-dependent FMA
  behavior and the remaining TLS rounding-load difference from C native.

Missing features:
- None.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/01_Raxpy/README.md`
- `benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_20260515_153432/benchmark_raxpy_n10000000_p512_repeat10_summary.png`
- `STATUS.md`

Exact commands run:
- `sed -n '1,260p' benchmarks/mpfr/01_Raxpy/README.md`
- `sed -n '1,80p' benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_20260515_153432/summary_raxpy_n10000000_p512_repeat10.csv`
- `tail -n 120 STATUS.md`
- `git status --short`
- `python3 - <<'PY' ...`
- `objdump -Cd --start-address=0x3cb0 --stop-address=0x3d50 build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01`
- `objdump -Cd --start-address=0x3ca8 --stop-address=0x3d08 build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_C_native_01_FMA`
- `objdump -Cd --start-address=0x3c70 --stop-address=0x3cb8 build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_01_mkII_STABLE_ROUNDING_FMA`
- `objdump -Cd --start-address=0x3f60 --stop-address=0x4020 build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_03_mkII_STABLE_ROUNDING`
- `objdump -Cd --start-address=0x3800 --stop-address=0x3898 build_bench_release/benchmarks/mpfr/01_Raxpy/Raxpy_mpfr_kernel_openmp_01_mkII_STABLE_ROUNDING_FMA`
- `ls -lh benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_20260515_153432`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Plot generation: PASS.
- Hotpath disassembly extraction: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- None.

## Phase: MPFR Raxpy Memory Bandwidth Documentation

Implemented features:
- Added estimated memory bandwidth used to
  `benchmarks/mpfr/01_Raxpy/README.md`.
- Documented both a 512-bit payload-only traffic model and a header-inclusive
  logical traffic model for the repeat-10 MPFR Raxpy benchmark.
- Recorded the key conversion from MFLOPS to bandwidth:
  `payload GB/s = avg_mflops * 0.096` and
  `header-inclusive GB/s = avg_mflops * 0.144`.

Missing features:
- No hardware-counter validation was added.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/01_Raxpy/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,340p' benchmarks/mpfr/01_Raxpy/README.md`
- `sed -n '1,80p' benchmarks/mpfr/01_Raxpy/results_raw/raxpy_n1e7_512_repeat10_20260515_153432/summary_raxpy_n10000000_p512_repeat10.csv`
- `tail -n 80 STATUS.md`
- `rg -n "MFLOPS|mflops|Elapsed|2\\.0|N\\*2|2 \\*" benchmarks/mpfr/01_Raxpy benchmarks/common -g '*.cpp' -g '*.hpp' -g '*.sh'`
- `printf '%s\n' ... | g++ -x c++ -std=c++17 -Iinclude - -lmpfr -lgmp -o /tmp/gmpfrxx_mkII_mpfr_size && /tmp/gmpfrxx_mkII_mpfr_size`
- `python3 - <<'PY' ...`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- MFLOPS definition check: PASS.  Raxpy uses `2.0 * N / elapsed`.
- MPFR layout check: PASS.  `sizeof(mpfr_t)=32`,
  `sizeof(__mpfr_struct)=32`, `sizeof(mp_limb_t)=8`.
- `git diff --check`: PASS.
- CTest: PASS.  156/156 tests passed.

Known issues:
- None.

## Phase: MPFR Rdot Full Repeat-10 Regeneration

Implemented features:
- Removed old checked-in MPFR Rdot benchmark result directories.
- Rebuilt the release benchmark tree.
- Re-ran the full MPFR Rdot benchmark suite with 68 variants and 10 repeats.
- Generated raw CSV, summary CSV, and serial/OpenMP bar plots with min/max
  range markers.
- Rewrote `benchmarks/mpfr/00_Rdot/README.md` from scratch so it only
  describes the regenerated result set.
- Added fresh analysis covering kernel shape, FMA lowering, explicit-context
  rounding, payload bandwidth estimates, hotpath disassembly, comparison with
  GMP Rdot, and lessons learned.

Missing features:
- No hardware-counter run was added.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/benchmark_rdot_mpfr_n10000000_p512_repeat10.log`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/raw_rdot_mpfr_n10000000_p512_repeat10.csv`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/summary_rdot_mpfr_n10000000_p512_repeat10.csv`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/rdot_mpfr_n10000000_p512_repeat10_serial.png`
- `benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/rdot_mpfr_n10000000_p512_repeat10_openmp.png`
- `STATUS.md`

Exact commands run:
- `git rm -r benchmarks/mpfr/00_Rdot/results_raw`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j`
- `/bin/bash -lc 'set -euo pipefail ... run all Rdot_mpfr executables from benchmarks/common/run_mpfr_benchmarks.sh with N=10000000 precision=512 repeat=10 ...'`
- `python3 benchmarks/mpfr/00_Rdot/plot_repeat_summary.py benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/benchmark_rdot_mpfr_n10000000_p512_repeat10.log --output-dir benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244 --output-prefix rdot_mpfr_n10000000_p512_repeat10 --title-prefix "MPFR Rdot N=10000000 precision=512 repeat=10"`
- `find benchmarks/mpfr/00_Rdot/results_raw -mindepth 1 -maxdepth 1 -type d -print`
- `ls -lh benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244`
- `rg -c "DIFF: .* OK" benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/benchmark_rdot_mpfr_n10000000_p512_repeat10.log`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`
- `git status --short`

Pass/fail result:
- Release configure: PASS.
- Release build: PASS.
- Full MPFR Rdot repeat-10 benchmark: PASS.  680/680 timed runs reported
  `OK`.
- Plot generation: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- The payload bandwidth estimate in the README is a lower bound and does not
  include MPFR headers, pointer chasing, allocator locality, or reduction
  overhead.

## Phase: MPFR Rdot README Folded Result Tables

Implemented features:
- Updated `benchmarks/mpfr/00_Rdot/README.md` so each execution mode has:
  a main interpretation table, a folded table sorted by Max MFLOPS, and a
  folded table sorted by Avg MFLOPS.
- Added separate Serial and OpenMP max-sorted tables generated from the
  checked-in repeat-10 summary CSV.
- Kept the existing average-sorted rankings, but moved them into folded
  sections for consistency with the requested README structure.

Missing features:
- None.

Tests added:
- None.

Tests updated:
- `benchmarks/mpfr/00_Rdot/README.md`
- `STATUS.md`

Exact commands run:
- `sed -n '1,360p' benchmarks/mpfr/00_Rdot/README.md`
- `sed -n '1,90p' benchmarks/mpfr/00_Rdot/results_raw/rdot_mpfr_n10000000_p512_repeat10_20260517_121244/summary_rdot_mpfr_n10000000_p512_repeat10.csv`
- `python3 - <<'PY' ... generate sorted Markdown tables from summary CSV ...`
- `rg -n "<summary>Serial results sorted by Max MFLOPS|<summary>Serial results sorted by Avg MFLOPS|<summary>OpenMP results sorted by Max MFLOPS|<summary>OpenMP results sorted by Avg MFLOPS" benchmarks/mpfr/00_Rdot/README.md`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Folded table presence check: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- None.

## Phase: MPFR Rgemv Full Repeat-10 Regeneration

Implemented features:
- Removed the old checked-in MPFR Rgemv result directory:
  `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858`.
- Rebuilt the release benchmark tree after adding the OpenMP 05/06/07 FMA
  native and mkII targets.
- Re-ran the full MPFR Rgemv benchmark suite with 43 variants and 10 repeats.
- Generated raw CSV, summary CSV, and serial/OpenMP bar plots with min/max
  range markers.
- Rewrote `benchmarks/mpfr/02_Rgemv/README.md` from scratch using only the
  regenerated result set.
- Added fresh analysis covering variant shapes, C native equivalence, payload
  bandwidth estimates, hotpath disassembly, FMA/FMMA behavior, explicit
  context, OpenMP row-blocking, and column-partition partial-y reduction.

Missing features:
- No hardware-counter run was added.

Tests added:
- None.

Tests updated:
- `benchmarks/CMakeLists.txt`
- `benchmarks/mpfr/02_Rgemv/run_repeat.sh`
- `benchmarks/mpfr/02_Rgemv/README.md`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_05_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_06_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_07_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_05_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_06_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054/benchmark_rgemv_mpfr_m4000_n4000_p512_repeat10.log`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054/raw_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054/rgemv_mpfr_m4000_n4000_p512_repeat10_serial.png`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054/rgemv_mpfr_m4000_n4000_p512_repeat10_openmp.png`
- `STATUS.md`

Exact commands run:
- `git rm -r benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_074858`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build_bench_release -j`
- `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/mpfr/02_Rgemv/run_repeat.sh build_bench_release 4000 4000 512 10`
- `find benchmarks/mpfr/02_Rgemv/results_raw -maxdepth 2 -type f | sort`
- `wc -l benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054/raw_rgemv_mpfr_m4000_n4000_p512_repeat10.csv benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `rg -c "Result OK" benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054/benchmark_rgemv_mpfr_m4000_n4000_p512_repeat10.log`
- `python3 - <<'PY' ... regenerate benchmarks/mpfr/02_Rgemv/README.md from summary CSV ...`
- `objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/02_Rgemv/<representative executable>`
- `git diff --check`
- `ctest --test-dir build_bench_release --output-on-failure`

Pass/fail result:
- Release configure: PASS.
- Release build: PASS.
- Full MPFR Rgemv repeat-10 benchmark: PASS.  430/430 timed runs reported
  `Result OK`.
- Plot generation: PASS.
- README regeneration: PASS.
- `git diff --check`: PASS.
- CTest: PASS.  158/158 tests passed.

Known issues:
- The bandwidth numbers in the README are model estimates derived from
  MFLOPS.  They are not hardware-counter measurements.

## Phase: MPFR Rgemv Fixed-Context Report Refresh

Implemented features:
- Made `mpfrxx::with_context` skip the destination precision check when
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` is enabled.
- Kept the checked precision-mismatch behavior for normal builds.
- Added a fixed-precision build of `test_mpfr_evaluation_context` so the
  context path is covered with the fastpath macro enabled.
- Added fixed-precision OpenMP MPFR Rgemv variants for kernel 05, 06, and 07,
  including FMA combinations.
- Reworked `Rgemv_mpfr_kernel_openmp_07_FMA.cpp` so the explicit-context path
  keeps wrapper syntax while passing a loop-local `mpfr_rnd_t` to
  `with_context(value, precision, rnd)`.
- Regenerated the MPFR Rgemv README from the new 512-bit repeat-10 benchmark
  run and included static result tables, plots, bandwidth estimates, C native
  equivalence notes, and hotpath disassembly.

Missing features:
- The requested 1024-bit `m=4000 n=4000 repeat=10` MPFR Rgemv sweep was
  started, but it was interrupted before summary CSV and plots were produced.
  No incomplete 1024-bit data is committed.

Tests added:
- `test_mpfr_evaluation_context_fixed_precision`

Tests updated:
- `include/gmpfrxx_mkII/detail/mpfr_impl.hpp`
- `tests/CMakeLists.txt`
- `tests/test_mpfr_evaluation_context.cpp`
- `benchmarks/CMakeLists.txt`
- `benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07_FMA.cpp`
- `benchmarks/mpfr/02_Rgemv/run_repeat.sh`
- `benchmarks/mpfr/02_Rgemv/README.md`
- `benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_121840/`
- `STATUS.md`

Exact commands run:
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- `cmake -S . -B build_bench_release -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build --target test_mpfr_evaluation_context test_mpfr_evaluation_context_fixed_precision -j`
- `cmake --build build_bench_release --target Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_02_mkII -j`
- `ctest --test-dir build -R 'test_mpfr_evaluation_context' --output-on-failure`
- `objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_02_mkII`
- `objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH`
- `cmake --build build -j`
- `cmake --build build_bench_release --target Rgemv_mpfr_kernel_openmp_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_openmp_02_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_01_mkII_FIXED_PRECISION_FASTPATH Rgemv_mpfr_kernel_02_mkII_FIXED_PRECISION_FASTPATH -j`
- `git diff --check`
- `ctest --test-dir build --output-on-failure`
- `cmake --build build_bench_release -j`
- `rm -rf benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_090054`
- `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/mpfr/02_Rgemv/run_repeat.sh build_bench_release 4000 4000 512 10`
- `OMP_NUM_THREADS=32 OMP_PLACES=cores OMP_PROC_BIND=spread benchmarks/mpfr/02_Rgemv/run_repeat.sh build_bench_release 4000 4000 1024 10`
- `rg -c "Result OK" benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_121840/benchmark_rgemv_mpfr_m4000_n4000_p512_repeat10.log`
- `wc -l benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_121840/raw_rgemv_mpfr_m4000_n4000_p512_repeat10.csv benchmarks/mpfr/02_Rgemv/results_raw/rgemv_mpfr_m4000_n4000_p512_repeat10_20260518_121840/summary_rgemv_mpfr_m4000_n4000_p512_repeat10.csv`
- `objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_C_native_openmp_07_FMA`
- `objdump -Cd --no-show-raw-insn build_bench_release/benchmarks/mpfr/02_Rgemv/Rgemv_mpfr_kernel_openmp_07_mkII_FIXED_PRECISION_FASTPATH_FMA`

Pass/fail result:
- Debug configure: PASS.
- Release benchmark configure: PASS.
- Focused test build: PASS.
- Focused CTest: PASS.  2/2 tests passed.
- Representative fixed-precision benchmark target build: PASS.
- Full debug build: PASS.
- `git diff --check`: PASS.
- Full CTest: PASS.  159/159 tests passed.
- Release benchmark build: PASS.
- MPFR Rgemv 512-bit repeat-10 benchmark: PASS.  490/490 runs reported
  `Result OK`.
- MPFR Rgemv 512-bit summary CSV: PASS.  49 variants were summarized.
- MPFR Rgemv 1024-bit repeat-10 benchmark: INTERRUPTED before summary output;
  no incomplete run directory is committed.
- Hotpath disassembly check: PASS.  The raw C OpenMP 07 FMA and mkII
  OpenMP 07 fixed-precision FMA loops both pass rounding from a register into
  `mpfr_fma`.

Known issues:
- Removing the context precision check under
  `GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH` is a caller contract.  Code
  compiled with this macro must only call `with_context(value, context)` when
  `value.precision() == context.precision`.
