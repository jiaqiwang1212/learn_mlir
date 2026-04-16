# Chapter 11: MLIR PDLL — Pattern Definition Language

## What is PDLL?

**PDLL** (Pattern Definition Language for LLVM) is a declarative language for writing MLIR rewrite patterns. It compiles to C++ via the `mlir-pdll` tool, generating a header that exposes `populateGeneratedPDLLPatterns()`.

### PDLL vs PDL

| | PDLL | PDL (older) |
|---|---|---|
| File extension | `.pdll` | `.pdl` |
| Syntax | High-level, ML-like | Low-level IR |
| Native C++ escapes | `[{ }]` blocks | Limited |
| Tooling | `mlir-pdll` | `mlir-pdll` (legacy) |
| Status | Current | Superseded |

PDLL is the recommended path. PDL files are the intermediate IR that PDLL compiles to — you rarely write PDL directly.

---

## PDLL File Structure

A PDLL file contains three kinds of declarations:

```pdll
// 1. Constraint: a predicate on an op, value, or attribute
Constraint MyConstraint(op: Op)[{
  // C++ code returning mlir::LogicalResult
  return mlir::success(/* ... */);
}];

// 2. Rewrite: a named rewrite action (optional native C++)
Rewrite MyRewrite(op: Op) -> Op[{
  // C++ code returning a new op
}];

// 3. Pattern: the match-and-rewrite rule
Pattern MyPattern {
  // Bind ops and values
  let target = op<dialect.opname>(/* operand patterns */);
  // Apply constraints
  MyConstraint(target);
  // Rewrite action
  replace target with /* replacement */;
}
```

### Op syntax

```pdll
op<arith.addi>           // matches any arith.addi
op<arith.constant>       // matches any arith.constant
op<arith.addi>(a, b)     // matches addi with specific operands
```

### Binding results

```pdll
let x = op<arith.constant>;   // bind the op
x.0                            // 0th result (Value) of op x
```

### Pattern actions

| Action | Meaning |
|--------|---------|
| `replace op with other_op` | replace op's results with other_op's results |
| `replace op with value` | replace op's results with a Value |
| `erase op` | remove the op (asserts use_empty internally) |

---

## C++ Integration Flow

```
fold_patterns.pdll
       │
       ▼  mlir-pdll (run by CMake via add_mlir_pdll_library)
FoldPatterns.h.inc          ← generated C++ header
       │
       ▼  #include "FoldPatterns.h.inc"
pdll_fold_demo.cpp
  populateGeneratedPDLLPatterns(patterns);   ← registers PDL patterns
  applyPatternsGreedily(func, frozen);       ← applies them
```

### CMake wiring

```cmake
# Compile .pdll → .h.inc using mlir-pdll
add_mlir_pdll_library(PDLLFoldPatternsIncGen
  fold_patterns.pdll
  FoldPatterns.h.inc
)

# Ensure .h.inc exists before .cpp compiles
add_dependencies(pdll_fold_demo PDLLFoldPatternsIncGen)

# Add build dir so #include "FoldPatterns.h.inc" resolves
target_include_directories(pdll_fold_demo PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
```

### In the C++ driver

```cpp
#include "FoldPatterns.h.inc"   // exposes populateGeneratedPDLLPatterns

// Register and apply
RewritePatternSet patterns(&ctx);
populateGeneratedPDLLPatterns(patterns);
FrozenRewritePatternSet frozen(std::move(patterns));
applyPatternsGreedily(func, frozen);
```

---

## Three Progressive Demos

### Demo 1 — pdll_fold_demo: Structural Match and Replace

`fold_patterns.pdll` matches `arith.addi` with two `arith.constant` operands and replaces it with the LHS constant:

```pdll
Pattern FoldConstantAdd {
  let lhs = op<arith.constant>;
  let rhs = op<arith.constant>;
  let add = op<arith.addi>(lhs.0, rhs.0);
  replace add with lhs.0;
}
```

**Before:**
```mlir
func.func @fold_test() -> i32 {
  %c3 = arith.constant 3 : i32
  %c4 = arith.constant 4 : i32
  %r  = arith.addi %c3, %c4 : i32
  return %r : i32
}
```

