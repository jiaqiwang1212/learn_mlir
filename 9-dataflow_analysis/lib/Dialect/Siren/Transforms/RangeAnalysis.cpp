//===- RangeAnalysis.cpp - Integer range analysis pass --------------------===//
//
// Ch-9: MLIR Dataflow Analysis Tutorial — Range Analysis Extension
//
// Demonstrates building a *custom* SparseForwardDataFlowAnalysis that tracks
// the minimum and maximum possible integer value of every SSA result.  After
// the fixed-point solve, the pass walks the IR and emits a diagnostic remark
// for every result whose range is provably tighter than [INT64_MIN, INT64_MAX].
//
// Key concepts demonstrated:
//   1. SparseForwardDataFlowAnalysis<StateT>  — custom forward analysis
//   2. Lattice<RangeLatticeValue>             — per-value state object
//   3. setToEntryState   — initialise function/block args to "unknown" (top)
//   4. visitOperation    — transfer functions for arith ops
//   5. propagateIfChanged — lattice update protocol
//
// Transfer functions:
//   arith.constant c      → [c, c]             (exact)
//   [a,b] + [c,d]         → [a+c, b+d]         (overflow ignored for clarity)
//   [a,b] * [c,d]         → [min(corners), max(corners)]
//   x remui N  (N const)  → [0, N-1]           (modulo bound)
//   anything else         → top                 (conservative)
//
// The pass is READ-ONLY: it never modifies the IR.
//===----------------------------------------------------------------------===//

#include "npu-mlir/Dialect/Siren/Transforms/RangeAnalysis.h"

#include "mlir/Analysis/DataFlow/DeadCodeAnalysis.h"
#include "mlir/Analysis/DataFlow/SparseAnalysis.h"
#include "mlir/Analysis/DataFlowFramework.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Attributes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Operation.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>

namespace mlir::npu_mlir {

#define GEN_PASS_DEF_RANGEANALYSIS
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

//===----------------------------------------------------------------------===//
// RangeAnalysis — transfer functions
//===----------------------------------------------------------------------===//

using RangeLattice = dataflow::Lattice<RangeLatticeValue>;

class RangeAnalysis
    : public dataflow::SparseForwardDataFlowAnalysis<RangeLattice> {
public:
  using SparseForwardDataFlowAnalysis::SparseForwardDataFlowAnalysis;

  /// Function / block arguments: we know nothing → widest range (top).
  void setToEntryState(RangeLattice *lattice) override {
    propagateIfChanged(lattice, lattice->join(RangeLatticeValue::getTop()));
  }

  /// Transfer function: derive result ranges from operand ranges.
  LogicalResult visitOperation(Operation *op,
                               ArrayRef<const RangeLattice *> operands,
                               ArrayRef<RangeLattice *> results) override {

    // ── arith.constant ────────────────────────────────────────────────────
    if (auto constOp = dyn_cast<arith::ConstantOp>(op)) {
      if (auto intAttr = dyn_cast<IntegerAttr>(constOp.getValue())) {
        int64_t val = intAttr.getInt();
        propagateIfChanged(results[0],
                           results[0]->join(RangeLatticeValue(val, val)));
        return success();
      }
      setAllToEntryStates(results);
      return success();
    }

    // ── arith.addi ────────────────────────────────────────────────────────
    // [a,b] + [c,d] = [a+c, b+d].  Overflow is not handled here.
    if (isa<arith::AddIOp>(op)) {
      const RangeLatticeValue &lhs = operands[0]->getValue();
      const RangeLatticeValue &rhs = operands[1]->getValue();
      if (!lhs.isEmpty() && !rhs.isEmpty())
        propagateIfChanged(
            results[0],
            results[0]->join(
                RangeLatticeValue(lhs.min + rhs.min, lhs.max + rhs.max)));
      return success();
    }

    // ── arith.muli ────────────────────────────────────────────────────────
    // Evaluate all four sign-combination corners and take min/max.
    if (isa<arith::MulIOp>(op)) {
      const RangeLatticeValue &lhs = operands[0]->getValue();
      const RangeLatticeValue &rhs = operands[1]->getValue();
      if (!lhs.isEmpty() && !rhs.isEmpty()) {
        int64_t c[4] = {lhs.min * rhs.min, lhs.min * rhs.max,
                         lhs.max * rhs.min, lhs.max * rhs.max};
        propagateIfChanged(results[0],
                           results[0]->join(RangeLatticeValue(
                               *std::min_element(c, c + 4),
                               *std::max_element(c, c + 4))));
      }
      return success();
    }

    // ── arith.remui ───────────────────────────────────────────────────────
    // Unsigned x % N is always in [0, N-1] when N is a known positive
    // constant.  This is the key pattern for array-index range proofs.
    if (isa<arith::RemUIOp>(op)) {
      const RangeLatticeValue &divisor = operands[1]->getValue();
      if (!divisor.isEmpty() && divisor.min == divisor.max &&
          divisor.min > 0) {
        propagateIfChanged(
            results[0],
            results[0]->join(RangeLatticeValue(0, divisor.min - 1)));
        return success();
      }
      setAllToEntryStates(results);
      return success();
    }

    // ── fallback: conservative ────────────────────────────────────────────
    setAllToEntryStates(results);
    return success();
  }
};

//===----------------------------------------------------------------------===//
// RangeAnalysisPass
//===----------------------------------------------------------------------===//

class RangeAnalysisPass : public impl::RangeAnalysisBase<RangeAnalysisPass> {
public:
  void runOnOperation() override {
    func::FuncOp func = getOperation();

    // -----------------------------------------------------------------
    // Step 1: Construct solver, load DeadCodeAnalysis (required companion
    // for any sparse analysis) plus our custom RangeAnalysis.
    // -----------------------------------------------------------------
    DataFlowSolver solver;
    solver.load<dataflow::DeadCodeAnalysis>();
    solver.load<RangeAnalysis>();

    if (failed(solver.initializeAndRun(func))) {
      func.emitError("DataFlowSolver failed during range analysis");
      return signalPassFailure();
    }

    // -----------------------------------------------------------------
    // Step 2: Walk results; emit remarks for constrained ranges.
    // Skip values that are still at top (unconstrained) or bottom
    // (unreachable) since they carry no useful information.
    // -----------------------------------------------------------------
    func.walk([&](Operation *op) {
      for (Value result : op->getResults()) {
        if (!isa<IntegerType, IndexType>(result.getType()))
          continue;

        auto *lattice = solver.lookupState<RangeLattice>(result);
        if (!lattice)
          continue;

        const RangeLatticeValue &range = lattice->getValue();
        if (range.isEmpty() || range.isTop())
          continue;

        std::string msg;
        llvm::raw_string_ostream os(msg);
        os << "value is in range: ";
        range.print(os);
        op->emitRemark(msg);
      }
    });
  }
};

} // namespace mlir::npu_mlir
