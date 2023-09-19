#include "ANGRYZISelLowering.h"
#include "MCTargetDesc/ANGRYZMCTargetDesc.h"
#include "ANGRYZRegisterInfo.h"
#include "ANGRYZ.h"
#include "ANGRYZSubtarget.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Analysis/VectorUtils.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetCallingConv.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
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

STATISTIC(NumTailCalls, "Number of tail calls");


/*
  TODO  
*/

static SDValue convertValVTToLocVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL,
                                   const ANGRYZSubtarget &Subtarget) {
  return Val;
}
static SDValue convertLocVTToValVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL,
                                   const ANGRYZSubtarget &Subtarget) {
  return Val;
}

// The caller is responsible for loading the full value if the argument is
// passed with CCValAssign::Indirect.
static SDValue unpackFromRegLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL,
                                const ISD::InputArg &In,
                                const ANGRYZTargetLowering &TLI) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  EVT LocVT = VA.getLocVT();
  SDValue Val;
  const TargetRegisterClass *RC = TLI.getRegClassFor(LocVT.getSimpleVT());
  Register VReg = RegInfo.createVirtualRegister(RC);
  RegInfo.addLiveIn(VA.getLocReg(), VReg);
  Val = DAG.getCopyFromReg(Chain, DL, VReg, LocVT);     


   // If input is sign extended from 32 bits, note it for the SExtWRemoval pass.
   // TODO
  if (In.isOrigArg()) {
    Argument *OrigArg = MF.getFunction().getArg(In.getOrigArgIndex());
    if (OrigArg->getType()->isIntegerTy()) {
      unsigned BitWidth = OrigArg->getType()->getIntegerBitWidth();
      if ((BitWidth <= 32 && In.Flags.isSExt()) ||
          (BitWidth < 32 && In.Flags.isZExt())) {
        /*
        RISCVMachineFunctionInfo *RVFI = MF.getInfo<RISCVMachineFunctionInfo>();
        RVFI->addSExt32Register(VReg);
        */
        llvm_unreachable(
      "ANGRYZISellowering::LowerFormalArguments --> unpackFromRegLoc incomplete implement");
      }
    }
  }

  if (VA.getLocInfo() == CCValAssign::Indirect)
    return Val;

  return convertLocVTToValVT(DAG, Val, VA, DL, TLI.getSubtarget());
}

// The caller is responsible for loading the full value if the argument is
// passed with CCValAssign::Indirect.
static SDValue unpackFromMemLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL) {
  llvm_unreachable(
      "ANGRYZISellowering::LowerFormalArguments --> unpackFromMemLoc incomplete implement");
}

ANGRYZTargetLowering::ANGRYZTargetLowering(const TargetMachine &TM, const ANGRYZSubtarget &STI) 
    :   TargetLowering(TM), STI(STI) {
    
    //todo
    addRegisterClass(MVT::i32, &(ANGRYZ::HPRRegClass));

    computeRegisterProperties(STI.getRegisterInfo());

}

void ANGRYZTargetLowering::validateCCReservedRegs(
    const SmallVectorImpl<std::pair<llvm::Register, llvm::SDValue>> &Regs,
    MachineFunction &MF) const {
  const Function &F = MF.getFunction();
  const ANGRYZSubtarget &STI = MF.getSubtarget<ANGRYZSubtarget>();

  if (llvm::any_of(Regs, [&STI](auto Reg) {
        return STI.isRegisterReservedByUser(Reg.first);
      }))
    F.getContext().diagnose(DiagnosticInfoUnsupported{
        F, "Argument register required, but has been reserved."});
}

const char *ANGRYZTargetLowering::getTargetNodeName(unsigned Opcode) const {
    switch (Opcode) {
        case ANGRYZISD::RET_FLAG : 
            return "ANGRYZISD::RET_FLAG";
        case ANGRYZISD::CALL : 
            return "ANGRYZISD::CALL";
        defalut : 
            return nullptr;
    }
    return nullptr;
}

