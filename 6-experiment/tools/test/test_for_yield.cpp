// test_for_yield.cpp
//
// Demonstrates that each scf.ForOp result at index i corresponds to the
// yield terminator's operand at the same index i, including nested ForOps
// where the outer for yields the inner for's results.
//
// Built IR:
//   func.func @test_for_yield(%arg0: i32, %arg1: i32) -> (i32, i32) {
//     %c0  = arith.constant 0  : index
//     %c10 = arith.constant 10 : index
//     %c5  = arith.constant 5  : index
//     %c1  = arith.constant 1  : index
//     %res0, %res1 = scf.for %i = %c0 to %c10 step %c1
//                      iter_args(%acc0 = %arg0, %acc1 = %arg1) -> (i32, i32) {
//       %inner0, %inner1 = scf.for %j = %c0 to %c5 step %c1
//                            iter_args(%iacc0 = %acc0, %iacc1 = %acc1) -> (i32,
//                            i32) {
//         scf.yield %iacc0, %iacc1 : i32, i32
//       }
//       scf.yield %inner0, %inner1 : i32, i32   // yield inner for's results
//     }
//     return %res0, %res1 : i32, i32
//   }
//
// The test then walks from the func.return operands backwards via
// getDefiningOp(), finds the outer ForOp, and zips for_op.getResults() with
// terminator->getOperands() to show the 1-to-1 correspondence — tracing
// through to the inner ForOp's results as well.

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/raw_ostream.h"

// ---------------------------------------------------------------------------
// Build the test IR programmatically
// ---------------------------------------------------------------------------
mlir::OwningOpRef<mlir::ModuleOp> buildTestIR(mlir::MLIRContext &context) {
  mlir::OpBuilder builder(&context);
  mlir::Location loc = builder.getUnknownLoc();

  auto module = mlir::ModuleOp::create(loc);
  builder.setInsertionPointToEnd(module.getBody());

  // func @test_for_yield(%arg0: i32, %arg1: i32) -> (i32, i32)
  auto i32Type = builder.getI32Type();
  auto funcType =
      builder.getFunctionType({i32Type, i32Type}, {i32Type, i32Type});
  auto funcOp =
      mlir::func::FuncOp::create(builder, loc, "test_for_yield", funcType);

  auto *entryBlock = funcOp.addEntryBlock();
  builder.setInsertionPointToStart(entryBlock);

  mlir::Value arg0 = entryBlock->getArgument(0); // initial value for acc0
  mlir::Value arg1 = entryBlock->getArgument(1); // initial value for acc1

  // Loop bounds
  mlir::Value c0 =
      mlir::arith::ConstantIndexOp::create(builder, loc, 0).getResult();
  mlir::Value c10 =
      mlir::arith::ConstantIndexOp::create(builder, loc, 10).getResult();
  mlir::Value c5 =
      mlir::arith::ConstantIndexOp::create(builder, loc, 5).getResult();
  mlir::Value c1 =
      mlir::arith::ConstantIndexOp::create(builder, loc, 1).getResult();

  // Outer scf.for: %i = %c0 to %c10 step %c1
  //   iter_args(%acc0=%arg0, %acc1=%arg1) -> (i32, i32)
  //   body: build inner for, yield inner for's results
  auto outerForOp = mlir::scf::ForOp::create(
      builder, loc, c0, c10, c1, mlir::ValueRange{arg0, arg1},
      [&](mlir::OpBuilder &outerB, mlir::Location outerLoc, mlir::Value /*iv*/,
          mlir::ValueRange outerIterArgs) {
        // Inner scf.for: %j = %c0 to %c5 step %c1
        //   iter_args(%iacc0=%acc0, %iacc1=%acc1) -> (i32, i32)
        auto innerForOp = mlir::scf::ForOp::create(
            outerB, outerLoc, c0, c5, c1,
            mlir::ValueRange{outerIterArgs[0], outerIterArgs[1]},
            [&](mlir::OpBuilder &innerB, mlir::Location innerLoc,
                mlir::Value /*jv*/, mlir::ValueRange innerIterArgs) {
              // Inner body: simply pass iter_args through
              mlir::scf::YieldOp::create(innerB, innerLoc, innerIterArgs);
            });

        // Outer body: yield the inner for's results
        mlir::scf::YieldOp::create(outerB, outerLoc, innerForOp.getResults());
      });

  // return %res0, %res1
  builder.setInsertionPointToEnd(entryBlock);
  mlir::func::ReturnOp::create(builder, loc, outerForOp.getResults());

  return mlir::OwningOpRef<mlir::ModuleOp>(module);
}

// ---------------------------------------------------------------------------
// Walk the IR: for each func.return operand, trace back through nested ForOps
// and print the result <-> yield-operand pairs at every level.
// ---------------------------------------------------------------------------
void printForOpCorrespondence(mlir::scf::ForOp forOp, unsigned returnIdx,
                              mlir::Value tracedVal, unsigned depth) {
  std::string indent(depth * 2, ' ');
  mlir::Operation *terminator = forOp.getBody()->getTerminator();

  llvm::outs() << indent << "ForOp [" << forOp.getLoc() << "]"
               << " zip(results, yield operands):\n";

  for (auto [res, operand] :
       llvm::zip(forOp.getResults(), terminator->getOperands())) {
    llvm::outs() << indent << "  [for result] ";
    res.print(llvm::outs());
    llvm::outs() << "  <->  [yield operand] ";
    operand.print(llvm::outs());
    if (tracedVal == res)
      llvm::outs() << "  <<< traced from return[" << returnIdx << "]";
    llvm::outs() << "\n";

    // Recurse if the yield operand itself comes from another ForOp
    if (mlir::Operation *defOp = operand.getDefiningOp())
      if (auto innerFor = mlir::dyn_cast<mlir::scf::ForOp>(defOp))
        printForOpCorrespondence(innerFor, returnIdx, operand, depth + 1);
  }
}

void test_for_yield_correspondence(mlir::ModuleOp module) {
  llvm::outs() << "=== ForOp Result <-> Yield Operand Correspondence ===\n";

  module.walk([](mlir::func::FuncOp funcOp) {
    mlir::Operation *returnOp = funcOp.getBody().back().getTerminator();
    llvm::outs() << "\nfunc '" << funcOp.getName()
                 << "' return op: " << returnOp->getName() << "\n";

    for (auto [idx, val] : llvm::enumerate(returnOp->getOperands())) {
      llvm::outs() << "\n  return operand[" << idx << "]: ";
      val.print(llvm::outs());
      llvm::outs() << "\n";

      mlir::Operation *defOp = val.getDefiningOp();
      llvm::outs() << "  def_op: " << defOp->getName().getStringRef() << "\n";

      if (auto forOp = mlir::dyn_cast<mlir::scf::ForOp>(defOp))
        printForOpCorrespondence(forOp, idx, val, 2);
    }
  });
}

// ---------------------------------------------------------------------------
int main() {
  mlir::MLIRContext context;
  context.loadDialect<mlir::func::FuncDialect>();
  context.loadDialect<mlir::scf::SCFDialect>();
  context.loadDialect<mlir::arith::ArithDialect>();

  auto module = buildTestIR(context);

  if (mlir::failed(mlir::verify(*module))) {
    llvm::errs() << "MLIR verification failed\n";
    return 1;
  }

  llvm::outs() << "=== Built IR ===\n";
  module->dump();
  llvm::outs() << "\n";

  test_for_yield_correspondence(*module);
  return 0;
}
