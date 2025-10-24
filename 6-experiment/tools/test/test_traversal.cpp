#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Parser/Parser.h"
#include "mlir/Support/FileUtilities.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

void test_traversal_1(mlir::ModuleOp module) {
  llvm::outs() << "=================Post Order1==================\n";
  module.walk([](mlir::Operation *op) {
    llvm::outs() << "Visiting operation: " << op->getName() << "\n";
    if (auto func_op = mlir::dyn_cast<mlir::func::FuncOp>(op)) {
      llvm::outs() << "  Function name: " << func_op.getName() << "\n";
    }
    if (auto module_op = mlir::dyn_cast<mlir::ModuleOp>(op)) {
      llvm::outs() << "  Module name: " << module_op.getName() << "\n";
    }
    return mlir::WalkResult::advance();
  });
  llvm::outs() << "=================Post Order2==================\n";
  module.walk([](mlir::Operation *op) {
    llvm::outs() << "Visiting operation: " << op->getName() << "\n";
    if (auto func_op = mlir::dyn_cast<mlir::func::FuncOp>(op)) {
      llvm::outs() << "  Function name: " << func_op.getName() << "\n";
      if (func_op.getName() == llvm::StringRef("inner_fn")) {
        llvm::outs() << "  Found inner function, interrupting walk.\n";
        return mlir::WalkResult::interrupt();
      }
    }
    if (auto module_op = mlir::dyn_cast<mlir::ModuleOp>(op)) {
      llvm::outs() << "  Module name: " << module_op.getName() << "\n";
    }
    return mlir::WalkResult::advance();
  });

  llvm::outs() << "=================Post Order3==================\n";
  module.walk([](mlir::Operation *op) {
    llvm::outs() << "Visiting operation: " << op->getName() << "\n";
    if (auto func_op = mlir::dyn_cast<mlir::func::FuncOp>(op)) {
      llvm::outs() << "  Function name: " << func_op.getName() << "\n";
      if (func_op.getName() == llvm::StringRef("main")) {
        llvm::outs() << "  Found inner function, interrupting walk.\n";
        return mlir::WalkResult::skip();
      }
    }
    if (auto module_op = mlir::dyn_cast<mlir::ModuleOp>(op)) {
      llvm::outs() << "  Module name: " << module_op.getName() << "\n";
      if (module_op.getName() == llvm::StringRef("Test_Traversal")) {
        llvm::outs()
            << "  Found module 'Test_Traversal', skipping its nested ops.\n";
        return mlir::WalkResult::interrupt();
      }
    }
    return mlir::WalkResult::advance();
  });
}

void test_traversal_2(mlir::ModuleOp module) {
  llvm::outs() << "=================Pre Order==================\n";
  module.walk<mlir::WalkOrder::PreOrder>([](mlir::Operation *op) {
    llvm::outs() << "Visiting operation: " << op->getName() << "\n";
    if (auto func_op = mlir::dyn_cast<mlir::func::FuncOp>(op)) {
      llvm::outs() << "  Function name: " << func_op.getName() << "\n";
    }
    if (auto module_op = mlir::dyn_cast<mlir::ModuleOp>(op)) {
      llvm::outs() << "  Module name: " << module_op.getName() << "\n";
    }
  });
}

void test_traversal_3(mlir::ModuleOp module) {
  llvm::outs() << "=================Get Ops==================\n";
  for (mlir::func::FuncOp funcOp : module.getOps<mlir::func::FuncOp>()) {
    llvm::outs() << "Function found: " << funcOp.getName() << "\n";
  }
}

void test_traversal_4(mlir::ModuleOp module) {
  llvm::outs() << "=================Test Func IR Structure==================\n";
  for (mlir::func::FuncOp funcOp : module.getOps<mlir::func::FuncOp>()) {
    llvm::outs() << "Function found: " << funcOp.getName() << "\n";
    if (funcOp.getName() == "main") {
      llvm::outs() << "Processing main function body:\n";
      llvm::outs() << "  Number of regions: " << funcOp->getRegions().size()
                   << "\n";
      for (auto &region : funcOp->getRegions()) {
        llvm::outs() << "  Region with " << region.getBlocks().size()
                     << " blocks:\n";
        for (auto &block : region.getBlocks()) {
          llvm::outs() << "  Block with " << block.getOperations().size()
                       << " operations:\n";
          for (auto &op : block.getOperations()) {
            llvm::outs() << "  Operation: " << op.getName() << "\n";
          }
        }
      }
    }
  }
}

void test_traversal_5(mlir::ModuleOp module) {
  llvm::outs()
      << "=================Test Module IR Structure==================\n";
  auto body = module.getBody(); // Body 就是主体
  for (auto &op : body->getOperations()) {
    if (auto funcOp = mlir::dyn_cast<mlir::func::FuncOp>(op)) {
      llvm::outs() << "Function found: " << funcOp.getName() << "\n";
      auto &func_body = funcOp.getBody(); // body 就是函数体
      for (auto &block : func_body.getBlocks()) {
        llvm::outs() << "  Block with " << block.getOperations().size()
                     << " operations:\n";
        for (auto &op : block.getOperations()) {
          llvm::outs() << "  Operation: " << op.getName() << "\n";
        }
      }
    }
  }
}
int main() {
  // 创建一个MLIR上下文
  mlir::MLIRContext context;
  context.loadDialect<mlir::memref::MemRefDialect>();
  context.loadDialect<mlir::func::FuncDialect>();
  context.loadDialect<mlir::scf::SCFDialect>();
  context.loadDialect<mlir::arith::ArithDialect>();

  mlir::OpBuilder builder(&context);

  const char *path = "/data1/wangjiaqi/workspace/llvm_essentials/6-experiment/"
                     "test/test_traversal.mlir";
  std::string error;
  auto file = mlir::openInputFile(path, &error);
  if (!file) {
    llvm::errs() << "Error opening file: " << error << "\n";
    return 1;
  }
  llvm::SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(file), llvm::SMLoc());

  mlir::OwningOpRef<mlir::ModuleOp> module =
      mlir::parseSourceFile<mlir::ModuleOp>(sourceMgr, &context);

  if (!module) {
    llvm::errs() << "Error parsing MLIR file\n";
    return 1;
  }
  if (mlir::failed(mlir::verify(*module))) {
    llvm::errs() << "MLIR verification failed\n";
    return 1;
  }
  module->dump();
  test_traversal_1(*module);
  test_traversal_2(*module);
  test_traversal_3(*module);
  test_traversal_4(*module);
  test_traversal_5(*module);
  return 0;
}