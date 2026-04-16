// RUN: mlir-opt %s --transform-interpreter 2>&1 | FileCheck %s
//
// Demo 4: Driving cleanup from Transform IR — three orthogonal mechanisms.
//
// Mechanisms (applied in C → B → A order so each has observable work):
//   C. transform.apply_cse          — eliminate common subexpressions (does NOT consume handle)
//   B. transform.apply_patterns     — apply canonicalization patterns (does NOT consume handle)
//   A. transform.apply_registered_pass "canonicalize"  — run full canonicalize pass (CONSUMES handle)
//
// Why C → B → A?
//   The payload has two identical arith.constant 0 ops and two arith.addi(x, 0) ops.
//   Running C first merges the duplicate constants (CSE cannot fold addi-with-zero).
//   Running B next applies the arith.addi(x, 0) → x canonicalization pattern.
//   Running A last reaches a guaranteed fixpoint (DCE, etc.), whatever B left behind.
//   If A ran first, it would subsume B and C, leaving nothing for them to demonstrate.
//
// Key API facts:
//   - apply_cse does NOT consume its operand handle
//   - apply_patterns does NOT consume its operand handle
//   - apply_registered_pass DOES consume its operand — always capture the returned handle
//   - {apply_cse} is a trailing UnitAttr in apply_patterns' attr-dict (AFTER the `{ }` region,
//     BEFORE the `: type`). It is NOT a nested op inside the region braces.
//
// Run with:
//   build/llvm/bin/mlir-opt 6-experiment/test/test_transform_passes.mlir \
//       --transform-interpreter

module attributes {transform.with_named_sequence} {
  // ── Payload IR ───────────────────────────────────────────────────────────
  // Two identical constants (CSE target) + two addi-with-zero ops (pattern target).
  func.func @messy(%x: i32) -> i32 {
    %c0a = arith.constant 0 : i32
    %c0b = arith.constant 0 : i32    // same value as %c0a — CSE will merge this
    %a   = arith.addi %x,  %c0a : i32  // x + 0 — canonicalize pattern folds to %x
    %b   = arith.addi %a,  %c0b : i32  // %x + 0 — same pattern
    return %b : i32
  }

  // ── Transform IR ─────────────────────────────────────────────────────────
  transform.named_sequence @__transform_main(%arg0: !transform.any_op) {

    %f = transform.structured.match ops{["func.func"]} in %arg0
        : (!transform.any_op) -> !transform.any_op

    // ── Mechanism C: transform.apply_cse ─────────────────────────────────
    // Merges %c0b into %c0a — both addi ops now consume the same constant.
    // Observable: IR still has two addi ops but only one arith.constant 0.
    // %f remains valid after this op (apply_cse does not consume its operand).
    transform.apply_cse to %f : !transform.any_op

    // ── Mechanism B: transform.apply_patterns ────────────────────────────
    // Applies canonicalization patterns greedily to ops inside %f.
    // arith.addi(x, 0) → x fires for both %a and %b; they are eliminated.
    // The func body effectively becomes `return %x`.
    // %f remains valid after this op (apply_patterns does not consume its operand).
    transform.apply_patterns to %f {
      transform.apply_patterns.canonicalization
    } : !transform.any_op
    // Note: {apply_cse} attribute intentionally omitted — we ran CSE explicitly
    // in Mechanism C above to demonstrate each mechanism independently.

    // ── Mechanism A: transform.apply_registered_pass ─────────────────────
    // Runs the full "canonicalize" compiler pass (includes DCE, constant folding, etc.)
    // to reach a clean fixpoint — removing any remaining dead ops.
    // apply_registered_pass CONSUMES %f — use the returned handle %f2, not %f.
    %f2 = transform.apply_registered_pass "canonicalize" to %f
        : (!transform.any_op) -> !transform.any_op

    transform.yield
  }
}

// After all three mechanisms, @messy should return %x (its argument) directly.
// CHECK: func.func @messy
// CHECK-NOT: arith.addi
// CHECK: return
