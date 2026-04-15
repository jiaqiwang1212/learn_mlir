#ifndef NPU_MLIR_CH10_DIAGNOSTIC_PASSES
#define NPU_MLIR_CH10_DIAGNOSTIC_PASSES

include "mlir/Pass/PassBase.td"

// Demonstrates MLIR's ScopedDiagnosticHandler (RAII): registers a custom
// handler that prints [CH10-DIAG] <SEV>: <msg> and suppresses warnings.
def DiagScoped : Pass<"diag-scoped", "mlir::func::FuncOp"> {
  let summary = "Scoped diagnostic handler demo (RAII, filters warnings)";
  let description = [{
    Demonstrates ScopedDiagnosticHandler by registering a custom handler that:
      - Prints all diagnostics as [CH10-DIAG] <SEV>: <msg>
      - Suppresses Warning-severity diagnostics silently
      - Auto-deregisters on scope exit (RAII)
    Emits Error (with attached Note), Warning (suppressed), and Remark from
    the first arith.constant in the function.
  }];
  let dependentDialects = ["mlir::func::FuncDialect",
                           "mlir::arith::ArithDialect"];
}

// Demonstrates DiagnosticEngine::registerHandler with explicit eraseHandler,
// showing both op->emitX and mlir::emitX(loc, msg) emission forms.
def DiagRegistered : Pass<"diag-registered", "mlir::func::FuncOp"> {
  let summary = "Raw DiagnosticEngine::registerHandler demo with explicit eraseHandler";
  let description = [{
    Demonstrates DiagnosticEngine::registerHandler (manual lifecycle):
      - Registers a handler and captures its HandlerID
      - Emits all 4 severities using both op->emitX and mlir::emitX(loc) forms
      - Calls eraseHandler(id) explicitly to close the lifecycle
      - Emits one final remark after erasure (goes to default SourceMgr handler)
    Shows that ScopedDiagnosticHandler is RAII sugar over registerHandler +
    eraseHandler (see mlir/IR/Diagnostics.h ScopedDiagnosticHandler::setHandler).
  }];
  let dependentDialects = ["mlir::func::FuncDialect",
                           "mlir::arith::ArithDialect"];
}

#endif // NPU_MLIR_CH10_DIAGNOSTIC_PASSES
