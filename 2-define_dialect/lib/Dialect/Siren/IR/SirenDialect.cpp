#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.cpp.inc"
#include "llvm/Support/raw_ostream.h"

namespace mlir::npu_mlir {

void SirenDialect::initialize() {
  llvm::outs() << "SirenDialect initialized\n";
}
} // namespace mlir::npu_mlir