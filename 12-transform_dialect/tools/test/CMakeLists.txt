get_property(extension_libs GLOBAL PROPERTY MLIR_EXTENSION_LIBS)

# Transform dialect C++ demo — links against the full extension set so that
# registerAllDialects() / registerAllExtensions() work correctly at runtime.
set(transform_demo_libs
  MLIRRegisterAllDialects
  MLIRRegisterAllExtensions
  MLIRLinalgDialect
  MLIRTensorDialect
  MLIRTransformDialect
  MLIRTransformDialectTransforms
  MLIRParser
  ${extension_libs}
)

add_llvm_executable(ch12-test-transform-demo test_transform_demo.cpp PARTIAL_SOURCES_INTENDED)
target_link_libraries(ch12-test-transform-demo PRIVATE ${transform_demo_libs})
install(TARGETS ch12-test-transform-demo RUNTIME DESTINATION bin/test)

message(STATUS "Ch12 Transform dialect demo registered")
