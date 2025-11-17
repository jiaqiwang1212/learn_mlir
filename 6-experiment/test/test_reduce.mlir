// 测试目标：经过 canonicalize 后，应该只保留被使用的 vector.transfer_read
// mlir-reduce 应该删除所有无关代码，保留最小复现
// mlir-reduce input.mlir -reduction-tree="traversal-mode=0 test=./check.sh" -o output.mlir
module {

  // ❌ 完全无关的函数 1（应该被 mlir-reduce 删除）
  func.func @unused_helper1(%x: i32, %y: i32) -> i32 {
    %sum = arith.addi %x, %y : i32
    %c2 = arith.constant 2 : i32
    %result = arith.muli %sum, %c2 : i32
    return %result : i32
  }
  
  // 主函数：包含多个 vector.transfer_read，只有一个被使用
  func.func @main(%arg0: memref<64xf32>, %arg1: memref<64xf32>, %arg2: memref<64xf32>) {
    %c0 = arith.constant 0 : index
    %c8 = arith.constant 8 : index
    %c16 = arith.constant 16 : index
    %c24 = arith.constant 24 : index
    %cst = arith.constant 0.0 : f32
    
    // ✅ 这个 read 被使用了（我们希望保留）
    %v0 = vector.transfer_read %arg0[%c0], %cst 
          : memref<64xf32>, vector<8xf32>
    
    // ❌ 这些 read 没被使用（应该被 canonicalize 删除）
    %v1 = vector.transfer_read %arg0[%c8], %cst 
          : memref<64xf32>, vector<8xf32>
    
    %v2 = vector.transfer_read %arg0[%c16], %cst 
          : memref<64xf32>, vector<8xf32>
    
    %v3 = vector.transfer_read %arg0[%c24], %cst 
          : memref<64xf32>, vector<8xf32>
    
    // ❌ 另一个无用的 read
    %v4 = vector.transfer_read %arg1[%c0], %cst 
          : memref<64xf32>, vector<8xf32>
    
    // ✅ 只有 v0 被使用
    %result = arith.addf %v0, %v0 : vector<8xf32>
    
    vector.transfer_write %result, %arg2[%c0] 
          : vector<8xf32>, memref<64xf32>
    
    return
  }
  
  // ❌ 完全无关的函数 2
  func.func @unused_helper2(%a: f32, %b: f32, %c: f32) -> f32 {
    %t1 = arith.mulf %a, %b : f32
    %t2 = arith.addf %t1, %c : f32
    %t3 = arith.divf %t2, %a : f32
    return %t3 : f32
  }
  
  // ❌ 完全无关的函数 3（嵌套控制流）
  func.func @unused_helper3(%arg: i32) -> i32 {
    %c0 = arith.constant 0 : i32
    %c10 = arith.constant 10 : i32
    
    %cmp = arith.cmpi slt, %arg, %c10 : i32
    %result = scf.if %cmp -> (i32) {
      %doubled = arith.muli %arg, %c10 : i32
      scf.yield %doubled : i32
    } else {
      scf.yield %c0 : i32
    }
    
    return %result : i32
  }
  
  // ❌ 包含循环的无关函数
  func.func @unused_loop(%lb: index, %ub: index, %step: index) -> index {
    %sum = scf.for %i = %lb to %ub step %step 
           iter_args(%acc = %lb) -> (index) {
      %next = arith.addi %acc, %i : index
      scf.yield %next : index
    }
    return %sum : index
  }
}