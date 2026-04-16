<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-14 | Updated: 2026-04-14 -->

# 6-experiment

## Purpose
Chapter 6: Free-form MLIR experiments beyond dialect definition. This chapter adds standalone C++ test executables that programmatically build MLIR IR and inspect it — covering SCF `ForOp`/`YieldOp` result↔yield correspondence, IR traversal patterns, LLVM ADT usage, and basic IR construction examples. The Siren dialect here mirrors ch-5 but is used primarily as a host context for experimentation.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Chapter build root; links `6-experiment` subtargets |
| `tools/test/test_for_yield.cpp` | **Hot path**: builds nested `scf.for` IR, traces result↔yield operand correspondence via `getDefiningOp()` + `llvm::zip` |
| `tools/test/test_traversal.cpp` | Walks a module's op tree using `walk()` and `getUsers()` patterns |
| `tools/test/test_example.cpp` | Minimal IR construction example |
| `tools/test/test_ADT.cpp` | Exercises LLVM ADT utilities (SmallVector, DenseMap, etc.) |
| `tools/test/CMakeLists.txt` | Registers each test as a standalone executable |
| `test/siren.mlir` | Siren dialect integration test |
| `test/test_for_yield.mlir` | (if present) MLIR-level for/yield test |
| `test/test_traversal.mlir` | IR traversal test in MLIR text format |
| `test/users_and_uses.mlir` | Demonstrates Value users/uses in MLIR IR |
| `test/check.sh` | Shell script running FileCheck-style checks |

## Subdirectories

| Directory | Purpose |
|-----------|---------|
| `include/` | Siren dialect headers (same structure as ch-5) |
| `lib/` | Siren dialect implementation |
| `tools/npu/` | `npu-mlir` compiler driver |
| `tools/npu-opt/` | `npu-opt` mlir-opt driver with Siren registered |
| `tools/test/` | **Standalone C++ test executables** (see `tools/test/AGENTS.md`) |
| `test/` | MLIR-format test files run via `npu-opt` or `check.sh` |

## For AI Agents

### Working In This Directory
- Test executables in `tools/test/` are the main deliverables for this chapter; they link against MLIR SCF, Arith, Func dialects directly.
- LLVM 22 API: use `Op::create(builder, loc, ...)` static factory everywhere (not `builder.create<Op>`).
- When building IR programmatically, call `mlir::verify(*module)` after construction to catch errors early.

### Building and Running Tests
```bash
./build_npu_mlir_fixed.sh ch-6
install/bin/test/test_for_yield
install/bin/test/test_traversal
install/bin/test/test_ADT
install/bin/test/test_example
```

### Common Patterns
```cpp
// ForOp result ↔ yield operand zip pattern:
auto *terminator = forOp.getBody()->getTerminator();
for (auto [result, yieldOperand] :
     llvm::zip(forOp.getResults(), terminator->getOperands())) {
  // result at index i corresponds to yieldOperand at index i
}

// Walk ops in a module:
module->walk([](mlir::Operation *op) {
  llvm::outs() << op->getName() << "\n";
});
```

## Dependencies

### Internal
- Siren dialect headers in `include/npu-mlir/Dialect/Siren/IR/`

### External
- `mlir/Dialect/SCF/IR/SCF.h` — `scf::ForOp`, `scf::YieldOp`
- `mlir/Dialect/Arith/IR/Arith.h` — `arith::ConstantIndexOp`
- `mlir/Dialect/Func/IR/FuncOps.h` — `func::FuncOp`, `func::ReturnOp`
- `mlir/IR/Builders.h`, `mlir/IR/BuiltinOps.h`, `mlir/IR/Verifier.h`
- `llvm/ADT/STLExtras.h` — `llvm::zip`

<!-- MANUAL: -->
