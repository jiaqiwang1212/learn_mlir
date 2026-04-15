//===- DiagScopedPass.cpp - ScopedDiagnosticHandler demo ------------------===//
//
// Ch-10: MLIR Diagnostic System Tutorial
//
// Demonstrates ScopedDiagnosticHandler (RAII):
//   1. Constructs a ScopedDiagnosticHandler at the top of runOnOperation()
//   2. The handler auto-deregisters when it goes out of scope at return
//   3. All diagnostics emitted inside the scope are intercepted by our handler
//
// Key concepts:
//   - ScopedDiagnosticHandler = RAII wrapper over registerHandler + eraseHandler
//   - suppressWarnings=true: Warning diagnostics are silently consumed
//   - InFlightDiagnostic chaining: .attachNote() adds a Note child diagnostic
//   - emitError is a severity label, NOT a pass exit trigger
//     (only signalPassFailure() causes pass failure)
//===----------------------------------------------------------------------===//

#include "npu-mlir/Dialect/Siren/Transforms/DiagScopedPass.h"
#include "npu-mlir/Dialect/Siren/Transforms/CustomDiagnosticHandler.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Diagnostics.h"

namespace mlir::npu_mlir {

// Include the TableGen-generated pass base class definition.
#define GEN_PASS_DEF_DIAGSCOPED
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

//===----------------------------------------------------------------------===//
// DiagScopedPass
//===----------------------------------------------------------------------===//

class DiagScopedPass : public impl::DiagScopedBase<DiagScopedPass> {
public:
  void runOnOperation() override {
    func::FuncOp func = getOperation();

    // -----------------------------------------------------------------------
    // Step 1: Install handler via RAII.
    //
    // ScopedDiagnosticHandler registers our callable with the context's
    // DiagnosticEngine and automatically erases it when 'scoped' destructs
    // at the closing brace of runOnOperation().
    //
    // Internally this is exactly:
    //   HandlerID id = ctx->getDiagEngine().registerHandler(handler);
    //   // ... at scope exit:
    //   ctx->getDiagEngine().eraseHandler(id);
    // (see mlir/IR/Diagnostics.h ScopedDiagnosticHandler::setHandler)
    // -----------------------------------------------------------------------
    ScopedDiagnosticHandler scoped(
        &getContext(), CustomDiagnosticHandler{/*suppressWarnings=*/true});

    // -----------------------------------------------------------------------
    // Step 2: Emit all four severity levels from the first arith.constant.
    //
    // Emit once total (not once per constant) for deterministic FileCheck
    // output. WalkResult::interrupt() stops the walk after the first op.
    // -----------------------------------------------------------------------
    bool emitted = false;
    func.walk([&](arith::ConstantOp op) -> WalkResult {
      if (emitted)
        return WalkResult::interrupt();
      emitted = true;

      // ERROR + attached NOTE (InFlightDiagnostic chaining).
      // The InFlightDiagnostic temporary destructs at ';', calling report()
      // which dispatches to the handler with the note already attached.
      op->emitError("scoped: value is constant")
          .attachNote(op->getLoc()) << "note: attached context";

      // WARNING — silently consumed by CustomDiagnosticHandler{suppressWarnings=true}.
      // Will NOT appear in the output.
      op->emitWarning("scoped: this warning will be suppressed");

      // REMARK — survives the filter and appears in the output.
      op->emitRemark("scoped: remark survives filter");

      return WalkResult::advance();
    });

    // Note: we do NOT call signalPassFailure() here.
    // emitError() sets a diagnostic severity label — it does NOT fail the pass.
    // The pass exits cleanly (exit code 0) because only signalPassFailure()
    // drives the pass manager's failure state.
  }
};

} // namespace mlir::npu_mlir
