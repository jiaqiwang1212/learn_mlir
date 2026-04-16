<!-- Generated: 2026-04-14 | Updated: 2026-04-16 -->

# learn_mlir

## Purpose
A hands-on MLIR learning repository that walks through building an NPU compiler dialect step by step. Each numbered chapter introduces a new MLIR concept by building on the previous chapter's Siren dialect — from bare dialect skeleton through ops, interfaces, traits, and pass infrastructure. The LLVM/MLIR toolchain (v22.1.0) lives as a git submodule in `externals/`.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Root build: TARGET-based routing selects which chapter to build |
| `build_llvm.sh` | One-shot script to configure and build LLVM/MLIR from the submodule |
| `build_npu_mlir_fixed.sh` | Builds a single chapter; set `TARGET=ch-N` env var before running |
| `build_npu_mlir.sh` | Older build script (superseded by `build_npu_mlir_fixed.sh`) |
| `README.md` | Chapter checklist (Chinese) — tracks completion status |
| `.clang-format` | Project-wide C++ formatting rules |
| `.gitmodules` | Points `externals/llvm-project` submodule to LLVM 22.1.0 |

## Subdirectories

| Directory | Purpose |
|-----------|---------|
| `2-define_dialect/` | Ch-2: Minimal Siren dialect skeleton (see `2-define_dialect/AGENTS.md`) |
| `3-define_op/` | Ch-3: AddOp and SubOp with canonicalization, folding, verify (see `3-define_op/AGENTS.md`) |
| `4-define_Interfaces/` | Ch-4: NpuOpInterface — attaching interfaces to ops (see `4-define_Interfaces/AGENTS.md`) |
| `5-define_Traits/` | Ch-5: RequireTwoOperandsOneResult trait (see `5-define_Traits/AGENTS.md`) |
| `6-experiment/` | Ch-6: SCF ForOp/YieldOp experiments, C++ test executables (see `6-experiment/AGENTS.md`) |
| `7-table_gen/` | Ch-7: TableGen syntax exploration notebook (see `7-table_gen/AGENTS.md`) |
| `8-define_Pass/` | Ch-8: Pass infrastructure with TestPassOne (see `8-define_Pass/AGENTS.md`) |
| `9-dataflow_analysis/` | Ch-9: Sparse constant propagation via DataFlowSolver (see `9-dataflow_analysis/AGENTS.md`) |
| `10-diagnostic_system/` | Ch-10: Diagnostic emission and custom handlers (see `10-diagnostic_system/AGENTS.md`) |
| `11-pdll/` | Ch-11: PDLL pattern rewriting — fold, strength reduction, DCE (see `11-pdll/AGENTS.md`) |
| `12-transform_dialect/` | Ch-12: Transform dialect DSL — tile, fuse, loop outline, pass integration (see `12-transform_dialect/AGENTS.md`) |
| `externals/` | Git submodule: llvm-project @ llvmorg-22.1.0 (do not edit) |
| `install/` | CMake install prefix — built binaries, libraries, and docs land here |
| `.claude/` | Claude Code settings and skills |
| `.vscode/` | VS Code launch/settings for MLIR LSP and debugging |

## For AI Agents

### Building a Chapter
```bash
# Build chapter N (replace N with 2,3,4,5,6,8,9,10,11,12):
./build_npu_mlir_fixed.sh ch-N

# Build LLVM/MLIR first if externals/llvm-project is not yet built:
./build_llvm.sh
```

The CMake `TARGET` variable routes to the matching subdirectory. Only one chapter builds at a time.

### Working In This Directory
- Each chapter is self-contained; modifying ch-3 does not affect ch-2.
- Chapter directories share the same `npu-mlir::SirenDialect` namespace and evolve the same dialect concept incrementally.
- Use `TARGET=ch-N` when running `build_npu_mlir_fixed.sh`; the script also sets `CMAKE_PREFIX_PATH` to the install dir of the LLVM submodule build.
- LLVM 22: use `Op::create(builder, loc, ...)` factory pattern (not the deprecated `builder.create<Op>(...)`).

### Testing Requirements
- Ch-6 provides standalone C++ test executables in `6-experiment/tools/test/` (test_ADT, test_example, test_for_yield, test_traversal).
- Other chapters use `mlir-opt`-style MLIR file tests in their `test/` directories.
- Run built test binaries from `install/bin/test/`.

### Common Patterns
- Every chapter follows: `include/npu-mlir/Dialect/Siren/IR/` for headers, `lib/Dialect/Siren/IR/` for implementations.
- TableGen files (`.td`) generate C++ via `mlir-tblgen`; the generated `.inc` files are consumed by the `.h`/`.cpp` counterparts.
- Two driver tools per chapter: `npu-mlir` (custom compiler driver) and `npu-opt` (standard opt tool with Siren dialect registered).

## Dependencies

### Internal
- `externals/llvm-project` — LLVM/MLIR source (submodule, built separately)
- `install/` — install prefix shared across chapters

### External
- LLVM/MLIR 22.1.0
- CMake ≥ 3.20
- C++17

<!-- MANUAL: -->
