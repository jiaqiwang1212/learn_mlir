<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-14 | Updated: 2026-04-14 -->

# 5-define_Traits

## Purpose
Chapter 5: Add op traits to the Siren dialect. Introduces `RequireTwoOperandsOneResult` — a custom C++ trait that enforces a structural contract on any op that declares it. Demonstrates MLIR's compile-time trait verification mechanism.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Chapter build root |
| `include/.../Traits/RequireTwoOperandsOneResult.h` | Custom C++ trait implementation |
| `include/.../Traits/NpuOpTraits.td` | TableGen wrappers for custom traits |
| `include/.../Interfaces/NpuOpInterface.td` | Interface definitions (carried over from ch-4) |
| `include/.../Interfaces/NpuOpInterface.h` | Interface header |
| `include/.../IR/SirenOps.td` | Ops updated to declare the custom trait |
| `lib/Interfaces/NpuOpInterface.cpp` | Interface implementations |
| `lib/.../IR/SirenOps.cpp` | Op implementations |
| `test/siren.mlir` | Integration test |

## Subdirectories

| Directory | Purpose |
|-----------|---------|
| `include/` | Headers and TableGen sources |
| `include/npu-mlir/Traits/` | Custom trait header and TableGen wrapper |
| `include/npu-mlir/Interfaces/` | Interfaces from ch-4 |
| `include/npu-mlir/Dialect/Siren/IR/` | Dialect/op headers |
| `lib/` | Implementations |
| `test/` | MLIR test files |
| `tools/` | Driver executables |

## For AI Agents

### Working In This Directory
- Traits are checked at `Op::verify()` time; `RequireTwoOperandsOneResult` asserts operand/result counts.
- Traits differ from interfaces: traits are checked statically; interfaces provide dynamic dispatch.
- The `.td` wrapper in `NpuOpTraits.td` uses `NativeTrait` to expose the C++ trait to TableGen.

### Testing Requirements
```bash
TARGET=ch-5 ./build_npu_mlir_fixed.sh
```

### Common Patterns
- Custom trait: inherit from `mlir::OpTrait::TraitBase<ConcreteType, TraitName>`.
- Declare on ops in `.td`: `let traits = [NpuRequireTwoOperandsOneResult]`.

## Dependencies

### Internal
- Extends ch-4 with traits alongside existing interfaces

### External
- `mlir/IR/OpDefinition.h`

<!-- MANUAL: -->
