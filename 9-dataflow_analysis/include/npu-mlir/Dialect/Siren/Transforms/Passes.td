#ifndef NPU_MLIR_SIREN_DIALECT_PASSES
#define NPU_MLIR_SIREN_DIALECT_PASSES

include "mlir/Pass/PassBase.td"

// A read-only pass that uses MLIR's sparse constant propagation framework
// to determine which SSA values are compile-time constants, then emits
// diagnostic remarks annotating each constant-valued op result.
def ConstantPropAnalysis : Pass<"constant-prop-analysis", "mlir::func::FuncOp"> {
  let summary = "Sparse constant propagation analysis (read-only, emits remarks)";
  let description = [{
    Runs MLIR's built-in SparseConstantPropagation + DeadCodeAnalysis on a
    func.func body. For every op result determined to be a compile-time
    constant, emits a remark of the form:
      remark: value is constant: <attr>
    The IR is never modified. This pass teaches how to wire up the MLIR
    DataFlow Analysis framework.
  }];
  let dependentDialects = ["mlir::func::FuncDialect",
                           "mlir::arith::ArithDialect"];
}

// A custom forward data-flow analysis that tracks minimum and maximum possible
// integer values for every SSA result, then emits remarks with the inferred
// range.  Transfer functions cover arith.constant, addi, muli, and remui.
def RangeAnalysis : Pass<"range-analysis", "mlir::func::FuncOp"> {
  let summary = "Integer range analysis (emits remarks with inferred value ranges)";
  let description = [{
    Runs a custom SparseForwardDataFlowAnalysis<Lattice<RangeLatticeValue>>
    on a func.func body.  For every integer-typed result whose range is
    provably tighter than [INT64_MIN, INT64_MAX], emits a remark:
      remark: value is in range: [lo, hi]

    Transfer functions:
      arith.constant c    -> [c, c]
      [a,b] + [c,d]       -> [a+c, b+d]
      [a,b] * [c,d]       -> [min(corners), max(corners)]
      x remui N (N const) -> [0, N-1]

    Teaches how to implement a fully custom dataflow analysis using the
    SparseForwardDataFlowAnalysis template.  The IR is never modified.
  }];
  let dependentDialects = ["mlir::func::FuncDialect",
                           "mlir::arith::ArithDialect"];
}

#endif // NPU_MLIR_SIREN_DIALECT_PASSES
