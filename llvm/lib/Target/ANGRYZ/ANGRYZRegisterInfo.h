#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZREGISTERINFO_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZREGISTERINFO_H

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include <cstdint>

#define GET_REGINFO_HEADER
#include "ANGRYZGenRegisterInfo.inc"

namespace llvm{

class ANGRYZRegisterInfo : public ANGRYZGenRegisterInfo {

public:
    explicit ANGRYZRegisterInfo(unsigned HwMode);

    const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

    BitVector getReservedRegs(const MachineFunction &MF) const override;

    bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;
    
    Register getFrameRegister(const MachineFunction &MF) const override;

    /*
    const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;
    */
};

}   //namespace llvm


#endif