#!/bin/bash
set -e

TARGET="$1"  # 接收目标作为命令行参数


cd build

# 生成构建档
cmake -G $BUILD_SYSTEM .. \
    -DLLVM_DIR=$LLVM_DIR \
    -DMLIR_DIR=$MLIR_DIR \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH \

if [ -z $TARGET ]; then
    echo "Error: No target specified."
    exit 1
fi


# 构建指定目标
cmake --build . --target $TARGET
# 安装构建的目标
ninja install
