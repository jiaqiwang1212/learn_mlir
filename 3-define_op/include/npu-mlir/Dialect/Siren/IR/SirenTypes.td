#ifndef NPU_MLIR_SIREN_TYPES_TD
#define NPU_MLIR_SIREN_TYPES_TD

include "npu-mlir/Dialect/Siren/IR/SirenDialect.td"
include "mlir/IR/AttrTypeBase.td"

// 定义一个新Type的基类
class Siren_Type<string name, string typeMnemonic> : TypeDef<SirenDialect, name>{
    let summary = "Siren Type";
    let description = [{
        This is a base type for Siren dialect types.
    }];

    let mnemonic = typeMnemonic;

    let parameters = (ins "Type":$elementType, ArrayRefParameter<"int64_t", "expression shape">:$shape);

    let assemblyFormat = "`<` $elementType `:` $shape `>`";
}

def SirenTensor : Siren_Type<"SirenTensor", "tensor">;


#endif // NPU_MLIR_SIREN_TYPES_TD