//===- diag_demo.cpp - Standalone MLIR Diagnostic System demo -------------===//
//
// Ch-10: MLIR Diagnostic System Tutorial — standalone executable
//
// This demo does NOT use mlir-opt or MlirOptMain. It builds a tiny IR
// programmatically, then exercises every major diagnostic API:
//
//   1. mlir::emitError/Remark/Warning(Location) — free functions
//   2. Operation::emitError/Remark/Warning()    — op-method forms
//   3. Operation::emitOpError()                 — prefixes 'op-name' op
//   4. attachNote()         — chain a Note onto an InFlightDiagnostic
//   5. lambda handler       — registerHandler with fall-through
//   6. ScopedDiagnosticHandler — RAII handler (auto-erases on scope exit)
//   7. eraseHandler         — manual lifecycle + after-erase fallthrough
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Diagnostics.h"
#include "mlir/IR/MLIRContext.h"
#include "llvm/Support/raw_ostream.h"

static void printSection(llvm::StringRef title) {
  llvm::errs() << "\n──────────────────────────────────────────\n"
               << "  " << title << "\n"
               << "──────────────────────────────────────────\n";
}

int main() {
  // ── Context + dialect setup ───────────────────────────────────────────────
  mlir::MLIRContext ctx;
  ctx.loadDialect<mlir::func::FuncDialect, mlir::arith::ArithDialect>();

  mlir::OpBuilder builder(&ctx);
  mlir::Location loc = builder.getUnknownLoc();

  // ── Build a tiny IR: func @demo() { %c = arith.constant 42 : i32; return }
  //
  // This gives us real Operation* objects so op->emitX() has a concrete op
  // to attach location and IR context to.
  auto module = mlir::ModuleOp::create(builder, loc);
  builder.setInsertionPointToEnd(module.getBody());

  auto funcType = builder.getFunctionType({}, {});
  auto funcOp = mlir::func::FuncOp::create(builder, loc, "demo", funcType);
  auto *block = funcOp.addEntryBlock();
  builder.setInsertionPointToStart(block);

  auto i32      = builder.getI32Type();
  auto intAttr  = builder.getIntegerAttr(i32, 42);
  auto constOp  = mlir::arith::ConstantOp::create(builder, loc, intAttr);
  mlir::func::ReturnOp::create(builder, loc);

  mlir::DiagnosticEngine &engine = ctx.getDiagEngine();

  // ── 1. mlir::emitX(Location) — free functions in mlir namespace ─────────
  //
  // Useful when you have a Location but not an Operation*.
  // Routes through DiagnosticEngine identically to the op-method forms.
  printSection("1. mlir::emitError/Remark/Warning(Location)  [free functions]");

  mlir::emitError(loc,   "free-fn error: invalid operand location");
  mlir::emitWarning(loc, "free-fn warning: suboptimal IR pattern");
  mlir::emitRemark(loc,  "free-fn remark: analysis observation");

  // ── 2. op->emitX() — methods that use the op's attached Location ─────────
  //
  // Equivalent to mlir::emitX(op->getLoc(), msg).
  // The op carries its Location, so you don't have to pass it explicitly.
  printSection("2. Operation::emitError/Remark/Warning()  [op-method forms]");

  constOp->emitError("error: constant value cannot be reassigned");
  constOp->emitWarning("warning: consider folding this at compile time");
  constOp->emitRemark("remark: constant found during analysis");

  // ── 3. op->emitOpError() — prefixes message with "'op-name' op " ─────────
  //
  // Unlike emitError(), emitOpError() prepends the dialect op name so the
  // message reads:  error: 'arith.constant' op <your message>
  // This is the standard style used by verifiers and type checkers inside
  // MLIR itself (e.g. in Op::verify() implementations).
  //
  // Note: emitOpError() only exists for Error severity — there is no
  // emitOpWarning() or emitOpRemark().
  printSection("3. Operation::emitOpError()  [prefixes 'op-name' op]");

  constOp->emitOpError("result type mismatch: expected i64 but got i32");
  funcOp->emitOpError("function body is empty");

  // ── 3. attachNote() — chain a Note onto an InFlightDiagnostic ────────────
  //
  // Notes CANNOT be emitted standalone: engine.emit(loc, Note) asserts.
  // They must be attached via .attachNote() on a parent diagnostic.
  // The parent InFlightDiagnostic destructs at ';', dispatching both
  // the error and its note together to the handler.
  printSection("4. attachNote() — chain a Note onto an Error");

  constOp->emitError("type mismatch on constant operand")
      .attachNote(constOp->getLoc())
          << "note: operand has type " << constOp.getType();

  // ── 4. lambda handler — counts diagnostics, falls through ────────────────
  //
  // registerHandler() pushes a handler onto the engine's stack.
  // Return mlir::failure() to fall through to the next handler (default).
  // Return mlir::success() to consume the diagnostic (no further output).
  printSection("5. lambda handler — count diagnostics, fall through to default");

  int diagCount = 0;
  auto handlerID = engine.registerHandler(
      [&diagCount](mlir::Diagnostic &d) -> mlir::LogicalResult {
        llvm::errs() << "[COUNTER #" << ++diagCount << "]"
                     << " sev=" << (int)d.getSeverity()
                     << " | " << d.str() << "\n";
        return mlir::failure(); // fall through → default handler also prints
      });

  constOp->emitError("counted error A");
  constOp->emitWarning("counted warning B");
  constOp->emitRemark("counted remark C");

  engine.eraseHandler(handlerID);
  llvm::errs() << "[COUNTER] handler erased. total intercepted: "
               << diagCount << "\n";

  // ── 5. ScopedDiagnosticHandler — RAII, auto-erases on scope exit ─────────
  //
  // ScopedDiagnosticHandler is a RAII wrapper over registerHandler +
  // eraseHandler. The handler is automatically removed when 'scoped' goes
  // out of scope at the closing brace — no manual eraseHandler needed.
  printSection("6. ScopedDiagnosticHandler (RAII)");
  {
    mlir::ScopedDiagnosticHandler scoped(
        &ctx, [](mlir::Diagnostic &d) -> mlir::LogicalResult {
          llvm::errs() << "[SCOPED] " << d.str() << "\n";
          return mlir::success(); // consume — default handler won't see this
        });

    constOp->emitError("inside scope: intercepted by SCOPED handler");
    constOp->emitRemark("inside scope: this too");

    // 'scoped' destructs here → eraseHandler() called automatically
  }
  constOp->emitRemark("outside scope: back to default handler (no [SCOPED] prefix)");

  // ── 6. Manual lifecycle: registerHandler → eraseHandler → prove fallthrough
  //
  // After eraseHandler(), the next diagnostic goes to the default handler
  // (no custom prefix). This makes the lifecycle boundary visible.
  printSection("7. registerHandler → eraseHandler → after-erase fallthrough");

  auto manualID = engine.registerHandler(
      [](mlir::Diagnostic &d) -> mlir::LogicalResult {
        llvm::errs() << "[MANUAL] " << d.str() << "\n";
        return mlir::success();
      });

  constOp->emitRemark("before erase: caught by [MANUAL] handler");
  engine.eraseHandler(manualID);
  constOp->emitRemark("after erase: no [MANUAL] prefix — default handler owns this");

  return 0;
}