void ANGRYZTargetLowering::analyzeInputArgs(MachineFunction &MF, CCState &CCInfo,
                      const SmallVectorImpl<ISD::InputArg> &Ins, bool IsRet,
                      ANGRYZCCAssignFn Fn) const {
  unsigned NumArgs = Ins.size();
  FunctionType *FType = MF.getFunction().getFunctionType();

  /*
    TODO
  std::optional<unsigned> FirstMaskArgument;
  if (STI.hasVInstructions())
    FirstMaskArgument = preAssignMask(Ins); 
  */
  for (unsigned i = 0; i != NumArgs; ++i) {
    MVT ArgVT = Ins[i].VT;
    ISD::ArgFlagsTy ArgFlags = Ins[i].Flags;

    Type *ArgTy = nullptr;
    if (IsRet)
      ArgTy = FType->getReturnType();
    else if (Ins[i].isOrigArg())
      ArgTy = FType->getParamType(Ins[i].getOrigArgIndex());
    
    //RISCVABI::ABI ABI = MF.getSubtarget<ANGRYZSubtarget>().getTargetABI();
    if (Fn( i, ArgVT, ArgVT, CCValAssign::Full,
           ArgFlags, CCInfo)) {
      LLVM_DEBUG(dbgs() << "InputArg #" << i << " has unhandled type "
                        << ArgVT << '\n');
      llvm_unreachable(nullptr);
    }
    
  }
}

