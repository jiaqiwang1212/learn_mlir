#include "mlir/Analysis/Liveness.h"
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

void test_liveness(mlir::ModuleOp module) {
  mlir::Liveness liveness(module);
  // 遍历整个module
  mlir::SmallVector<mlir::Value> res_values;
  int64_t num_ops = 0;
  module->walk([&](mlir::Operation *op) {
    // 对每个操作进行处理
    llvm::outs() << "Results:\n";
    for (auto result : op->getResults()) {
      res_values.push_back(result);
      llvm::outs() << liveness.isDeadAfter(result, op) << "\n";
    }
    llvm::outs() << "Operands:\n";
    for (auto operand : op->getOperands()) {
      llvm::outs() << liveness.isDeadAfter(operand, op) << "\n";
    }
    op->dumpPretty();
    num_ops++;
  });
  llvm::outs() << "Total operations: " << num_ops << "\n";
}

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
  // module->dump();
  test_liveness(*module);
  return 0;
}