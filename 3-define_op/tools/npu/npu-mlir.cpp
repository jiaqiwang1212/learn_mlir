#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "llvm/Support/raw_ostream.h"

int main() {
  // 创建一个MLIR上下文
  mlir::MLIRContext context;

  // 加载SirenDialect到上下文中
  auto siren = context.getOrLoadDialect<mlir::npu_mlir::SirenDialect>();

  mlir::OpBuilder builder(&context);

  auto loc = builder.getUnknownLoc();

  // 创建一个Module
  auto module = builder.create<mlir::ModuleOp>(loc);

  // 设置insert point到Module
  builder.setInsertionPointToStart(module.getBody());

  auto lhs = builder.create<mlir::arith::ConstantIntOp>(loc, 1, 8);
  auto rhs = builder.create<mlir::arith::ConstantIntOp>(loc, 2, 8);

  auto mulOp = builder.create<mlir::npu_mlir::MulOp>(
      loc, builder.getIntegerType(8), lhs, rhs);

  auto divOp = builder.create<mlir::npu_mlir::DivOp>(
      loc, builder.getIntegerType(8), mlir::ValueRange{lhs, rhs});

  if (failed(mlir::verify(module))) {
    llvm::errs() << "Module verification failed\n";
    return 1;
  } else {
    module->dump();
  }

  return 0;
}