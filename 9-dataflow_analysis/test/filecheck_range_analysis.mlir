// RUN: ch-9-opt --range-analysis %s 2>&1 | FileCheck %s
//
// Verify the integer range analysis pass infers tight bounds for four cases:
//
//   %c100 = arith.constant 100  → [100, 100]  (exact constant)
//   %x    = remui %input, %c100 → [0, 99]     (modulo bound: result < divisor)
//   %c3   = arith.constant 3    → [3, 3]      (exact constant)
//   %y    = addi %x, %c3        → [3, 102]    (interval sum: [0+3, 99+3])
//
// %input is a function argument — range is unknown (top), so no remark.
// %y range [3, 102] spans above 100, showing the analysis is conservative:
// a bounds check against limit=100 on %y cannot be eliminated.

// CHECK: remark: value is in range: [100, 100]
// CHECK: remark: value is in range: [0, 99]
// CHECK: remark: value is in range: [3, 3]
// CHECK: remark: value is in range: [3, 102]
func.func @compute(%input: i64) -> i64 {
  %c100 = arith.constant 100 : i64
  %x    = arith.remui %input, %c100 : i64
  %c3   = arith.constant 3 : i64
  %y    = arith.addi %x, %c3 : i64
  func.return %y : i64
}
