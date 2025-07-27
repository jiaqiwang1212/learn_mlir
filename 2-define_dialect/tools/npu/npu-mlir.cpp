#include "mlir/IR/MLIRContext.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "llvm/Support/raw_ostream.h"

int main() {
  // 创建一个MLIR上下文
  mlir::MLIRContext context;

  // 加载SirenDialect到上下文中
  auto siren = context.getOrLoadDialect<mlir::npu_mlir::SirenDialect>();

  // 检查SirenDialect是否成功加载
  if (!siren) {
    llvm::errs() << "Failed to load SirenDialect\n";
    return 1;
  } else {
    llvm::outs() << "SirenDialect loaded successfully\n";
    llvm::outs() << "Namespace: " << siren->getNamespace() << "\n";
  }
  return 0;
}