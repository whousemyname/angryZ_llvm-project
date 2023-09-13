#include "ANGRYZTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target& llvm::getANGRYZTarget() {
    static Target ANGRYZTarget;
    return ANGRYZTarget;
}


/*
 * 
*/
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeANGRYZTargetInfo () {
    /*
        name : 对对应的是llc 输出target的name，比如llc --verison输出的就是name以及Desc
        BackendName ： 必须与td中的target定义的名称相同
    */
    RegisterTarget<Triple::angryz, false> X(getANGRYZTarget(), "angryz", "32-bit ANGRYZ", "ANGRYZ");
}