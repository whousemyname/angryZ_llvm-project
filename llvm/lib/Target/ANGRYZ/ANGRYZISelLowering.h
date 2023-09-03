#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELLOWERING_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELLOWERING_H

#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "ANGRYZSubtarget.h"
#include "ANGRYZISelLowering.h"

namespace llvm{

class ANGRYZSubtarget;
class ANGRYZRegisterInfo;

class ANGRYZTargetLowering : public TargetLowering {
private:
    const ANGRYZSubtarget &Subtarget;
public:
    explicit ANGRYZTargetLowering(const TargetMachine &TM, const ANGRYZSubtarget &STI);

    const ANGRYZSubtarget &getSubtarget() const {
        return Subtarget;
    }

    // This method returns the name of a target specific DAG node.
    const char *getTargetNodeName(unsigned Opcode) const override;

    // Lower incoming arguments, copy physregs into vregs
    SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                                bool IsVarArg,
                                const SmallVectorImpl<ISD::InputArg> &Ins,
                                const SDLoc &DL, SelectionDAG &DAG,
                                SmallVectorImpl<SDValue> &InVals) const override;

    SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                        SelectionDAG &DAG) const override;
};

namespace ANGRYZISD {
enum NodeType : unsigned {
    FIRST_NUMBER = ISD::BUILTIN_OP_END, 
    RET_FLAG, 
};
}   //namespace ANGRYZISD


}   //namespace llvm

#endif