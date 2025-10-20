#ifndef NPU_MLIR_SIREN_ATTRS_TD
#define NPU_MLIR_SIREN_ATTRS_TD

include "mlir/IR/AttrTypeBase.td"
include "npu-mlir/Dialect/Siren/IR/SirenDialect.td"

class SirenAttr<string name, string typeMnemonic> : AttrDef<SirenDialect, name, []> {
  let mnemonic = typeMnemonic;  // 在.mlir 文件中使用的助记符
  let summary = "Siren 自定义属性";
  let description = [{
    这是一个 Siren 方言的自定义属性示例。
  }];
}

def SirenCustomAttr : SirenAttr<"Custom", "custom_attr"> {
    let summary = "custom attribute for Siren dialect";
    let description = [{
      This is a custom attribute specific to the Siren dialect.
    }];
    let parameters = (ins
        "std::string":$value
    );
    let assemblyFormat = "`<` $value `>`";
  let constBuilderCall =
    "::mlir::npu_mlir::CustomAttr::get($_builder.getContext(), $0)";
}

// 让 ODS 知道这类 Attribute 才算匹配（用 isa 判断）
def SirenCustomAttrC : Attr<
  CPred<"mlir::isa<::mlir::npu_mlir::CustomAttr>($_self)">,
  "CustomAttr constraint"> {

  // 存储类型：就是你的自定义属性类本体
  let storageType = "::mlir::npu_mlir::CustomAttr";

  // builder 侧“裸值”的 C++ 类型（建议用 StringRef，适配字符串字面量）
  let returnType  = "std::string";

  // 从 Attribute 读回裸值（用于生成的 getter 等；可选但推荐）
  let convertFromStorage = "$_self.getValue()";

  // 关键：教 ODS 怎样把“裸值”包成你的属性
  let constBuilderCall =
    "::mlir::npu_mlir::CustomAttr::get($_builder.getContext(), $0)";
}



#endif // NPU_MLIR_SIREN_ATTRS_TD