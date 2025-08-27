#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/InitAllDialects.h"
#include "mlir/Parser/Parser.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

int main() {
  // 创建一个MLIR上下文
  mlir::MLIRContext context;

  // 加载SirenDialect到上下文中
  context.getOrLoadDialect<mlir::npu_mlir::SirenDialect>();
  // 注册所有内置的MLIR dialects
  mlir::registerAllDialects(context);

  mlir::OpBuilder builder(&context);

  const char *path = "/data1/wangjiaqi/workspace/llvm_essentials/6-experiment/"
                     "test/test_liveness.mlir";
  std::string error;
  auto file = mlir::openInputFile(path, &error);
  if (!file) {
    llvm::errs() << "Error opening file: " << error << "\n";
    return 1;
  }
  // llvm::SourceMgr 是LLVM/MLIR中的源代码管理器，用于处理和管理源代码文件
  llvm::SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(file), llvm::SMLoc());

  mlir::OwningOpRef<mlir::ModuleOp> module =
      mlir::parseSourceFile<mlir::ModuleOp>(sourceMgr, &context);

  if (!module) {
    llvm::errs() << "Error parsing MLIR file\n";
    return 1;
  }
  module->dump();
  return 0;
}