# Chapter 12 — Transform Dialect

## What Is the Transform Dialect?

The Transform dialect is an embedded scheduling DSL inside MLIR. Instead of
writing C++ pass code to orchestrate transformations, you write a *transform
schedule* directly in MLIR IR. The schedule describes *what* to transform and
*how*, while the `--transform-interpreter` pass (or `applyTransforms()` in C++)
executes it at compile time.

### Three-layer mental model

```
┌──────────────────────────────────────────────────┐
│  Transform IR  (the schedule you write)          │
│  transform.named_sequence @__transform_main      │
│    match → tile → fuse → outline → cleanup       │
└───────────────────────┬──────────────────────────┘
                        │  operates on
┌───────────────────────▼──────────────────────────┐
│  Payload IR  (the code being compiled)           │
│  func.func @fc_relu { linalg.matmul ... }        │
└───────────────────────┬──────────────────────────┘
                        │  referenced through
┌───────────────────────▼──────────────────────────┐
│  Handles  (!transform.any_op, !transform.op<"…">) │
│  SSA values in Transform IR that point to        │
│  one or more payload ops — like typed pointers   │
└──────────────────────────────────────────────────┘
```

### Why Transform dialect instead of C++ passes?

| C++ pass | Transform dialect |
|----------|------------------|
| Transformation logic buried in C++ | Schedule visible as IR, inspectable and debuggable |
| Recompile to change the schedule | Change the `.mlir` file and re-run |
| Hard to compose transformations | Ops compose naturally via SSA handles |
| No standard way to express tile+fuse | `tile_using_forall` + `fuse_into_containing_op` |

### Entry point conventions

The `--transform-interpreter` pass looks for a `transform.named_sequence` named
`@__transform_main` inside the module. The module must carry the attribute
`{transform.with_named_sequence}` so the verifier validates the sequences.

```mlir
module attributes {transform.with_named_sequence} {
  // ── Payload IR ────────────────────────────────
  func.func @foo(%a: i32, %b: i32) -> i32 { ... }

  // ── Transform IR ──────────────────────────────
  transform.named_sequence @__transform_main(%arg0: !transform.any_op) {
    // %arg0 is a handle to the top-level op (the module itself)
    ...
    transform.yield
  }
}
```

---

## Op Reference

### Selection / navigation

| Op | Signature | Effect | Consumes handle? |
|----|-----------|--------|-----------------|
| `transform.structured.match` | `(container) -> handle` | Returns a handle to all ops matching the filter (e.g. `ops{["linalg.matmul"]}`) inside the container | No |
| `transform.split_handle` | `(multi) -> (h1, h2, …)` | Splits a handle that points to N ops into N individual handles; result count must equal op count exactly | No |
| `transform.cast` | `(any_op) -> op<"…">` | Narrows handle type for static checking; both old and new handles remain valid | No |
| `transform.get_parent_op` | `(child) -> parent` | Returns a handle to the enclosing op of the given kind | No |

### Structured (linalg) transformations

| Op | Signature | Effect | Consumes handle? |
|----|-----------|--------|-----------------|
| `transform.structured.tile_using_forall` | `(op) -> (tiled_op, forall_loop)` | Tiles a structured op into an `scf.forall` loop nest with the given tile sizes | Yes (input op) |
| `transform.structured.fuse_into_containing_op` | `(producer, loop) -> (fused_op, updated_loop)` | Fuses a producer op into a containing loop (e.g. forall), computing only the slice needed per tile | Yes (producer) |
| `transform.structured.vectorize_children_and_apply_patterns` | `(container) -> ()` | Vectorizes all structured ops inside the container | Yes |

### Loop transforms

| Op | Signature | Effect | Consumes handle? |
|----|-----------|--------|-----------------|
| `transform.loop.outline` | `(loop) -> (func, call)` | Extracts the loop body into a new `func.func`; inserts a call at the original site | Yes (loop) |
| `transform.loop.unroll` | `(loop) -> ()` | Fully unrolls the loop | Yes |
| `transform.loop.pipeline` | `(loop) -> (pipelined)` | Software-pipelines the loop | Yes |

### Cleanup / optimization

