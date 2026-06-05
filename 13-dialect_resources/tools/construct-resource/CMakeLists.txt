add_llvm_executable(construct-resource construct-resource.cpp)
target_link_libraries(construct-resource PRIVATE
  NPUMLIRSirenCh13Dialect
  MLIRFuncDialect
  MLIRIR)
mlir_check_all_link_libraries(construct-resource)

install(TARGETS construct-resource
  RUNTIME DESTINATION bin
)
