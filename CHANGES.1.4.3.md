# Changes for 1.4.3

## 1.4.3 - 2026-09-18

Binary adapter portability maintenance release. The public source archive is
distributed as `gmpfrxx_mkII.1.4.3.tar.xz`; the runtime version APIs report
`1.4.3`.

### Binary adapter header portability

- Make the unsupported `binary80_native_traits` diagnostic dependent on the
  native type, matching the binary128 trait implementation.
- Allow binary128 adapter headers to be included on platforms where binary80
  is unavailable, including arm64 targets whose `long double` is binary128.
- Preserve the existing binary80 conversion path when a supported binary80
  native type is used.

### Regression coverage

- Add an adapter-header smoke test that compiles the binary128 adapter without
  requiring a binary80 native type.
