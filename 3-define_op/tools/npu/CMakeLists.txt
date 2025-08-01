# get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
# get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
# message(STATUS "dialect_libs: ${dialect_libs}")
# set(libs
#   ${dialect_libs}
#   ${conversion_libs}
#   NPUMLIRSirenDialect
# )
add_llvm_executable(ch-3 npu-mlir.cpp)
# target_link_libraries(ch-2 PRIVATE ${libs})
target_link_libraries(ch-3 PRIVATE NPUMLIRSirenDialect)

install(TARGETS ch-3
  RUNTIME DESTINATION bin
)