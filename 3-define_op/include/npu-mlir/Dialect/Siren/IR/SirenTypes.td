#ifndef NPU_MLIR_SIREN_TYPES_TD
#define NPU_MLIR_SIREN_TYPES_TD

include "npu-mlir/Dialect/Siren/IR/SirenDialect.td"
include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/CommonTypeConstraints.td"
include "mlir/IR/BuiltinTypeInterfaces.td"

// 使用这个可以在创建Siren_Type对elementType进行约束
def UI4 : UI<4>;
def Siren_ElementType : AnyTypeOf<[F32, F16, UI16, UI8, SI8, SI16,UI4, SI32, SI64]> {
}

// 定义一个新Type的基类
class Siren_Type<string name, string typeMnemonic> : TypeDef<SirenDialect, name, []>{
    let summary = "Siren Type";
    let description = [{
        This is a base type for Siren dialect types.
    }];

    let mnemonic = typeMnemonic;

    let parameters = (ins Siren_ElementType:$elementType, ArrayRefParameter<"int64_t", "expression shape">:$shape);
    
    let assemblyFormat = "`<` $elementType `:` $shape `>`";
}

def SirenTensor : Siren_Type<"SirenTensor", "tensor">;  // 这个会生成一个 SirenTensorType 类

// "Tensor"#EleType 是出错信息 
class STensor<Pred constraints, Type EleType> :
    DialectType<SirenDialect, And<[CPred<"mlir::isa<SirenTensorType>($_self)">, constraints]>,
    "Tensor"#EleType>{
        Type elementType = EleType;
}

// 会在op那里生成对操作的constraint
// 这里是定义一个约束条件，要求是SirenTensor且元素类型是F16
// 还可以定义其他的约束条件，比如F32, Int32等
def S_HTensor: STensor<CPred<"mlir::cast<SirenTensorType>($_self).getElementType().isF16()">, F16>;

def S_Tensor : AnyTypeOf<[S_HTensor]>;

#endif // NPU_MLIR_SIREN_TYPES_TD