#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZINSTRINFO_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZINSTRINFO_H

#include "ANGRYZRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/DiagnosticInfo.h"

#define GET_INSTRINFO_HEADER
#define GET_INSTRINFO_OPERAND_ENUM
#include "ANGRYZGenInstrInfo.inc"

namespace llvm {

class ANGRYZSubtarget;

class ANGRYZInstrInfo : public ANGRYZGenInstrInfo {
protected:
  const ANGRYZSubtarget &STI;
public:
  explicit ANGRYZInstrInfo(ANGRYZSubtarget &STI);

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator MBBI, Register SrcReg,
                        bool IsKill, int FrameIndex,
                        const TargetRegisterClass *RC,
                        const TargetRegisterInfo *TRI,
                        Register VReg) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, Register DstReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const override;
};  // end class ANGRYZInstrInfo 

}   //end namespace llvm

#endif