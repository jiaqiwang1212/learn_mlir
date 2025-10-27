#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/raw_ostream.h"

// 不修改时使用 ArrayRef
void passByArrayRef(llvm::ArrayRef<int> arrRef) {
  for (int val : arrRef) {
    // Do something with val
    llvm::outs() << val << "\n";
  }
}

// 需要修改时使用 SmallVectorImpl，但不要使用 SmallVector<T, N> 传递参数
void passBySmallVectorImpl(llvm::SmallVectorImpl<int> &vecImpl) {
  vecImpl.push_back(5); // Example modification
  for (int val : vecImpl) {
    // Do something with val
    llvm::outs() << val << "\n";
  }
}

// StringRef 不拥有数据，只是一个轻量级的字符串视图
void passByStringRef(llvm::StringRef strRef) {
  llvm::outs() << "String content: " << strRef << "\n";
}

int main() {
  llvm::SmallVector<int, 4> vec = {1, 2, 3, 4};
  passByArrayRef(vec);
  passBySmallVectorImpl(vec);
  for (int val : vec) {
    llvm::outs() << "After modification: " << val << "\n";
  }

  passByStringRef("Hello, LLVM!");
  passByStringRef(std::string("Hello, std::string!"));
  passByStringRef(llvm::StringRef("Hello, StringRef!"));
  return 0;
}