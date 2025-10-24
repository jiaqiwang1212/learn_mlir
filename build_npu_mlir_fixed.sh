#!/bin/bash
set -e

TARGET="$1"  # 接收目标作为命令行参数 (ch-2, ch-3, ..., ch-6)

if [ -z "$TARGET" ]; then
    echo "Error: No target specified."
    echo "Usage: $0 <ch-2|ch-3|ch-4|ch-5|ch-6>"
    exit 1
fi

# 确保环境变量已设置
if [ -z "$BUILD_SYSTEM" ]; then
    echo "Error: Environment variables not set. Please run: source env_setup.sh"
    exit 1
fi

cd build

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
        cmake --build . --target ch-2-opt mlir-doc
        ;;
    ch-3)
        cmake --build . --target ch-3-opt mlir-doc
        ;;
    ch-4)
        cmake --build . --target ch-4-opt mlir-doc
        ;;
    ch-5)
        cmake --build . --target ch-5-opt mlir-doc
        ;;
    ch-6)
        # ch-6 构建 test_traversal 而不是 ch-6-opt
        cmake --build . --target ch6-all-tests mlir-doc
        ;;
    *)
        echo "Error: Unknown target $TARGET"
        echo "Valid targets: ch-2, ch-3, ch-4, ch-5, ch-6"
        exit 1
        ;;
esac

# 安装构建的目标
ninja install

echo "Build completed successfully for $TARGET"
