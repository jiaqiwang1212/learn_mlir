#ifndef NPU_MLIR_SIREN_DIALECT_TD
#define NPU_MLIR_SIREN_DIALECT_TD

include "mlir/IR/DialectBase.td"

def SirenDialect : Dialect {
  // Todo
  let name = "siren";
  let cppNamespace = "::mlir::npu_mlir";
  let description = "Siren dialect for NPU MLIR.";
}


#endif // NPU_MLIR_SIREN_DIALECT_TD
