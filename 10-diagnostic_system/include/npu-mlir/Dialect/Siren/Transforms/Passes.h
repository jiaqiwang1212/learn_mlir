//===- Passes.h - Pass Entrypoints ------------------------------*- C++ -*-===//
//
// Ch-10: MLIR Diagnostic System Tutorial
//
// Registration entrypoint for all ch-10 diagnostic system passes.
//===----------------------------------------------------------------------===//

#ifndef NPU_MLIR_DIALECT_SIREN_TRANSFORMS_PASSES_H_
#define NPU_MLIR_DIALECT_SIREN_TRANSFORMS_PASSES_H_

#include "mlir/Pass/Pass.h"
#include "npu-mlir/Dialect/Siren/Transforms/DiagScopedPass.h"
#include "npu-mlir/Dialect/Siren/Transforms/DiagRegisteredPass.h"

namespace mlir {
namespace npu_mlir {

//===----------------------------------------------------------------------===//
// Registration
//===----------------------------------------------------------------------===//

/// Generate the code for registering passes.
#define GEN_PASS_REGISTRATION
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

} // namespace npu_mlir
} // namespace mlir

#endif // NPU_MLIR_DIALECT_SIREN_TRANSFORMS_PASSES_H_
