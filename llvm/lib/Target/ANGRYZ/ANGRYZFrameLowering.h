#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZFRAMELOWERING_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZFRAMELOWERING_H



#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

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
};

}   // namespace llvm


#endif