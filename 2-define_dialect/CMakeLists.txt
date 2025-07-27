include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include) # 这里很重要
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(tools)