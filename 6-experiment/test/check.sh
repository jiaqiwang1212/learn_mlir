#!/usr/bin/env bash
# filepath: check_canonicalize.sh

MLIR_OPT="/data1/wangjiaqi/workspace/llvm_essentials/externals/llvm-project/build/bin/mlir-opt"

# 1. 原始文件必须包含 vector.transfer_read
if ! grep -q "vector.transfer_read" "$1"; then
  exit 0  # 没有目标操作 → Not interesting
fi

# 2. 运行 mlir-opt 进行优化（这里不进行优化）
output=$($MLIR_OPT "$1" 2>&1)

# output=$($MLIR_OPT "$1" --canonicalize 2>&1)

# 3. 检查是否还有 vector.transfer_read
echo "$output" | grep -q "vector.transfer_read"  # 还有的话就会返回 0
if [[ $? -eq 0 ]]; then
  # 还有 transfer_read → Interesting（保留这个简化）
  exit 1   # 返回 1 表示 Interesting
else
  # 所有 transfer_read 都被删除了 → Not interesting
  exit 0
fi