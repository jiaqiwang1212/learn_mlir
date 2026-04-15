#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "mlir/IR/BuiltinTypes.h"
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.cpp.inc"
#include "npu-mlir/Dialect/Siren/IR/SirenOps.h"
#include "npu-mlir/Dialect/Siren/IR/SirenTypes.h"
#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir::npu_mlir;

#define GET_OP_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenOps.cpp.inc"

#define GET_TYPEDEF_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenTypes.cpp.inc"

#define GET_ATTRDEF_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.cpp.inc"

namespace mlir::npu_mlir {

void SirenDialect::initialize() {
  llvm::outs() << "SirenDialect initialized\n";

  addTypes<
#define GET_TYPEDEF_LIST
#include "npu-mlir/Dialect/Siren/IR/SirenTypes.cpp.inc"
      >();

  addOperations<
#define GET_OP_LIST
#include "npu-mlir/Dialect/Siren/IR/SirenOps.cpp.inc"
      >();

  addAttributes<
#define GET_ATTRDEF_LIST
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.cpp.inc"
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