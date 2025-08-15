#include "npu-mlir/Dialect/Siren/IR/SirenOps.h"
#include "llvm/Support/raw_ostream.h"

namespace mlir::npu_mlir {
void AddOp::printOpName() { llvm::outs() << "siren.add\n"; }

llvm::LogicalResult AddOp::canonicalize(AddOp op,
                                        ::mlir::PatternRewriter &rewriter) {
  llvm::outs() << "Canonicalizing siren.add operation\n";
  // 在这里可以添加具体的规范化逻辑
  return llvm::failure();
}

mlir::OpFoldResult SubOp::fold(FoldAdaptor adaptor) {
  // 在这里可以添加折叠逻辑
  llvm::outs() << "Folding siren.sub operation\n"; //
  // 將兩個constant操作的結果相減
  // 两边都是常量：常量折叠
  Attribute lhs = adaptor.getLhs();
  Attribute rhs = adaptor.getRhs();
  if (!lhs || !rhs) {
    return {}; // 返回一个空的 OpFoldResult 表示折叠失败
  }

  auto lhsAttr = mlir::dyn_cast<mlir::IntegerAttr>(lhs);
  auto rhsAttr = mlir::dyn_cast<mlir::IntegerAttr>(rhs); // 再次检查类型是否正确
  if (!lhsAttr || !rhsAttr) {
    return {};
  }
  APInt lhsVal = lhsAttr.getValue();
  APInt rhsVal = rhsAttr.getValue();
  // 执行減法计算
  APInt sum = lhsVal - rhsVal;

  // 获取的是操作的结果类型，确保新常量的类型正确
  return IntegerAttr::get(getType(), sum);
}
} // namespace mlir::npu_mlir