<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-14 | Updated: 2026-04-14 -->

# 3-define_op

## Purpose
Chapter 3: Add operations to the Siren dialect. Introduces `AddOp` and `SubOp` with full MLIR op lifecycle features: TableGen op definitions, custom `build()`, `verify()`, `canonicalize()`, and `fold()` hooks. Also introduces `SirenAttrs` and `SirenTypes` TableGen files alongside `SirenOps`.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Chapter build root |
| `include/.../IR/SirenOps.h` | Op header; includes TableGen-generated `SirenOps.h.inc` |
| `include/.../IR/SirenOps.td` | TableGen op definitions for `AddOp` and `SubOp` |
| `include/.../IR/SirenAttrs.td` | TableGen attribute definitions |
| `include/.../IR/SirenTypes.td` | TableGen type definitions |
| `include/.../TypeTrait/TypeTraits.h` | Custom C++ type trait header |
| `include/.../TypeTrait/NpuTypeTraits.td` | TableGen type traits |
| `lib/.../IR/SirenOps.cpp` | Op implementations: `printOpName`, `canonicalize`, `verify`, `build`, `fold` |
| `lib/.../IR/SirenDialect.cpp` | Dialect init (registers ops) |
| `test/siren.mlir` | MLIR integration test file |
| `test/test_liveness.mlir` | Liveness analysis test |
| `test/test.dot` | GraphViz output from dialect IR dump |

## Subdirectories

| Directory | Purpose |
|-----------|---------|
| `include/` | Public headers and TableGen sources |
| `include/npu-mlir/TypeTrait/` | Custom type trait definitions |
| `lib/` | Op and dialect implementations |
| `test/` | MLIR-level test files |
| `tools/` | `npu-mlir` and `npu-opt` drivers |

## For AI Agents

### Working In This Directory
- `AddOp`: binary op with `lhs`/`rhs` operands; result type matches `lhs` type.
- `SubOp`: implements `fold()` for constant-folding two `IntegerAttr` operands.
- When adding new ops, define them in `SirenOps.td` first, then implement in `SirenOps.cpp`.
- LLVM 22 API: use `Op::create(builder, loc, ...)` not `builder.create<Op>(...)`.

### Testing Requirements
```bash
./build_npu_mlir_fixed.sh ch-3
install/bin/ch-3-opt test/siren.mlir   # runs the MLIR test
```

### Common Patterns
- `SirenOps.cpp` pattern for custom build: populate `odsState.addOperands` and `odsState.addTypes`.
- `canonicalize` returns `llvm::failure()` if no transformation applied.
- `fold` returns `{}` (empty `OpFoldResult`) when folding is not possible.

## Dependencies

### Internal
- Builds on `2-define_dialect/` patterns for dialect registration

### External
- `mlir/IR/BuiltinAttributes.h` (for `IntegerAttr`, `APInt`)
- `llvm/Support/raw_ostream.h`

<!-- MANUAL: -->
