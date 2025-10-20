// File: get_users_vs_uses.mlir
// mlir-opt users_and_uses.mlir  -test-print-defuse
module {
  func.func @demo(%arg0: i32, %arg1: i32) -> i32 {
    %c5 = arith.constant 5 : i32        // TARGET A: 测 %c5
    %c2 = arith.constant 2 : i32

    // U1：同一个用户里使用两次（两个操作数位置都用的是 %c5）
    %add_c_c = arith.addi %c5, %c5 : i32    // uses of %c5: operand#0, operand#1

    // U2：使用一次（放在 operand#1）
    %sub_a_c = arith.subi %arg0, %c5 : i32  // use of %c5: operand#1

    // U3：使用一次（放在 operand#0）
    %mul_c_a = arith.muli %c5, %arg1 : i32  // use of %c5: operand#0

    // TARGET B: 测 %arg0 —— 同一用户双重使用
    %add_a_a = arith.addi %arg0, %arg0 : i32  // uses of %arg0: operand#0, operand#1

    // TARGET C: 测 %add_c_c 的被用情况（既有“同一用户两次”，也有不同用户）
    %y1 = arith.addi %add_c_c, %add_c_c : i32 // uses of %add_c_c: operand#0, operand#1
    %y2 = arith.addi %add_c_c, %c2 : i32      // use of %add_c_c: operand#0
    %y3 = arith.muli %arg1, %add_c_c : i32    // use of %add_c_c: operand#1

    %ret = arith.addi %y1, %y2 : i32
    return %ret : i32
  }
}