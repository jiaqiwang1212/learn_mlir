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
| 09-dataflow_analysis | 稀疏常量传播，DataFlowSolver | ✅ |
| 10-diagnostic_system | 诊断发射与自定义处理器 | ✅ |
| 11-pdll | PDLL 模式重写：折叠、强度削减、死代码消除 | ✅ |
| 12-transform_dialect | Transform Dialect DSL：tile、fuse、循环提取、Pass 集成 | ✅ |