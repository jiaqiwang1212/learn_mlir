#ifndef NPU_MLIR_TYPE_TRAITS_H
#define NPU_MLIR_TYPE_TRAITS_H
#include "mlir/IR/Types.h"
#include "llvm/Support/raw_ostream.h"
using namespace mlir;

namespace mlir {
template <typename ConcreteType>
class MyTypeTrait : public TypeTrait::TraitBase<ConcreteType, MyTypeTrait> {

public:
  void printTypeTrait() {
    llvm::outs() << "This is MyTypeTrait for type: " << ConcreteType::name
                 << "\n";
  }
};
} // namespace mlir

#endif // NPU_MLIR_TYPE_TRAITS_H