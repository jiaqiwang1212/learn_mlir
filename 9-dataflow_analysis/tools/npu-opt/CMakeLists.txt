add_llvm_executable(ch-9-opt npu-opt.cpp)
target_link_libraries(ch-9-opt PRIVATE MLIROptLib MLIRTransforms NPUMLIRSirenTransforms)
mlir_check_all_link_libraries(ch-9-opt)

install(TARGETS ch-9-opt
  RUNTIME DESTINATION bin
)
