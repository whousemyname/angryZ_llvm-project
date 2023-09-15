#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZREGISTERINFO_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZREGISTERINFO_H


#include "llvm/CodeGen/TargetRegisterInfo.h"



#define GET_REGINFO_HEADER
#include "ANGRYZGenRegisterInfo.inc"

namespace llvm{

class ANGRYZRegisterInfo : public ANGRYZGenRegisterInfo {

public:
    explicit ANGRYZRegisterInfo(unsigned HwMode);

    const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

    BitVector getReservedRegs(const MachineFunction &MF) const override;

    bool eliminateFrameIndex(MachineBasicBlock::iterator MBBI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;
    
    Register getFrameRegister(const MachineFunction &MF) const override;

    /*
    const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;
    */

    // Update DestReg to have the value SrcReg plus an offset.  This is
    // used during frame layout, and we may need to ensure that if we
    // split the offset internally that the DestReg is always aligned,
    // assuming that source reg was.
    void adjustReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator II,
                 const DebugLoc &DL, Register DestReg, Register SrcReg,
                 StackOffset Offset, MachineInstr::MIFlag Flag,
                 MaybeAlign RequiredAlign) const;
};  //end class ANGRYZRegisterInfo

}   //namespace llvm


#endif