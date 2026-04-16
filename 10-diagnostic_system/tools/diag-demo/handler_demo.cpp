//===- handler_demo.cpp - All four MLIR diagnostic handler types ----------===//
//
// Ch-10: MLIR Diagnostic System Tutorial — handler showcase
//
// Demonstrates all four built-in handler types in mlir/IR/Diagnostics.h:
//
//   1. ScopedDiagnosticHandler        — RAII lambda handler
//   2. SourceMgrDiagnosticHandler     — file:line:col + source-caret output
//   3. SourceMgrDiagnosticVerifierHandler — expected-* annotation matching
//   4. ParallelDiagnosticHandler      — thread-safe deterministic ordering
//
// Each section uses its own C++ scope so that RAII handlers deregister before
// the next section begins — no handler overlap.
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/AsmState.h"       // ParserConfig
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Diagnostics.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Parser/Parser.h"     // parseSourceFile
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

static void printSection(llvm::StringRef title) {
  llvm::errs() << "\n══════════════════════════════════════════\n"
               << "  " << title << "\n"
               << "══════════════════════════════════════════\n";
}

// ── MLIR source used by Section 2 (SourceMgrDiagnosticHandler) ─────────────
//
// Valid MLIR — parses cleanly.  After parsing we walk the IR and emit an error
// at the arith.addi op's location.  SourceMgrDiagnosticHandler resolves that
// FileLineColLoc back into this string and prints the source caret.
static constexpr const char kSrcMgrSrc[] =
    R"mlir(func.func @add(%a: i32, %b: i32) -> i32 {
  %sum = arith.addi %a, %b : i32
  return %sum : i32
}
)mlir";

// ── MLIR source used by Section 3 (SourceMgrDiagnosticVerifierHandler) ──────
//
// The "// expected-remark @+1 {{...}}" annotation tells the verifier handler:
// "I expect a remark diagnostic on the NEXT source line whose message contains
// the given substring."  After parsing we walk and emit that remark manually.
// verifyHandler.verify() returns success iff all expected-* were matched and
// no unexpected diagnostics were left unmatched.
static constexpr const char kVerifySrc[] =
    R"mlir(func.func @verify_demo() {
  // expected-remark @+1 {{verifier: checking return op}}
  return
}
)mlir";

