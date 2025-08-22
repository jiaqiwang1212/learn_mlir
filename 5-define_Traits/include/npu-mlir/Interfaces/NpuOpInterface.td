#ifndef NPU_OP_INTERFACE_TD
#define NPU_OP_INTERFACE_TD 
include "mlir/IR/OpBase.td"

//===----------------------------------------------------------------------===//
def NpuOpInterface : OpInterface<"NpuOpInterface", []> {
  let description = [{
    This interface is used to define operations that can be executed on an NPU.
  }];
  let cppNamespace = "::mlir::npu_mlir";

  let methods = [
    // 声明一个接口方法，但是不提供实现，也就是每个实现这个接口的Op都需要提供自己的实现。
    InterfaceMethod<
        [{return op num of operands}],
        "int32_t",
        "get_num_operands_1",
        (ins)
    >,
    // 提供一个默认实现，要注意的是，使用这个interface的Op必须有getNumOperands方法。
    InterfaceMethod<
        [{return op num of operands}],
        "int32_t",
        "get_num_operands_2",
        (ins),
        "",
        [{return $_op.getNumOperands();}]
    >,
    // 提供一个method的实现，op无法重写这个方法。
    InterfaceMethod<
        [{return op num of operands}],
        "int32_t",
        "get_num_operands_3",
        (ins),
        [{return $_op.getNumOperands();}]
    >
  ];

}


#endif // NPU_OP_INTERFACE_TD