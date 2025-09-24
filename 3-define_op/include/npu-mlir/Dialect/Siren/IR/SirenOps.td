#ifndef NPU_MLIR_SIREN_OPS_TD
#define NPU_MLIR_SIREN_OPS_TD

include "mlir/IR/OpBase.td"
include "mlir/IR/BuiltinAttributes.td"
include "npu-mlir/Dialect/Siren/IR/SirenDialect.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/IR/CommonAttrConstraints.td"
include "npu-mlir/Dialect/Siren/IR/SirenTypes.td"

def AddOp: Op<SirenDialect, "add", []> {
  
  // One-line human-readable description of what the op does.
  let summary = "Add two values.";

  // Additional, longer human-readable description of what the op does.
  let description = "This AddOp for me to test some function of op table gen.";                                                                                                                                                                                                                                      

  // Optional. The group of ops this op is part of.
  // 与生成的文档相关，这个不用关心
  // OpDocGroup opDocGroup = ?;

  // Dag containing the arguments of the op. Default to 0 arguments.
  dag arguments = (ins I8:$lhs, I8:$rhs);

  // // The list of results of the op. Default to 0 results.
  dag results = (outs I8:$result);

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
  let builders = [
    OpBuilder<(ins "Value":$lhs, "Value":$rhs)>
  ];

  // Avoid generating default build functions.  Custom builders must be
  // provided.
  let skipDefaultBuilders = 1;

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

  // A bit indicating if the operation has additional invariants that need to
  // verified (aside from those verified by other ODS constructs). If set to `1`,
  // an additional `LogicalResult verify()` declaration will be generated on the
  // operation class. The operation should implement this method and verify the
  // additional necessary invariants. This verifier shouldn't access any nested
  // operations because those operations may be ill-formed. Use the
  // `hasRegionVerifier` below instead.
  // 对没有region的op来说，可以使用这个filed来对op进行verify
  // 对有region的op来说，应该使用hasRegionVerifier来对region进行verify
  let hasVerifier = 1;

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

  // Whether this op has a static "canonicalize" method to perform "match and
  // rewrite patterns".
  // 这个和上面的hasCanonicalizer不同, 这个是指是否有一个静态的canonicalize方法
  // 如果这个设置为1， 则会生成一个静态的canonicalize方法， 也就是只有这一种pattern
  let hasCanonicalizeMethod = 1;

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

def SubOp: Op<SirenDialect, "sub", []> {
  
  let summary = "Sub two values.";

  let description = "This SubOp for me to test some function of op table gen.";                                                                                                                                                                                                                                      

  dag arguments = (ins I8:$lhs, I8:$rhs, DefaultValuedOptionalAttr<I32Attr, "5">:$level);

  dag results = (outs I8:$result);

  let hasFolder = 1;
}

def MulOp: Op<SirenDialect, "mul", []> {
  
  let summary = "Mul two values.";

  let description = "This Mul for me to test some function of op table gen.";                                                                                                                                                                                                                                      

  dag arguments = (ins I8:$lhs, I8:$rhs, DefaultValuedOptionalAttr<I32Attr, "5">:$level);

  dag results = (outs I8:$result);
}

def DivOp: Op<SirenDialect, "div", []> {
  
  let summary = "Div two values.";

  let description = "This Div for me to test some function of op table gen.";                                                                                                                                                                                                                                      

  dag arguments = (ins I8:$lhs, I8:$rhs, OptionalAttr<I32Attr>:$level);

  dag results = (outs I8:$result);
}

def SqrtOp: Op<SirenDialect, "sqrt", []> {
  
  let summary = "sqrt one values.";

  let description = "This Div for me to test some function of op table gen.";                                                                                                                                                                                                                                      

  dag arguments = (ins S_Tensor:$lhs);

  dag results = (outs S_Tensor:$result);
}

#endif // NPU_MLIR_SIREN_Op_TD
