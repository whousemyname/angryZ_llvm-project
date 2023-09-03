#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZFRAMELOWERING_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZFRAMELOWERING_H


#include "ANGRYZSubtarget.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {

class ANGRYZFrameLowering : public TargetFrameLowering {

class ANGRYZSubtarget;

private:
    const ANGRYZSubtarget &Subtarget;

public:
    explicit ANGRYZFrameLowering(const ANGRYZSubtarget &STI) ;

    void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
    void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
    bool hasFP(const MachineFunction &MF) const override;
};

}   // namespace llvm


#endif