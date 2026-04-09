cmake_minimum_required(VERSION 3.20.0)

project(LPUC LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# https://llvm.org/docs/CMake.html
find_package(LLVM REQUIRED CONFIG)
find_package(MLIR REQUIRED CONFIG)

# For document
set(MLIR_BINARY_DIR ${CMAKE_INSTALL_PREFIX})

message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")

# 包含LLVM和MLIR的CMake模块
list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR} ${MLIR_CMAKE_DIR})

message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)
include(LLVMConfig)
include(MLIRConfig)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${MLIR_INCLUDE_DIRS})


# separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
# add_definitions(${LLVM_DEFINITIONS_LIST})
# message(STATUS "LLVM_DEFINITIONS: ${LLVM_DEFINITIONS_LIST}")



# Link against LLVM librarie
if(${TARGET} STREQUAL "ch-2")
    add_subdirectory(2-define_dialect)
elseif(${TARGET} STREQUAL "ch-3")
    add_subdirectory(3-define_op)
elseif(${TARGET} STREQUAL "ch-4")
    add_subdirectory(4-define_Interfaces)
elseif(${TARGET} STREQUAL "ch-5")
    add_subdirectory(5-define_Traits)
elseif(${TARGET} STREQUAL "ch-6")
    add_subdirectory(6-experiment)
elseif(${TARGET} STREQUAL "ch-8")
    add_subdirectory(8-define_Pass)
endif()
add_subdirectory(llvm)


# add_subdirectory(3-define_op)
# add_subdirectory(4-define_type)
# add_subdirectory(5-define_attribute)
