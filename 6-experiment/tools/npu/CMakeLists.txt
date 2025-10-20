get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
get_property(extension_libs GLOBAL PROPERTY MLIR_EXTENSION_LIBS)
get_property(translation_libs GLOBAL PROPERTY MLIR_TRANSLATION_LIBS)
message(STATUS "dialect_libs: ${dialect_libs}")
message(STATUS "conversion_libs: ${conversion_libs}")
message(STATUS "extension_libs: ${extension_libs}")
message(STATUS "translation_libs: ${translation_libs}")
set(libs
  MLIRSCFDialect
  ${conversion_libs}
  NPUMLIRSirenDialect
)
add_llvm_executable(ch-6 npu-mlir.cpp)
# target_link_libraries(ch-2 PRIVATE ${libs})
target_link_libraries(ch-6 PRIVATE NPUMLIRSirenDialect MLIRTransforms ${dialect_libs})

install(TARGETS ch-6
  RUNTIME DESTINATION bin
)