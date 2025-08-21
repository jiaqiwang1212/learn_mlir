module @TestModule {
  func.func @example(%a: i32, %b: i32) -> i32 {
    // —— for 之前的两个 OP ——
    %c1_i32 = arith.constant 1 : i32
    %sum    = arith.addi %a, %c1_i32 : i32
    %prod   = arith.muli %sum, %b     : i32

    // 循环边界
    %c0  = arith.constant 0 : index
    %c10 = arith.constant 10 : index
    %c1  = arith.constant 1 : index

    // —— for 循环（带 iter_args），内部两个 OP，累计并返回 i32 ——
    %final = scf.for %i = %c0 to %c10 step %c1
             iter_args(%acc = %prod) -> (i32) {
      %t0 = arith.addi %acc, %c1_i32 : i32
      %t1 = arith.muli %t0, %b        : i32
      scf.yield %t1 : i32
    }

    func.return %final : i32
  }
}

// mlir-opt --test-print-liveness test_liveness.mlir