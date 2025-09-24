#ifndef NPU_MLIR_SIREN_TYPES_H
#define NPU_MLIR_SIREN_TYPES_H

#include "mlir/IR/DialectImplementation.h"
#include "llvm/ADT/TypeSwitch.h"

#define GET_TYPEDEF_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenTypes.h.inc"

#endif // NPU_MLIR_SIREN_TYPES_H