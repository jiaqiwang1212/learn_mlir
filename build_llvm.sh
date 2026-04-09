#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
PROJECT_ROOT=$DIR
THIRDPARTY_LLVM_DIR=$PROJECT_ROOT/externals/llvm-project

BUILD_SYSTEM=Ninja
LLVM_BUILD_DIR=$THIRDPARTY_LLVM_DIR/build

case "$(uname -m)" in
  x86_64)  LLVM_TARGET="X86" ;;
  arm64|aarch64) LLVM_TARGET="AArch64" ;;
  *) echo "Unsupported architecture: $(uname -m)"; exit 1 ;;
esac

mkdir -p $LLVM_BUILD_DIR
# mkdir -p $LLVM_INSTALL_DIR

cd $LLVM_BUILD_DIR

cmake ../llvm -G $BUILD_SYSTEM \
      -DLLVM_PARALLEL_COMPILE_JOBS=10 \
      -DLLVM_PARALLEL_LINK_JOBS=4 \
      -DLLVM_BUILD_EXAMPLES=ON \
      -DCMAKE_BUILD_TYPE=Debug \
      -DLLVM_ENABLE_ASSERTIONS=ON \
      -DLLVM_CCACHE_BUILD=OFF \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DLLVM_ENABLE_PROJECTS="llvm;mlir" \
      -DLLVM_TARGETS_TO_BUILD="$LLVM_TARGET"

cmake --build .
cmake --build . --target check-llvm check-mlir
