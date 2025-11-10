#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h"

int main(int argc, char **argv) {

  // 注册SirenDialect到注册表中, 注册表只会在opt工具中使用
  mlir::DialectRegistry registry;
  registry.insert<mlir::npu_mlir::SirenDialect, mlir::func::FuncDialect,
                  mlir::arith::ArithDialect>();
  mlir::registerTransformsPasses();

  mlir::npu_mlir::registerTestPassOne();
  // /data1/wangjiaqi/workspace/llvm_essentials/install/bin/ch-2-opt
  // --show-dialects
  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "npu-opt", registry));
}