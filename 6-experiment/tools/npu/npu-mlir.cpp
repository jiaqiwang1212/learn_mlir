#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/InitAllDialects.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "llvm/Support/raw_ostream.h"

int main() {
  // 创建一个MLIR上下文
  mlir::MLIRContext context;

  // 加载SirenDialect到上下文中
  context.getOrLoadDialect<mlir::npu_mlir::SirenDialect>();
  // 注册所有内置的MLIR dialects
  mlir::registerAllDialects(context);

  mlir::OpBuilder builder(&context);

  auto loc = builder.getUnknownLoc();

  return 0;
}