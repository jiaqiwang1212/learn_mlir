#!/bin/sh

mkdir -p $LLVM_BUILD_DIR
# mkdir -p $LLVM_INSTALL_DIR

cd $LLVM_BUILD_DIR

cmake ../llvm -G $BUILD_SYSTEM \
      -DLLVM_PARALLEL_COMPILE_JOBS=64 \
      -DLLVM_PARALLEL_LINK_JOBS=32 \
      -DLLVM_BUILD_EXAMPLES=OFF \
      -DCMAKE_BUILD_TYPE=Debug \
      -DLLVM_ENABLE_ASSERTIONS=ON \
      -DLLVM_CCACHE_BUILD=OFF \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DLLVM_ENABLE_PROJECTS="llvm;mlir" \
      -DLLVM_TARGETS_TO_BUILD="X86"

cmake --build . --target check-llvm check-mlir 
