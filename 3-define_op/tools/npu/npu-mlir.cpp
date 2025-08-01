#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "llvm/Support/raw_ostream.h"

int main() {
  // 创建一个MLIR上下文
  mlir::MLIRContext context;

  // 加载SirenDialect到上下文中
  auto siren = context.getOrLoadDialect<mlir::npu_mlir::SirenDialect>();

  mlir::OpBuilder builder(&context);

  auto loc = builder.getUnknownLoc();

  auto addOp = builder.create<mlir::npu_mlir::AddOp>(
      loc, builder.getStringAttr("addOp"));

  auto helper = siren->getDebugLevelAttrHelper();

  // 给op添加discardable attr
  if (helper.isAttrPresent(addOp)) {
    llvm::errs() << "addOp has debug_level attribute\n";
  } else {
    helper.setAttr(addOp, builder.getStringAttr("info"));
  }

  // 1. dump时会verify
  addOp->dump();

  // 2. verify时会检查discardable attr
  if (mlir::failed(mlir::verify(addOp))) {
    llvm::errs() << "addOp verification failed\n";
  } else {
    llvm::outs() << "addOp verification succeeded\n";
  }
  return 0;
}