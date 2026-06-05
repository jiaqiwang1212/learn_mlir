#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;
  registry.insert<mlir::npu_mlir::SirenDialect, mlir::func::FuncDialect,
                  mlir::arith::ArithDialect>();
  mlir::registerTransformsPasses();
  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "npu-opt", registry));
}
