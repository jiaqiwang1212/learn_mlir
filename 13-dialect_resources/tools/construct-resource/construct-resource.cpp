// Demo: construct a module with a dialect resource blob via the C++ API and
// print it.
//
// Run:  install/bin/construct-resource
//
// This shows the inverse of parsing: the blob is created programmatically,
// embedded into a WeightAttr, and the printer emits the {-# dialect_resources
// #-} section automatically because the op references the blob handle.

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/AsmState.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/MLIRContext.h"
#include "npu-mlir/Dialect/Siren/IR/SirenBlobManager.h"
#include "npu-mlir/Dialect/Siren/IR/SirenDialect.h"
#include "npu-mlir/Dialect/Siren/IR/SirenOps.h"
#include "llvm/Support/raw_ostream.h"

int main() {
  mlir::MLIRContext ctx;
  ctx.loadDialect<mlir::npu_mlir::SirenDialect, mlir::func::FuncDialect>();

  // Retrieve the blob manager registered with the siren dialect.
  auto *dialect = ctx.getLoadedDialect<mlir::npu_mlir::SirenDialect>();
  auto *blobMgr = dialect->getRegisteredInterface<
      mlir::npu_mlir::SirenResourceBlobManagerInterface>();

  // Create the blob: two little-endian uint32 values (8 bytes).
  std::array<uint32_t, 2> payload = {0xDEADBEEFu, 0x12345678u};
  auto blob = mlir::HeapAsmResourceBlob::allocateAndCopyWithAlign(
      llvm::ArrayRef<char>(reinterpret_cast<const char *>(payload.data()),
                           payload.size() * sizeof(uint32_t)),
      alignof(uint32_t));

  // Insert the blob under a named key; the returned handle is later embedded
  // in WeightAttr so the printer knows which blob entry to emit.
  mlir::npu_mlir::SirenDialectResourceBlobHandle handle =
      blobMgr->insert("api_weight", std::move(blob));

  // Build:  func.func @demo() -> i8 { %0 = siren.weight <api_weight> : i8 }
  mlir::OpBuilder builder(&ctx);
  auto loc = builder.getUnknownLoc();
  auto module = mlir::ModuleOp::create(loc);

  auto funcType = mlir::FunctionType::get(&ctx, {}, {builder.getI8Type()});
  builder.setInsertionPointToEnd(module.getBody());
  auto func = mlir::func::FuncOp::create(builder, loc, "demo", funcType);
  auto *block = func.addEntryBlock();
  builder.setInsertionPointToEnd(block);

  auto weightAttr = mlir::npu_mlir::WeightAttr::get(&ctx, handle);
  auto weightOp = mlir::npu_mlir::WeightOp::create(
      builder, loc, builder.getI8Type(), weightAttr);
  mlir::func::ReturnOp::create(builder, loc, weightOp.getResult());

  // The OpAsmInterface emits {-# dialect_resources #-} for all referenced
  // blobs.
  module.print(llvm::outs());
  llvm::outs() << "\n";
  return 0;
}
