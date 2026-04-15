add_mlir_dialect_library(NPUMLIRSirenTransforms
    ConstantPropAnalysis.cpp

  ADDITIONAL_HEADER_DIRS
  ${NPU_MLIR_INCLUDE_DIRS}/npu-mlir/Dialect/Siren

  DEPENDS
  NPUMLIRSirenTransformsIncGen

  LINK_LIBS PUBLIC
  MLIRAnalysis
  MLIRArithDialect
  MLIRFuncDialect
  MLIRIR
  MLIRPass
  MLIRTransforms
  MLIRTransformUtils
)
