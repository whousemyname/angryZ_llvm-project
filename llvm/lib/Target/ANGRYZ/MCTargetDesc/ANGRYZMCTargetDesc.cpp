#include "ANGRYZMCTargetDesc.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeANGRYZTargetMC() {
    /*
     * TargetRegistry::RegisterMCInstrInfo(getANGRYZTarget(), createANGRYZMCInstrInfo);
       ......
       todo
    */

}