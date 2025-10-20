include "mlir/IR/AttrTypeBase.td"

def MyTypeTrait : NativeTypeTrait<"MyTypeTrait",[{private:
  int internalCounter = 0;
public:
  void incrementCounter();
  }], [{
  void $cppClass::incrementCounter() {
    this->internalCounter++;
  }
  }]> {
  let cppNamespace = "::mlir";
}