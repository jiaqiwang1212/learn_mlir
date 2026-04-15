// RUN: ch-9-opt --constant-prop-analysis %s 2>&1 | FileCheck %s
//
// Ch-9: Constant Propagation Analysis — FileCheck Tests
//
// Remarks from op->emitRemark() go to stderr automatically.
// Piping 2>&1 merges stderr+stdout so FileCheck sees all output in order:
//   1. remark/note lines (from stderr, emitted during pass execution)
//   2. module { ... } output (from stdout, printed after the pass)
//
// The analysis uses MLIR's built-in SparseConstantPropagation +
// DeadCodeAnalysis to identify compile-time-constant SSA values.

// ---------------------------------------------------------------------------
// Test 1: arith.constant ops — always constant (value is their attribute).
// arith.addi of two known constants folds to 7 via the operation folder.
// ---------------------------------------------------------------------------

// CHECK: remark: value is constant: 3 : i32
// CHECK: remark: value is constant: 4 : i32
// CHECK: remark: value is constant: 7 : i32
func.func @test_simple_constants() -> i32 {
  %c3 = arith.constant 3 : i32
  %c4 = arith.constant 4 : i32
  %sum = arith.addi %c3, %c4 : i32
  func.return %sum : i32
}

// ---------------------------------------------------------------------------
// Test 2: Unknown function argument.
// %c10 is a constant; %sum depends on the unknown %arg0 so it is NOT constant.
// ---------------------------------------------------------------------------

// CHECK: remark: value is constant: 10 : i32
// (no remark for arith.addi %c10, %arg0 because %arg0 is not a constant)
func.func @test_unknown_input(%arg0: i32) -> i32 {
  %c10 = arith.constant 10 : i32
  %sum = arith.addi %c10, %arg0 : i32
  func.return %sum : i32
}

// ---------------------------------------------------------------------------
// Test 3: Multiplication of two constants folds to 6.
// ---------------------------------------------------------------------------

// CHECK: remark: value is constant: 2 : i32
// CHECK: remark: value is constant: 3 : i32
// CHECK: remark: value is constant: 6 : i32
func.func @test_mul_constants() -> i32 {
  %c2 = arith.constant 2 : i32
  %c3 = arith.constant 3 : i32
  %prod = arith.muli %c2, %c3 : i32
  func.return %prod : i32
}
