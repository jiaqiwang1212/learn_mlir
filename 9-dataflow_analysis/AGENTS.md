<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-16 | Updated: 2026-04-16 -->

# 9-dataflow_analysis

## Purpose
Introduces the MLIR **DataFlow Analysis** framework by implementing a **Sparse Constant Propagation** pass. The analysis is read-only — it does not modify IR; instead it emits diagnostic remarks to annotate which SSA values are compile-time constants. Builds on the Siren dialect from ch-8.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Chapter root: routes `ch-9` target to subdirectories |
| `README.md` | Detailed explanation of lattices, solver, and sparse vs dense analysis (Chinese + English) |
| `include/npu-mlir/Dialect/Siren/Transforms/ConstantPropAnalysis.h` | Pass header for `--constant-prop-analysis` |
| `include/npu-mlir/Dialect/Siren/Transforms/Passes.td` | TableGen pass declaration |
| `include/npu-mlir/Dialect/Siren/Transforms/Passes.h` | Pass registration entry point |
| `lib/Dialect/Siren/Transforms/ConstantPropAnalysis.cpp` | Core implementation: `DataFlowSolver` + `SparseConstantPropagation` |
| `tools/npu-opt/npu-opt.cpp` | `ch-9-opt` driver — registers Siren dialect and the analysis pass |
| `test/filecheck_const_prop.mlir` | FileCheck test: known constants annotated, function args are not |
| `test/lit.cfg.py` | llvm-lit configuration for `ch-9-dataflow` test suite |

## Subdirectories

| Directory | Purpose |
|-----------|---------|
| `include/` | Headers for Siren IR and Transforms |
| `lib/` | Implementation of the constant propagation pass |
| `tools/npu-opt/` | `ch-9-opt` driver executable |
| `test/` | FileCheck + lit tests |

## For AI Agents

### Building
```bash
# From repo root
./build_npu_mlir_fixed.sh ch-9
# Binary: install/bin/ch-9-opt
```

### Running the Analysis
```bash
install/bin/ch-9-opt --constant-prop-analysis your_file.mlir 2>&1
```
Remarks appear on stderr. Constants in `arith.constant` and folded arithmetic results are annotated; function arguments remain unknown.

### Testing
```bash
externals/llvm-project/build/bin/llvm-lit 9-dataflow_analysis/test/ -v
# Expected: PASS: ch-9-dataflow :: filecheck_const_prop.mlir
```

### Common Patterns
- Always load `DeadCodeAnalysis` **before** `SparseConstantPropagation`; the sparse analysis depends on reachability edges from the dead-code analysis.
- Query lattice values via `solver.lookupState<dataflow::Lattice<dataflow::ConstantValue>>(value)`.
- Three lattice states: `Uninitialized` (not yet visited), known constant (`getConstantValue() != nullptr`), unknown (joined from multiple paths).
- This chapter does **not** define a custom dialect — it extends the Siren dialect with a transform-only pass.

## Dependencies

### Internal
- Siren dialect (reused from ch-8 — no new ops defined here)

### External
- `MLIRAnalysis` CMake target (covers `DataFlowFramework.h`, `SparseAnalysis.h`, `ConstantPropagationAnalysis.h`, `DeadCodeAnalysis.h`)
- LLVM/MLIR 22.1.0

<!-- MANUAL: -->
