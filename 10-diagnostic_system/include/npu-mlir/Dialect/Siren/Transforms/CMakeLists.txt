set(LLVM_TARGET_DEFINITIONS Passes.td)
mlir_tablegen(Passes.h.inc -gen-pass-decls -name Ch10Diagnostic)
add_public_tablegen_target(NPUMLIRCh10DiagnosticTransformsIncGen)

add_mlir_doc(Passes Ch10DiagnosticPasses ./ -gen-pass-doc)
