#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELDAGTODAG_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELDAGTODAG_H

#include "ANGRYZ.h"
#include "ANGRYZTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/KnownBits.h"

namespace llvm {

class ANGRYZDAGToDAGISel : public SelectionDAGISel {
private:
    const ANGRYZSubtarget *Subtarget = nullptr;

public:
    static char ID;
    ANGRYZDAGToDAGISel() = delete;
    explicit ANGRYZDAGToDAGISel(ANGRYZTargetMachine &TargetMachine, 
                                CodeGenOpt::Level OptLevel) 
        : SelectionDAGISel(ID, TargetMachine, OptLevel) {}

    bool runOnMachineFunction(MachineFunction &MF) override {   //debug_b
        Subtarget = &MF.getSubtarget<ANGRYZSubtarget>();
        return SelectionDAGISel::runOnMachineFunction(MF);
    }
    void Select(SDNode *Node) override;

#include "ANGRYZGenDAGISel.inc"

};

namespace ANGRYZ {

    
}   //namespace ANGRYZ

}   //namespace llvm

#endif