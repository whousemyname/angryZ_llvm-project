#include "ANGRYZISelLowering.h"
#include "MCTargetDesc/ANGRYZMCTargetDesc.h"
#include "ANGRYZRegisterInfo.h"
#include "ANGRYZ.h"
#include "ANGRYZSubtarget.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Analysis/VectorUtils.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicsRISCV.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/KnownBits.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include <optional>

using namespace llvm;

#define DEBUG_TYPE "angryz-lower"

#include "ANGRYZGenCallingConv.inc"

ANGRYZTargetLowering::ANGRYZTargetLowering(const TargetMachine &TM, const ANGRYZSubtarget &STI) 
    :   TargetLowering(TM), STI(STI) {
    
    //todo
    addRegisterClass(MVT::i32, &(ANGRYZ::HPRRegClass));

    computeRegisterProperties(STI.getRegisterInfo());

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