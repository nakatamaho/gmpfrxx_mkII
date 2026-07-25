# P2A Real Comparison Embeddings

- MPLAPACK P1: 
- Locked upstream base: 
- Clean clone: 
- Branch: 
- Dirty evidence snapshot: 
- Snapshot SHA256SUMS: 

The failed tar capture was caused by placing  after file operands. The old
partial snapshot was preserved. No dirty-clone files were transplanted. The
external  and all complex/EDD/TD work were
excluded.

## Rows and API

 uses the existing  path. 
and  use opt-in explicit constructors backed by direct MPFR component
sums.  uses  through an explicit adapter type.
 uses native .  uses direct
 from .

The minimal public API is explicit construction/assignment to an owning
; no reverse conversion or mixed operators were added.
No precision metadata is inspected or negotiated.

## Tests

Configuration and build:

-- The CXX compiler identification is GNU 15.2.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring incomplete, errors occurred!
Test project /home/docker/build-p2a

Result: 189/189 tests passed. New tests cover dd, qd, GMP MPF, binary80, and
binary128; dd/qd/binary sentinels retain information below binary64. Existing
fresh-process default tests cover the 512-bit defaults.

No MPFC, complex interop, reverse conversion, mixed arithmetic, precision
sweeps, rounding sweeps, EDD, or TD work was added.
