//===- DiagRegisteredPass.h - registerHandler demo --------------*- C++ -*-===//
//
// Ch-10: MLIR Diagnostic System Tutorial
//
// Pass declaration for DiagRegisteredPass (--diag-registered).
// Demonstrates manual handler registration via DiagnosticEngine::registerHandler
// with explicit eraseHandler(id) lifecycle management.
//===----------------------------------------------------------------------===//

#ifndef NPU_MLIR_DIALECT_SIREN_TRANSFORMS_DIAGREGISTEREDPASS_H_
#define NPU_MLIR_DIALECT_SIREN_TRANSFORMS_DIAGREGISTEREDPASS_H_

#include "mlir/Pass/Pass.h"

namespace mlir::npu_mlir {

#define GEN_PASS_DECL_DIAGREGISTERED
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

} // namespace mlir::npu_mlir

#endif // NPU_MLIR_DIALECT_SIREN_TRANSFORMS_DIAGREGISTEREDPASS_H_
