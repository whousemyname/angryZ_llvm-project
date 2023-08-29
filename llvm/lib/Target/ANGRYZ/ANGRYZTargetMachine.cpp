
#include "ANGRYZTargetMachine.h"
#include "TargetInfo/ANGRYZTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeANGRYZTarget() {
    //RegisterTargetMachine<ANGRYZTargetMachine> X(getANGRYZTarget());
}
