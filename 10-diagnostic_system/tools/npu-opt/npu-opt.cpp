//===- npu-opt.cpp - ch-10 diagnostic system driver -----------------------===//
//
// Ch-10: MLIR Diagnostic System Tutorial
//
// Minimal mlir-opt driver that registers:
//   --diag-scoped      ScopedDiagnosticHandler (RAII) demo
//   --diag-registered  DiagnosticEngine::registerHandler demo
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "mlir/Transforms/Passes.h"
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h"

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;
  registry.insert<mlir::func::FuncDialect, mlir::arith::ArithDialect>();
  mlir::registerTransformsPasses();

  mlir::npu_mlir::registerDiagScoped();
  mlir::npu_mlir::registerDiagRegistered();

  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "ch-10-opt", registry));
}
