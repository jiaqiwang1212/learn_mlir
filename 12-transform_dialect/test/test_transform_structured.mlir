// RUN: mlir-opt %s --transform-interpreter 2>&1 | FileCheck %s
//
// Demo 2: Structured op transformations — tile and fuse.
//
// Teaches:
//   - transform.structured.tile_using_forall — tile a structured op into an scf.forall loop
//   - transform.structured.fuse_into_containing_op — fuse a producer into the tiling loop
//   - Handle chaining: the loop handle from tiling is fed directly into fusion
//   - Handle invalidation: tiling CONSUMES the tiled op's handle; fusion returns a fresh handle
//
// Background: This demo implements the canonical "fc_relu" tile+fuse flow from the
// MLIR Transform dialect Ch1 tutorial. See:
//   externals/llvm-project/mlir/docs/Tutorials/transform/Ch1.md
//
// Run with:
//   build/llvm/bin/mlir-opt 6-experiment/test/test_transform_structured.mlir \
//       --transform-interpreter

module attributes {transform.with_named_sequence} {
  // ── Payload IR ───────────────────────────────────────────────────────────
  // A simplified "fully-connected + bias" layer: matmul followed by elementwise add.
  func.func @fc_relu(%lhs: tensor<64x64xf32>, %rhs: tensor<64x64xf32>,
                     %bias: tensor<64x64xf32>, %out: tensor<64x64xf32>)
                     -> tensor<64x64xf32> {
    %matmul = linalg.matmul
        ins(%lhs, %rhs : tensor<64x64xf32>, tensor<64x64xf32>)
        outs(%out : tensor<64x64xf32>) -> tensor<64x64xf32>
    %biased = linalg.elementwise kind=#linalg.elementwise_kind<add>
        ins(%matmul, %bias : tensor<64x64xf32>, tensor<64x64xf32>)
        outs(%out : tensor<64x64xf32>) -> tensor<64x64xf32>
    return %biased : tensor<64x64xf32>
  }

  // ── Transform IR ─────────────────────────────────────────────────────────
  transform.named_sequence @__transform_main(%arg0: !transform.any_op) {

    // Step 1: Match the ops we want to transform.
    %mm = transform.structured.match ops{["linalg.matmul"]} in %arg0
        : (!transform.any_op) -> !transform.any_op
    %ew = transform.structured.match ops{["linalg.elementwise"]} in %arg0
        : (!transform.any_op) -> !transform.any_op

    // Step 2: Tile the elementwise op with tile sizes [8, 32].
    // tile_using_forall returns TWO handles: (tiled_op, forall_loop).
    // The tiled_op is the linalg.elementwise operating on the sub-tensor slice.
    // The forall_loop is the surrounding scf.forall loop that iterates the tiles.
    // IMPORTANT: tile_using_forall CONSUMES %ew — do not use %ew after this op.
    %tiled_ew, %loop = transform.structured.tile_using_forall %ew tile_sizes [8, 32]
        : (!transform.any_op) -> (!transform.any_op, !transform.any_op)

    // Step 3: Fuse the matmul into the forall loop produced by tiling.
    // "Fusing into" means: instead of computing the full matmul result and then
    // slicing it for each tile, we compute only the matmul slice needed for each tile.
    // This achieves producer-consumer fusion with the tiling loop.
    //
    // fuse_into_containing_op returns TWO handles: (fused_op, updated_loop).
    // IMPORTANT: fuse_into_containing_op CONSUMES %mm (the producer) — do not use %mm after this op.
    // The containing-op handle %loop is only READ (not consumed) and remains valid.
    %mm_fused, %loop_updated = transform.structured.fuse_into_containing_op
        %mm into %loop
        : (!transform.any_op, !transform.any_op) -> (!transform.any_op, !transform.any_op)

    // Step 4: Emit a remark at the outer forall loop to verify the transformation ran.
    transform.debug.emit_remark_at %loop_updated, "outer forall loop"
        : !transform.any_op

    transform.yield
  }
}

// After the transform, @fc_relu should contain a single scf.forall that
// houses both the tiled linalg.matmul and the tiled linalg.elementwise.
// CHECK: scf.forall
// CHECK: remark: outer forall loop
