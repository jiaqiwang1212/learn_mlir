// RUN: ch-10-opt --diag-registered %s 2>&1 | FileCheck %s
//
// Tests for DiagRegisteredPass (--diag-registered):
//
//   1. registerHandler: custom [CH10-DIAG] format for all 4 severities
//   2. Error emitted via op->emitError with attached Note
//   3. Warning emitted via mlir::emitWarning(loc, msg)   [free-function form]
//   4. Remark emitted via mlir::emitRemark(loc, msg)     [free-function form]
//   5. eraseHandler: post-erase remark goes to default handler (no custom prefix)
//
// The test IR has one arith.constant and one func.return so each emission fires once.

func.func @test_registered() -> i32 {
  %c0 = arith.constant 0 : i32
  return %c0 : i32
}

// --- Handler active: all 4 severities with [CH10-DIAG] prefix ---

// Error + Note chain (from op->emitError(...).attachNote(...)):
// MLIR auto-attaches a "see current operation" note before our explicit note,
// so we use CHECK (not CHECK-NEXT) for the NOTE line:
// CHECK: [CH10-DIAG] ERROR: registered: via op->emitError
// CHECK: [CH10-DIAG] NOTE: registered: direct-engine note

// Warning via free-function form:
// CHECK: [CH10-DIAG] WARNING: registered: via mlir::emitWarning

// Remark via free-function form:
// CHECK: [CH10-DIAG] REMARK: registered: via free function form

// --- After eraseHandler: no custom prefix ---

// Our handler is gone; this remark goes to SourceMgr default printer.
// The actual line contains a location prefix: <file>:<line>:<col>: remark: ...
// CHECK-NOT: [CH10-DIAG] REMARK: after-erase
// CHECK: remark: after-erase: default handler owns this
