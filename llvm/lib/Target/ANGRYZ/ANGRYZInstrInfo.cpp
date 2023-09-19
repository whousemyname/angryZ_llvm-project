#include "ANGRYZInstrInfo.h"
#include "ANGRYZ.h"
#include "ANGRYZSubtarget.h"
#include "ANGRYZTargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#define GET_INSTRINFO_NAMED_OPS
#include "ANGRYZGenInstrInfo.inc"

ANGRYZInstrInfo::ANGRYZInstrInfo(ANGRYZSubtarget &STI)
    :   ANGRYZGenInstrInfo(ANGRYZ::ADJCALLSTACKDOWN, ANGRYZ::ADJCALLSTACKUP), 
        STI(STI){

}

void ANGRYZInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                      MachineBasicBlock::iterator I, Register SrcReg,
                      bool IsKill, int FI,
                      const TargetRegisterClass *RC,
                      const TargetRegisterInfo *TRI,
                      Register VReg) const {
  // TODO ANGRYZInstrInfo::storeRegToStackSlot
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  MachineFunction *MF = MBB.getParent();
  MachineFrameInfo &MFI = MF->getFrameInfo();


  unsigned Opcode;
  bool IsScalableVector = true;
  if (ANGRYZ::HPRRegClass.hasSubClassEq(RC)) {
    Opcode = ANGRYZ::SW;    // TODO : SD的情况
    IsScalableVector = false;
  } else  // TODO : else if 
    llvm_unreachable("Can't store this register to stack slot");

  if (IsScalableVector) {
    llvm_unreachable(" TODO : IsScalableVector not impletement in ANGRYZInstrInfo::storeRegToStackSlot!");
  } else {
    MachineMemOperand *MMO = MF->getMachineMemOperand(
        MachinePointerInfo::getFixedStack(*MF, FI), MachineMemOperand::MOStore,
        MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

    BuildMI(MBB, I, DL, get(Opcode))
        .addReg(SrcReg, getKillRegState(IsKill))
        .addFrameIndex(FI)
        .addImm(0)
        .addMemOperand(MMO);
  }
}

void ANGRYZInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator I, Register DstReg,
                            int FI, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const {

  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  MachineFunction *MF = MBB.getParent();
  MachineFrameInfo &MFI = MF->getFrameInfo();
  unsigned Opcode;
  bool IsScalableVector = true;
  if (ANGRYZ::HPRRegClass.hasSubClassEq(RC)) {
    Opcode = ANGRYZ::LW;  // TODO 
    IsScalableVector = false;
  } else
    llvm_unreachable("Can't load this register from stack slot");

  if (IsScalableVector) {
    // TODO :
  } else {
    MachineMemOperand *MMO = MF->getMachineMemOperand(
        MachinePointerInfo::getFixedStack(*MF, FI), MachineMemOperand::MOLoad,
        MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

    BuildMI(MBB, I, DL, get(Opcode), DstReg)
        .addFrameIndex(FI)
        .addImm(0)
        .addMemOperand(MMO);
  }
}