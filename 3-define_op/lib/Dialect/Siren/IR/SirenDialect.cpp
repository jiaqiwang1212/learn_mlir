#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.cpp.inc"
#include "llvm/Support/raw_ostream.h"

#define GET_OP_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenOps.cpp.inc"

namespace mlir::npu_mlir {

void SirenDialect::initialize() {
  llvm::outs() << "SirenDialect initialized\n";
  addOperations<
#define GET_OP_LIST
#include "npu-mlir/Dialect/Siren/IR/SirenOps.cpp.inc"
      >();
}

void SirenDialect::print_name() { llvm::outs() << "SirenDialect\n"; }

SirenDialect::~SirenDialect() = default;
} // namespace mlir::npu_mlir