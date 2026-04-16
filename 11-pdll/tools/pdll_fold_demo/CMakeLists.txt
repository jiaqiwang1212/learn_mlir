# --- mlir-pdll guard ----------------------------------------------------------
# MLIR_PDLL_TABLEGEN_EXE is set by find_package(MLIR REQUIRED CONFIG) via
# MLIRConfig.cmake. It resolves to mlir-pdll in the LLVM build tree.
if(NOT MLIR_PDLL_TABLEGEN_EXE)
  message(FATAL_ERROR
    "MLIR_PDLL_TABLEGEN_EXE not set — mlir-pdll was not built.\n"
    "Fix: cd externals/llvm-project/build && ninja mlir-pdll")
endif()
message(STATUS "mlir-pdll: ${MLIR_PDLL_TABLEGEN_EXE}")

# --- PDLL -> .h.inc -----------------------------------------------------------
add_mlir_pdll_library(PDLLFoldPatternsIncGen
  fold_patterns.pdll
  FoldPatterns.h.inc
)

# --- Executable ---------------------------------------------------------------
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)

add_llvm_executable(ch11-pdll-fold-demo
  pdll_fold_demo.cpp
  PARTIAL_SOURCES_INTENDED
)

add_dependencies(ch11-pdll-fold-demo PDLLFoldPatternsIncGen)

target_include_directories(ch11-pdll-fold-demo PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(ch11-pdll-fold-demo PRIVATE
  MLIRArithDialect
  MLIRFuncDialect
  MLIRIR
  MLIRPDLDialect
  MLIRPDLInterpDialect
  MLIRRewrite
  MLIRTransformUtils
  MLIRTransforms
  ${conversion_libs}
)

mlir_check_all_link_libraries(ch11-pdll-fold-demo)
install(TARGETS ch11-pdll-fold-demo RUNTIME DESTINATION bin)
