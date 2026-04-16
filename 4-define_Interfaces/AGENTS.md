<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-14 | Updated: 2026-04-14 -->

# 4-define_Interfaces

## Purpose
Chapter 4: Attach MLIR interfaces to the Siren ops. Introduces `NpuOpInterface` (a TableGen-defined op interface) and `NpuDialectInterface` (a C++ dialect interface). Demonstrates how interfaces decouple algorithms from specific op types via dynamic dispatch.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Chapter build root |
| `include/.../Interfaces/NpuOpInterface.td` | TableGen definition of `NpuOpInterface` |
| `include/.../Interfaces/NpuOpInterface.h` | C++ interface header; includes TableGen-generated `.inc` |
| `include/.../Interfaces/NpuDialectInterface.h` | C++ dialect-level interface |
| `include/.../IR/SirenOps.h` | Siren ops updated to declare interface implementations |
| `include/.../IR/SirenOps.td` | Ops updated to reference `NpuOpInterface` |
| `lib/Interfaces/NpuOpInterface.cpp` | Interface method implementations |
| `lib/.../IR/SirenOps.cpp` | Op implementations including interface methods |
| `test/siren.mlir` | Integration test |
| `test/test_liveness.mlir` | Liveness test |

## Subdirectories

| Directory | Purpose |
|-----------|---------|
| `include/` | Headers and TableGen sources |
| `include/npu-mlir/Interfaces/` | Interface definitions (`.td` and `.h`) |
| `include/npu-mlir/Dialect/Siren/IR/` | Dialect/op headers |
| `lib/` | Implementations |
| `lib/Interfaces/` | Interface method bodies |
| `test/` | MLIR test files |
| `tools/` | Driver executables |

## For AI Agents

### Working In This Directory
- `NpuOpInterface` is declared in `.td` with methods; the generated `.inc` provides the C++ boilerplate.
- Ops declare interface conformance in their `.td` definition via `let interfaces = [NpuOpInterface]`.
- `NpuDialectInterface` is a pure C++ interface (no TableGen) for dialect-level queries.

### Testing Requirements
```bash
./build_npu_mlir_fixed.sh ch-4
```

### Common Patterns
- Interface `.td`: use `def NpuOpInterface : OpInterface<"NpuOpInterface">` with `methods` list.
- Header pattern: `#include "NpuOpInterface.h.inc"` after forward declarations.
- Interface implementations go in `lib/Interfaces/NpuOpInterface.cpp`, NOT in `SirenOps.cpp`.

## Dependencies

### Internal
- Extends `3-define_op/` Siren ops with interface conformance

### External
- `mlir/IR/OpDefinition.h`

<!-- MANUAL: -->
