#ifndef NPU_MLIR_SIREN_ATTRS_TD
#define NPU_MLIR_SIREN_ATTRS_TD

include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/OpAsmInterface.td"
include "npu-mlir/Dialect/Siren/IR/SirenDialect.td"

// An attribute that holds an opaque reference to a named dialect resource blob.
// The blob bytes live in the {-# dialect_resources: { siren: { key: "0x..." } } #-}
// section of the .mlir file; the attribute just carries the handle (name).
def SirenWeightAttr : AttrDef<SirenDialect, "Weight", []> {
  let mnemonic = "weight";
  let summary = "Opaque reference to a named siren dialect resource blob";
  let parameters = (ins
    ResourceHandleParameter<"SirenDialectResourceBlobHandle">:$handle
  );
  let assemblyFormat = "`<` $handle `>`";
}

#endif // NPU_MLIR_SIREN_ATTRS_TD