int main() {
  mlir::MLIRContext ctx;
  ctx.loadDialect<mlir::func::FuncDialect, mlir::arith::ArithDialect>();

  // ──────────────────────────────────────────────────────────────────────────
  // 1. ScopedDiagnosticHandler
  //
  // RAII wrapper over registerHandler + eraseHandler.
  // Accepts any callable (lambda, functor, function pointer).
  // Returns success() to consume; failure() to fall through to default.
  // ──────────────────────────────────────────────────────────────────────────
  printSection("1. ScopedDiagnosticHandler  (RAII lambda)");
  {
    mlir::OpBuilder builder(&ctx);
    mlir::Location loc = builder.getUnknownLoc();

    mlir::ScopedDiagnosticHandler scoped(
        &ctx, [](mlir::Diagnostic &d) -> mlir::LogicalResult {
          llvm::errs() << "[SCOPED] sev=" << (int)d.getSeverity()
                       << "  msg=" << d.str() << "\n";
          return mlir::success(); // consume — don't fall through to default
        });

    mlir::emitError(loc,   "scoped catches this error");
    mlir::emitWarning(loc, "scoped catches this warning");
    mlir::emitRemark(loc,  "scoped catches this remark");

    // 'scoped' destructs here → eraseHandler() called automatically.
    // Diagnostics after this point go to the default handler.
  }

  // ──────────────────────────────────────────────────────────────────────────
  // 2. SourceMgrDiagnosticHandler
  //
  // Wraps llvm::SourceMgr.  When a diagnostic fires with a FileLineColLoc,
  // this handler resolves it against the SourceMgr buffer and prints:
  //
  //   <file>:<line>:<col>: <severity>: <message>
  //     <source line>
  //     ^~~~~~
  //
  // This is what mlir-opt uses for its normal diagnostic output.
  // ──────────────────────────────────────────────────────────────────────────
  printSection("2. SourceMgrDiagnosticHandler  (file:line:col + source caret)");
  {
    // Add our source buffer to the SourceMgr under the name "<add.mlir>".
    llvm::SourceMgr srcMgr;
    srcMgr.AddNewSourceBuffer(
        llvm::MemoryBuffer::getMemBuffer(kSrcMgrSrc, "<add.mlir>"),
        llvm::SMLoc());

    // Install the handler — it registers itself on 'ctx' via ScopedDiagnosticHandler.
    mlir::SourceMgrDiagnosticHandler smHandler(srcMgr, &ctx);

    // Parse from the SourceMgr so that ops get FileLineColLoc("<add.mlir>", ...).
    mlir::ParserConfig config(&ctx);
    auto module = mlir::parseSourceFile<mlir::ModuleOp>(srcMgr, config);
    if (!module) {
      llvm::errs() << "parse failed\n";
      return 1;
    }

    // Emit an error at the arith.addi op.  The handler resolves its location
    // and prints the source line with a caret beneath it.
    module->walk([](mlir::arith::AddIOp op) {
      op->emitError("result type i32 is narrower than expected i64");
      return mlir::WalkResult::interrupt();
    });

    // smHandler destructs → handler removed.
  }

  // ──────────────────────────────────────────────────────────────────────────
  // 3. SourceMgrDiagnosticVerifierHandler
  //
  // Extends SourceMgrDiagnosticHandler.  Instead of printing diagnostics it
  // matches them against "// expected-<sev> @<offset> {{pattern}}" annotations
  // embedded in the source text.  Call verify() after all diagnostics have
  // been emitted:
  //   • success — every expected-* was matched, no unexpected leftovers
  //   • failure — unmatched expectations or unexpected diagnostics
  //
  // This is how `mlir-opt --verify-diagnostics` works.
  //
  // ⚠ LIFETIME NOTE (MLIR 22 bug-workaround):
  //   SourceMgrDiagnosticVerifierHandler::registerInContext() registers a
  //   second handler whose HandlerID is never saved, so it is never explicitly
  //   erased.  The lambda captures 'this' by reference; after the object
  //   destructs, that handler becomes a dangling callback inside the engine.
  //   If any subsequent code emits diagnostics through the same engine, it
  //   will invoke the zombie handler and crash (UAF on impl→ExpectedDiag).
  //
  //   Fix: use a dedicated MLIRContext for the verifier so that both the
  //   verifier object and its leaked handler live (and die) in an isolated
  //   DiagnosticEngine that nobody else touches after this scope.
  // ──────────────────────────────────────────────────────────────────────────
  printSection("3. SourceMgrDiagnosticVerifierHandler  (expected-* matching)");
  {
    // Isolated context — the leaked handler stays here and is harmless.
    mlir::MLIRContext verifyCtx;
    verifyCtx.loadDialect<mlir::func::FuncDialect>();

    llvm::SourceMgr verifySrcMgr;
    verifySrcMgr.AddNewSourceBuffer(
        llvm::MemoryBuffer::getMemBuffer(kVerifySrc, "<verify_demo.mlir>"),
        llvm::SMLoc());

    mlir::SourceMgrDiagnosticVerifierHandler verifyHandler(verifySrcMgr, &verifyCtx);

    // Parse with verifyAfterParse=false so the only diagnostic is the one we
    // emit manually below (prevents surprise verifier errors from the IR).
    mlir::ParserConfig config(&verifyCtx, /*verifyAfterParse=*/false);
    auto module = mlir::parseSourceFile<mlir::ModuleOp>(verifySrcMgr, config);
    if (!module) {
      llvm::errs() << "parse failed\n";
      return 1;
    }

    // Emit the remark that kVerifySrc's "expected-remark @+1" annotation
    // expects on the `return` op's line.
    //
    // Use the free-function form mlir::emitRemark(loc, msg) — NOT op->emitRemark().
    // The op-method form auto-attaches a "see current operation: ..." note that
    // the verifier would flag as an unexpected diagnostic and fail.
    module->walk([](mlir::func::ReturnOp op) {
      mlir::emitRemark(op->getLoc(), "verifier: checking return op");
      return mlir::WalkResult::interrupt();
    });

    // verify() checks that every expected-* annotation was matched.
    mlir::LogicalResult result = verifyHandler.verify();
    llvm::errs() << "[VERIFIER] verify() => "
                 << (mlir::succeeded(result) ? "success — all expected-* matched"
                                             : "FAILED — unmatched annotations")
                 << "\n";

    // verifyHandler and verifyCtx destruct here.
    // The leaked registerInContext handler is part of verifyCtx's engine;
    // destroying verifyCtx tears down the engine and all its handlers safely.
  }

  // ──────────────────────────────────────────────────────────────────────────
  // 4. ParallelDiagnosticHandler
  //
  // Thread-safe handler that buffers diagnostics per-thread and flushes them
  // in deterministic order (by orderID) when the handler destructs.
  //
  // Use case: parallel pass execution where diagnostics from N worker threads
  // should appear in the same order as single-threaded execution would produce.
  //
  // Protocol:
  //   1. Call setOrderIDForThread(i) in each thread before emitting.
  //   2. Emit diagnostics normally.
  //   3. Call eraseOrderIDForThread() when the thread is done.
  //   4. ParallelDiagnosticHandler destructs → flushes in orderID order to
  //      whatever handler is next in the stack (default handler here).
  // ──────────────────────────────────────────────────────────────────────────
  printSection("4. ParallelDiagnosticHandler  (thread-safe ordered output)");
  {
    mlir::OpBuilder builder(&ctx);
    mlir::Location loc = builder.getUnknownLoc();

    // Catcher: receives the diagnostics that parallelHandler flushes on
    // destruction.  Must be declared BEFORE parallelHandler so that it
    // destructs AFTER (C++ LIFO order), meaning it is still registered when
    // parallelHandler's flush fires.
    mlir::ScopedDiagnosticHandler catcher(
        &ctx, [](mlir::Diagnostic &d) -> mlir::LogicalResult {
          llvm::errs() << "[FLUSHED] " << d.str() << "\n";
          return mlir::success();
        });

    mlir::ParallelDiagnosticHandler parallelHandler(&ctx);

    // Simulate three tasks whose diagnostics arrive out of order.
    // In real parallel compilation each task runs on a separate thread;
    // here we use the same thread with explicit setOrderIDForThread /
    // eraseOrderIDForThread calls to demonstrate the buffering.
    //
    // Emission order:  C(2) → A(0) → B(1)   (intentionally scrambled)
    // Flush order:     A(0) → B(1) → C(2)   (restored by orderID)
    struct Task { unsigned orderID; const char *msg; };
    for (auto [id, msg] : {Task{2, "task C (orderID=2) — emitted first"},
                           Task{0, "task A (orderID=0) — emitted second"},
                           Task{1, "task B (orderID=1) — emitted third"}}) {
      parallelHandler.setOrderIDForThread(id);
      mlir::emitRemark(loc, msg);
      parallelHandler.eraseOrderIDForThread();
    }

    // parallelHandler destructs here → flushes buffered diagnostics in
    // orderID order (A → B → C) to the default handler → printed to stderr.
    llvm::errs() << "[PARALLEL] handler destructing — flush in orderID order follows:\n";
  }

  return 0;
}
