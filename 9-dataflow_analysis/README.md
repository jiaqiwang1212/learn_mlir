# Ch-9: MLIR Dataflow Analysis

本章介绍 MLIR 的 **DataFlow Analysis 框架**，通过实现一个**稀疏常量传播（Sparse Constant Propagation）分析**来学习其核心概念。

分析是**只读的**：不修改 IR，只通过 diagnostic remark 标注哪些 SSA 值是编译期常量。

---

## 核心概念

### 1. 格（Lattice）

DataFlow 分析的基本数据结构。每个 SSA 值对应一个格值，表示分析对该值的认识程度：

```
        ⊤ (uninitialized — 还未分析)
        |
  known constant      ← 已知是某个具体常量，例如 3 : i32
        |
        ⊥ (unknown — 多个分支汇聚，无法确定)
```

`ConstantValue` 类表示这三种状态：

| 状态 | 含义 | 判断方式 |
|------|------|----------|
| Uninitialized | Solver 尚未处理该值 | `cv.isUninitialized() == true` |
| Known constant | 确定是某个常量属性 | `cv.getConstantValue() != nullptr` |
| Unknown | 多条路径汇聚为不同值 | `!cv.isUninitialized() && cv.getConstantValue() == nullptr` |

### 2. DataFlowSolver

Solver 驱动整个 worklist 算法，直到所有格值收敛：

```cpp
DataFlowSolver solver;
solver.load<dataflow::DeadCodeAnalysis>();        // 必须先加载
solver.load<dataflow::SparseConstantPropagation>();
if (failed(solver.initializeAndRun(funcOp)))
    return signalPassFailure();
```

> **为什么需要 `DeadCodeAnalysis`？**
> 稀疏常量传播依赖死代码分析提供的"可达边"信息。如果只加载
> `SparseConstantPropagation` 而不加载 `DeadCodeAnalysis`，Solver 会报错。

### 3. 查询格值

Solver 运行完毕后，通过 `lookupState` 查询任意 SSA 值的格值：

```cpp
auto *lattice =
    solver.lookupState<dataflow::Lattice<dataflow::ConstantValue>>(value);

if (!lattice || lattice->getValue().isUninitialized())
    continue;  // 未分析到

Attribute constAttr = lattice->getValue().getConstantValue();
if (constAttr)
    op->emitRemark("value is constant: " + constAttr.str());
// constAttr == nullptr 表示 unknown（不是常量）
```

### 4. 稀疏（Sparse）vs 稠密（Dense）

| 类型 | 格附着到 | 适用场景 |
|------|----------|----------|
| **Sparse** | 每个 SSA **值** | 常量传播、整数范围分析 |
| **Dense** | 每个程序**点**（op 前/后） | 活跃变量、到达定值 |

本章使用 Sparse 分析，因为常量值只需跟踪 def 点，无需每个程序点都存储状态。

---

## 目录结构

```
9-dataflow_analysis/
├── include/npu-mlir/Dialect/Siren/
│   ├── IR/                        # Siren 方言（复用自 ch-8）
│   └── Transforms/
│       ├── Passes.td              # TableGen pass 声明
│       ├── Passes.h               # 注册入口
│       └── ConstantPropAnalysis.h # Pass 头文件
├── lib/npu-mlir/Dialect/Siren/
│   ├── IR/                        # Siren 方言实现
│   └── Transforms/
│       └── ConstantPropAnalysis.cpp  # ← 核心实现
├── tools/
│   ├── npu-opt/npu-opt.cpp        # ch-9-opt 驱动程序
│   └── npu/npu-mlir.cpp           # 手写 IR 驱动程序
└── test/
    ├── lit.cfg.py                 # llvm-lit 配置
    └── filecheck_const_prop.mlir  # FileCheck 测试
```

---

## 构建

```bash
# 在项目根目录执行
./build_npu_mlir_fixed.sh ch-9
```

构建产物：`install/bin/ch-9-opt`

---

## 运行分析

```bash
# 分析一个 MLIR 文件，remark 输出到 stderr
install/bin/ch-9-opt --constant-prop-analysis your_file.mlir 2>&1
```

示例输入：

```mlir
func.func @example() -> i32 {
  %c3  = arith.constant 3 : i32
  %c4  = arith.constant 4 : i32
  %sum = arith.addi %c3, %c4 : i32   // 3 + 4 = 7，可折叠
  func.return %sum : i32
}
```

示例输出：

```
your_file.mlir:2:9: remark: value is constant: 3 : i32
your_file.mlir:3:9: remark: value is constant: 4 : i32
your_file.mlir:4:10: remark: value is constant: 7 : i32
```

函数参数（未知输入）不会产生 remark：

```mlir
func.func @unknown(%arg0: i32) -> i32 {
  %c10 = arith.constant 10 : i32
  %sum = arith.addi %c10, %arg0 : i32  // arg0 未知，sum 也未知
  func.return %sum : i32
}
// 只有 %c10 产生 remark
```

---

## 运行测试

```bash
externals/llvm-project/build/bin/llvm-lit 9-dataflow_analysis/test/ -v
```

期望输出：

```
-- Testing: 1 tests, 1 workers --
PASS: ch-9-dataflow :: filecheck_const_prop.mlir (1 of 1)

Testing Time: 0.17s
  Passed: 1 (100.00%)
```

---

## 关键头文件

| 头文件 | 用途 |
|--------|------|
| `mlir/Analysis/DataFlowFramework.h` | `DataFlowSolver` 类 |
| `mlir/Analysis/DataFlow/SparseAnalysis.h` | `SparseForwardDataFlowAnalysis`、`Lattice<T>` |
| `mlir/Analysis/DataFlow/ConstantPropagationAnalysis.h` | `ConstantValue`、`SparseConstantPropagation` |
| `mlir/Analysis/DataFlow/DeadCodeAnalysis.h` | `DeadCodeAnalysis`（必须配合使用） |

CMake 链接库：`MLIRAnalysis`（包含以上所有分析的实现）

---

## 延伸阅读

- [MLIR DataFlow Analysis Tutorial](https://mlir.llvm.org/docs/Tutorials/DataFlowAnalysis/)
- `externals/llvm-project/mlir/include/mlir/Analysis/DataFlow/` — 框架头文件
- `externals/llvm-project/mlir/lib/Analysis/DataFlow/` — 框架实现
