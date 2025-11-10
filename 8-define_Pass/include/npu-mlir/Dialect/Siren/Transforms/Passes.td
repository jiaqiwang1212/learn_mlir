#ifndef NPU_MLIR_SIREN_DIALECT_PASSES
#define NPU_MLIR_SIREN_DIALECT_PASSES

include "mlir/Pass/PassBase.td"

def TestPassOne : Pass<"test-pass-one"> {
  let summary = "Siren Dialect test pass";
//   let constructor = "mlir::affine::createAffineDataCopyGenerationPass()";
  let dependentDialects = ["mlir::func::FuncDialect",
                           "mlir::arith::ArithDialect",
                           "::mlir::npu_mlir::SirenDialect"];
//   let options = [
//   ];
}

#endif // NPU_MLIR_SIREN_DIALECT_PASSES