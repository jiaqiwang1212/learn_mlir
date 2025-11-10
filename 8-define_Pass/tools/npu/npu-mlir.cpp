#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "npu-mlir/Dialect/Siren/IR/SirenOps.h"
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

  // 测试当创建siren_tensor_type时，elementType的约束
  // auto test_siren_tensor_type_err = mlir::npu_mlir::SirenTensorType::get(
  //     &context, builder.getF64Type(), llvm::ArrayRef<int64_t>{1, 3, 224,
  //     224});

  // 创建自已自定义类型
  auto siren_tensor_type = mlir::npu_mlir::SirenTensorType::get(
      &context, builder.getF16Type(), llvm::ArrayRef<int64_t>{1, 3, 224, 224});

  // 测试TypeTrait, 这个是对于每个使用了MyTypeTrait的Type都会有这个方法
  siren_tensor_type.printTypeTrait();

  // 这个是每个使用了MyTypeTrait的Type都会有一份在自已的代码里
  siren_tensor_type.incrementCounter();

  // 添加一个func
  auto funcType =
      builder.getFunctionType({siren_tensor_type}, {siren_tensor_type});
  auto func = builder.create<mlir::func::FuncOp>(loc, "test_func", funcType);
  auto &entryBlock = *func.addEntryBlock();
  builder.setInsertionPointToStart(&entryBlock);

  auto lhs = builder.create<mlir::arith::ConstantIntOp>(loc, 1, 8);
  auto rhs = builder.create<mlir::arith::ConstantIntOp>(loc, 2, 8);

  auto mulOp = builder.create<mlir::npu_mlir::MulOp>(
      loc, builder.getIntegerType(8), lhs, rhs);

  auto divOp = builder.create<mlir::npu_mlir::DivOp>(
      loc, builder.getIntegerType(8), mlir::ValueRange{lhs, rhs});

  auto siren_attr = mlir::npu_mlir::CustomAttr::get(&context, "example");
  auto sqrtOp = builder.create<mlir::npu_mlir::SqrtOp>(
      loc, siren_tensor_type, entryBlock.getArgument(0), siren_attr);

  // 直接从string创建自定义属性
  auto c = builder.create<mlir::npu_mlir::UseCustomOp>(loc, "world");

  // 创建external attr
  auto externAttr =
      mlir::npu_mlir::MyExternAttr::get(&context, builder.getI32Type());
  auto mycustomop = builder.create<mlir::npu_mlir::MyCustomOp>(loc, externAttr);

  builder.create<mlir::func::ReturnOp>(loc, sqrtOp.getResult());

  if (failed(mlir::verify(module))) {
    llvm::errs() << "Module verification failed\n";
    return 1;
  } else {
    module->dump();
  }

  return 0;
}