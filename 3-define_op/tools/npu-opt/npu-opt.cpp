#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"

int main(int argc, char **argv) {

  mlir::DialectRegistry registry;
  registry.insert<mlir::npu_mlir::SirenDialect>();
  registry.insert<mlir::func::FuncDialect>();
  registry.insert<mlir::arith::ArithDialect>();
  mlir::registerTransformsPasses();

  // /data1/wangjiaqi/workspace/llvm_essentials/install/bin/ch-2-opt
  // --show-dialects
  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "npu-opt", registry));
}