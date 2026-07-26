# Changes for 1.1.0

## 1.1.0 release candidate - 2026-07-26

This backward-compatible minor release adds opt-in, explicit comparison
embeddings used by MPLAPACK. The source archive follows the established
`gmpfrxx_mkII.1.1.0.tar.xz` naming convention, and the runtime version APIs
report `1.1.0`.

### One-way comparison embeddings

- `mpfrxx::mpfr_class` can explicitly materialize finite results from
  `double`, dd, qd, binary80, binary128, and `gmpxx::mpf_class` through the
  corresponding opt-in adapter headers.
- `mpfrxx::mpc_class` can explicitly materialize the corresponding finite
  complex source families, including `gmpxx::mpfc_class`.
- dd and qd values are reconstructed directly from their stored components.
- binary80 and binary128 use native MPFR long-double and float128 imports.
- GMP MPF and MPFC use direct GMP-to-MPFR component imports.
- Extended sources do not pass through binary64.

### Precision and scope

- Ordinary default construction remains 512 binary bits for
  `mpfrxx::mpfr_class` and `gmpxx::mpf_class`.
- Adapter assignment uses ordinary destination semantics without inspecting,
  comparing, negotiating, or preserving source/destination precision metadata.
- The adapters are explicit, one-way comparison boundaries. Reverse
  conversion, mixed-backend arithmetic, adapter compound assignment,
  exact-rounding guarantees, round-trip guarantees, and edd/td MPLAPACK
  support are not included.
- MPLAPACK-specific utility APIs remain downstream and no MPLAPACK header or
  type is required by this package.

### Packaging

- The installed CMake package continues to export
  `gmpfrxx_mkII::gmpfrxx_mkII` and declares GMP, MPFR, and MPC dependencies
  through relocatable package metadata.
- Release verification disables dependency auto-fetch and validates all
  supported adapters from installed headers.
