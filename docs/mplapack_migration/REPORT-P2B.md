# P2B Complex Comparison Embeddings

## Repository and base

- Repository: `git@github.com:nakatamaho/gmpfrxx_mkII.git`
- Branch: `topic/mplapack-compare-embed-complex`
- Accepted MPLAPACK P1: `2ebf798fd0081ecdc5c1b53fc117431c406bf884`
- Approved P2A and P2B base: `0bd061c0b62d87256a1d0c18a9aea453524c79c5`
- P2A remote after push: `0bd061c0b62d87256a1d0c18a9aea453524c79c5`
- P2B implementation commit: `60221e1efa38bfe435e8ea378504cd9e41e5cc18`

## Immutable requirements

| File | SHA-256 |
|---|---|
| `LOCK.json` | `9fb757df92a36e996e0ea28c9dcf9351cf5063ff95646d750a188e3f7f32b075` |
| `interop_requirements.tsv` | `2bb9700d76cd50a6c4328f497b35e3794065fecc915eac40408f01c0188db00f` |
| `SPIKE.md` | `29b4bf3c831c0a892773139be3e8283153bfb4db4d74726b0893a807cae0928d` |
| `INTEROP_POLICY.md` | `859fe8ebc18100c1225d04633ceaa38140dd5ac48f6203538950bfc40886a9ed` |

The first three files are byte-identical to the accepted MPLAPACK P1 tree.

## Required rows and implementation

The selected API is explicit construction or assignment to an owning
`mpfrxx::mpc_class` through one opt-in `external_mpc_complex_traits` hook.

| Matrix ID | Source | Component route | Status |
|---|---|---|---|
| `COMPLEX-DOUBLE` | `std::complex<double>` | existing `mpc_set_d_d` route | pre-existing, verified |
| `COMPLEX-DD` | `dd_complex` | accepted P2A dd component sum for real and imaginary parts | implemented |
| `COMPLEX-QD` | `qd_complex` | accepted P2A qd component sum for real and imaginary parts | implemented |
| `COMPLEX-BINARY80` | `std::complex<mplapack_binary80_t>` | `_Float64x` to native `long double`, then accepted `mpfr_set_ld` route | implemented |
| `COMPLEX-BINARY128` | `std::complex<mplapack_binary128_t>` | `_Float128` to MPFR float128 type, then `mpfr_set_float128` | implemented |
| `COMPLEX-GMP` | `gmpxx::mpfc_class` | direct `mpfr_set_f` from each MPF component | implemented |

The dd and qd adapters use constrained traits and do not include MPLAPACK
production headers. No independent real conversion was duplicated.

## Tests

`tests/test_complex_compare_adapters.cpp` maps all six matrix IDs. Every case
uses nonzero and distinct components. Extended sources test information below
binary64 in both component positions. The real/imaginary values differ, so a
component-copy or swap defect fails. Imaginary components are negative, which
also supplies the directed sign check.

The existing fresh-process default tests remain the single 512-bit smoke.
No precision metadata is inspected by P2B adapters or tests.

Commands:

```text
cmake -S . -B build-p2b -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-I/home/docker/MPLAPACK/include -DGMPFRXX_MKII_DEPS_AUTO_FETCH=OFF -DGMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC
cmake --build build-p2b -j32
ctest --test-dir build-p2b --output-on-failure
```

Result: `190/190` tests passed, including all accepted P2A tests and existing
compile-fail tests.

Installed-tree validation installs to a fresh `/tmp` prefix, configures the
committed consumer only through `find_package(gmpfrxx_mkII CONFIG REQUIRED)`,
links `gmpfrxx_mkII::gmpfrxx_mkII`, and runs an MPFC-to-MPC sentinel. It passed.

## Gate

```text
bash docs/mplapack_migration/gate-P2B.sh
```

Complete output is committed as `REPORT-P2B-gate.log`.

No reverse conversion, mixed arithmetic, precision negotiation or metadata
handling, EDD, TD, or P2C utility work was added. Numerical/ABI deviations:
none. Remaining blockers: none. The P2B candidate is intentionally unpushed.
