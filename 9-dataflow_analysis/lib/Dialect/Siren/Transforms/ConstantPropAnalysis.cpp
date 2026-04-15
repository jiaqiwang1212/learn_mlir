//===- ConstantPropAnalysis.cpp - Constant propagation analysis pass ------===//
//
// Ch-9: MLIR Dataflow Analysis Tutorial
//
// This pass demonstrates the MLIR DataFlow Analysis framework by running
// sparse constant propagation on a func.func body and emitting diagnostic
// remarks that identify which SSA values are compile-time constants.
//
// Key concepts demonstrated:
//   1. DataFlowSolver  — orchestrates the worklist algorithm
//   2. DeadCodeAnalysis — required companion; prunes dead blocks during solve
//   3. SparseConstantPropagation — uses op folders to determine constant values
//   4. Lattice<ConstantValue> — the per-value state queried after the solve
//
// The pass is READ-ONLY: it never modifies the IR.
//===----------------------------------------------------------------------===//

#include "npu-mlir/Dialect/Siren/Transforms/ConstantPropAnalysis.h"

// DataFlow framework headers
#include "mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h"
#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlowFramework.h"

// Dialect headers
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"

// IR helpers
#include "mlir/IR/Operation.h"
#include "llvm/Support/raw_ostream.h"

namespace mlir::npu_mlir {

// Include the TableGen-generated pass base class definition.
#define GEN_PASS_DEF_CONSTANTPROPANALYSIS
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

//===----------------------------------------------------------------------===//
// ConstantPropAnalysisPass
//===----------------------------------------------------------------------===//

class ConstantPropAnalysisPass
    : public impl::ConstantPropAnalysisBase<ConstantPropAnalysisPass> {
public:
  void runOnOperation() override {
    func::FuncOp func = getOperation();

    // -----------------------------------------------------------------
    // Step 1: Create the solver and load analyses.
    //
    // DeadCodeAnalysis must always be loaded alongside
    // SparseConstantPropagation — the sparse analysis relies on
    // liveness information from the dead-code analysis to decide
    // which branch targets are reachable.
    // -----------------------------------------------------------------
    DataFlowSolver solver;
    solver.load<dataflow::DeadCodeAnalysis>();
    solver.load<dataflow::SparseConstantPropagation>();

    // -----------------------------------------------------------------
    // Step 2: Run the fixed-point solver.
    //
    // initializeAndRun walks the IR and drives the worklist algorithm
    // until all lattice values have converged.
    // -----------------------------------------------------------------
    if (failed(solver.initializeAndRun(func))) {
      func.emitError("DataFlowSolver failed during constant propagation");
      return signalPassFailure();
    }

    // -----------------------------------------------------------------
    // Step 3: Query results and emit remarks.
    //
    // Walk every operation in the function. For each SSA result, look
    // up its lattice state. If the state is a known constant (not
    // uninitialized and not "unknown"), emit a diagnostic remark.
    // -----------------------------------------------------------------
    func.walk([&](Operation *op) {
      for (Value result : op->getResults()) {
        // Look up the lattice value for this SSA value.
        auto *lattice =
            solver.lookupState<dataflow::Lattice<dataflow::ConstantValue>>(
                result);

        // Null means the analysis never touched this value (e.g. block
        // arguments of unreachable blocks).
        if (!lattice)
          continue;

        const dataflow::ConstantValue &cv = lattice->getValue();

        // Uninitialized: solver has not yet produced a result.
        if (cv.isUninitialized())
          continue;

        // getConstantValue() returns a null Attribute for the "unknown"
        // state (multiple conflicting constants joined to bottom).
        Attribute constAttr = cv.getConstantValue();
        if (!constAttr)
          continue;

        // Known constant — emit a remark on the defining op.
        std::string msg;
        llvm::raw_string_ostream os(msg);
        os << "value is constant: ";
        constAttr.print(os);
        op->emitRemark(msg);
      }
    });
  }
};

} // namespace mlir::npu_mlir
