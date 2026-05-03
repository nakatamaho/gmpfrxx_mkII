# Header Boundary and API Audit

This document records the public header and target dependency boundaries for `gmpfrxx_mkII`.

## Public Headers

| Header | Public types | Required target | Link dependencies |
|---|---|---|---|
| `gmpxx_mkII.h` | `mpz_class`, `mpq_class`, `mpf_class`, `mpfc_class` | `gmpxx_mkII` | GMP only |
| `mpfrxx_mkII.h` | `mpz_class`, `mpq_class`, `mpfr_class`, `mpc_class` | `mpfrxx_mkII` | GMP, MPFR, MPC |
| `gmpfrxx_mkII.h` | all public wrapper types | `gmpfrxx_mkII` | GMP, MPFR, MPC |

All public wrapper classes live in:

```cpp
namespace gmpfrxx
```

The project does not provide default global aliases.

## GMP-only Boundary

`gmpxx_mkII.h` is the GMP-only public header.

It must:

- include `<gmp.h>`
- expose `gmpfrxx::mpz_class`
- expose `gmpfrxx::mpq_class`
- expose `gmpfrxx::mpf_class`
- expose `gmpfrxx::mpfc_class`
- link through the `gmpxx_mkII` CMake interface target
- link GMP only

It must not:

- include `<mpfr.h>`
- include `<mpc.h>`
- include `mpfr_impl.hpp`
- include `mpc_impl.hpp`
- reference `mpfr_*` symbols
- reference `mpc_*` symbols
- expose `gmpfrxx::mpfr_class`
- expose `gmpfrxx::mpc_class`
- link MPFR
- link MPC

`mpfc_class` is part of this GMP-only boundary. It is implemented from two `mpf_class` components and must not use GNU MPC.

## MPFR/MPC Boundary

`mpfrxx_mkII.h` is the MPFR/MPC public header.

It must:

- include `<gmp.h>`
- include `<mpfr.h>`
- include `<mpc.h>`
- expose `gmpfrxx::mpz_class`
- expose `gmpfrxx::mpq_class`
- expose `gmpfrxx::mpfr_class`
- expose `gmpfrxx::mpc_class`
- link through the `mpfrxx_mkII` CMake interface target
- link GMP, MPFR, and MPC

It must not:

- expose `gmpfrxx::mpf_class`
- expose `gmpfrxx::mpfc_class`
- include `mpf_impl.hpp`
- include `mpfc_impl.hpp`

`mpc_class` is the MPC-backed complex type. It stores `mpc_t`, calls GNU MPC APIs, and belongs only to the MPFR/MPC header family and aggregator header.

## Aggregator Boundary

`gmpfrxx_mkII.h` is an aggregator header.

It exposes:

- `gmpfrxx::mpz_class`
- `gmpfrxx::mpq_class`
- `gmpfrxx::mpf_class`
- `gmpfrxx::mpfc_class`
- `gmpfrxx::mpfr_class`
- `gmpfrxx::mpc_class`

It links through the `gmpfrxx_mkII` CMake interface target and requires GMP, MPFR, and MPC.

## API Audit Checklist

Current Phase 13 audit status:

| Check | Status | Enforcement |
|---|---:|---|
| `gmpxx_mkII.h` does not include `<mpfr.h>` | DONE | `test_header_boundaries` |
| `gmpxx_mkII.h` does not include `<mpc.h>` | DONE | `test_header_boundaries` |
| `gmpxx_mkII` target does not link MPFR or MPC | DONE | CMake target definition and GMP-only runtime tests |
| `mpfrxx_mkII.h` exposes `mpfr_class` and `mpc_class` | DONE | header smoke tests and compile tests |
| `mpfrxx_mkII.h` does not expose `mpf_class` | DONE | compile-fail test |
| `mpfrxx_mkII.h` does not expose `mpfc_class` | DONE | compile-fail test |
| `mpfrxx_mkII` target links GMP, MPFR, and MPC | DONE | CMake target definition and MPFR/MPC runtime tests |
| No public class leaks into the global namespace | DONE | public headers define classes in `namespace gmpfrxx`; no global aliases are provided |
| No `<gmpxx.h>` include exists in production headers | DONE | `test_header_boundaries` |
| No `libgmpxx` link exists | DONE | CMake target definitions |
| Compile-fail tests are active | DONE | CTest |
| `STATUS.md` is up to date | DONE | Updated after each phase |
| Curated reference notes exist for relied-on upstream behavior | DONE | `reference/upstream/*/curated/` |
| MPC usage is limited to `mpc_class`, MPFR/MPC headers, tests, and reference docs | DONE | source scan and header boundary tests |

## Target Dependency Graph

```text
gmpxx_mkII
  -> GMP

mpfrxx_mkII
  -> GMP
  -> MPFR
  -> MPC

gmpfrxx_mkII
  -> GMP
  -> MPFR
  -> MPC
```
