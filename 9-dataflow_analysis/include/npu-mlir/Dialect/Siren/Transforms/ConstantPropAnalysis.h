#ifndef NPU_MLIR_DIALECT_SIREN_TRANSFORMS_CONSTANTPROPANALYSIS_H_
#define NPU_MLIR_DIALECT_SIREN_TRANSFORMS_CONSTANTPROPANALYSIS_H_

#include "mlir/Pass/Pass.h"

namespace mlir::npu_mlir {

#define GEN_PASS_DECL_CONSTANTPROPANALYSIS
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

} // namespace mlir::npu_mlir

#endif // NPU_MLIR_DIALECT_SIREN_TRANSFORMS_CONSTANTPROPANALYSIS_H_
