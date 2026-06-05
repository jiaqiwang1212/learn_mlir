# 有关MLIR的用法实验
prequests: llvm-22.1.0


| 章节 | 内容 | 完成 |
| --- | --- | --- |
| 01-搭建mlir工程 | 工程初始化，CMake配置，LLVM子模块构建 | ✅ |
| 02-define_dialect | 创建最小 Siren dialect 骨架 | ✅ |
| 03-define_op | 添加 AddOp/SubOp，规范化、折叠与验证 | ✅ |
| 04-define_Interfaces | 定义 NpuOpInterface，为 Op 挂接接口 | ✅ |
| 05-define_Traits | 定义 RequireTwoOperandsOneResult trait | ✅ |
| 06-experiment | SCF ForOp/YieldOp 实验，C++ 独立测试可执行文件 | ✅ |
| 07-table_gen | TableGen 语法探索 | ✅ |
| 08-define_Pass | Pass 基础设施，TestPassOne | ✅ |
| 09-dataflow_analysis | 稀疏常量传播，DataFlowSolver，整数范围分析 | ✅ |
| 10-diagnostic_system | 诊断发射与自定义处理器 | ✅ |
| 11-pdll | PDLL 模式重写：折叠、强度削减、死代码消除 | ✅ |
| 12-transform_dialect | Transform Dialect DSL：tile、fuse、循环提取、Pass 集成 | ✅ |
| 13-dialect_resources | Dialect 资源 Blob：ResourceBlobManagerDialectInterface、ResourceHandleParameter、C++ API 构造与打印 | ✅ |

---

## 构建方法

```bash
# 先构建 LLVM/MLIR（仅需一次）
./build_llvm.sh

# 构建指定章节（替换 ch-N）
./build_npu_mlir_fixed.sh ch-N
```

产物安装到 `install/bin/`。

---

## 各章节使用说明

### 01-搭建mlir工程
工程脚手架，无可执行产物。参考 `build_llvm.sh` 了解 LLVM 子模块的构建方式。

### 02-define_dialect
最小 Siren dialect 骨架：dialect 声明、命名空间、CMake 脚手架。

```bash
./build_npu_mlir_fixed.sh ch-2

# 查看已注册的 dialect
install/bin/ch-2-opt --show-dialects

# 解析并打印空模块
echo 'module {}' | install/bin/ch-2-opt -

# C++ 独立测试：验证 dialect 加载与命名空间
install/bin/ch-2
```

### 03-define_op
AddOp / SubOp，带规范化（canonicalize）、常量折叠与操作数验证。

```bash
./build_npu_mlir_fixed.sh ch-3

# 运行规范化 Pass（可触发折叠）
install/bin/ch-3-opt 3-define_op/test/filecheck_ops.mlir --split-input-file

# 带规范化的完整流水线
install/bin/ch-3-opt 3-define_op/test/filecheck_ops.mlir \
    --split-input-file --canonicalize
```

### 04-define_Interfaces
NpuOpInterface：在 TableGen 中声明接口，在 Op 上实现并调用。

```bash
./build_npu_mlir_fixed.sh ch-4

# 解析并验证接口绑定
install/bin/ch-4-opt 4-define_Interfaces/test/siren.mlir

# C++ 独立测试：调用接口方法并打印结果
install/bin/ch-4
```

### 05-define_Traits
RequireTwoOperandsOneResult trait：在 TableGen 中声明 trait，ODS 自动验证操作数与结果数量。

```bash
./build_npu_mlir_fixed.sh ch-5

# 解析并验证 trait 约束
install/bin/ch-5-opt 5-define_Traits/test/siren.mlir

# C++ 独立测试
install/bin/ch-5
```

### 06-experiment
SCF ForOp / YieldOp 实验，以及多个 C++ 独立测试可执行文件（ADT、遍历、归约）。

```bash
./build_npu_mlir_fixed.sh ch-6

# LLVM ADT 字符串类型演示
install/bin/test/test_ADT

# SCF ForOp / YieldOp 构造与遍历
install/bin/test/test_for_yield

# Op 用户与使用链（Uses/Users）演示
install/bin/test/test_traversal
```

