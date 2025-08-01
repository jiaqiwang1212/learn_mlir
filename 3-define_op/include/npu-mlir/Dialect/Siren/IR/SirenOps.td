#ifndef NPU_MLIR_SIREN_OPS_TD
#define NPU_MLIR_SIREN_OPS_TD

include "mlir/IR/OpBase.td"
include "mlir/IR/BuiltinAttributes.td"
include "npu-mlir/Dialect/Siren/IR/SirenDialect.td"

def AddOp: Op<SirenDialect, "add", []> {
  
  // // One-line human-readable description of what the op does.
  // string summary = "";

  // // Additional, longer human-readable description of what the op does.
  // string description = "";

  // // Optional. The group of ops this op is part of.
  // OpDocGroup opDocGroup = ?;

  // Dag containing the arguments of the op. Default to 0 arguments.
  dag arguments = (ins StrAttr:$name);

  // // The list of results of the op. Default to 0 results.
  // dag results = (outs);

  // // The list of regions of the op. Default to 0 regions.
  // dag regions = (region);

  // // The list of successors of the op. Default to 0 successors.
  // dag successors = (successor);

  // Attribute getters can be added to the op by adding an Attr member
  // with the name and type of the attribute. E.g., adding int attribute
  // with name "value" and type "i32":
  //   I32Attr value;

  // Define the hooks used for building, parsing, printing, verification.

  // Custom builder.
  // In addition to the custom builder provided here, and unless
  // skipDefaultBuilders is set, two default builders are generated, with the
  // following signatures:
  //
  // ```c++
  // static void build(OpBuilder &, OperationState &odsState,
  //                   Type <result0-name>, Type <result1-name>, ...,
  //                   Value <arg0-name>, Value <arg1-name>, ...,
  //                   Attribute <attr0-name>, Attribute <attr1-name>, ...);
  // ```
  // * where the attributes follow the same declaration order as in the op.
  //
  // ```c++
  // static void build(OpBuilder &, OperationState &odsState,
  //                   TypeRange resultTypes,
  //                   ValueRange operands,
  //                   ArrayRef<NamedAttribute> attributes);
  // ```
  // list<OpBuilder> builders = ?;

  // // Avoid generating default build functions.  Custom builders must be
  // // provided.
  // bit skipDefaultBuilders = 0;

  // // Custom assembly format.
  // /// This field corresponds to a declarative description of the assembly format
  // /// for this operation. If populated, the `hasCustomAssemblyFormat` field is
  // /// ignored.
  // string assemblyFormat = ?;
  // /// This field indicates that the operation has a custom assembly format
  // /// implemented in C++. When set to `1` a `parse` and `print` method are generated
  // /// on the operation class. The operation should implement these methods to
  // /// support the custom format of the operation. The methods have the form:
  // ///   * ParseResult parse(OpAsmParser &parser, OperationState &result)
  // ///   * void print(OpAsmPrinter &p)
  // bit hasCustomAssemblyFormat = 0;

  // // A bit indicating if the operation has additional invariants that need to
  // // verified (aside from those verified by other ODS constructs). If set to `1`,
  // // an additional `LogicalResult verify()` declaration will be generated on the
  // // operation class. The operation should implement this method and verify the
  // // additional necessary invariants. This verifier shouldn't access any nested
  // // operations because those operations may be ill-formed. Use the
  // // `hasRegionVerifier` below instead.
  // bit hasVerifier = 0;

  // // A bit indicating if the operation has additional invariants that need to
  // // verified and which associate with regions (aside from those verified by the
  // // traits). If set to `1`, an additional `LogicalResult verifyRegions()`
  // // declaration will be generated on the operation class. The operation should
  // // implement this method and verify the additional necessary invariants
  // // associated with regions. Note that this method is invoked after all the
  // // region ops are verified.
  // bit hasRegionVerifier = 0;

  // // Whether this op has associated canonicalization patterns.
  // bit hasCanonicalizer = 0;

  // // Whether this op has a static "canonicalize" method to perform "match and
  // // rewrite patterns".
  // bit hasCanonicalizeMethod = 0;

  // // Whether this op has a folder.
  // bit hasFolder = 0;

  // // Whether to let ops implement their custom `readProperties` and
  // // `writeProperties` methods to emit bytecode.
  // bit useCustomPropertiesEncoding = 0;

  // // Op traits.
  // // Note: The list of traits will be uniqued by ODS.
  // list<Trait> traits = props;

  // Additional code that will be added to the public part of the generated
  // C++ code of the op declaration.
  // 这个是生成在.h文件中的代码
  let extraClassDeclaration = [{
    void printOpName();
    void printInfo();
  }];

  // Additional code that will be added to the generated source file. The
  // generated code is placed inside the op's C++ namespace. `$cppClass` is
  // replaced by the op's C++ class name.
  // 这个是生成在.cpp文件中的代码
  let extraClassDefinition = [{
    void $cppClass::printInfo(){llvm::outs() << "Siren AddOp\n";}
  }];
}


#endif // NPU_MLIR_SIREN_Op_TD
