module attributes {
  spirv.target_env = #spirv.target_env<#spirv.vce<v1.0, [Shader], []>, #spirv.resource_limits<>>
} {
  func.func @test_arith_ops(%arg0: i32, %arg1: i32, %arg2: f32, %arg3: f32) -> (i32, i32, f32, f32, i1) {
    // 整数加法
    %0 = arith.addi %arg0, %arg1 : i32
    
    // 整数乘法
    %1 = arith.muli %arg0, %arg1 : i32
    
    // 浮点加法
    %2 = arith.addf %arg2, %arg3 : f32
    
    // 浮点乘法
    %3 = arith.mulf %arg2, %arg3 : f32
    
    // 整数比较
    %4 = arith.cmpi slt, %arg0, %arg1 : i32
    
    return %0, %1, %2, %3, %4 : i32, i32, f32, f32, i1
  }
  
  func.func @test_constants() -> (i32, f32) {
    // 整数常量
    %c42 = arith.constant 42 : i32
    
    // 浮点常量
    %f3_14 = arith.constant 3.14 : f32
    
    return %c42, %f3_14 : i32, f32
  }
  
  func.func @test_type_conversion(%arg0: i32, %arg1: f32) -> (f32, i32) {
    // 整数转浮点
    %0 = arith.sitofp %arg0 : i32 to f32
    
    // 浮点转整数
    %1 = arith.fptosi %arg1 : f32 to i32
    
    return %0, %1 : f32, i32
  }
}


// 测试ApplyPatternAction: mlir-opt test_action.mlir --convert-arith-to-spirv --log-actions-to=-