// RUN: mlir-opt %s --transform-interpreter 2>&1 | FileCheck %s
//
// Demo 3: Loop transforms — tile then outline.
//
// Teaches:
//   - Two-stage tile_using_forall: first for actual tiling, then for materializing an
//     outer single-iteration loop suitable for outlining
//   - transform.loop.outline — extract a loop's body into a new func.func
//   - apply_registered_pass "canonicalize" — fold the scf.execute_region wrapper that
//     loop.outline inserts around the outlined call
//   - Handle naming: use distinct SSA names (%_ is fine once per region; use %canon etc.)
//   - apply_registered_pass CONSUMES its handle — capture the returned handle
//
// Run with:
//   build/llvm/bin/mlir-opt 6-experiment/test/test_transform_loop.mlir \
//       --transform-interpreter

module attributes {transform.with_named_sequence} {
  // ── Payload IR ───────────────────────────────────────────────────────────
  func.func @outline_me(%a: tensor<32x32xf32>, %b: tensor<32x32xf32>,
                        %o: tensor<32x32xf32>) -> tensor<32x32xf32> {
    %r = linalg.matmul ins(%a, %b : tensor<32x32xf32>, tensor<32x32xf32>)
                       outs(%o : tensor<32x32xf32>) -> tensor<32x32xf32>
    return %r : tensor<32x32xf32>
  }

  // ── Transform IR ─────────────────────────────────────────────────────────
  // %arg0 must NOT be marked {transform.readonly} because we consume it with
  // apply_registered_pass at the end of this sequence.
  transform.named_sequence @__transform_main(%arg0: !transform.any_op) {

    // Step 1: Match the matmul we want to transform.
    %mm = transform.structured.match ops{["linalg.matmul"]} in %arg0
        : (!transform.any_op) -> !transform.any_op

    // Step 2: First tile — splits the 32x32 matmul into 8x8 tiles.
    // Returns (tiled_op, inner_forall). We keep %tiled for the next tile step.
    %tiled, %inner = transform.structured.tile_using_forall %mm tile_sizes [8, 8]
        : (!transform.any_op) -> (!transform.any_op, !transform.any_op)

    // Step 3: Second tile with size [1] — materializes a single-iteration outer forall.
    // This outer loop is what we will outline. Its body contains the [8,8]-tiled matmul.
    // %_ discards the inner tiled-op handle (we only need the outer loop for outlining).
    %_, %outer = transform.structured.tile_using_forall %tiled tile_sizes [1]
        : (!transform.any_op) -> (!transform.any_op, !transform.any_op)

    // Step 4: Outline the outer forall loop into a new function @outlined_matmul.
    // loop.outline returns (function_handle, call_handle).
    // Internally, loop.outline wraps the loop in scf.execute_region before extracting —
    // the canonicalize step below folds that wrapper away.
    %func, %call = transform.loop.outline %outer {func_name = "outlined_matmul"}
        : (!transform.any_op) -> (!transform.any_op, !transform.op<"func.call">)

    transform.debug.emit_remark_at %func, "outlined function" : !transform.any_op

    // Step 5: Canonicalize to fold the scf.execute_region wrapper that loop.outline inserts.
    // NOTE: We cannot apply canonicalize to %arg0 (the module) because the transform
    // sequence is nested inside that same module — applying a transform to an ancestor
    // of the transform op itself is forbidden by the interpreter.
    // Solution: match and canonicalize the func.func ops instead (not the module).
    // apply_patterns does NOT consume the handle, so no re-match is needed.
    %funcs_after = transform.structured.match ops{["func.func"]} in %arg0
        : (!transform.any_op) -> !transform.any_op
    transform.apply_patterns to %funcs_after {
      transform.apply_patterns.canonicalization
    } : !transform.any_op

    transform.yield
  }
}

// After the transform, the module should contain:
//   1. @outlined_matmul — the newly created function
//   2. @outline_me — now contains a func.call to @outlined_matmul
// CHECK: func.func @outlined_matmul
// CHECK: remark: outlined function
