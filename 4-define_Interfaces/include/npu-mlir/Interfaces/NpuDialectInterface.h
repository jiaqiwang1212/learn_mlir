#ifndef NPU_MLIR_DIALECT_INTERFACES_H
#define NPU_MLIR_DIALECT_INTERFACES_H
#include "mlir/IR/DialectInterface.h"
#include "mlir/IR/OpDefinition.h" // 视需要：Operation/Pattern等
#include "mlir/IR/PatternMatch.h"

namespace mlir::npu_mlir {
class NpuDialectInterface
    : public mlir::DialectInterface::Base<NpuDialectInterface> {
public:
  using Base::Base; // 继承构造函数：NpuDialectInterface(Dialect*)
  virtual ~NpuDialectInterface() = default;

  explicit NpuDialectInterface(::mlir::Dialect *dialect) : Base(dialect) {}

  /// 钩子举例：让每个方言贡献一批 lowering pattern
  virtual void printSomeInfo() const {
    llvm::outs() << "Default NpuDialectInterface implementation\n";
  };
  // 默认实现：不做任何事

  /// 钩子举例：判断某个 op 是否在该方言的“合法集合”里
  virtual bool isLegalOp() const { return true; }
};
} // namespace mlir::npu_mlir

#endif // NPU_MLIR_DIALECT_INTERFACES_H