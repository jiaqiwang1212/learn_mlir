add_llvm_executable(ch-10-opt npu-opt.cpp)
target_link_libraries(ch-10-opt PRIVATE MLIROptLib MLIRTransforms NPUMLIRCh10DiagnosticTransforms)
mlir_check_all_link_libraries(ch-10-opt)

install(TARGETS ch-10-opt
  RUNTIME DESTINATION bin
)
