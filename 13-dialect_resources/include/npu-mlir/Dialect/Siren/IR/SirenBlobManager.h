#ifndef NPU_MLIR_SIREN_BLOB_MANAGER_H
#define NPU_MLIR_SIREN_BLOB_MANAGER_H

#include "mlir/IR/DialectResourceBlobManager.h"
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.h"

namespace mlir::npu_mlir {

// The siren dialect's blob manager interface.
// Retrieve via:
//   ctx.getLoadedDialect<SirenDialect>()
//       ->getRegisteredInterface<SirenResourceBlobManagerInterface>()
struct SirenResourceBlobManagerInterface
    : public ::mlir::ResourceBlobManagerDialectInterfaceBase<
          SirenDialectResourceBlobHandle> {
  using ResourceBlobManagerDialectInterfaceBase<
      SirenDialectResourceBlobHandle>::ResourceBlobManagerDialectInterfaceBase;
};

} // namespace mlir::npu_mlir

#endif // NPU_MLIR_SIREN_BLOB_MANAGER_H
