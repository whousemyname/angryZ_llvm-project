#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELLOWERING_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZISELLOWERING_H

#include "ANGRYZ.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/TargetParser/RISCVTargetParser.h"
#include <optional>

namespace llvm{

class ANGRYZRegisterInfo;

namespace ANGRYZISD {
enum NodeType : unsigned {
    FIRST_NUMBER = ISD::BUILTIN_OP_END, 
    RET_FLAG, 

    CALL, 
    
};
}   //namespace ANGRYZISD

class ANGRYZTargetLowering : public TargetLowering {
private:
    const ANGRYZSubtarget &STI;

public:
    explicit ANGRYZTargetLowering(const TargetMachine &TM, const ANGRYZSubtarget &STI);

    const ANGRYZSubtarget &getSubtarget() const {
        return STI;
    }

    // This method returns the name of a target specific DAG node.
    const char *getTargetNodeName(unsigned Opcode) const override;


    //这三个hook必须override ： LowerCall, LowerFormalArguments, LowerReturn
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

  SDValue LowerCall(CallLoweringInfo &/*CLI*/,
                    SmallVectorImpl<SDValue> &/*InVals*/) const override;

  typedef bool ANGRYZCCAssignFn (unsigned ValNo, MVT ValVT,
                      MVT LocVT, CCValAssign::LocInfo LocInfo,
                      ISD::ArgFlagsTy ArgFlags, CCState &State);


private:
  void analyzeInputArgs(MachineFunction &MF, CCState &CCInfo,
                        const SmallVectorImpl<ISD::InputArg> &Ins, bool IsRet,
                        ANGRYZCCAssignFn Fn) const;
  void analyzeOutputArgs(MachineFunction &MF, CCState &CCInfo,
                         const SmallVectorImpl<ISD::OutputArg> &Outs,
                         bool IsRet, CallLoweringInfo *CLI,
                         ANGRYZCCAssignFn Fn) const;

  /// Generate error diagnostics if any register used by CC has been marked
  /// reserved.
  void validateCCReservedRegs(
      const SmallVectorImpl<std::pair<llvm::Register, llvm::SDValue>> &Regs,
      MachineFunction &MF) const;
};


}   //namespace llvm

#endif