#ifndef NPU_MLIR_DIALECT_SIREN_TESTPASSONE_H_
#define NPU_MLIR_DIALECT_SIREN_TESTPASSONE_H_

#include "mlir/Pass/Pass.h"
namespace mlir::npu_mlir {

// 这里采用的方案是将每一个Pass单独放在一个头文件中，然后在Passes.h中包含这些头文件。
// 当然也可以将所有的Pass的Declare都放在Passes.h中。
#define GEN_PASS_DECL_TESTPASSONE
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"
} // namespace mlir::npu_mlir

#endif // NPU_MLIR_DIALECT_SIREN_TESTPASSONE_H_