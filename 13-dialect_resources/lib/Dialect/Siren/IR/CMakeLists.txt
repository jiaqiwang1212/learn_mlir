add_mlir_dialect_library(NPUMLIRSirenCh13Dialect
  SirenDialect.cpp

  DEPENDS
  MLIRSirenCh13OpsIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRFuncDialect
  MLIRArithDialect
)
