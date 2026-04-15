// RUN: ch-10-opt --diag-scoped %s 2>&1 | FileCheck %s
//
// Tests for DiagScopedPass (--diag-scoped):
//
//   1. Custom handler intercepts Error with attached Note
//   2. Warning is filtered (suppressed) — must NOT appear in output
//   3. Remark survives the filter
//   4. RAII handler auto-deregisters at pass end (no output after scope)
//
// The test IR contains exactly one arith.constant so each severity fires once.

func.func @test_scoped() -> i32 {
  %c42 = arith.constant 42 : i32
  return %c42 : i32
}

// Error fired first. MLIR auto-attaches a "see current operation" note before
// our explicit attachNote(), so we use CHECK (not CHECK-NEXT) for the NOTE:
// CHECK: [CH10-DIAG] ERROR: scoped: value is constant
// CHECK: [CH10-DIAG] NOTE: note: attached context

// Warning must NOT appear (suppressWarnings=true):
// CHECK-NOT: [CH10-DIAG] WARNING: scoped: this warning will be suppressed
// CHECK-NOT: warning:

// Remark survives the filter:
// CHECK: [CH10-DIAG] REMARK: scoped: remark survives filter
