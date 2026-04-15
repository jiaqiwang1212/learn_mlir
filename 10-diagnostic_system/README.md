# Ch-10: MLIR Diagnostic System

本章介绍 MLIR Diagnostic System 的完整生命周期，包括：
- **生产侧 (Production)**：如何发出诊断（`emitError`、`emitWarning`、`emitRemark`、`attachNote`）
- **消费侧 (Consumption)**：如何通过自定义 Handler 拦截、格式化和过滤诊断

## 目录

- [概念概览](#概念概览)
- [诊断生命周期图](#诊断生命周期图)
- [ScopedDiagnosticHandler 的本质](#scopeddiagnostichandler-的本质)
- [Severity 级别表](#severity-级别表)
- [Pass 详解](#pass-详解)
  - [--diag-scoped：RAII Handler](#--diag-scoped-raii-handler)
  - [--diag-registered：手动注册](#--diag-registered-手动注册)
- [构建与运行](#构建与运行)
- [关键头文件](#关键头文件)

---

## 概念概览

MLIR 的诊断系统由三层组成：

1. **发出层**：op 或 pass 调用 `op->emitError(msg)` / `mlir::emitWarning(loc, msg)` 等，返回 `InFlightDiagnostic` 临时对象。
2. **引擎层**：`DiagnosticEngine`（存储在 `MLIRContext` 中）维护一个 Handler 栈，按 LIFO 顺序分发诊断。
3. **Handler 层**：每个 Handler 返回 `success()`（已消费）或 `failure()`（传递给下一个 Handler）。

> **重要**：`emitError()` 是一个**严重性标签**，不是 pass 失败信号。  
> Pass 失败由 `signalPassFailure()` 触发，与是否调用 `emitError` 无关。  
> （参见 `mlir/lib/Pass/Pass.cpp` 中的 `passState->irAndPassFailed`）

---

## 诊断生命周期图

```
op->emitError("msg")
    │
    │  返回 InFlightDiagnostic (Error, loc, "msg")
    │  可链式调用 .attachNote(loc) << "context"
    ▼
InFlightDiagnostic 在 ; 处析构 → 调用 report()
    │
    ▼
DiagnosticEngine (存储在 MLIRContext 中)
    │  按 LIFO 顺序遍历 Handler 栈
    ▼
┌─────────────────────────────────────────────────────┐
│  Handler N  (ScopedDiagnosticHandler / RAII 注册)   │ ← 最后注册，最先调用
│  Handler 1  (registerHandler id=1 / 手动注册)       │
│  Handler 0  (SourceMgrDiagnosticHandler / MlirOpt)  │ ← 默认 Handler
└─────────────────────────────────────────────────────┘
    │
    │  每个 Handler 返回：
    │    success()  → 已消费，停止传递
    │    failure()  → 未消费，传给下一个 Handler
    ▼
(若所有 Handler 均返回 failure()，诊断被丢弃或走默认路径)
```

---

## ScopedDiagnosticHandler 的本质

`ScopedDiagnosticHandler` 是对 `registerHandler` + `eraseHandler` 的 **RAII 封装**。

查看 `mlir/include/mlir/IR/Diagnostics.h`（`ScopedDiagnosticHandler::setHandler`）：

```cpp
template <typename FuncTy>
void setHandler(FuncTy &&handler) {
  auto &diagEngine = ctx->getDiagEngine();
  if (handlerID)
    diagEngine.eraseHandler(handlerID);          // 清除旧 Handler
  handlerID = diagEngine.registerHandler(        // 注册新 Handler
      std::forward<FuncTy>(handler));
}
```

析构函数调用 `diagEngine.eraseHandler(handlerID)`。

**结论**：`ScopedDiagnosticHandler` = 构造时 `registerHandler` + 析构时 `eraseHandler`。  
`--diag-registered` Pass 让你看到相同的原始 API，`--diag-scoped` 展示了封装后的 RAII 版本。

---

## Severity 级别表

| 级别      | C++ 调用                          | 说明                          |
|-----------|-----------------------------------|-------------------------------|
| `Error`   | `op->emitError("msg")`            | 严重问题，但不等于 pass 失败  |
| `Warning` | `op->emitWarning("msg")`          | 潜在问题，可被 Handler 过滤   |
| `Remark`  | `op->emitRemark("msg")`           | 信息性输出，如分析结果        |
| `Note`    | `diag.attachNote(loc) << "msg"`   | 附加上下文，必须挂载在父诊断上 |

> **注意**：`Note` 不能独立发出。`mlir/IR/Diagnostics.h` 中有断言：
> ```cpp
> assert(severity != DiagnosticSeverity::Note &&
>        "notes should not be emitted directly");
> ```
> Note 必须通过 `InFlightDiagnostic::attachNote()` 附加到父诊断上。

---

## Pass 详解

### `--diag-scoped`：RAII Handler

**文件**：`lib/Dialect/Siren/Transforms/DiagScopedPass.cpp`

```cpp
// RAII：scoped 析构时自动注销 Handler
ScopedDiagnosticHandler scoped(
    &getContext(), CustomDiagnosticHandler{/*suppressWarnings=*/true});

// 对第一个 arith.constant 发出诊断（emit-once 保证 FileCheck 确定性）
op->emitError("scoped: value is constant")
    .attachNote(op->getLoc()) << "note: attached context";  // Error + Note
op->emitWarning("scoped: this warning will be suppressed"); // Warning → 被过滤
op->emitRemark("scoped: remark survives filter");           // Remark → 通过
```

**预期输出**：

```
[CH10-DIAG] ERROR: scoped: value is constant
[CH10-DIAG] NOTE: note: attached context
[CH10-DIAG] REMARK: scoped: remark survives filter
```

> Warning 被 `CustomDiagnosticHandler{suppressWarnings=true}` 静默消费，不出现在输出中。

---

### `--diag-registered`：手动注册

**文件**：`lib/Dialect/Siren/Transforms/DiagRegisteredPass.cpp`

```cpp
// 手动注册：捕获 HandlerID
DiagnosticEngine &engine = getContext().getDiagEngine();
DiagnosticEngine::HandlerID id =
    engine.registerHandler(CustomDiagnosticHandler{/*suppressWarnings=*/false});

// Form A：op->emitX（Operation 方法）+ attachNote 链式调用
op->emitError("registered: via op->emitError")
    .attachNote(op->getLoc()) << "registered: direct-engine note";

// Form B：mlir::emitX(loc, msg)（自由函数形式）
mlir::emitWarning(op->getLoc(), "registered: via mlir::emitWarning(loc, msg)");
mlir::emitRemark(op->getLoc(), "registered: via free function form");

// 手动注销 Handler
engine.eraseHandler(id);

// 注销后的诊断由默认 SourceMgr Handler 处理
mlir::emitRemark(op->getLoc(), "after-erase: default handler owns this");
```

**预期输出**：

```
[CH10-DIAG] ERROR: registered: via op->emitError
[CH10-DIAG] NOTE: registered: direct-engine note
[CH10-DIAG] WARNING: registered: via mlir::emitWarning(loc, msg)
[CH10-DIAG] REMARK: registered: via free function form
<file>:<line>:<col>: remark: after-erase: default handler owns this
```

> 最后一行没有 `[CH10-DIAG]` 前缀，证明 Handler 已被注销，由默认 SourceMgr Handler 打印（带位置信息前缀）。

---

## 构建与运行

```bash
# 1. 构建 ch-10
TARGET=ch-10 ./build_npu_mlir_fixed.sh

# 2. 确认 binary 已安装
install/bin/ch-10-opt --help | grep "diag-"
# 应输出：
#   --diag-scoped    - Scoped diagnostic handler demo (RAII, filters warnings)
#   --diag-registered - Raw DiagnosticEngine::registerHandler demo with explicit eraseHandler

# 3. 手动运行 --diag-scoped
install/bin/ch-10-opt --diag-scoped 10-diagnostic_system/test/filecheck_diag_scoped.mlir 2>&1

# 4. 手动运行 --diag-registered
install/bin/ch-10-opt --diag-registered 10-diagnostic_system/test/filecheck_diag_registered.mlir 2>&1

# 5. 运行 lit 测试
externals/llvm-project/build/bin/llvm-lit 10-diagnostic_system/test/ -v
# 预期：PASS: ch-10-diagnostic :: filecheck_diag_scoped.mlir
#        PASS: ch-10-diagnostic :: filecheck_diag_registered.mlir
#        2 tests passed
```

---

## 关键头文件

| 头文件                          | 说明                                                    |
|---------------------------------|---------------------------------------------------------|
| `mlir/IR/Diagnostics.h`         | `DiagnosticEngine`、`ScopedDiagnosticHandler`、`InFlightDiagnostic`、`Diagnostic` |
| `mlir/IR/MLIRContext.h`         | `context.getDiagEngine()`                               |
| `mlir/IR/Location.h`            | `Location`、`FileLineColLoc` 等位置类型                 |
