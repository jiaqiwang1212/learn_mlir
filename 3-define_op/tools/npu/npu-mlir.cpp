#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "llvm/Support/raw_ostream.h"

int main() {
  // 创建一个MLIR上下文
  mlir::MLIRContext context;

  // 加载SirenDialect到上下文中
  context.getOrLoadDialect<mlir::npu_mlir::SirenDialect>();

  mlir::OpBuilder builder(&context);

  auto loc = builder.getUnknownLoc();

  auto addOp = builder.create<mlir::npu_mlir::AddOp>(loc);

  addOp.printOpName();
  return 0;
}