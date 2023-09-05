#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZ_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZ_H

#include "llvm/Support/CodeGen.h"
#include "llvm/Target/TargetMachine.h"


namespace llvm{
class AsmPrinter;
class FunctionPass;
class InstructionSelector;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineOperand;
class PassRegistry;
class ANGRYZSubtarget;
class ANGRYZTargetMachine;

FunctionPass *createANGRYZISelDag(ANGRYZTargetMachine &TM, CodeGenOpt::Level OptLevel);

}   //namespace llvm


#endif