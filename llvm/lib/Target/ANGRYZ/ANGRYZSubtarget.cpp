
#include "ANGRYZSubtarget.h"
#include "ANGRYZTargetMachine.h" 
#include "llvm/TargetParser/Triple.h"
using namespace llvm;

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "ANGRYZGenSubtargetInfo.inc"

/*
    使用tablegen生成的CC_ANGRYZ，对于riscv是使用CC_RISCV，自己实现，并没有调用tblgen生成的
*/


ANGRYZSubtarget::ANGRYZSubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                 StringRef FS, StringRef ABIName, const TargetMachine &TM) 
    :   ANGRYZGenSubtargetInfo(TT, CPU, TuneCPU, FS), 
        FrameLowering(initializeSubtargetDependencies(TT, CPU, TuneCPU, FS, ABIName)), 
        InstrInfo(*this),
        RegInfo(getHwMode()),
        TLInfo(TM, *this){
    

    //todo  riscv在这里重新设置 legalizer以及instrselector用于 global selector
}

ANGRYZSubtarget &
ANGRYZSubtarget::initializeSubtargetDependencies(const Triple &TT, StringRef CPU,
                                                StringRef TuneCPU, StringRef FS,
                                                StringRef ABIName) {
    // Determine default and user-specified characteristics
    /*
        ABI的计算
        其他一些属性的计算
        调用ParseSubtargetFeatures()方法
        像reginfo instrinfo framelowering targetlowering等的信息在构造中进行初始化

    */
    ParseSubtargetFeatures(CPU, TuneCPU, FS);
    return *this;
}
