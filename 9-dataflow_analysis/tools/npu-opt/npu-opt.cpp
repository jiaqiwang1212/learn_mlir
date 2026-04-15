#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h"

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;
  registry.insert<mlir::func::FuncDialect, mlir::arith::ArithDialect>();
  mlir::registerTransformsPasses();

  mlir::npu_mlir::registerConstantPropAnalysis();

  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "ch-9-opt", registry));
}
