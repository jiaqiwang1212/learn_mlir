// test_transform_demo.cpp
//
// Demonstrates the programmatic C++ entry point for the Transform dialect.
//
// This program:
//   1. Parses a hardcoded MLIR module that contains BOTH payload IR and a
//      transform.named_sequence (the same single-module layout used by --transform-interpreter)
//   2. Locates the @__transform_main named sequence
//   3. Calls mlir::transform::applyTransforms() to execute the sequence
//   4. Prints before/after IR to stdout; the remark from the transform appears on stderr
//
// Key API facts:
//   - registerAllDialects() + registerAllExtensions() mirrors what transform-opt does
//   - enforceToplevelTransformOp=false: allows passing an inner named_sequence directly
//     (rather than a top-level transform.sequence wrapping it)
//   - The payload root and the transform sequence are siblings inside the same module;
//     transform.structured.match on func.func ops won't accidentally match the sequence
//     itself because transform.named_sequence is not a func.func
//   - If the interpreter errors "transform op is nested in the payload", the fallback
//     is to parse payload and transform from two separate modules and pass the transform
//     module as a separate root to applyTransforms
//
// Build: wired into 6-experiment/tools/test/CMakeLists.txt via add_test_executable()
// Run:   ./build/6-experiment/tools/test/test_transform_demo

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Dialect/Transform/IR/TransformDialect.h"
#include "mlir/Dialect/Transform/IR/TransformOps.h"
#include "mlir/Dialect/Transform/Interfaces/TransformInterfaces.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Diagnostics.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllExtensions.h"
#include "mlir/Parser/Parser.h"
#include "llvm/Support/raw_ostream.h"

// A simple module with:
//   - @demo: the payload function we want to inspect
//   - @__transform_main: the transform sequence that matches func.func ops and emits a remark
//
// The transform.with_named_sequence attribute tells the verifier to validate the sequences.
static const char *kModuleStr = R"MLIR(
module attributes {transform.with_named_sequence} {

  func.func @demo(%a: i32, %b: i32) -> i32 {
    %0 = arith.addi %a, %b : i32
    return %0 : i32
  }

  transform.named_sequence @__transform_main(
      %root: !transform.any_op {transform.readonly}) {
    %f = transform.structured.match ops{["func.func"]} in %root
        : (!transform.any_op) -> !transform.any_op
    transform.debug.emit_remark_at %f, "hello from C++ applyTransforms"
        : !transform.any_op
    transform.yield
  }

}
)MLIR";

int main() {
  // Register every built-in dialect and every dialect extension (including
  // Transform dialect extensions for Linalg, SCF, Func, Tensor, etc.).
  // This mirrors the pattern used by all mlir/examples/transform/ programs.
  mlir::DialectRegistry registry;
  mlir::registerAllDialects(registry);
  mlir::registerAllExtensions(registry);

  mlir::MLIRContext context(registry);
  context.loadAllAvailableDialects();

  // Parse the module from the hardcoded string.
  auto module = mlir::parseSourceString<mlir::ModuleOp>(kModuleStr, &context);
  if (!module) {
    llvm::errs() << "error: failed to parse module\n";
    return 1;
  }
  if (mlir::failed(mlir::verify(*module))) {
    llvm::errs() << "error: module verification failed\n";
    return 1;
  }

  // Register a diagnostic handler so transform.debug.emit_remark_at output
  // appears on stderr. Without this, remarks are silently discarded.
  mlir::ScopedDiagnosticHandler diagHandler(
      &context, [](mlir::Diagnostic &diag) -> mlir::LogicalResult {
        if (diag.getSeverity() == mlir::DiagnosticSeverity::Remark)
          llvm::errs() << "remark: " << diag.str() << "\n";
        return mlir::success();
      });

  llvm::outs() << "=== Before ===\n";
  module->print(llvm::outs());
  llvm::outs() << "\n\n";

  // Find the @__transform_main named sequence inside the module.
  mlir::transform::TransformOpInterface entry;
  module->walk([&](mlir::Operation *op) -> mlir::WalkResult {
    // Check if this operation's name matches "transform.named_sequence"
    if (op->getName().getStringRef() != "transform.named_sequence")
      return mlir::WalkResult::advance();
    // Check if it has a symbol name attribute equal to "__transform_main"
    auto symName = op->getAttrOfType<mlir::StringAttr>(
        mlir::SymbolTable::getSymbolAttrName());
    if (!symName || symName.getValue() != "__transform_main")
      return mlir::WalkResult::advance();
    entry = mlir::dyn_cast<mlir::transform::TransformOpInterface>(op);
    return mlir::WalkResult::interrupt();
  });

  if (!entry) {
    llvm::errs() << "error: could not find @__transform_main named sequence\n";
    return 1;
  }

  // Apply the transform sequence to the module.
  //
  // payloadRoot = module.get()  — the whole module is the payload root
  // transform   = entry         — the @__transform_main named sequence
  // extraMapping = {}           — no additional handle/payload bindings
  // options     = default       — expensive checks enabled (debug mode)
  // enforceToplevelTransformOp = false — we pass an inner named_sequence, not
  //   a top-level transform.sequence wrapper, so relax the toplevel check
  mlir::transform::TransformOptions options;
  if (mlir::failed(mlir::transform::applyTransforms(
          module.get(), entry, /*extraMapping=*/{}, options,
          /*enforceToplevelTransformOp=*/false))) {
    llvm::errs() << "error: applyTransforms failed\n";
    return 1;
  }

  llvm::outs() << "=== After ===\n";
  module->print(llvm::outs());
  llvm::outs() << "\n";

  // Expected output:
  //   stdout: === Before === <module> === After === <module>
  //   stderr: remark: hello from C++ applyTransforms (anchored at @demo)
  return 0;
}
