#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {

  struct EncodeFunctionName : public FunctionPass {
    static char ID;
    EncodeFunctionName() : FunctionPass(ID) {}
    bool runOnFunction(Function &F) override {
      errs() << "EncodeFunctionName: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }
  };
}

char EncodeFunctionName::ID = 0;
static RegisterPass<EncodeFunctionName> X("encode", "encode function name Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
static llvm::RegisterStandardPasses Y(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    [](const llvm::PassManagerBuilder &Builder,
       llvm::legacy::PassManagerBase &PM) { PM.add(new EncodeFunctionName()); });