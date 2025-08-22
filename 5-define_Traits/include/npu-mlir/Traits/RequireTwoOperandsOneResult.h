#ifndef REQUIRE_TWO_OPERANDS_ONE_RESULTS_H
#define REQUIRE_TWO_OPERANDS_ONE_RESULTS_H

#include "mlir/IR/OpDefinition.h"
namespace mlir::OpTrait::npu_mlir {
template <typename ConcreteOp>
class RequireTwoOperandsOneResult
    : public mlir::OpTrait::TraitBase<ConcreteOp, RequireTwoOperandsOneResult> {
public:
  static mlir::LogicalResult verifyTrait(mlir::Operation *op) {
    llvm::outs() << "RequireTwoOperandsOneResult::verifyTrait\n";
    if (op->getNumOperands() != 2) {
      return op->emitOpError() << "requires exactly two operands";
    }
    if (op->getNumResults() != 1) {
      return op->emitOpError() << "requires exactly one result";
    }
    return mlir::success();
  }
};
} // namespace mlir::OpTrait::npu_mlir

#endif // REQUIRE_TWO_OPERANDS_ONE_RESULTS_H