SDValue ANGRYZTargetLowering::LowerCall(CallLoweringInfo &CLI,
                  SmallVectorImpl<SDValue> &InVals) const {
  
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  bool &IsTailCall = CLI.IsTailCall;
  bool IsVarArg = CLI.IsVarArg;
  CallingConv::ID CallConvID = CLI.CallConv;
  EVT PtrVT = getPointerTy(DAG.getDataLayout());
  MVT XLenVT = MVT::i32;    //TODO

  MachineFunction &MF = DAG.getMachineFunction();


// Analyze the operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState ArgCCInfo(CallConvID, IsVarArg, MF, ArgLocs, *DAG.getContext());

  if (CallConvID == CallingConv::GHC)
    ArgCCInfo.AnalyzeCallOperands(Outs, CC_ANGRYZ);    //TODO
  else 
    ArgCCInfo.AnalyzeCallOperands(Outs, CC_ANGRYZ);     //TODO
  

  /*
    TODO ：tailCall相关的
  */
  // Check if it's really possible to do a tail call.
  //if (IsTailCall)
    //IsTailCall = isEligibleForTailCallOptimization(ArgCCInfo, CLI, MF, ArgLocs);
  if (IsTailCall)
    ++NumTailCalls;
  else if (CLI.CB && CLI.CB->isMustTailCall())
    report_fatal_error("failed to perform tail call elimination on a call "
                       "site marked musttail");

  
  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = ArgCCInfo.getStackSize(); 

  // Create local copies for byval args
  SmallVector<SDValue, 8> ByValArgs;
  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (!Flags.isByVal())
      continue;

    SDValue Arg = OutVals[i];
    unsigned Size = Flags.getByValSize();
    Align Alignment = Flags.getNonZeroByValAlign();

    int FI =
        MF.getFrameInfo().CreateStackObject(Size, Alignment, /*isSS=*/false);
    SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue SizeNode = DAG.getConstant(Size, DL, XLenVT);

    Chain = DAG.getMemcpy(Chain, DL, FIPtr, Arg, SizeNode, Alignment,
                          /*IsVolatile=*/false,
                          /*AlwaysInline=*/false, IsTailCall,
                          MachinePointerInfo(), MachinePointerInfo());
    ByValArgs.push_back(FIPtr);
  }

  if (!IsTailCall)
    Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, CLI.DL);

  // Copy argument values to their designated locations.
  SmallVector<std::pair<Register, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;

  for (unsigned i = 0, j = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue = OutVals[i];
    ISD::ArgFlagsTy Flags = Outs[i].Flags;

    // Handle passing f64 on RV32D with a soft float ABI as a special case.
    bool IsF64OnRV32DSoftABI =
        VA.getLocVT() == MVT::i32 && VA.getValVT() == MVT::f64;
    if (IsF64OnRV32DSoftABI && VA.isRegLoc()) {
      //TODO  ANGRYZISelLowering::LowerCall
    }

    // IsF64OnRV32DSoftABI && VA.isMemLoc() is handled below in the same way
    // as any other MemLoc.
    // Promote the value if needed.
    // For now, only handle fully promoted and indirect arguments.
    if (VA.getLocInfo() == CCValAssign::Indirect) {
        // TODO
    } else {
      ArgValue = convertValVTToLocVT(DAG, ArgValue, VA, DL, STI);
    }

    // Use local copy if it is a byval arg.
    if (Flags.isByVal())
      ArgValue = ByValArgs[j++];

    if (VA.isRegLoc()) {
      // Queue up the argument copies and emit them at the end.
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), ArgValue));
    } else {
      assert(VA.isMemLoc() && "Argument not register or memory");
      assert(!IsTailCall && "Tail call not allowed if stack is used "
                            "for passing parameters");
      /*    TODO 
      // Work out the address of the stack slot.
      if (!StackPtr.getNode())
        StackPtr = DAG.getCopyFromReg(Chain, DL, RISCV::X2, PtrVT);
      SDValue Address =
          DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr,
                      DAG.getIntPtrConstant(VA.getLocMemOffset(), DL));

      // Emit the store.
      MemOpChains.push_back(
          DAG.getStore(Chain, DL, ArgValue, Address, MachinePointerInfo()));
      */
    }   
  } //end for (unsigned i = 0, j = 0, e = ArgLocs.size(); i != e; ++i)
  

  // Join the stores, which are independent of one another.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  SDValue Glue;

  // Build a sequence of copy-to-reg nodes, chained and glued together.
  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, Glue);
    Glue = Chain.getValue(1);
  }
  // Validate that none of the argument registers have been marked as
  // reserved, if so report an error. Do the same for the return address if this
  // is not a tailcall.
  validateCCReservedRegs(RegsToPass, MF);  
  if (!IsTailCall &&
      MF.getSubtarget<ANGRYZSubtarget>().isRegisterReservedByUser(ANGRYZ::H1))
    MF.getFunction().getContext().diagnose(DiagnosticInfoUnsupported{
        MF.getFunction(),
        "Return address register required, but has been reserved."});
  
  // If the callee is a GlobalAddress/ExternalSymbol node, turn it into a
  // TargetGlobalAddress/TargetExternalSymbol node so that legalize won't
  // split it and then direct call can be matched by PseudoCALL.
  if (GlobalAddressSDNode *S = dyn_cast<GlobalAddressSDNode>(Callee)) {
    const GlobalValue *GV = S->getGlobal();
    /*    TODO
    unsigned OpFlags = RISCVII::MO_CALL;
    if (!getTargetMachine().shouldAssumeDSOLocal(*GV->getParent(), GV))
      OpFlags = RISCVII::MO_PLT;
    */
    Callee = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0, 0);
  } else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    /*
    unsigned OpFlags = RISCVII::MO_CALL;

    if (!getTargetMachine().shouldAssumeDSOLocal(*MF.getFunction().getParent(),
                                                 nullptr))
      OpFlags = RISCVII::MO_PLT;

    Callee = DAG.getTargetExternalSymbol(S->getSymbol(), PtrVT, OpFlags);
    */
  } 

  // The first call operand is the chain and the second is the target address.
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (auto &Reg : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
 
  if (!IsTailCall) {
    // Add a register mask operand representing the call-preserved registers.
    const TargetRegisterInfo *TRI = STI.getRegisterInfo();
    const uint32_t *Mask = TRI->getCallPreservedMask(MF, CallConvID);
    assert(Mask && "Missing call preserved mask for calling convention");
    Ops.push_back(DAG.getRegisterMask(Mask));
  } 

  // Glue the call to the argument copies, if any.
  if (Glue.getNode())
    Ops.push_back(Glue);

  assert((!CLI.CFIType || CLI.CB->isIndirectCall()) &&
         "Unexpected CFI type for a direct call");

  // Emit the call.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  if (IsTailCall) {
    /*  TODO LowerReturn tailcall
    MF.getFrameInfo().setHasTailCall();
    SDValue Ret = DAG.getNode(RISCVISD::TAIL, DL, NodeTys, Ops);
    if (CLI.CFIType)
      Ret.getNode()->setCFIType(CLI.CFIType->getZExtValue());
    DAG.addNoMergeSiteInfo(Ret.getNode(), CLI.NoMerge);
    return Ret;
    */
  }

  Chain = DAG.getNode(ANGRYZISD::CALL, DL, NodeTys, Ops);
  if (CLI.CFIType)
    Chain.getNode()->setCFIType(CLI.CFIType->getZExtValue());
  DAG.addNoMergeSiteInfo(Chain.getNode(), CLI.NoMerge);
  Glue = Chain.getValue(1);

  // Mark the end of the call, which is glued to the call itself.
  Chain = DAG.getCALLSEQ_END(Chain, NumBytes, 0, Glue, DL);
  Glue = Chain.getValue(1);


  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RetCCInfo(CallConvID, IsVarArg, MF, RVLocs, *DAG.getContext());
  analyzeInputArgs(MF, RetCCInfo, Ins, /*IsRet=*/true, CC_ANGRYZ);


  // Copy all of the result registers out of their specified physreg.
  for (auto &VA : RVLocs) {
    // Copy the value out
    SDValue RetValue =
        DAG.getCopyFromReg(Chain, DL, VA.getLocReg(), VA.getLocVT(), Glue);
    // Glue the RetValue to the end of the call sequence
    Chain = RetValue.getValue(1);
    Glue = RetValue.getValue(2);

    if (VA.getLocVT() == MVT::i32 && VA.getValVT() == MVT::f64) {
      /*  TODO 
      assert(VA.getLocReg() == ArgGPRs[0] && "Unexpected reg assignment");
      SDValue RetValue2 =
          DAG.getCopyFromReg(Chain, DL, ArgGPRs[1], MVT::i32, Glue);
      Chain = RetValue2.getValue(1);
      Glue = RetValue2.getValue(2);
      RetValue = DAG.getNode(RISCVISD::BuildPairF64, DL, MVT::f64, RetValue,
                             RetValue2);
      */ 
    }

    RetValue = convertLocVTToValVT(DAG, RetValue, VA, DL, STI);

    InVals.push_back(RetValue);
  }

  return Chain;  
}

