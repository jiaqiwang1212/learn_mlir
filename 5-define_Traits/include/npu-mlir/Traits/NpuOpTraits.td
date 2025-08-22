#ifndef NPU_OP_TRAITS_TD
#define NPU_OP_TRAITS_TD

include "mlir/IR/OpBase.td"


def RequireTwoOperandsOneResultOpTrait : NativeOpTrait<"npu_mlir::RequireTwoOperandsOneResult">;


#endif // NPU_OP_TRAITS_TD