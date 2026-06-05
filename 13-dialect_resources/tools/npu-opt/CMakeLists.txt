add_llvm_executable(ch-13-opt npu-opt.cpp)
target_link_libraries(ch-13-opt PRIVATE NPUMLIRSirenCh13Dialect MLIROptLib MLIRTransforms)
mlir_check_all_link_libraries(ch-13-opt)

install(TARGETS ch-13-opt
  RUNTIME DESTINATION bin
)
