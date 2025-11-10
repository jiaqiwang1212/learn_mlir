#include "npu-mlir/Dialect/Siren/Transforms/TestPassOne.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "npu-mlir/Dialect/Siren/IR/SirenOps.h"

namespace mlir::npu_mlir {

#define GEN_PASS_DEF_TESTPASSONE
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

class TestPassOne : public impl::TestPassOneBase<TestPassOne> {
public:
  void runOnOperation() override {}
};
} // namespace mlir::npu_mlir