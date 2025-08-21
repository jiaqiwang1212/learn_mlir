add_mlir_library(MLIRNpuOpInterface
  NpuOpInterface.cpp


  DEPENDS
  MLIRNpuOpInterfaceIncGen

  LINK_LIBS PUBLIC
  MLIRIR
)