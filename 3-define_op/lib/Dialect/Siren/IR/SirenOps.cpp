#include "npu-mlir/Dialect/Siren/IR/SirenOps.h"
#include "llvm/Support/raw_ostream.h"

namespace mlir::npu_mlir {
void AddOp::printOpName() { llvm::outs() << "siren.add\n"; }
} // namespace mlir::npu_mlir