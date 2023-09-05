#include  "ANGRYZRegisterInfo.h"
#include "MCTargetDesc/ANGRYZMCTargetDesc.h"
#include "ANGRYZ.h"
#include "ANGRYZSubtarget.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Support/ErrorHandling.h"

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
    BitVector Reserved(getNumRegs());
    Reserved.set(ANGRYZ::H0);
    Reserved.set(ANGRYZ::H2);
    Reserved.set(ANGRYZ::H3);
    Reserved.set(ANGRYZ::H4);
    return Reserved;
}

bool ANGRYZRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                        unsigned FIOperandNum,
                        RegScavenger *RS) const {
    //todo
}

Register ANGRYZRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    return ANGRYZ::H8;
}


/*
    todo
*/
/*
const uint32_t *ANGRYZRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                    CallingConv::ID) const {
                        
}
*/