**After:**
```mlir
func.func @fold_test() -> i32 {
  %c3 = arith.constant 3 : i32
  return %c3 : i32
}
```

### Demo 2 — pdll_strength_demo: Native C++ Constraint

`strength_patterns.pdll` uses a `[{ }]` native C++ constraint to check that the multiplier constant equals 2, then replaces `muli(x, 2)` with `addi(x, x)`:

```pdll
Constraint isConstantTwo(op: Op)[{
  auto constOp = llvm::dyn_cast<mlir::arith::ConstantIntOp>(op.get());
  if (!constOp) return mlir::failure();
  return mlir::success(constOp.value() == 2);
}];

Pattern StrengthReduceMulByTwo {
  let multiplier = op<arith.constant>;
  isConstantTwo(multiplier);
  let input : Value;
  let mul = op<arith.muli>(input, multiplier.0);
  replace mul with op<arith.addi>(input, input);
}
```

**Before:**
```mlir
%x  = arith.constant 5 : i32
%c2 = arith.constant 2 : i32
%r  = arith.muli %x, %c2 : i32
```

**After:**
```mlir
%x = arith.constant 5 : i32
%r = arith.addi %x, %x : i32
```

### Demo 3 — pdll_dce_demo: Erase with use_empty Guard

`dce_patterns.pdll` erases dead `arith.constant` ops. The `IsUnused` constraint prevents erasing live ops (which would hit an assertion in `RewriterBase::eraseOp`):

```pdll
Constraint IsUnused(op: Op)[{
  return mlir::success(op->use_empty());
}];

Pattern EraseDeadConstant {
  let dead = op<arith.constant>;
  IsUnused(dead);
  erase dead;
}
```

**Before:**
```mlir
func.func @dce_test() {
  %c10 = arith.constant 10 : i32   // never used
  %c20 = arith.constant 20 : i32   // never used
  return
}
```

**After:**
```mlir
func.func @dce_test() {
  return
}
```

---

## PDLL vs Classic C++ DRR Comparison

The same constant-folding rule expressed two ways:

### Classic C++ RewritePattern

```cpp
struct FoldConstantAddPattern : public OpRewritePattern<arith::AddIOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(arith::AddIOp op,
                                PatternRewriter &rewriter) const override {
    auto lhs = op.getLhs().getDefiningOp<arith::ConstantIntOp>();
    auto rhs = op.getRhs().getDefiningOp<arith::ConstantIntOp>();
    if (!lhs || !rhs)
      return failure();
    // Replace uses of addi result with the lhs constant
    rewriter.replaceAllUsesWith(op.getResult(), lhs.getResult());
    rewriter.eraseOp(op);
    return success();
  }
};

// Registration
patterns.add<FoldConstantAddPattern>(&ctx);
```

### PDLL Equivalent

```pdll
Pattern FoldConstantAdd {
  let lhs = op<arith.constant>;
  let rhs = op<arith.constant>;
  let add = op<arith.addi>(lhs.0, rhs.0);
  replace add with lhs.0;
}
```

### Key Differences

| Aspect | C++ RewritePattern | PDLL |
|--------|-------------------|------|
| Lines of code | ~15 | ~5 |
| Compile step | None | `mlir-pdll` generates `.h.inc` |
| Native C++ | Full C++ | `[{ }]` escape blocks |
| Readability | Imperative | Declarative |
| Debuggability | Full debugger | PDL interpreter |
| Best for | Complex rewrites | Pattern-heavy passes |

---

## Building

```bash
TARGET=ch-11 ./build_npu_mlir_fixed.sh
```

Binaries land in `install/bin/`:
```bash
./install/bin/pdll_fold_demo
./install/bin/pdll_strength_demo
./install/bin/pdll_dce_demo
```

---

## Reference

- MLIR PDLL docs: https://mlir.llvm.org/docs/PDLL/
- PDLL tests: `externals/llvm-project/mlir/test/mlir-pdll/`
- `add_mlir_pdll_library`: `externals/llvm-project/mlir/cmake/modules/AddMLIR.cmake`
