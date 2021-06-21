#include "llvm/Transforms/Obfuscation/StringObf.h"
#include "llvm/Transforms/Obfuscation/Utils.h"
#include "llvm/CryptoUtils.h"


using namespace llvm;
using namespace std;


namespace {
    struct StringObf : public FunctionPass {
        static char ID; // Pass identification, replacement for typeid
        bool flag;

        StringObf() : FunctionPass(ID) {}
        StringObf(bool flag) : FunctionPass(ID) {
            this->flag = flag;
        }

        bool runOnFunction(Function &F){
            if (toObfuscate(flag, &F, "strobf")) {
                for (BasicBlock &basicBlock : F) {
//                    errs() << ""<< basicBlock.getName() << "\r\n";
                    for (Instruction &inst : basicBlock){
//                        errs() << "StringObf: "<< inst << "\r\n";
                        for (Value *op : inst.operands()){
                            Value * stripPtr = op->stripPointerCasts();
                            if (stripPtr->getName().contains(".str")){
//                                errs() << *op <<"\r\n";
                                GlobalVariable* GV = dyn_cast<GlobalVariable>(stripPtr);
                                if (GV) {
                                    ConstantDataSequential *CDS =
                                            dyn_cast<ConstantDataSequential>(GV->getInitializer());
                                    if (CDS) {
                                        std::string str = CDS->getRawDataValues().str();
//                                        errs() << "str:" << str << "\r\n";
                                        uint8_t xor_key = llvm::cryptoutils->get_uint8_t();
//                                        errs() << "xor_key: "<< xor_key <<"\r\n";
                                        for (int i = 0; i < str.size(); ++i) {
                                            str[i] = str[i] ^ xor_key;
                                        }
//                                        errs() << "stripPtr->getName(): "<< stripPtr->getName() <<"\r\n";
                                        new AllocaInst(ArrayType::get(Type::getInt8Ty(F.getContext()),str.size()), 0, nullptr, 1,
                                                       Twine(stripPtr->getName() + ".allocaa"), &inst);


//                                        errs() << "xor str: "<< str <<"\r\n";
                                        AllocaInst* allocaInst_str =  new AllocaInst(ArrayType::get(Type::getInt8Ty(F.getContext()), str.size()), 0, nullptr, 1,
                                                                                     Twine(stripPtr->getName() + ".array"), &inst);
                                        Twine* twine_bitcast = new Twine(stripPtr->getName() + ".bitcast");
                                        BitCastInst* bitCastInst_str = new BitCastInst(allocaInst_str, Type::getInt8PtrTy(F.getParent()->getContext()), twine_bitcast->str(), &inst);

                                        ConstantInt* constantInt_xor_key = ConstantInt::get(Type::getInt8Ty(F.getContext()), xor_key);

                                        AllocaInst* allocaInst_xor_key = new AllocaInst(Type::getInt8Ty(F.getContext()), 0, nullptr, 1,Twine(stripPtr->getName() + ".key"), &inst);
                                        StoreInst* storeInst1_xor_key = new StoreInst(constantInt_xor_key, allocaInst_xor_key);
                                        storeInst1_xor_key->insertAfter(allocaInst_xor_key);
                                        LoadInst* loadInst_xor_key = new LoadInst(allocaInst_xor_key, "");
                                        loadInst_xor_key->insertAfter(storeInst1_xor_key);

                                        for (int i = 0; i < str.size(); ++i) {

                                            ConstantInt* index = ConstantInt::get(Type::getInt8Ty(F.getContext()), i);
                                            GetElementPtrInst* getElementPtrInst = GetElementPtrInst::CreateInBounds(bitCastInst_str, index);
                                            getElementPtrInst->insertBefore(&inst);

                                            ConstantInt* enc_ch = ConstantInt::get(Type::getInt8Ty(F.getContext()), str[i]);

                                            BinaryOperator* xor_inst = BinaryOperator::CreateXor(enc_ch, loadInst_xor_key);
                                            xor_inst->insertAfter(getElementPtrInst);

                                            StoreInst* storeInst =  new StoreInst(xor_inst, getElementPtrInst);
                                            storeInst->insertAfter(xor_inst);


                                        }

                                        op->replaceAllUsesWith(bitCastInst_str);
                                        GV->eraseFromParent();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return false;
        }
    };
}

char StringObf::ID = 0;
static RegisterPass<StringObf> X("stringobf", "String obf");

Pass *llvm::createStringObf(bool flag) {
    return new StringObf(flag);
}