// Lower incoming arguments, copy physregs into vregs
SDValue ANGRYZTargetLowering::LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                            bool IsVarArg,
                            const SmallVectorImpl<ISD::InputArg> &Ins,
                            const SDLoc &DL, SelectionDAG &DAG,
                            SmallVectorImpl<SDValue> &InVals) const {
  
  MachineFunction &MF = DAG.getMachineFunction();
  // Used with vargs to acumulate store chains.

  std::vector<SDValue> OutChains;
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  // 不再使用这种默认处理 ： 使用自定义函数解析，
  //但是本质还是调用了tablegen自动生成的CC_ANGRYZ
  //CCInfo.AnalyzeFormalArguments(Ins, CC_ANGRYZ);
  if (CallConv == CallingConv::GHC)
    CCInfo.AnalyzeFormalArguments(Ins, CC_ANGRYZ);
  else 
    analyzeInputArgs(MF, CCInfo, Ins, false, CC_ANGRYZ);

  for (unsigned int i = 0, end = ArgLocs.size(); i < end; ++i) {
      CCValAssign &VA = ArgLocs[i];
      SDValue ArgValue;
      if (VA.getLocVT() == MVT::i32 && VA.getValVT() == MVT::f64)
        //
        ;
      else if (VA.isRegLoc())
        ArgValue = unpackFromRegLoc(DAG, Chain, VA, DL, Ins[i], *this);
      else 
        ArgValue = unpackFromMemLoc(DAG, Chain, VA, DL);

      if (VA.getLocInfo() == CCValAssign::Indirect) {
        llvm_unreachable("uncomplete implement");
      }
      InVals.push_back(ArgValue);
  }

  if (IsVarArg) {
    llvm_unreachable("uncomplete implement");
  }
  
  if (!OutChains.empty()) {
    llvm_unreachable("uncomplete implement");
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
