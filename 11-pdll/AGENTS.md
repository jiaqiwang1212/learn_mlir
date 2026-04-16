# Ch-11 PDLL — Agent Guide

## Purpose

Ch-11 teaches MLIR PDLL (Pattern Definition Language) through three progressive standalone demo executables targeting `arith` dialect ops. No custom dialect is defined — the chapter is tools-only.

## Build

```bash
# From repo root
TARGET=ch-11 ./build_npu_mlir_fixed.sh

# Binaries installed to:
install/bin/ch11-pdll-fold-demo
install/bin/ch11-pdll-strength-demo
install/bin/ch11-pdll-dce-demo
```

## Directory Structure

```
11-pdll/
  CMakeLists.txt              ← routes TARGET=ch-11 to tools/
  README.md                   ← PDLL concepts + DRR comparison
  AGENTS.md                   ← this file
  tools/
    CMakeLists.txt            ← three add_subdirectory calls
    pdll_fold_demo/
      CMakeLists.txt          ← add_mlir_pdll_library + add_llvm_executable
      fold_patterns.pdll      ← PDLL pattern: replace addi(c,c) with lhs
      pdll_fold_demo.cpp      ← driver: build IR, apply patterns, print
    pdll_strength_demo/
      CMakeLists.txt
      strength_patterns.pdll  ← PDLL pattern: muli(x,2) → addi(x,x)
      pdll_strength_demo.cpp  ← driver
    pdll_dce_demo/
      CMakeLists.txt
      dce_patterns.pdll       ← PDLL pattern: erase dead arith.constant
      pdll_dce_demo.cpp       ← driver
```

## Key Technical Facts

- **mlir-pdll binary**: `externals/llvm-project/build/bin/mlir-pdll` (NOT `install/bin/`)
- **CMake variable**: `MLIR_PDLL_TABLEGEN_EXE` set by `find_package(MLIR REQUIRED CONFIG)`
- **Generated headers**: each `.pdll` → `.h.inc` in `${CMAKE_CURRENT_BINARY_DIR}`
- **Serialization**: `add_dependencies(target PDLLTablegenTarget)` ensures `.h.inc` exists before `.cpp` compiles
- **LLVM 22 API**: use `Op::create(builder, loc, ...)` static factory — never `builder.create<Op>(...)` (deprecated)

## PDLL Concepts Covered

| Demo | PDLL Feature |
|------|-------------|
| pdll_fold_demo | Pattern block, op matching, replace action |
| pdll_strength_demo | Native C++ constraint `[{ }]`, operand capture, inline op creation |
| pdll_dce_demo | IsUnused constraint, erase action, use_empty safety |

## Expected Output

```
# pdll_fold_demo
=== Before: addi(%c3, %c4) present
=== After:  addi gone, function returns %c3

# pdll_strength_demo
=== Before: muli(%x, %c2) present
=== After:  addi(%x, %x) present, muli gone

# pdll_dce_demo
=== Before: two arith.constant ops in function body
=== After:  function body empty (both constants erased)
```
