<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-16 | Updated: 2026-04-16 -->

# 12-transform_dialect

## Purpose
Dedicated chapter for the MLIR Transform dialect — an embedded DSL that expresses
compiler transformations as first-class IR operations. The payload IR and the
transform schedule live in the same module; the `--transform-interpreter` pass
executes the schedule.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Routes to `tools/` subdirectory |
| `tools/test/CMakeLists.txt` | Builds `ch12-test-transform-demo` executable |
| `tools/test/test_transform_demo.cpp` | C++ driver: parse module, call `applyTransforms()` |
| `test/test_transform_basic.mlir` | Demo 1: match, split_handle, cast, emit_remark_at |
| `test/test_transform_structured.mlir` | Demo 2: tile_using_forall, fuse_into_containing_op |
| `test/test_transform_loop.mlir` | Demo 3: tile then loop.outline, apply_patterns |
| `test/test_transform_passes.mlir` | Demo 4: apply_cse, apply_patterns, apply_registered_pass |

## Running the .mlir Tests

The `.mlir` files use `// RUN: mlir-opt %s --transform-interpreter` directives.
Run them directly with the mlir-opt binary from the LLVM build tree:

```bash
# From repo root
BUILD_MLIR_OPT=externals/llvm-project/build/bin/mlir-opt

$BUILD_MLIR_OPT 12-transform_dialect/test/test_transform_basic.mlir \
    --transform-interpreter 2>&1

$BUILD_MLIR_OPT 12-transform_dialect/test/test_transform_structured.mlir \
    --transform-interpreter 2>&1

$BUILD_MLIR_OPT 12-transform_dialect/test/test_transform_loop.mlir \
    --transform-interpreter 2>&1

$BUILD_MLIR_OPT 12-transform_dialect/test/test_transform_passes.mlir \
    --transform-interpreter 2>&1
```

## Building the C++ Demo

```bash
./build_npu_mlir_fixed.sh ch-12
# Binary lands at: install/bin/test/ch12-test-transform-demo
```

## Concepts Covered

| Demo | Key ops / APIs |
|------|----------------|
| Basic wiring | `transform.named_sequence`, `transform.with_named_sequence`, `structured.match`, `split_handle`, `cast`, `debug.emit_remark_at` |
| Structured transforms | `structured.tile_using_forall`, `structured.fuse_into_containing_op`, handle invalidation rules |
| Loop transforms | `loop.outline`, `apply_patterns.canonicalization`, two-stage tiling |
| Pass integration | `apply_cse`, `apply_patterns`, `apply_registered_pass` (consumes handle) |
| C++ integration | `applyTransforms()`, `registerAllDialects()`, `registerAllExtensions()`, `ScopedDiagnosticHandler` |

## For AI Agents

- The `.mlir` tests are self-contained — no dialect headers, no build step needed.
- `test_transform_demo.cpp` uses `mlir::registerAllDialects` + `mlir::registerAllExtensions`
  (from `MLIRRegisterAllDialects` / `MLIRRegisterAllExtensions` CMake targets).
- Handle invalidation: `tile_using_forall`, `fuse_into_containing_op`, and
  `apply_registered_pass` **consume** their input handles. `apply_cse` and
  `apply_patterns` do **not** consume.
- The transform sequence must live inside the same module as the payload when using
  `--transform-interpreter` (single-module layout). For `applyTransforms()` in C++,
  pass `enforceToplevelTransformOp=false` when the entry point is a `named_sequence`
  rather than a top-level `transform.sequence`.
