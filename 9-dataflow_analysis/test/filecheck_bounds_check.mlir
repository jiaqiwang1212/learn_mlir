// RUN: ch-9-opt --range-analysis %s 2>&1 | FileCheck %s
//
// Show range analysis working end-to-end with a realistic bounds-check idiom
// over `index` type.
//
// %x = remui %input, 100 → analysis deduces %x ∈ [0, 99].
// The runtime check (cmpi + andi + cf.assert) is therefore always true and
// could be eliminated by an optimisation pass that consumes the lattice
// results.  This pass only proves the ranges; it does not rewrite the IR.
//
// Expected remarks (remarks fire only for non-top, non-empty ranges):
//   %c100  → [100, 100]
//   %x     → [0, 99]
//   %c0    → [0, 0]
//   %c99   → [99, 99]
//
// No remark for:
//   %input   — function argument, range is unknown (top)
//   %lo_ok, %hi_ok, %in_bnds — cmpi / andi not handled, fall back to top
//   %val     — memref.load result unknown at compile time

// CHECK: remark: value is in range: [100, 100]
// CHECK: remark: value is in range: [0, 99]
// CHECK: remark: value is in range: [0, 0]
// CHECK: remark: value is in range: [99, 99]
func.func @bounds_check(%input: index, %arr: memref<100xindex>) -> index {
  %c100 = arith.constant 100 : index
  %x    = arith.remui %input, %c100 : index

  // Runtime bounds check — redundant because %x ∈ [0, 99] is proven above.
  %c0      = arith.constant 0 : index
  %c99     = arith.constant 99 : index
  %lo_ok   = arith.cmpi sge, %x, %c0  : index
  %hi_ok   = arith.cmpi sle, %x, %c99 : index
  %in_bnds = arith.andi %lo_ok, %hi_ok : i1
  cf.assert %in_bnds, "out of bounds"

  %val = memref.load %arr[%x] : memref<100xindex>
  func.return %val : index
}
