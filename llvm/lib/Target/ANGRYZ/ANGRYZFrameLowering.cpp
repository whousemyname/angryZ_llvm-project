#include "ANGRYZFrameLowering.h"
#include "ANGRYZSubtarget.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/Support/LEB128.h"

#include <algorithm>
using namespace llvm;

ANGRYZFrameLowering::ANGRYZFrameLowering(const ANGRYZSubtarget &STI) 
    :   TargetFrameLowering(StackGrowsDown, Align(16), 0, Align(16)), 
        STI(STI){
}

void ANGRYZFrameLowering::emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const {}
void ANGRYZFrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {}
bool ANGRYZFrameLowering::hasFP(const MachineFunction &MF) const {return false;}

