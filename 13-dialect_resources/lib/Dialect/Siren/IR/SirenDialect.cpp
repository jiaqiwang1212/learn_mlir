#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "mlir/IR/BuiltinTypes.h"
// SirenAttrs.h must come before SirenDialect.cpp.inc so the
// SirenDialectResourceBlobHandle alias is in scope for the generated code.
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.h"
#include "npu-mlir/Dialect/Siren/IR/SirenBlobManager.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.cpp.inc"
#include "npu-mlir/Dialect/Siren/IR/SirenOps.h"
#include "llvm/ADT/TypeSwitch.h"

#define GET_OP_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenOps.cpp.inc"

#define GET_ATTRDEF_CLASSES
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.cpp.inc"

namespace mlir::npu_mlir {

// The parser casts the dialect to OpAsmDialectInterface when it encounters
// {-# dialect_resources: { siren: { key: "0x..." } } #-}.
// This interface delegates storage to SirenResourceBlobManagerInterface.
struct SirenOpAsmInterface : public ::mlir::OpAsmDialectInterface {
  SirenOpAsmInterface(::mlir::Dialect *dialect,
                      SirenResourceBlobManagerInterface &mgr)
      : ::mlir::OpAsmDialectInterface(dialect), blobManager(mgr) {}

  std::string
  getResourceKey(const ::mlir::AsmDialectResourceHandle &handle) const override {
    return ::mlir::cast<SirenDialectResourceBlobHandle>(handle).getKey().str();
  }

  ::mlir::FailureOr<::mlir::AsmDialectResourceHandle>
  declareResource(::mlir::StringRef key) const final {
    return blobManager.insert(key);
  }

  ::mlir::LogicalResult
  parseResource(::mlir::AsmParsedResourceEntry &entry) const final {
    ::mlir::FailureOr<::mlir::AsmResourceBlob> blob = entry.parseAsBlob();
    if (failed(blob))
      return ::mlir::failure();
    blobManager.update(entry.getKey(), std::move(*blob));
    return ::mlir::success();
  }

  void buildResources(
      ::mlir::Operation *op,
      const ::llvm::SetVector<::mlir::AsmDialectResourceHandle>
          &referencedResources,
      ::mlir::AsmResourceBuilder &provider) const final {
    blobManager.buildResources(provider, referencedResources.getArrayRef());
  }

private:
  SirenResourceBlobManagerInterface &blobManager;
};

void SirenDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "npu-mlir/Dialect/Siren/IR/SirenOps.cpp.inc"
      >();

  addAttributes<
#define GET_ATTRDEF_LIST
#include "npu-mlir/Dialect/Siren/IR/SirenAttrs.cpp.inc"
      >();

  auto &blobInterface = addInterface<SirenResourceBlobManagerInterface>();
  addInterface<SirenOpAsmInterface>(blobInterface);
}

SirenDialect::~SirenDialect() = default;

} // namespace mlir::npu_mlir
