#include "ANGRYZISelLowering.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineValueType.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetLowering.h"

using namespace llvm;

#include "ANGRYZGenCallingConv.inc"

ANGRYZTargetLowering::ANGRYZTargetLowering(const TargetMachine &TM, const ANGRYZSubtarget &STI) 
    :   TargetLowering(TM), Subtarget(STI) {
    
    //todo
    addRegisterClass(MVT::i32, &(ANGRYZ::HPRRegClass));

    computeRegisterProperties(Subtarget.getRegisterInfo());

}

const char *ANGRYZTargetLowering::getTargetNodeName(unsigned Opcode) const {
    switch (Opcode) {
        case ANGRYZISD::RET_FLAG: 
            return "ANGRYZISD::RET_FLAG";
        defalut : 
            return nullptr;
    }
}

// Lower incoming arguments, copy physregs into vregs
SDValue ANGRYZTargetLowering::LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                            bool IsVarArg,
                            const SmallVectorImpl<ISD::InputArg> &Ins,
                            const SDLoc &DL, SelectionDAG &DAG,
                            SmallVectorImpl<SDValue> &InVals) const {

    SmallVector<CCValAssign, 16> ArgLocs;
    MachineFunction &MF = DAG.getMachineFunction();
    CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
    CCInfo.AnalyzeFormalArguments(Ins, CC_ANGRYZ);

    for (unsigned i = 0, e = ArgLocs.size(); i < e; ++i) {
        CCValAssign &VA = ArgLocs[i];
    }

    return Chain;
}


SDValue ANGRYZTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                    const SmallVectorImpl<ISD::OutputArg> &Outs,
                    const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                    SelectionDAG &DAG) const {
    SmallVector<CCValAssign, 16> RVLocs;

    CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs, 
                   *DAG.getContext());
    CCInfo.AnalyzeReturn(Outs, RetCC_ANGRYZ);

    SDValue Glue;
    SmallVector<SDValue, 4> RetOps(1, Chain);

    for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
        CCValAssign &VA = RVLocs[i];
        assert(VA.isRegLoc() && "can only return in registers!");
        Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Glue);
        Glue = Chain.getValue(1);
        RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

    RetOps[0] = Chain;

    if (Glue.getNode()) {   
        RetOps.push_back(Glue);
    }
    return DAG.getNode(ANGRYZISD::RET_FLAG, DL, MVT::Other, RetOps);
}