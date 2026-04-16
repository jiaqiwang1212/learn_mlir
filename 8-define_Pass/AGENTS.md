<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-14 | Updated: 2026-04-14 -->

# 8-define_Pass

## Purpose
Chapter 8: Define MLIR transformation passes. Introduces the pass infrastructure with `TestPassOne` — a skeleton pass using the TableGen-driven `PassBase` pattern. Demonstrates `Passes.td`, the `GEN_PASS_REGISTRATION` macro, and how passes are registered with `mlir-opt`.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Chapter build root |
| `include/.../Transforms/Passes.td` | TableGen pass definitions (`def TestPassOne`) |
| `include/.../Transforms/Passes.h` | Pass registration header; uses `GEN_PASS_REGISTRATION` macro |
| `include/.../Transforms/TestPassOne.h` | Per-pass header |
| `lib/.../Transforms/TestPassOne.cpp` | Pass implementation: `runOnOperation()` skeleton |
| `include/.../IR/SirenOps.td` | Full op suite (AddOp, SubOp + attrs/types from ch-3) |
| `include/.../TypeTrait/` | Type traits (carried over from ch-3) |
| `test/siren.mlir` | Integration test |

## Subdirectories

| Directory | Purpose |
|-----------|---------|
| `include/` | Headers and TableGen sources |
| `include/npu-mlir/Dialect/Siren/IR/` | Dialect, ops, attrs, types headers |
| `include/npu-mlir/Dialect/Siren/Transforms/` | Pass headers and TableGen |
| `include/npu-mlir/TypeTrait/` | Type trait headers |
| `lib/` | Implementations |
| `lib/Dialect/Siren/IR/` | Dialect and op implementations |
| `lib/Dialect/Siren/Transforms/` | Pass implementations |
| `test/` | MLIR test files |
| `tools/` | Driver executables |

## For AI Agents

### Working In This Directory
- Pass implementation pattern: subclass `impl::TestPassOneBase<TestPassOne>`, override `runOnOperation()`.
- `Passes.td` uses `def TestPassOne : Pass<"test-pass-one", "mlir::func::FuncOp">` idiom.
- The `GEN_PASS_DEF_TESTPASSONE` macro in `TestPassOne.cpp` pulls in the generated base class.
- Registration: `#define GEN_PASS_REGISTRATION` + include `Passes.h.inc` in `Passes.h`.

### Testing Requirements
```bash
./build_npu_mlir_fixed.sh ch-8
install/bin/ch-8-opt --test-pass-one test/siren.mlir
```

### Common Patterns
```cpp
// TestPassOne.cpp skeleton:
#define GEN_PASS_DEF_TESTPASSONE
#include "npu-mlir/Dialect/Siren/Transforms/Passes.h.inc"
class TestPassOne : public impl::TestPassOneBase<TestPassOne> {
  void runOnOperation() override { /* transformation logic */ }
};
```

## Dependencies

### Internal
- Full Siren dialect (ops, attrs, types from ch-3 equivalent)
- `mlir/Dialect/Arith/IR/Arith.h`, `mlir/Dialect/Func/IR/FuncOps.h`

### External
- `mlir/Pass/Pass.h`

<!-- MANUAL: -->
