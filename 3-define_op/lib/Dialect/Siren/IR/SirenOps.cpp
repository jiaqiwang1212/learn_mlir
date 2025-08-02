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
} // namespace mlir::npu_mlir