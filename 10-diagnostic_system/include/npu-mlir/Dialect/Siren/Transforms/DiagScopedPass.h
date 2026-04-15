//===- DiagScopedPass.h - ScopedDiagnosticHandler demo ----------*- C++ -*-===//
//
// Ch-10: MLIR Diagnostic System Tutorial
//
// Pass declaration for DiagScopedPass (--diag-scoped).
// Demonstrates RAII-based handler registration via ScopedDiagnosticHandler.
//===----------------------------------------------------------------------===//

#ifndef NPU_MLIR_DIALECT_SIREN_TRANSFORMS_DIAGSCOPEDPASS_H_
#define NPU_MLIR_DIALECT_SIREN_TRANSFORMS_DIAGSCOPEDPASS_H_

#include "mlir/Pass/Pass.h"

namespace mlir::npu_mlir {

#define GEN_PASS_DECL_DIAGSCOPED
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

} // namespace mlir::npu_mlir

#endif // NPU_MLIR_DIALECT_SIREN_TRANSFORMS_DIAGSCOPEDPASS_H_
