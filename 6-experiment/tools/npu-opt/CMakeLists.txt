# get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
# get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
# set(libs
#   ${dialect_libs}
#   ${conversion_libs}
#   NPUMLIRSirenDialect
# )
add_llvm_executable(ch-6-opt npu-opt.cpp)
target_link_libraries(ch-6-opt PRIVATE NPUMLIRSirenDialect MLIROptLib MLIRTransforms)
mlir_check_all_link_libraries(ch-3-opt)

install(TARGETS ch-6-opt
  RUNTIME DESTINATION bin
)