#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELDAGTODAG_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELDAGTODAG_H

#include "ANGRYZ.h"
#include "ANGRYZTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
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

    StringRef getPassName() const override;
    void Select(SDNode *Node) override;


    /*
        ComplexPattern-select
    */

    bool SelectAddrRegImm(SDValue Addr, SDValue &Base, SDValue &Offset, 
                          bool IsINX = false);
    bool SelectAddrFrameIndex (SDValue Addr, SDValue &Base, SDValue &Offset);

#include "ANGRYZGenDAGISel.inc"

};

namespace ANGRYZ {

    
}   //namespace ANGRYZ

}   //namespace llvm

#endif