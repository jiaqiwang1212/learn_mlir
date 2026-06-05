#ifndef NPU_MLIR_SIREN_DIALECT_TD
#define NPU_MLIR_SIREN_DIALECT_TD

include "mlir/IR/DialectBase.td"
include "mlir/IR/OpBase.td"

def SirenDialect : Dialect {
  let name = "siren";
  let cppNamespace = "::mlir::npu_mlir";
  let description = "Siren dialect for Ch-13: dialect resources demo.";
  let dependentDialects = ["mlir::func::FuncDialect", "mlir::arith::ArithDialect"];
  let useDefaultAttributePrinterParser = 1;
  // No custom types in ch-13: omit useDefaultTypePrinterParser to avoid
  // declaring parseType/printType overrides with no matching definitions.
  let hasNonDefaultDestructor = 1;
}

#endif // NPU_MLIR_SIREN_DIALECT_TD
