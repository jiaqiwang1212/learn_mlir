#include "mlir/IR/Builders.h"
#include "mlir/IR/DialectInterface.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "npu-mlir/Interfaces/NpuDialectInterface.h"
#include "npu-mlir/Interfaces/NpuOpInterface.h"
#include "llvm/Support/raw_ostream.h"

void test_op_interface() {
  llvm::outs() << "Test NpuOpInterface\n";
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

  auto subOp = builder.create<mlir::npu_mlir::SubOp>(
      loc, builder.getIntegerType(8), lhs, rhs);

  auto addOp = builder.create<mlir::npu_mlir::AddOp>(loc, lhs, rhs);

  if (auto op = llvm::dyn_cast<mlir::npu_mlir::NpuOpInterface>(
          addOp.getOperation())) {
    llvm::outs() << op.get_num_operands_1() << "\n"; // 调用接口方法
    llvm::outs() << op.get_num_operands_2() << "\n"; // 调用覆盖方法
    llvm::outs() << op.get_num_operands_3() << "\n"; // 调用接口方法
  }

  if (failed(mlir::verify(module))) {
    llvm::errs() << "Module verification failed\n";
  } else {
    module->dump();
  }
}

void test_dialect_interface() {
  llvm::outs() << "Test NpuDialectInterface\n";
  // 创建一个MLIR上下文
  mlir::MLIRContext context;

  // 加载SirenDialect到上下文中
  auto siren = context.getOrLoadDialect<mlir::npu_mlir::SirenDialect>();

  // 1. 测试在SirenDialect上获取NpuDialectInterface接口实例
  if (auto iface =
          siren
              ->getRegisteredInterface<mlir::npu_mlir::NpuDialectInterface>()) {
    iface->printSomeInfo();
  }

  // 2. 获取该上下文中所有注册的NpuDialectInterface接口实例
  mlir::DialectInterfaceCollection<mlir::npu_mlir::NpuDialectInterface> ifaces(
      &context);

  for (auto iface : ifaces) {
    iface.printSomeInfo();
    auto dialect = iface.getDialect();
    llvm::outs() << "Dialect name: " << dialect->getNamespace() << "\n";
  }
}

int main() {
  // test_op_interface();
  test_dialect_interface();
  return 0;
}