#include  "ANGRYZRegisterInfo.h"
#include "ANGRYZFrameLowering.h"
#include "MCTargetDesc/ANGRYZMCTargetDesc.h"
#include "ANGRYZ.h"
#include "ANGRYZSubtarget.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TypeSize.h"
#include <cstdint>

#define GET_REGINFO_TARGET_DESC
#include "ANGRYZGenRegisterInfo.inc"
using namespace llvm;


/*
struct ANGRYZGenRegisterInfo : public TargetRegisterInfo {
  explicit ANGRYZGenRegisterInfo(unsigned RA, unsigned D = 0, unsigned E = 0,
      unsigned PC = 0, unsigned HwMode = 0);
*/
ANGRYZRegisterInfo::ANGRYZRegisterInfo(unsigned HwMode) 
    :   ANGRYZGenRegisterInfo(ANGRYZ::H1, 0, 0, 0, HwMode) {

}

const MCPhysReg *ANGRYZRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
    return CC_Save_SaveList;
}

BitVector ANGRYZRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  const ANGRYZFrameLowering *TFI = getFrameLowering(MF);
  BitVector Reserved(getNumRegs());

  //isRegisterReservedByUser

  //markSuperRegs


  Reserved.set(ANGRYZ::H0);
  Reserved.set(ANGRYZ::H2);
  Reserved.set(ANGRYZ::H3);
  Reserved.set(ANGRYZ::H4);

  if (TFI->hasFP(MF)) 
    Reserved.set(ANGRYZ::H8);

  return Reserved;
}

bool ANGRYZRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MBBI, int SPAdj,
                        unsigned FIOperandNum,
                        RegScavenger *RS) const {   //TODO eliminateFrameIndex
    //todo
    MachineInstr &MI = *MBBI;
    MachineFunction &MF = *MBBI->getParent()->getParent();
    MachineRegisterInfo &RI = MF.getRegInfo();
    const ANGRYZSubtarget &ST = MF.getSubtarget<ANGRYZSubtarget>();
    DebugLoc DL = MI.getDebugLoc();

    int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
    Register FrameReg;
    StackOffset Offset = 
        getFrameLowering(MF)->getFrameIndexReference(MF, FrameIndex, FrameReg);
    bool IsRVVSpill = false;    //TODO
    if (!IsRVVSpill)
      Offset += StackOffset::getFixed(MI.getOperand(FIOperandNum + 1).getImm());

    if (Offset.getScalable()) {

    }

    if (!isInt<32>(Offset.getFixed())) 
      report_fatal_error(
          "Frame offsets outside of the signed 32-bit range not supported");
    
    if (!IsRVVSpill) {
      if (MI.getOpcode() == ANGRYZ::ADDI && !isInt<12>(Offset.getFixed())) {
        MI.getOperand(FIOperandNum + 1).ChangeToImmediate(0);
      } else {
        int64_t Val = Offset.getFixed();
        int64_t Lo12 = SignExtend64<12>(Val);
        MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Lo12);
        Offset = 
            StackOffset::get((uint64_t)Val - (uint64_t)Lo12,
                Offset.getScalable());
      }
    }

    if (Offset.getScalable() || Offset.getFixed()) {

    } else {
      MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg,
                                       false, false, false);
    }

    if (MI.getOpcode() == ANGRYZ::ADDI && 
        MI.getOperand(0).getReg() == MI.getOperand(1).getReg() && 
        MI.getOperand(2).getImm() == 0) {

      MI.eraseFromParent();
      return true;
    }

    switch (MI.getOpcode()) {
      
    }
    return false;
}

Register ANGRYZRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = getFrameLowering(MF);
  return TFI->hasFP(MF) ? ANGRYZ::H8 : ANGRYZ::H2;
}


/*
    todo
*/
/*
const uint32_t *ANGRYZRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                    CallingConv::ID) const {
                        
}
*/

/*
  toRead
  todo
*/
void ANGRYZRegisterInfo::adjustReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator II,
                 const DebugLoc &DL, Register DestReg, Register SrcReg,
                 StackOffset Offset, MachineInstr::MIFlag Flag,
                 MaybeAlign RequiredAlign) const {

  if (DestReg == SrcReg && !Offset.getFixed() && !Offset.getScalable())
      return;
  
  MachineFunction &MF = *MBB.getParent();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  const ANGRYZSubtarget &ST = MF.getSubtarget<ANGRYZSubtarget>();
  const ANGRYZInstrInfo *TII = ST.getInstrInfo();

  bool KillSrcReg = false;

  if (Offset.getScalable()) {
    /*
      todo
    */
  }
  int64_t Val = Offset.getFixed();
  if (DestReg == SrcReg && Val == 0) 
    return;
  
  const uint64_t Align = RequiredAlign.valueOrOne().value();

  if (isInt<12>(Val)) {
    BuildMI(MBB, II, DL, TII->get(ANGRYZ::ADDI), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrcReg))
        .addImm(Val)
        .setMIFlag(Flag);
    return ;
  }
}   //end function adjustReg


