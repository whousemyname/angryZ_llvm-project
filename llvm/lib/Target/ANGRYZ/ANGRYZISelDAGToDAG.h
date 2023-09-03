#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELDAGTODAG_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELDAGTODAG_H

#include "ANGRYZTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Support/CodeGen.h"

namespace llvm {

class ANGRYZDAGToDAGISel : public SelectionDAGISel {
    const ANGRYZSubtarget *subtarget = nullptr;

public:
    static char ID;
    ANGRYZDAGToDAGISel() = delete;
    explicit ANGRYZDAGToDAGISel(ANGRYZTargetMachine &TargetMachine, 
                                CodeGenOpt::Level OptLevel) 
        : SelectionDAGISel(ID, TargetMachine, OptLevel) {}

    bool runOnMachineFunction(MachineFunction &MF) override {   //debug_b
        subtarget = &MF.getSubtarget<ANGRYZSubtarget>();
        return SelectionDAGISel::runOnMachineFunction(MF);
    }
    void Select(SDNode *Node) override;


private:
#include "ANGRYZGenDAGISel.inc"
};

namespace ANGRYZ {

    
}   //namespace ANGRYZ

}   //namespace llvm

#endif