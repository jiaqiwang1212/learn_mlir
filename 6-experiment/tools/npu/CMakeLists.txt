get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
# get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
message(STATUS "dialect_libs: ${dialect_libs}")
# set(libs
#   ${dialect_libs}
#   ${conversion_libs}
#   NPUMLIRSirenDialect
# )
add_llvm_executable(ch-6 npu-mlir.cpp)
# target_link_libraries(ch-2 PRIVATE ${libs})
target_link_libraries(ch-6 PRIVATE NPUMLIRSirenDialect MLIRTransforms ${dialect_libs})

install(TARGETS ch-6
  RUNTIME DESTINATION bin
)