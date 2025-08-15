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

mlir::LogicalResult
SirenDialect::verifyOperationAttribute(::mlir::Operation *op,
                                       ::mlir::NamedAttribute attribute) {
  // 在这里可以添加对discardable attribute的验证逻辑
  llvm::outs() << "discardable attribute verify\n";
  if (attribute.getName() == "siren.debug_level") {
    auto debugLevelAttr =
        mlir::dyn_cast<mlir::StringAttr>(attribute.getValue());
    if (!debugLevelAttr) {
      op->emitError("debug_level must be a StringAttr");
      return mlir::failure();
    }
    llvm::outs() << "siren.debug_level: " << debugLevelAttr.getValue() << "\n";
  } else {
    op->emitError("Unknown attribute: ") << attribute.getName();
    return mlir::failure();
  }
  return mlir::success();
}

Operation *SirenDialect::materializeConstant(OpBuilder &builder,
                                             Attribute value, Type type,
                                             Location loc) {
  return builder.create<arith::ConstantOp>(loc, type,
                                           mlir::cast<mlir::TypedAttr>(value));
}
} // namespace mlir::npu_mlir