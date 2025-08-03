set(LLVM_TARGET_DEFINITIONS SirenOps.td)
mlir_tablegen(SirenDialect.h.inc -gen-dialect-decls -dialect=siren)
mlir_tablegen(SirenDialect.cpp.inc -gen-dialect-defs -dialect=siren)

mlir_tablegen(SirenOps.h.inc -gen-op-decls)
mlir_tablegen(SirenOps.cpp.inc -gen-op-defs)
add_public_tablegen_target(MLIRSirenOpsIncGen)

# 这个等价于上面的命令,只是生成的文件名有所区别
# add_mlir_dialect(SirenOps siren)

# 生成文档
add_mlir_doc(SirenDialect SirenDialect Dialects/Siren/ -gen-dialect-doc)
add_mlir_doc(SirenOps SirenOps Dialects/Siren/ -gen-op-doc)
