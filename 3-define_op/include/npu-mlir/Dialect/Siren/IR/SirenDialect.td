#ifndef NPU_MLIR_SIREN_DIALECT_TD
#define NPU_MLIR_SIREN_DIALECT_TD

include "mlir/IR/DialectBase.td"

def SirenDialect : Dialect {
  // Dialect的名字
  let name = "siren";
  
  // 生成的Dialcet类所在的命名空间
  let cppNamespace = "::mlir::npu_mlir";

  // Dialect 的简短描述  TODO 在哪里体现？
  let description = "Siren dialect for NPU MLIR.";
  // A list of dialects this dialect will load on construction as dependencies.
  // These are dialects that this dialect may involve in canonicalization
  // pattern or interfaces.
  // 声明当前Dialect依赖的其他Dialect
  let dependentDialects = ["mlir::func::FuncDialect", "mlir::arith::ArithDialect"];

  // A list of key/value pair representing an attribute type and a name.
  // This will generate helper classes on the dialect to be able to
  // manage discardable attributes on operations in a type safe way.
  // dag discardableAttrs = (ins);
  // let discardableAttrs = (ins
  //   "StringAttr":$debug_level     // 源文件信息
  // );


  // An optional code block containing extra declarations to place in the
  // dialect declaration.
  let extraClassDeclaration = [{
    void print_name();
  }];

  // // If this dialect overrides the hook for materializing constants.
  // bit hasConstantMaterializer = 0;

  /// If the dialect definition provides a non-default destructor.
  /// If false, a default destructor implementation will be generated.
  // 为1时则没有默认析构函数，需要用户提供析构函数实现
  // 如果为0，则会生成一个默认的析构函数实现。
  let hasNonDefaultDestructor = 1;

  // // If this dialect overrides the hook for verifying operation attributes.
  // let hasOperationAttrVerify = 0;

  // // If this dialect overrides the hook for verifying region argument
  // // attributes.
  // bit hasRegionArgAttrVerify = 0;

  // // If this dialect overrides the hook for verifying region result attributes.
  // bit hasRegionResultAttrVerify = 0;

  // // If this dialect overrides the hook for op interface fallback.
  // bit hasOperationInterfaceFallback = 0;

  // // If this dialect should use default generated attribute parser boilerplate.
  // // When set, ODS will generate declarations for the attribute parsing and
  // // printing hooks in the dialect and default implementations that dispatch to
  // // each individual attribute directly.
  // bit useDefaultAttributePrinterParser = 0;

  // // If this dialect should use default generated type parser boilerplate:
  // // When set, ODS will generate declarations for the type parsing and printing
  // // hooks in the dialect and default implementations that dispatch to each
  // // individual type directly.
  // bit useDefaultTypePrinterParser = 0;

  // // If this dialect overrides the hook for canonicalization patterns.
  // bit hasCanonicalizer = 0;

  // // If this dialect can be extended at runtime with new operations or types.
  // bit isExtensible = 0;

  // // Whether inherent Attributes defined in ODS will be stored as Properties.
  // bit usePropertiesForAttributes = 1;
}


#endif // NPU_MLIR_SIREN_DIALECT_TD
