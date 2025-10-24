// traversal_test.mlir
module @Test_Traversal{
  // 顶层符号 1：global（可用 Symbol 接口枚举到）
  memref.global @G : memref<4xi32> = dense<[1, 2, 3, 4]>

  // 顶层符号 2：私有函数（walk/typed 枚举都能拿到；SymbolTable 也能按名查找）
  func.func private @callee(%x: i32) -> i32 {
    %c1_i32 = arith.constant 1 : i32
    %y = arith.addi %x, %c1_i32 : i32
    return %y : i32
  }

  // 顶层符号 3：入口函数。里面有 for + if，形成多层嵌套 Region/Block
  func.func @main() -> i32 {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c4 = arith.constant 4 : index
    %zero = arith.constant 0 : i32

    %G = memref.get_global @G : memref<4xi32>

    // scf.for：1 个 region，region 里 1 个 block（含 terminator scf.yield）
    %res = scf.for %i = %c0 to %c4 step %c1 iter_args(%acc = %zero) -> (i32) {
      %v   = memref.load %G[%i] : memref<4xi32>
      %v2  = func.call @callee(%v) : (i32) -> i32
      %cmp = arith.cmpi sgt, %v2, %acc : i32

      // scf.if：两个子 region（then/else），各 1 个 block
      %new = scf.if %cmp -> (i32) {
        scf.yield %v2 : i32
      } else {
        scf.yield %acc : i32
      }

      scf.yield %new : i32
    }
    return %res : i32
  }

  // 嵌套模块：验证 Symbol 遍历/查找时的层级作用域
  module @inner {
    func.func @inner_fn() {
      return
    }
  }
}
