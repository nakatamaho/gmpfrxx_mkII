# Changes for 1.3.1

## 1.3.1 - 2026-08-10

Shared-library compatibility maintenance release. The public source archive is
distributed as `gmpfrxx_mkII.1.3.1.tar.xz`; the runtime version APIs report
`1.3.1`.

### Compatibility

- Added the portable `GMPFRXX_MKII_HIDDEN` visibility macro. GCC and Clang use
  `visibility("hidden")`; other compilers receive an empty annotation.
- Marked internal expression leaf and node types hidden so template
  instantiations involving those implementation types cannot be exported from
  shared libraries.
- Marked the expression-building operator templates for integer/rational,
  `mpf_class`, `mpfc_class`, `mpfr_class`, and `mpc_class` hidden.
- Prevented Mach-O weak-definition coalescing from resolving expression
  template implementations to incompatible definitions in another dylib.
- Kept public numeric classes and public source-level APIs unchanged.

### Release Verification

- AppleClang 17 standalone CMake build passed with
  `GMPFRXX_MKII_COMPONENTS=GMP,MPFR,MPC`.
- Full CTest passed 154/154 tests on macOS.
- MPLAPACK's GMP SVD driver completed all 10,260 tests without the previous
  segmentation fault.
- The rebuilt MPLAPACK GMP dylib exported no weak external `binary_expr` or
  `unary_expr` specializations.
