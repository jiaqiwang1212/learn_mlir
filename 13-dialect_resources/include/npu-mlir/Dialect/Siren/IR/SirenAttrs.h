#ifndef NPU_MLIR_SIREN_ATTRS_H
#define NPU_MLIR_SIREN_ATTRS_H

#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/DialectResourceBlobManager.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"

namespace mlir::npu_mlir {
// The alias must be declared before SirenAttrs.h.inc is included because the
// generated ResourceHandleParameter code references this type by name.
using SirenDialectResourceBlobHandle =
    ::mlir::DialectResourceBlobHandle<SirenDialect>;
} // namespace mlir::npu_mlir

#define GET_ATTRDEF_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.h.inc"

#endif // NPU_MLIR_SIREN_ATTRS_H
