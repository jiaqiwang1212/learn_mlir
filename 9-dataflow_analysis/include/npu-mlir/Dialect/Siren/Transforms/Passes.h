//===- Passes.h - Pass Entrypoints ------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef NPU_MLIR_DIALECT_SIREN_TRANSFORMS_PASSES_H_
#define NPU_MLIR_DIALECT_SIREN_TRANSFORMS_PASSES_H_

#include "mlir/Pass/Pass.h"
#include "npu-mlir/Dialect/Siren/Transforms/ConstantPropAnalysis.h"

namespace mlir {
namespace npu_mlir {

//===----------------------------------------------------------------------===//
// Registration
//===----------------------------------------------------------------------===//

/// Generate the code for registering passes.
#define GEN_PASS_REGISTRATION
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"

} // namespace npu_mlir
} // namespace mlir

#endif // NPU_MLIR_DIALECT_SIREN_TRANSFORMS_PASSES_H_
