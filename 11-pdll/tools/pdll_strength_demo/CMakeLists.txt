# --- mlir-pdll guard ----------------------------------------------------------
if(NOT MLIR_PDLL_TABLEGEN_EXE)
  message(FATAL_ERROR
    "MLIR_PDLL_TABLEGEN_EXE not set — mlir-pdll was not built.\n"
    "Fix: cd externals/llvm-project/build && ninja mlir-pdll")
endif()
message(STATUS "mlir-pdll: ${MLIR_PDLL_TABLEGEN_EXE}")

# --- PDLL -> .h.inc -----------------------------------------------------------
add_mlir_pdll_library(PDLLStrengthPatternsIncGen
  strength_patterns.pdll
  StrengthPatterns.h.inc
)

# --- Executable ---------------------------------------------------------------
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)

add_llvm_executable(ch11-pdll-strength-demo
  pdll_strength_demo.cpp
  PARTIAL_SOURCES_INTENDED
)

add_dependencies(ch11-pdll-strength-demo PDLLStrengthPatternsIncGen)

target_include_directories(ch11-pdll-strength-demo PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(ch11-pdll-strength-demo PRIVATE
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

mlir_check_all_link_libraries(ch11-pdll-strength-demo)
install(TARGETS ch11-pdll-strength-demo RUNTIME DESTINATION bin)
