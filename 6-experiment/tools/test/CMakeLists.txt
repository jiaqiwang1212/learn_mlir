get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
get_property(extension_libs GLOBAL PROPERTY MLIR_EXTENSION_LIBS)
get_property(translation_libs GLOBAL PROPERTY MLIR_TRANSLATION_LIBS)


# 定义一个列表来跟踪所有测试可执行文件
set(TEST_EXECUTABLES)

# ===== 添加测试可执行文件的宏 =====
# PARTIAL_SOURCES_INTENDED 允许同一目录下有多个独立的可执行文件
macro(add_test_executable name source)
  add_llvm_executable(${name} ${source} PARTIAL_SOURCES_INTENDED)
  target_link_libraries(${name} PRIVATE MLIRTransforms)
  install(TARGETS ${name} RUNTIME DESTINATION bin/test)
  list(APPEND TEST_EXECUTABLES ${name})
endmacro()

# ===== 在这里添加所有测试 =====
add_test_executable(test_traversal test_traversal.cpp)
add_test_executable(test_example test_example.cpp)


# ===== 创建聚合目标：构建所有测试 =====
add_custom_target(ch6-all-tests DEPENDS ${TEST_EXECUTABLES})

# 打印信息，方便调试
message(STATUS "Ch6 Test Executables: ${TEST_EXECUTABLES}")
