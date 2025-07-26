#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <string>
#include <vector>
static llvm::LLVMContext Context;

static llvm::Module *ModuleOb = new llvm::Module("MyModule", Context);

static std::vector<std::string> FuncArgs;
typedef llvm::SmallVector<llvm::BasicBlock *, 16> BBList;
typedef llvm::SmallVector<llvm::Value *, 16> ValList;

// 添加一个func
llvm::Function *createFunc(llvm::IRBuilder<> &Builder, std::string Name) {
  std::vector<llvm::Type *> Integers(FuncArgs.size(),
                                     llvm::Type::getInt32Ty(Context));
  llvm::FunctionType *funcType =
      llvm::FunctionType::get(Builder.getInt32Ty(), Integers, false);
  llvm::Function *fooFunc = llvm::Function::Create(
      funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
  return fooFunc;
}

void setFuncArgs(llvm::Function *fooFunc, std::vector<std::string> FuncArgs) {
  unsigned i = 0;
  llvm::Function::arg_iterator AI, AE;
  for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE;
       ++AI, ++i) {
    AI->setName(FuncArgs[i]);
  }
}

// 添加一个bb
llvm::BasicBlock *createBB(llvm::Function *fooFunc, std::string Name) {
  return llvm::BasicBlock::Create(Context, Name, fooFunc);
}

llvm::Value *createArith(llvm::IRBuilder<> &Builder, llvm::Value *L,
                         llvm::Value *R) {
  return Builder.CreateMul(L, R, "multmp");
}

llvm::GlobalVariable *createGlob(llvm::IRBuilder<> &Builder, std::string Name) {
  ModuleOb->getOrInsertGlobal(Name, Builder.getInt32Ty());
  llvm::GlobalVariable *gVar = ModuleOb->getNamedGlobal(Name);
  gVar->setLinkage(llvm::GlobalValue::CommonLinkage);
  gVar->setAlignment(llvm::Align(4));
  return gVar;
}

llvm::Value *createIfElse(llvm::IRBuilder<> &Builder, BBList List, ValList VL) {
  llvm::Value *Condtn = VL[0];
  llvm::Value *arg1 = VL[1];
  llvm::BasicBlock *thenBB = List[0];
  llvm::BasicBlock *elseBB = List[1];
  llvm::BasicBlock *mergeBB = List[2];

  Builder.CreateCondBr(Condtn, thenBB, elseBB); // 条件跳转

  Builder.SetInsertPoint(thenBB);
  llvm::Value *ThenVal =
      Builder.CreateAdd(arg1, Builder.getInt32(1), "thenaddtmp");
  Builder.CreateBr(mergeBB);

  Builder.SetInsertPoint(elseBB);
  llvm::Value *ElseVal =
      Builder.CreateSub(arg1, Builder.getInt32(2), "elseaddtmp");
  Builder.CreateBr(mergeBB);

  unsigned PhiBBSize = List.size();
  Builder.SetInsertPoint(mergeBB);
  llvm::PHINode *Phi =
      Builder.CreatePHI(llvm::Type::getInt32Ty(Context), PhiBBSize, "iftmp");
  Phi->addIncoming(ThenVal, thenBB);
  Phi->addIncoming(ElseVal, elseBB);

  return Phi;
}

llvm::Value *createLoop(llvm::IRBuilder<> &Builder, BBList List, ValList VL,
                        llvm::Value *StartVal, llvm::Value *EndVal) {
  llvm::BasicBlock *PreheaderBB = Builder.GetInsertBlock();
  llvm::Value *val = VL[0];
  llvm::BasicBlock *loopBB = List[0];

  Builder.CreateBr(loopBB);
  Builder.SetInsertPoint(loopBB);
  llvm::PHINode *IndVar =
      Builder.CreatePHI(llvm::Type::getInt32Ty(Context), 2, "i");
  IndVar->addIncoming(StartVal, PreheaderBB);
  llvm::Value *Add = Builder.CreateAdd(val, Builder.getInt32(5), "addtmp");
  llvm::Value *StepVal = Builder.getInt32(1);
  llvm::Value *NextVal = Builder.CreateAdd(IndVar, StepVal, "nextval");
  llvm::Value *EndCond = Builder.CreateICmpULT(IndVar, EndVal, "endcond");

  EndCond = Builder.CreateICmpNE(EndCond, Builder.getInt1(0), "loopcond");
  llvm::BasicBlock *AfterBB = List[1];
  Builder.CreateCondBr(EndCond, loopBB, AfterBB);
  Builder.SetInsertPoint(AfterBB);
  IndVar->addIncoming(NextVal, loopBB);
  return Add; // 返回循环体内的计算结果
}

int main() {
  FuncArgs.push_back("a");
  FuncArgs.push_back("b");

  static llvm::IRBuilder<> Builder(Context);
  llvm::GlobalVariable *gVar = createGlob(Builder, "x");

  llvm::Function *fooFunc = createFunc(Builder, "foo");
  setFuncArgs(fooFunc, FuncArgs);
  llvm::BasicBlock *entry = createBB(fooFunc, "entry");

  Builder.SetInsertPoint(entry);
  llvm::Value *arg1 = fooFunc->getArg(0);
  llvm::Value *constant = Builder.getInt32(2);
  llvm::Value *val = createArith(Builder, arg1, constant);

  llvm::Value *val2 = Builder.getInt32(100);
  llvm::Value *Compare = Builder.CreateICmpULT(val, val2, "compare");
  llvm::Value *Condtn =
      Builder.CreateICmpNE(Compare, Builder.getInt1(0), "ifcond");

  ValList VL;
  VL.push_back(Condtn);
  VL.push_back(arg1);

  llvm::BasicBlock *thenBB = createBB(fooFunc, "then");
  llvm::BasicBlock *elseBB = createBB(fooFunc, "else");
  llvm::BasicBlock *mergeBB = createBB(fooFunc, "ifcont");
  BBList List;
  List.push_back(thenBB);
  List.push_back(elseBB);
  List.push_back(mergeBB);

  llvm::Value *v = createIfElse(Builder, List, VL);

  List.clear(); // 清空 List 变量
  VL.clear();   // 清空 ValList

  VL.push_back(arg1);

  llvm::BasicBlock *loopBB = createBB(fooFunc, "loop");
  llvm::BasicBlock *AfterBB = createBB(fooFunc, "afterloop");
  List.push_back(loopBB);
  List.push_back(AfterBB);

  llvm::Value *StartVal = Builder.getInt32(1);
  llvm::Value *EndVal = fooFunc->getArg(1);
  llvm::Value *res = createLoop(Builder, List, VL, StartVal, EndVal);

  Builder.CreateRet(res); // return 插入到entry block

  llvm::verifyFunction(*fooFunc);
  ModuleOb->dump();

  return 0;
}