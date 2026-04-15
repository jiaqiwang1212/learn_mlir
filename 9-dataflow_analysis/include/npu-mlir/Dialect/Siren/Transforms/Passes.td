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

#endif // NPU_MLIR_SIREN_DIALECT_PASSES
