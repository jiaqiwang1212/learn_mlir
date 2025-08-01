#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.cpp.inc"
#include "llvm/Support/raw_ostream.h"

namespace mlir::npu_mlir {

void SirenDialect::initialize() {
  llvm::outs() << "SirenDialect initialized\n";
}

void SirenDialect::print_name() { llvm::outs() << "SirenDialect\n"; }

SirenDialect::~SirenDialect() = default;
} // namespace mlir::npu_mlir