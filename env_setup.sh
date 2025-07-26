#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PROJECT_ROOT=$DIR

THIRDPARTY_LLVM_DIR=$PROJECT_ROOT/externals/llvm-project

export BUILD_SYSTEM=Ninja
export LLVM_BUILD_DIR=$THIRDPARTY_LLVM_DIR/build
export LLVM_DIR=$THIRDPARTY_LLVM_DIR/build/lib/cmake/llvm
export MLIR_DIR=$THIRDPARTY_LLVM_DIR/build/lib/cmake/mlir
# export LLVM_INSTALL_DIR=$THIRDPARTY_LLVM_DIR/install

export INSTALL_PATH=${INSTALL_PATH:-$PROJECT_ROOT/install}
# export PYTHONPATH=$INSTALL_PATH/python_packages:$PYTHONPATH
# export PYTHONPATH=$PROJECT_ROOT/python:$PYTHONPATH

# export PATH=$INSTALL_PATH/bin:$PATH

