#!/bin/bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
PROJECT_ROOT=$DIR
THIRDPARTY_LLVM_DIR=$PROJECT_ROOT/externals/llvm-project

BUILD_SYSTEM=Ninja
LLVM_BUILD_DIR=$THIRDPARTY_LLVM_DIR/build
LLVM_DIR=$THIRDPARTY_LLVM_DIR/build/lib/cmake/llvm
MLIR_DIR=$THIRDPARTY_LLVM_DIR/build/lib/cmake/mlir
INSTALL_PATH=${INSTALL_PATH:-$PROJECT_ROOT/install}

TARGET="$1"  # 接收目标作为命令行参数 (ch-2, ch-3, ..., ch-6)

if [ -z "$TARGET" ]; then
    echo "Error: No target specified."
    echo "Usage: $0 <ch-2|ch-3|ch-4|ch-5|ch-6|ch-8|ch-9|ch-10|ch-11>"
    exit 1
fi

mkdir -p "$PROJECT_ROOT/build"
cd "$PROJECT_ROOT/build"

# 生成构建文件
cmake -G "$BUILD_SYSTEM" .. \
    -DLLVM_DIR="$LLVM_DIR" \
    -DMLIR_DIR="$MLIR_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
    -DTARGET="$TARGET"

# 根据不同的 target 构建对应的目标
case "$TARGET" in
    ch-2)
        cmake --build . --target ch-2-opt ch-2 mlir-doc
        ;;
    ch-3)
        cmake --build . --target ch-3-opt ch-3 mlir-doc
        ;;
    ch-4)
        cmake --build . --target ch-4-opt ch-4 mlir-doc
        ;;
    ch-5)
        cmake --build . --target ch-5-opt ch-5 mlir-doc
        ;;
    ch-6)
        # ch-6 构建 test_traversal 而不是 ch-6-opt
        cmake --build . --target ch6-all-tests mlir-doc
        ;;
    ch-8)
        cmake --build . --target ch-8-opt ch-8 mlir-doc
        ;;
    ch-9)
        cmake --build . --target ch-9-opt mlir-doc
        ;;
    ch-10)
        cmake --build . --target ch-10-opt mlir-doc
        ;;
    ch-11)
        cmake --build . --target ch11-pdll-fold-demo ch11-pdll-strength-demo ch11-pdll-dce-demo mlir-doc
        ;;
    ch-12)
        cmake --build . --target ch12-test-transform-demo mlir-doc
        ;;
    *)
        echo "Error: Unknown target $TARGET"
        echo "Valid targets: ch-2, ch-3, ch-4, ch-5, ch-6, ch-8, ch-9, ch-10, ch-11, ch-12"
        exit 1
        ;;
esac

# 安装构建的目标
ninja install

echo "Build completed successfully for $TARGET"