| Op | Signature | Effect | Consumes handle? |
|----|-----------|--------|-----------------|
| `transform.apply_cse` | `(func) -> ()` | Eliminates common subexpressions inside the target op | No |
| `transform.apply_patterns` | `(target) { nested pattern ops } -> ()` | Applies a set of rewrite patterns greedily to ops inside target; patterns declared in the nested region (e.g. `transform.apply_patterns.canonicalization`) | No |
| `transform.apply_registered_pass` | `(target) -> (new_target)` | Runs a named compiler pass (e.g. `"canonicalize"`, `"cse"`) on the target op; **always capture the returned handle** — the input is consumed | Yes |

### Debugging

| Op | Signature | Effect |
|----|-----------|--------|
| `transform.debug.emit_remark_at` | `(handle, "message")` | Emits a compiler remark anchored at each op in the handle; visible on stderr or with `--verify-diagnostics` |
| `transform.debug.emit_param_as_remark` | `(param, anchor)` | Emits a transform parameter (e.g. a computed integer) as a remark |

### Control flow

| Op | Notes |
|----|-------|
| `transform.named_sequence @name(%arg0: !transform.any_op) { … }` | Defines a reusable transform sequence; called by the interpreter or by `transform.include` |
| `transform.yield` | Required terminator of every named sequence |
| `transform.include @name` | Calls another named sequence |

### Handle types

| Type | Meaning |
|------|---------|
| `!transform.any_op` | Handle to one or more payload ops of any kind |
| `!transform.op<"dialect.op_name">` | Typed handle — static guarantee that all referenced ops are of the named kind |
| `!transform.any_value` | Handle to one or more payload SSA values |
| `!transform.param<i64>` | Carries a compile-time integer parameter (not a payload op) |

---

## Demo Design Principles

Each demo is self-contained and ordered to teach one concept at a time. Reading
them in sequence builds a complete mental model of the Transform dialect.

### Demo 1 — Basic wiring (`test_transform_basic.mlir`)

**Goal:** understand the minimum scaffolding to run a transform sequence.

**Design choices:**
- Payload is a trivial `arith.addi` chain — no linalg, no tensors. Keeps
  attention on the transform side.
- Introduces `structured.match` as the universal selection tool.
- Uses `split_handle` to show that a multi-op handle must be split before
  addressing ops individually.
- Uses `cast` to show typed vs untyped handles — cast does *not* consume the
  original handle.
- Uses `debug.emit_remark_at` as the observable side-effect so the test needs
  no IR mutation to produce output.
- FileCheck lines verify remarks appear, not IR shape — the simplest possible
  correctness check.

### Demo 2 — Tile and fuse (`test_transform_structured.mlir`)

**Goal:** understand the canonical tile+fuse flow for structured ops.

**Design choices:**
- Payload is `linalg.matmul` + `linalg.elementwise` (fc_relu pattern) — the
  MLIR tutorial's canonical motivating example.
- Introduces `tile_using_forall` as the primary tiling op (preferred over the
  deprecated `tile`).
- Immediately follows with `fuse_into_containing_op` to show producer-consumer
  fusion driven by the tiling loop handle.
- Handle invalidation is front and center: comments call out exactly which
  handles are consumed by which ops.
- FileCheck verifies `scf.forall` appears in the output — confirms tiling
  actually happened.

### Demo 3 — Loop outline (`test_transform_loop.mlir`)

**Goal:** show multi-step transform composition and loop extraction.

**Design choices:**
- Two-stage tiling demonstrates handle chaining: the first tile's output handle
  becomes the second tile's input.
- `loop.outline` shows how to extract a loop body into a new function — a key
  technique for kernel extraction in GPU/NPU compilation flows.
- `apply_patterns.canonicalization` is introduced to fold the
  `scf.execute_region` wrapper that `loop.outline` inserts internally.
- The canonicalize step is applied to `func.func` ops rather than the module
  root because applying a transform to an ancestor of the transform op itself
  is forbidden by the interpreter — the comment explains this constraint.
- FileCheck verifies the outlined function name appears in the output.

### Demo 4 — Cleanup passes (`test_transform_passes.mlir`)

**Goal:** distinguish three orthogonal cleanup mechanisms and understand their
execution order.

**Design choices:**
- Payload is carefully constructed so that *each* mechanism has unique,
  observable work: two duplicate constants (CSE target), two `addi(x,0)` ops
  (pattern target), and residual dead ops (pass target).
- Mechanisms run in C → B → A order (apply_cse → apply_patterns →
  apply_registered_pass) so each has something left to do. Running A first
  would subsume B and C, obscuring what each mechanism does independently.
- Comments spell out exactly which IR state each step changes — makes the demo
  readable as a reference card.
