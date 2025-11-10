#ifndef NPU_MLIR_SIREN_ATTRS_H
#define NPU_MLIR_SIREN_ATTRS_H

#include "mlir/IR/DialectImplementation.h"
#include "npu-mlir/TypeTrait/TypeTraits.h"
#include "llvm/ADT/TypeSwitch.h"

#define GET_ATTRDEF_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.h.inc"

#endif // NPU_MLIR_SIREN_ATTRS_H