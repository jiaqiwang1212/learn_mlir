set(LLVM_TARGET_DEFINITIONS SirenOps.td)
mlir_tablegen(SirenDialect.h.inc -gen-dialect-decls -dialect=siren)
mlir_tablegen(SirenDialect.cpp.inc -gen-dialect-defs -dialect=siren)

mlir_tablegen(SirenOps.h.inc -gen-op-decls)
mlir_tablegen(SirenOps.cpp.inc -gen-op-defs)

add_public_tablegen_target(MLIRSirenOpsIncGen)
