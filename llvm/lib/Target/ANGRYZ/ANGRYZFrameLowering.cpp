#include "ANGRYZFrameLowering.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Support/Alignment.h"

using namespace llvm;

ANGRYZFrameLowering::ANGRYZFrameLowering(const ANGRYZSubtarget &STI) 
    :   TargetFrameLowering(StackGrowsDown, Align(16), 0, Align(16)), 
        Subtarget(STI){
}

void ANGRYZFrameLowering::emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const {}
void ANGRYZFrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {}
bool ANGRYZFrameLowering::hasFP(const MachineFunction &MF) const {return false;}

