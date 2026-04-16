get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)

add_llvm_executable(ch10-diag-demo diag_demo.cpp PARTIAL_SOURCES_INTENDED)
target_link_libraries(ch10-diag-demo PRIVATE
  MLIRFuncDialect
  MLIRArithDialect
  ${conversion_libs}
)
mlir_check_all_link_libraries(ch10-diag-demo)

install(TARGETS ch10-diag-demo RUNTIME DESTINATION bin)

add_llvm_executable(ch10-handler-demo handler_demo.cpp PARTIAL_SOURCES_INTENDED)
target_link_libraries(ch10-handler-demo PRIVATE
  MLIRFuncDialect
  MLIRArithDialect
  MLIRParser
  ${conversion_libs}
)
mlir_check_all_link_libraries(ch10-handler-demo)

install(TARGETS ch10-handler-demo RUNTIME DESTINATION bin)
