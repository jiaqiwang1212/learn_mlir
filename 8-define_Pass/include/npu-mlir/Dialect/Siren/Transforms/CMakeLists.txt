set(LLVM_TARGET_DEFINITIONS Passes.td)
# 为什么会生成两份代码？
mlir_tablegen(Passes.h.inc -gen-pass-decls -name Siren)
add_public_tablegen_target(NPUMLIRSirenTransformsIncGen)

add_mlir_doc(Passes SirenPasses ./ -gen-pass-doc)