### 07-table_gen
TableGen 语法探索笔记，无独立可执行产物。参考 `7-table_gen/` 目录下的 `.td` 文件学习 ODS 语法。

### 08-define_Pass
Pass 基础设施：PassManager、Pass 注册、TestPassOne（遍历 Op 并打印信息）。

```bash
./build_npu_mlir_fixed.sh ch-8

# 运行 TestPassOne Pass
install/bin/ch-8-opt 8-define_Pass/test/siren.mlir --test-pass-one

# 查看所有可用 Pass
install/bin/ch-8-opt --help | grep test
```

### 09-dataflow_analysis
两种数据流分析：稀疏常量传播分析（read-only，emits remarks）与整数范围分析。

```bash
./build_npu_mlir_fixed.sh ch-9

# 稀疏常量传播（以 remark 形式输出推断值）
install/bin/ch-9-opt --constant-prop-analysis \
    9-dataflow_analysis/test/filecheck_const_prop.mlir

# 整数范围分析（输出推断的值域范围）
install/bin/ch-9-opt --range-analysis \
    9-dataflow_analysis/test/filecheck_range_analysis.mlir

# 边界检查习语演示
install/bin/ch-9-opt --range-analysis \
    9-dataflow_analysis/test/filecheck_bounds_check.mlir
```

### 10-diagnostic_system
诊断系统：自定义诊断发射、作用域诊断处理器（ScopedDiagnosticHandler）、诊断注册。

```bash
./build_npu_mlir_fixed.sh ch-10

# 诊断发射演示（emitError/Remark/Warning 自由函数）
install/bin/ch10-diag-demo

# 作用域诊断处理器演示（RAII lambda handler）
install/bin/ch10-handler-demo

# 通过 opt 触发注册诊断
install/bin/ch-10-opt \
    10-diagnostic_system/test/filecheck_diag_registered.mlir
```

### 11-pdll
PDLL 模式重写：常量折叠、强度削减（mul → shift）、死代码消除。

```bash
./build_npu_mlir_fixed.sh ch-11

# 常量折叠演示（add(c1, c2) → 常量）
install/bin/ch11-pdll-fold-demo

# 强度削减演示（乘法 → 移位）
install/bin/ch11-pdll-strength-demo

# 死代码消除演示（无用常量 Op 删除）
install/bin/ch11-pdll-dce-demo
```

### 12-transform_dialect
Transform Dialect DSL：tile、fuse、循环提取（loop outline）、Pass 集成。

```bash
./build_npu_mlir_fixed.sh ch-12

# 一键运行所有 Transform 演示（推荐）
install/bin/test/ch12-test-transform-demo

# 也可通过 mlir-opt 直接运行各测试文件
MLIR_OPT=externals/llvm-project/build/bin/mlir-opt

$MLIR_OPT 12-transform_dialect/test/test_transform_basic.mlir \
    --transform-interpreter

$MLIR_OPT 12-transform_dialect/test/test_transform_loop.mlir \
    --transform-interpreter

$MLIR_OPT 12-transform_dialect/test/test_transform_structured.mlir \
    --transform-interpreter
```

### 13-dialect_resources
Dialect 资源 Blob：`ResourceBlobManagerDialectInterface` + `OpAsmDialectInterface` 双接口注册，`ResourceHandleParameter` TableGen 属性，以及通过 C++ API 程序化构造并打印资源。

```bash
./build_npu_mlir_fixed.sh ch-13

# 解析含 dialect_resources 节的 .mlir 文件
install/bin/ch-13-opt 13-dialect_resources/test/test_resource_blob.mlir

# 解析 siren.weight op（引用资源 blob）
install/bin/ch-13-opt 13-dialect_resources/test/test_weight_op.mlir

# 往返测试（parse → print → parse，输出应一致）
install/bin/ch-13-opt 13-dialect_resources/test/test_weight_op.mlir \
    | install/bin/ch-13-opt -

# 用 C++ API 程序化构造资源并打印
install/bin/construct-resource

# construct-resource 的输出可直接送入 ch-13-opt 再次解析
install/bin/construct-resource | install/bin/ch-13-opt -
```
