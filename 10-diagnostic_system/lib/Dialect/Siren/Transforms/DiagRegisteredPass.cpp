//===- DiagRegisteredPass.cpp - registerHandler lifecycle demo ------------===//
//
// Ch-10: MLIR Diagnostic System Tutorial
//
// Demonstrates DiagnosticEngine::registerHandler (manual lifecycle):
//   1. Calls engine.registerHandler() and captures the returned HandlerID
//   2. Emits all 4 severities using both op->emitX and mlir::emitX(loc) forms
//   3. Calls engine.eraseHandler(id) to explicitly close the lifecycle
//   4. Emits one more diagnostic after erasure — goes to default SourceMgr
//      handler (no [CH10-DIAG] prefix), proving deregistration happened
//
// The contrast with ScopedDiagnosticHandler:
//   ScopedDiagnosticHandler::setHandler does exactly:
//     diagEngine.eraseHandler(handlerID);            // clear old
//     handlerID = diagEngine.registerHandler(fn);    // register new
//   ...and the destructor calls diagEngine.eraseHandler(handlerID).
//   The manual approach below makes this lifecycle explicit.
//===----------------------------------------------------------------------===//

#include "npu-mlir/Dialect/Siren/Transforms/DiagRegisteredPass.h"
#include "npu-mlir/Dialect/Siren/Transforms/CustomDiagnosticHandler.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Diagnostics.h"

namespace mlir::npu_mlir {

// Include the TableGen-generated pass base class definition.
#define GEN_PASS_DEF_DIAGREGISTERED
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

//===----------------------------------------------------------------------===//
// DiagRegisteredPass
//===----------------------------------------------------------------------===//

class DiagRegisteredPass : public impl::DiagRegisteredBase<DiagRegisteredPass> {
public:
  void runOnOperation() override {
    func::FuncOp func = getOperation();

    // -----------------------------------------------------------------------
    // Step 1: Manual handler registration.
    //
    // registerHandler returns a HandlerID that must be passed to eraseHandler
    // to remove the handler. This is the raw API that ScopedDiagnosticHandler
    // wraps with RAII (see mlir/IR/Diagnostics.h:536-542).
    // -----------------------------------------------------------------------
    DiagnosticEngine &engine = getContext().getDiagEngine();
    DiagnosticEngine::HandlerID id =
        engine.registerHandler(CustomDiagnosticHandler{/*suppressWarnings=*/false});

    // -----------------------------------------------------------------------
    // Step 2: Emit all 4 severity levels from the first func.return op.
    //
    // Demonstrates two emission forms:
    //   Form A: op->emitX(msg)         — method on Operation
    //   Form B: mlir::emitX(loc, msg)  — free function taking Location
    // Both forms route through DiagnosticEngine and reach our handler.
    // -----------------------------------------------------------------------
    func.walk([&](func::ReturnOp op) -> WalkResult {
      // Form A: op->emitError with an attached Note.
      // NOTE: engine.emit(loc, DiagnosticSeverity::Note) is FORBIDDEN —
      // mlir/IR/Diagnostics.h:462 asserts "notes should not be emitted directly".
      // Notes must be attached via .attachNote() on a parent InFlightDiagnostic.
      op->emitError("registered: via op->emitError")
          .attachNote(op->getLoc()) << "registered: direct-engine note";

      // Form B: free-function mlir::emitWarning(loc, msg)
      mlir::emitWarning(op->getLoc(), "registered: via mlir::emitWarning(loc, msg)");

      // Form B: free-function mlir::emitRemark(loc, msg)
      mlir::emitRemark(op->getLoc(), "registered: via free function form");

      return WalkResult::interrupt(); // process only the first return op
    });

    // -----------------------------------------------------------------------
    // Step 3: Explicit lifecycle close.
    //
    // eraseHandler(id) removes our custom handler from the engine.
    // After this point, diagnostics go to the next handler in the stack
    // (the default SourceMgr handler installed by MlirOptMain).
    // -----------------------------------------------------------------------
    engine.eraseHandler(id);

    // -----------------------------------------------------------------------
    // Step 4: Emit after erasure — proves handler was removed.
    //
    // This remark is printed by the default SourceMgr handler with format:
    //   <filename>:<line>:<col>: remark: after-erase: default handler owns this
    // No [CH10-DIAG] prefix — our handler is gone.
    // -----------------------------------------------------------------------
    func.walk([&](func::ReturnOp op) -> WalkResult {
      mlir::emitRemark(op->getLoc(), "after-erase: default handler owns this");
      return WalkResult::interrupt();
    });
  }
};

} // namespace mlir::npu_mlir