- Key API hazard highlighted: `apply_registered_pass` *consumes* its handle
  and returns a new one. This is different from `apply_cse` and
  `apply_patterns` which do not consume.
- FileCheck verifies `arith.addi` is gone from the output — confirms all three
  mechanisms ran.

### C++ Demo — `applyTransforms()` (`tools/test/test_transform_demo.cpp`)

**Goal:** show the programmatic entry point for driving transforms from C++.

**Design choices:**
- Uses `registerAllDialects()` + `registerAllExtensions()` — the same pattern
  as `mlir-opt` itself. Avoids enumerating individual dialect/extension libs
  which is fragile.
- `ScopedDiagnosticHandler` is installed before `applyTransforms()` so that
  `debug.emit_remark_at` output appears on stderr rather than being silently
  discarded.
- `enforceToplevelTransformOp=false` because the entry point is a
  `named_sequence` (an inner op), not a top-level `transform.sequence` wrapper.
- Prints before/after IR to stdout so the transform's effect is visible without
  a FileCheck harness.

---

## Demos

| File | What it teaches |
|------|-----------------|
| `test/test_transform_basic.mlir` | `structured.match`, `split_handle`, `cast`, `debug.emit_remark_at` |
| `test/test_transform_structured.mlir` | `tile_using_forall`, `fuse_into_containing_op`, handle invalidation |
| `test/test_transform_loop.mlir` | `loop.outline`, two-stage tiling, `apply_patterns.canonicalization` |
| `test/test_transform_passes.mlir` | `apply_cse`, `apply_patterns`, `apply_registered_pass` |
| `tools/test/test_transform_demo.cpp` | C++ `applyTransforms()` entry point |

## Running the .mlir Tests

```bash
MLIR_OPT=externals/llvm-project/build/bin/mlir-opt

# Demo 1: basic wiring — match, split_handle, cast, emit_remark_at
$MLIR_OPT 12-transform_dialect/test/test_transform_basic.mlir \
    --transform-interpreter 2>&1

# Demo 2: structured transforms — tile and fuse
$MLIR_OPT 12-transform_dialect/test/test_transform_structured.mlir \
    --transform-interpreter 2>&1

# Demo 3: loop transforms — tile then outline
$MLIR_OPT 12-transform_dialect/test/test_transform_loop.mlir \
    --transform-interpreter 2>&1

# Demo 4: cleanup passes — apply_cse, apply_patterns, apply_registered_pass
$MLIR_OPT 12-transform_dialect/test/test_transform_passes.mlir \
    --transform-interpreter 2>&1
```

## Building the C++ Demo

```bash
./build_npu_mlir_fixed.sh ch-12
# Binary: install/bin/test/ch12-test-transform-demo
./install/bin/test/ch12-test-transform-demo
```

## Key Concepts

### Handle invalidation rules

| Op | Consumes input handle? |
|----|----------------------|
| `transform.structured.match` | No |
| `transform.split_handle` | No |
| `transform.cast` | No |
| `transform.debug.emit_remark_at` | No |
| `transform.apply_cse` | No |
| `transform.apply_patterns` | No |
| `transform.structured.tile_using_forall` | Yes (tiled op handle) |
| `transform.structured.fuse_into_containing_op` | Yes (producer handle) |
| `transform.loop.outline` | Yes (loop handle) |
| `transform.apply_registered_pass` | Yes — capture the returned handle |

### C++ integration

```cpp
mlir::DialectRegistry registry;
mlir::registerAllDialects(registry);
mlir::registerAllExtensions(registry);
mlir::MLIRContext context(registry);
context.loadAllAvailableDialects();

auto module = mlir::parseSourceString<mlir::ModuleOp>(src, &context);

// Install diagnostic handler so remarks appear on stderr
mlir::ScopedDiagnosticHandler diagHandler(&context,
    [](mlir::Diagnostic &d) -> mlir::LogicalResult {
      if (d.getSeverity() == mlir::DiagnosticSeverity::Remark)
        llvm::errs() << "remark: " << d.str() << "\n";
      return mlir::success();
    });

// Find @__transform_main, then apply it
mlir::transform::TransformOptions options;
mlir::transform::applyTransforms(
    module.get(), entry, /*extraMapping=*/{}, options,
    /*enforceToplevelTransformOp=*/false);
```

Pass `enforceToplevelTransformOp=false` when the entry point is a
`named_sequence` rather than a top-level `transform.sequence`.
