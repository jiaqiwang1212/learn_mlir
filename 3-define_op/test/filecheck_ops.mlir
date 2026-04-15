// FileCheck test for ch-3 Siren dialect ops.
//
// RUN: ch-3-opt %s --split-input-file | FileCheck %s --check-prefix=PARSE
// RUN: ch-3-opt %s --split-input-file --canonicalize | FileCheck %s --check-prefix=CANON

// ===----------------------------------------------------------------------===
// Test 1: siren.add roundtrip
// ===----------------------------------------------------------------------===
// Verify that siren.add is parsed and printed back correctly.
//
// PARSE-LABEL: func.func @test_add_roundtrip
// PARSE:         %[[RES:.+]] = "siren.add"(%arg0, %arg1)
// PARSE:         return %[[RES]]
//
// CANON-LABEL: func.func @test_add_roundtrip
// CANON:         "siren.add"
func.func @test_add_roundtrip(%a: i8, %b: i8) -> i8 {
  %0 = "siren.add"(%a, %b) : (i8, i8) -> i8
  return %0 : i8
}

// -----

// ===----------------------------------------------------------------------===
// Test 2: siren.sub constant folding
// ===----------------------------------------------------------------------===
// When both operands of siren.sub are constants, the op folds away and
// is replaced by a single arith.constant with the computed value.
//
// PARSE-LABEL: func.func @test_sub_constants
// PARSE:         "siren.sub"
// PARSE-NOT:     arith.constant 7
//
// CANON-LABEL: func.func @test_sub_constants
// CANON-NOT:     siren.sub
// CANON:         %[[C:.+]] = arith.constant 7 : i8
// CANON-NEXT:    return %[[C]]
func.func @test_sub_constants() -> i8 {
  %c10 = arith.constant 10 : i8
  %c3  = arith.constant 3  : i8
  %0   = "siren.sub"(%c10, %c3) : (i8, i8) -> i8
  return %0 : i8
}

// -----

// ===----------------------------------------------------------------------===
// Test 3: siren.sub with variable operand does NOT fold
// ===----------------------------------------------------------------------===
// Folding requires both operands to be constants.
// When one operand is an argument, fold() returns {} and the op survives.
//
// PARSE-LABEL: func.func @test_sub_no_fold
// CANON-LABEL: func.func @test_sub_no_fold
// CANON:         "siren.sub"
// CANON-NOT:     arith.constant
func.func @test_sub_no_fold(%a: i8) -> i8 {
  %c5  = arith.constant 5 : i8
  %0   = "siren.sub"(%a, %c5) : (i8, i8) -> i8
  return %0 : i8
}

// -----

// ===----------------------------------------------------------------------===
// Test 4: siren.sub with optional level attribute
// ===----------------------------------------------------------------------===
// SubOp has DefaultValuedOptionalAttr<I32Attr, "5"> for $level.
// When not supplied, the attribute is absent from the output.
// When supplied, it appears in the generic form.
//
// PARSE-LABEL: func.func @test_sub_with_level
// PARSE:         "siren.sub"
// PARSE-SAME:    level = 2
func.func @test_sub_with_level(%a: i8, %b: i8) -> i8 {
  %0 = "siren.sub"(%a, %b) {level = 2 : i32} : (i8, i8) -> i8
  return %0 : i8
}
