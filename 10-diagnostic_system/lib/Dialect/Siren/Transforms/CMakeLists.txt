add_mlir_dialect_library(NPUMLIRCh10DiagnosticTransforms
    CustomDiagnosticHandler.cpp
    DiagScopedPass.cpp
    DiagRegisteredPass.cpp

  ADDITIONAL_HEADER_DIRS
  ${NPU_MLIR_INCLUDE_DIRS}/npu-mlir/Dialect/Siren

  DEPENDS
  NPUMLIRCh10DiagnosticTransformsIncGen

  LINK_LIBS PUBLIC
  MLIRArithDialect
  MLIRFuncDialect
  MLIRIR
  MLIRPass
  MLIRSupport
  MLIRTransforms
  MLIRTransformUtils
)
