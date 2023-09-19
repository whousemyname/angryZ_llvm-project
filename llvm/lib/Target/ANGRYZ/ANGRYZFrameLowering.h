#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZFRAMELOWERING_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZFRAMELOWERING_H



#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/MC/TargetRegistry.h"

namespace llvm {
class ANGRYZSubtarget;


class ANGRYZFrameLowering : public TargetFrameLowering {
protected:
  const ANGRYZSubtarget &STI;

public:
  explicit ANGRYZFrameLowering(const ANGRYZSubtarget &STI) ;

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  bool hasFP(const MachineFunction &MF) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI) const override;
  /*
    [PrologEpilogInserter] 这个pass中的 hook，
    pass中部分过程是设置 CSR被调用者保存寄存器 在stack的spill以及restore
    subtarget --》 registerInfo --》 calleeSavedRegister
    之后调用这个hook来判断是否需要设置这个寄存器
  */
  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;  
  /*
    [PrologEpilogInserter] 的 hook
    replaceFrameIndicesBackward --》 replaceFrameIndicesBackward --》eliminateFrameIndex --》getFrameIndexReference
  */
  StackOffset getFrameIndexReference(const MachineFunction &MF, int FI,
                                     Register &FrameReg) const override;

  uint64_t getStackSizeWithRVVPadding(const MachineFunction &MF) const;

  // Get the first stack adjustment amount for SplitSPAdjust.
  // Return 0 if we don't want to to split the SP adjustment in prologue and
  // epilogue.
  uint64_t getFirstSPAdjustAmount(const MachineFunction &MF) const;

private:
  void determineFrameLayout (MachineFunction &MF) const;
};  //end class ANGRYZFrameLowering

}   // namespace llvm


#endif