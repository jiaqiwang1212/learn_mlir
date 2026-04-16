<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-16 | Updated: 2026-04-16 -->

# 10-diagnostic_system

## Purpose
Covers the full lifecycle of the **MLIR Diagnostic System** — both the production side (emitting errors/warnings/remarks from ops and passes) and the consumption side (intercepting diagnostics with custom handlers). Two passes are implemented: `--diag-scoped` (RAII `ScopedDiagnosticHandler`) and `--diag-registered` (manual `registerHandler`/`eraseHandler`). A separate standalone demo (`handler_demo`) showcases `ParallelDiagnosticHandler` and `SourceMgrDiagnosticVerifierHandler`.

## Key Files

| File | Description |
|------|-------------|
| `CMakeLists.txt` | Chapter root: routes `ch-10` target to subdirectories |
| `README.md` | Bilingual reference: diagnostic lifecycle diagram, severity table, pass details, build/run instructions |
| `include/npu-mlir/Dialect/Siren/Transforms/CustomDiagnosticHandler.h` | Shared handler functor: prefixes `[CH10-DIAG]`, optionally suppresses warnings |
| `include/npu-mlir/Dialect/Siren/Transforms/DiagScopedPass.h` | Header for `--diag-scoped` pass |
| `include/npu-mlir/Dialect/Siren/Transforms/DiagRegisteredPass.h` | Header for `--diag-registered` pass |
| `include/npu-mlir/Dialect/Siren/Transforms/Passes.h` | Pass registration entry point |
| `include/npu-mlir/Dialect/Siren/Transforms/Passes.td` | TableGen pass declarations |
| `lib/Dialect/Siren/Transforms/CustomDiagnosticHandler.cpp` | Handler implementation |
| `lib/Dialect/Siren/Transforms/DiagScopedPass.cpp` | RAII-style handler demo pass |
| `lib/Dialect/Siren/Transforms/DiagRegisteredPass.cpp` | Manual-register-style handler demo pass |
| `tools/diag-demo/diag_demo.cpp` | Standalone `ch10-diag-demo`: emitting API walkthrough |
| `tools/diag-demo/handler_demo.cpp` | Standalone `ch10-handler-demo`: `ParallelDiagnosticHandler` + `SourceMgrDiagnosticVerifierHandler` |
| `tools/npu-opt/npu-opt.cpp` | `ch-10-opt` driver — registers Siren dialect and both passes |
| `test/filecheck_diag_scoped.mlir` | FileCheck test for `--diag-scoped` output |
| `test/filecheck_diag_registered.mlir` | FileCheck test for `--diag-registered` output |
| `test/lit.cfg.py` | llvm-lit configuration |

## Subdirectories

| Directory | Purpose |
|-----------|---------|
| `include/` | Headers for Siren Transforms (handler + passes) |
| `lib/` | Implementation of handler and two demo passes |
| `tools/diag-demo/` | Standalone executables for diagnostic API exploration |
| `tools/npu-opt/` | `ch-10-opt` driver |
| `test/` | FileCheck + lit tests |

## For AI Agents

### Building
```bash
./build_npu_mlir_fixed.sh ch-10
# Binaries: install/bin/ch-10-opt, install/bin/ch10-diag-demo, install/bin/ch10-handler-demo
```

### Running
```bash
# RAII handler pass (suppressWarnings=true)
install/bin/ch-10-opt --diag-scoped 10-diagnostic_system/test/filecheck_diag_scoped.mlir 2>&1

# Manual-register handler pass (suppressWarnings=false)
install/bin/ch-10-opt --diag-registered 10-diagnostic_system/test/filecheck_diag_registered.mlir 2>&1
```

### Testing
```bash
externals/llvm-project/build/bin/llvm-lit 10-diagnostic_system/test/ -v
# Expected: PASS × 2 (filecheck_diag_scoped + filecheck_diag_registered)
```

### Common Patterns
- `emitError()` is a **severity label**, not a pass failure signal. Only `signalPassFailure()` fails the pass manager.
- `Note` severity cannot be emitted standalone — it must be chained via `InFlightDiagnostic::attachNote()`.
- `ScopedDiagnosticHandler` = RAII wrapper around `registerHandler` + `eraseHandler` (auto-deregisters on scope exit).
- Handler LIFO order: last registered is called first; return `success()` to consume, `failure()` to forward.
- `SourceMgrDiagnosticVerifierHandler` and `ParallelDiagnosticHandler` require their own isolated `MLIRContext` scope to avoid UAF crashes (the handler registers a dangling lambda in the context's `DiagnosticEngine`).

## Dependencies

### Internal
- Siren dialect (no new ops — transforms only)

### External
- `mlir/IR/Diagnostics.h` — `DiagnosticEngine`, `ScopedDiagnosticHandler`, `InFlightDiagnostic`
- `mlir/IR/MLIRContext.h` — `context.getDiagEngine()`
- LLVM/MLIR 22.1.0

<!-- MANUAL: -->
