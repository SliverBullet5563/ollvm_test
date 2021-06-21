//
// Created by tom on 2021/6/16.
//

#ifndef OLLVM_STRINGOBF_H
#define OLLVM_STRINGOBF_H
// LLVM include
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/CryptoUtils.h"

// Namespace
using namespace llvm;
using namespace std;

namespace llvm {
    Pass *createStringObf (bool flag);
}
#endif //OLLVM_STRINGOBF_H
