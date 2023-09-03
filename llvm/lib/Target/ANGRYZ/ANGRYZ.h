#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZ_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZ_H


#include "ANGRYZTargetMachine.h"
#include "llvm/Support/CodeGen.h"

namespace llvm{

class FunctionPass;

FunctionPass *createANGRYZISelDag(ANGRYZTargetMachine &TM, CodeGenOpt::Level OptLevel);

}   //namespace llvm


#endif