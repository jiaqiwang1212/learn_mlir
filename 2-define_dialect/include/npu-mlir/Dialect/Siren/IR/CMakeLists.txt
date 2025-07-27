set(LLVM_TARGET_DEFINITIONS SirenDialect.td)
mlir_tablegen(SirenDialect.h.inc -gen-dialect-decls -dialect=siren)
mlir_tablegen(SirenDialect.cpp.inc -gen-dialect-defs -dialect=siren)

add_public_tablegen_target(MLIRSirenOpsIncGen)
