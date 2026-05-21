#ifndef NPU_MLIR_DIALECT_SIREN_TRANSFORMS_RANGEANALYSIS_H_
#define NPU_MLIR_DIALECT_SIREN_TRANSFORMS_RANGEANALYSIS_H_

#include "mlir/Pass/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdint>
#include <limits>

namespace mlir::npu_mlir {

/// A closed integer interval [min, max].
///
/// Default-constructed value is the **bottom** element (min > max = empty
/// interval): "no value has flowed here yet."  getTop() returns the widest
/// interval [INT64_MIN, INT64_MAX]: "value could be anything."
///
/// Used as the ValueT in Lattice<RangeLatticeValue> for forward analysis.
/// Lattice<T> requires:
///   - static T join(const T &lhs, const T &rhs)  — conservative merge
///   - bool operator==(const T &) const
///   - void print(raw_ostream &) const
struct RangeLatticeValue {
  int64_t min = std::numeric_limits<int64_t>::max();
  int64_t max = std::numeric_limits<int64_t>::min();

  RangeLatticeValue() = default;
  RangeLatticeValue(int64_t lo, int64_t hi) : min(lo), max(hi) {}

  static RangeLatticeValue getTop() {
    return {std::numeric_limits<int64_t>::min(),
            std::numeric_limits<int64_t>::max()};
  }

  bool isEmpty() const { return min > max; }
  bool isTop() const {
    return min == std::numeric_limits<int64_t>::min() &&
           max == std::numeric_limits<int64_t>::max();
  }

  /// Conservative join (union).  join([a,b],[c,d]) = [min(a,c), max(b,d)].
  /// Monotone: join(join(x,y), x) == join(x,y).
  static RangeLatticeValue join(const RangeLatticeValue &lhs,
                                const RangeLatticeValue &rhs) {
    if (lhs.isEmpty()) return rhs;
    if (rhs.isEmpty()) return lhs;
    return {std::min(lhs.min, rhs.min), std::max(lhs.max, rhs.max)};
  }

  bool operator==(const RangeLatticeValue &rhs) const {
    return min == rhs.min && max == rhs.max;
  }

  void print(llvm::raw_ostream &os) const {
    if (isEmpty())
      os << "empty";
    else
      os << "[" << min << ", " << max << "]";
  }
};

#define GEN_PASS_DECL_RANGEANALYSIS
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

} // namespace mlir::npu_mlir

#endif // NPU_MLIR_DIALECT_SIREN_TRANSFORMS_RANGEANALYSIS_H_
