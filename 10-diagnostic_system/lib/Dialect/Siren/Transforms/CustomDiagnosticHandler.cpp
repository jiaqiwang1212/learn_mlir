//===- CustomDiagnosticHandler.cpp - Shared diagnostic formatter ----------===//
//
// Ch-10: MLIR Diagnostic System Tutorial
//
// Implements the CustomDiagnosticHandler callable used by both
// DiagScopedPass and DiagRegisteredPass. Formats diagnostics as:
//   [CH10-DIAG] <SEV>: <msg>
// and walks attached notes to print them with the NOTE label.
//===----------------------------------------------------------------------===//

#include "npu-mlir/Dialect/Siren/Transforms/CustomDiagnosticHandler.h"
#include "llvm/Support/raw_ostream.h"

namespace mlir::npu_mlir {

LogicalResult CustomDiagnosticHandler::operator()(Diagnostic &diag) {
  // DiagnosticEngine asserts that Note-severity diagnostics are never emitted
  // as standalone top-level diagnostics (see Diagnostics.h:462-464).
  // Notes only arrive as children via diag.getNotes() on a parent diagnostic.
  // If this assert fires, someone called engine.emit(loc, Note) directly —
  // which is a bug: use InFlightDiagnostic::attachNote() instead.
  assert(diag.getSeverity() != DiagnosticSeverity::Note &&
         "Top-level Note diagnostics should never reach a handler directly");

  // Silently consume warnings when suppression is enabled.
  if (suppressWarnings && diag.getSeverity() == DiagnosticSeverity::Warning)
    return success();

  llvm::StringRef label;
  switch (diag.getSeverity()) {
  case DiagnosticSeverity::Error:
    label = "ERROR";
    break;
  case DiagnosticSeverity::Warning:
    label = "WARNING";
    break;
  case DiagnosticSeverity::Remark:
    label = "REMARK";
    break;
  case DiagnosticSeverity::Note:
    llvm_unreachable("Note diagnostics cannot reach a handler as top-level; "
                     "use attachNote() on a parent InFlightDiagnostic");
  }

  // Print the primary diagnostic with our custom prefix.
  llvm::errs() << "[CH10-DIAG] " << label << ": " << diag.str() << "\n";

  // Walk any notes attached to this diagnostic and print each one.
  for (const Diagnostic &note : diag.getNotes())
    llvm::errs() << "[CH10-DIAG] NOTE: " << note.str() << "\n";

  // Return success() to mark this diagnostic as handled.
  // This prevents the default SourceMgr printer from printing a duplicate.
  return success();
}

} // namespace mlir::npu_mlir
