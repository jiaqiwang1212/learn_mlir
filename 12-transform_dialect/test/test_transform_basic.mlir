// RUN: mlir-opt %s --transform-interpreter 2>&1 | FileCheck %s
//
// Demo 1: Basic Transform dialect wiring.
//
// Teaches:
//   - transform.named_sequence @__transform_main — the mandatory entry point
//   - module attributes {transform.with_named_sequence} — required module attr
//   - transform.structured.match — selecting ops by kind from a handle
//   - transform.split_handle — splitting a multi-op handle into individual handles
//   - transform.cast — narrowing a handle's type for better static checking
//   - transform.debug.emit_remark_at — printing a remark anchored at a payload op
//
// Run with:
//   build/llvm/bin/mlir-opt 6-experiment/test/test_transform_basic.mlir \
//       --transform-interpreter

module attributes {transform.with_named_sequence} {
  // ── Payload IR ───────────────────────────────────────────────────────────
  func.func @basic(%a: i32, %b: i32, %c: i32) -> i32 {
    %0 = arith.addi %a, %b : i32
    %1 = arith.addi %0, %c : i32
    return %1 : i32
  }

  // ── Transform IR ─────────────────────────────────────────────────────────
  // The interpreter pass looks for a named sequence called @__transform_main.
  // The first argument %arg0 receives a handle to the top-level op (the module).
  transform.named_sequence @__transform_main(%arg0: !transform.any_op) {

    // Step 1: Match all func.func ops inside the module.
    // The result is a handle associated with every func.func found.
    %funcs = transform.structured.match ops{["func.func"]} in %arg0
        : (!transform.any_op) -> !transform.any_op
    // Emits a remark at each matched op's location. Visible with --verify-diagnostics
    // or in stderr when running mlir-opt.
    transform.debug.emit_remark_at %funcs, "found a function" : !transform.any_op

    // Step 2: Match all arith.addi ops — this handle is associated with BOTH adds.
    %adds = transform.structured.match ops{["arith.addi"]} in %arg0
        : (!transform.any_op) -> !transform.any_op

    // Split the multi-op handle into two individual handles, one per op.
    // The number of results must equal the number of ops in the handle.
    %a1, %a2 = transform.split_handle %adds
        : (!transform.any_op) -> (!transform.any_op, !transform.any_op)

    transform.debug.emit_remark_at %a1, "first add" : !transform.any_op
    transform.debug.emit_remark_at %a2, "second add" : !transform.any_op

    // Step 3: Cast the generic handle to a typed handle.
    // Typed handles carry static guarantees about the op kind.
    // cast does NOT consume %funcs — both handles remain valid afterward.
    %funcs_typed = transform.cast %funcs
        : !transform.any_op to !transform.op<"func.func">
    transform.debug.emit_remark_at %funcs_typed, "typed func handle"
        : !transform.op<"func.func">

    transform.yield
  }
}

// CHECK: remark: found a function
// CHECK: remark: first add
// CHECK: remark: second add
// CHECK: remark: typed func handle
