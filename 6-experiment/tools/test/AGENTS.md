<!-- Parent: ../../AGENTS.md -->
<!-- Generated: 2026-04-14 | Updated: 2026-04-14 -->

# tools/test

## Purpose
Standalone C++ test executables for chapter 6. Each file is an independent `main()` program that builds MLIR IR programmatically and inspects it, without requiring any MLIR text file input. These are the primary experiment artifacts for ch-6.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Registers each `.cpp` as a named executable via `add_llvm_executable` |
| `test_for_yield.cpp` | **Primary test**: builds nested `scf.for` with `iter_args`, then traces each `ForOp` result back to the corresponding `YieldOp` operand at the same index. Validates MLIR's result↔yield correspondence rule. |
| `test_traversal.cpp` | Builds a module with Siren ops and exercises `walk()`, `getUsers()`, `getUses()` to traverse the IR graph |
| `test_example.cpp` | Minimal hello-world IR construction — good template for new experiments |
| `test_ADT.cpp` | Exercises LLVM ADT: `SmallVector`, `DenseMap`, `SetVector`, etc. No MLIR IR construction needed |

## For AI Agents

### Working In This Directory
- Each file is fully self-contained with its own `main()`. Do not share state between test files.
- LLVM 22 factory pattern is required throughout:
  ```cpp
  // Correct (LLVM 22+):
  auto op = SomeOp::create(builder, loc, ...);
  // Wrong (deprecated):
  auto op = builder.create<SomeOp>(loc, ...);
  ```
- `FuncOp::create(builder, loc, name, funcType)` inserts the op automatically — do **not** call `module.getBody()->push_back(funcOp)` after.
- ForOp body-builder lambda signature: `(OpBuilder&, Location, Value /*iv*/, ValueRange /*iterArgs*/)`.
- Always call `mlir::verify(*module)` before accessing results to catch IR errors.

### Adding a New Test
1. Create `test_myname.cpp` with a `main()` that registers needed dialects in `MLIRContext`.
2. Add to `CMakeLists.txt`:
   ```cmake
   add_llvm_executable(test_myname test_myname.cpp)
   target_link_libraries(test_myname PRIVATE MLIRSCFDialect MLIRArithDialect ...)
   install(TARGETS test_myname DESTINATION bin/test)
   ```
3. Build with `./build_npu_mlir_fixed.sh ch-6`, run from `install/bin/test/test_myname`.

### Testing Requirements
```bash
./build_npu_mlir_fixed.sh ch-6
install/bin/test/test_for_yield    # expected: prints result↔yield pairs
install/bin/test/test_traversal    # expected: prints op walk output
install/bin/test/test_ADT          # expected: ADT usage demo output
install/bin/test/test_example      # expected: example IR dump
```

### Common Patterns
```cpp
// Canonical MLIRContext setup for SCF tests:
mlir::MLIRContext context;
context.loadDialect<mlir::arith::ArithDialect,
                    mlir::func::FuncDialect,
                    mlir::scf::SCFDialect>();

// ForOp result ↔ YieldOp operand correspondence:
auto *terminator = forOp.getBody()->getTerminator();
for (auto [res, operand] : llvm::zip(forOp.getResults(), terminator->getOperands())) {
  // index-i result corresponds to index-i yield operand
}
```

## Dependencies

### Internal
- `6-experiment/include/npu-mlir/Dialect/Siren/IR/` (for Siren dialect tests if needed)

### External
- `MLIRSCFDialect`, `MLIRArithDialect`, `MLIRFuncDialect`
- `MLIRIRDialect`, `MLIRBuiltinToLLVMIRTranslation`
- `llvm/ADT/STLExtras.h` — `llvm::zip`
- `llvm/Support/raw_ostream.h`

<!-- MANUAL: -->
