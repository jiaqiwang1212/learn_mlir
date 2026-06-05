#ifndef NPU_MLIR_SIREN_OPS_TD
#define NPU_MLIR_SIREN_OPS_TD

include "mlir/IR/OpBase.td"
include "npu-mlir/Dialect/Siren/IR/SirenDialect.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "npu-mlir/Dialect/Siren/IR/SirenAttrs.td"

// Loads a weight tensor from a named dialect resource blob.
// The {-# dialect_resources: { siren: { <name>: "0x..." } } #-} section
// of the .mlir file carries the actual bytes; this op holds the handle.
def Siren_WeightOp : Op<SirenDialect, "weight", []> {
  let summary = "Load a weight from a named siren dialect resource blob";
  let description = [{
    References a named resource blob via its handle and produces an i8 result.
    The actual bytes live in the `{-# dialect_resources #-}` preamble of the
    enclosing .mlir file; this op only holds the opaque handle (the key name).

    Example:
    ```mlir
    {-# dialect_resources: { siren: { w0: "0x04000000DEADBEEF" } } #-}
    %0 = siren.weight <w0> : i8
    ```
  }];
  let arguments = (ins SirenWeightAttr:$weight);
  let results   = (outs I8:$result);
  let assemblyFormat = "$weight attr-dict `:` type($result)";
}

#endif // NPU_MLIR_SIREN_OPS_TD
