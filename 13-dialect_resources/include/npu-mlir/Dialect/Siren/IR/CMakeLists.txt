set(LLVM_TARGET_DEFINITIONS SirenOps.td)
mlir_tablegen(SirenDialect.h.inc -gen-dialect-decls -dialect=siren)
mlir_tablegen(SirenDialect.cpp.inc -gen-dialect-defs -dialect=siren)

mlir_tablegen(SirenOps.h.inc -gen-op-decls)
mlir_tablegen(SirenOps.cpp.inc -gen-op-defs)

mlir_tablegen(SirenAttrs.h.inc -gen-attrdef-decls -attrdefs-dialect=siren)
mlir_tablegen(SirenAttrs.cpp.inc -gen-attrdef-defs -attrdefs-dialect=siren)
add_public_tablegen_target(MLIRSirenCh13OpsIncGen)

add_mlir_doc(SirenDialect SirenDialect Dialects/Siren/ -gen-dialect-doc)
add_mlir_doc(SirenOps SirenOps Dialects/Siren/ -gen-op-doc)
