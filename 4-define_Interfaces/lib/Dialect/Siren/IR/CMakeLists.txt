add_mlir_dialect_library(NPUMLIRSirenDialect
  SirenDialect.cpp
  SirenOps.cpp
  # ADDITIONAL_HEADER_DIRS
  # ${NPU_MLIR_INCLUDE_DIRS}/npu-mlir/Dialect/Siren

  DEPENDS
  MLIRSirenOpsIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRFuncDialect
  MLIRArithDialect
)