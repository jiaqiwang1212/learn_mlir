//===- CustomDiagnosticHandler.h - Shared diagnostic handler ----*- C++ -*-===//
//
// Ch-10: MLIR Diagnostic System Tutorial
//
// A reusable handler callable that prints diagnostics as:
//   [CH10-DIAG] <SEV>: <msg>
// Used by both DiagScopedPass and DiagRegisteredPass to demonstrate the
// same handler class working with different registration APIs.
//===----------------------------------------------------------------------===//

#ifndef NPU_MLIR_DIALECT_SIREN_TRANSFORMS_CUSTOMDIAGNOSTICHANDLER_H_
#define NPU_MLIR_DIALECT_SIREN_TRANSFORMS_CUSTOMDIAGNOSTICHANDLER_H_

#include "mlir/IR/Diagnostics.h"

namespace mlir::npu_mlir {

/// A diagnostic handler callable that formats diagnostics as:
///   [CH10-DIAG] <SEV>: <msg>
/// where <SEV> is one of: ERROR, WARNING, REMARK, NOTE.
///
/// When suppressWarnings=true, Warning-severity diagnostics are consumed
/// silently (returns success() without printing).
///
/// Always returns success() to mark the diagnostic as handled — this prevents
/// the default SourceMgr printer from duplicating the output.
///
/// NOTE: emitError's "Error" is a diagnostic severity label, NOT a pass exit
/// code. The pass exits cleanly because (a) this handler returns success()
/// consuming the diagnostic, and (b) the pass never calls signalPassFailure().
/// Exit code is driven solely by signalPassFailure() — see Pass.cpp.
struct CustomDiagnosticHandler {
  bool suppressWarnings = false;
  LogicalResult operator()(Diagnostic &diag);
};

} // namespace mlir::npu_mlir

#endif // NPU_MLIR_DIALECT_SIREN_TRANSFORMS_CUSTOMDIAGNOSTICHANDLER_H_
