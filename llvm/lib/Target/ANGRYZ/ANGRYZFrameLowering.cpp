#include "ANGRYZFrameLowering.h"
#include "ANGRYZInstrInfo.h"
#include "ANGRYZRegisterInfo.h"
#include "ANGRYZSubtarget.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/LEB128.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TypeSize.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
using namespace llvm;

ANGRYZFrameLowering::ANGRYZFrameLowering(const ANGRYZSubtarget &STI) 
    :   TargetFrameLowering(StackGrowsDown, Align(16), 0, Align(16)), 
        STI(STI){
    //todo
}


static Register getFPReg(const ANGRYZSubtarget &STI) {
    return ANGRYZ::H8;
}

static Register getSPReg (const ANGRYZSubtarget &STI) {
    return ANGRYZ::H2;
}


/*
    目前暂时按照riscv实现
    什么是影子调用栈？目前正常调用流程还不用进入这里
*/
// For now we use x3, a.k.a gp, as pointer to shadow call stack.
// User should not use x3 in their asm.
static void emitSCSPrologue(MachineFunction &MF, MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI,
                            const DebugLoc &DL) {
    if (!MF.getFunction().hasFnAttribute(Attribute::ShadowCallStack))
        return ;
}


/*
  toRead
  直接复制的riscv
*/
static SmallVector<CalleeSavedInfo, 8>
getUnmanagedCSI(const MachineFunction &MF,
                const std::vector<CalleeSavedInfo> &CSI) {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  SmallVector<CalleeSavedInfo, 8> NonLibcallCSI;

  for (auto &CS : CSI) {
    int FI = CS.getFrameIdx();
    if (FI >= 0 && MFI.getStackID(FI) == TargetStackID::Default)
      NonLibcallCSI.push_back(CS);
  }

  return NonLibcallCSI;
}

void ANGRYZFrameLowering::emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const {

    MachineFrameInfo &MFI = MF.getFrameInfo();
    //riscv在这里实现了自己的MachineFunctionInfo
    const ANGRYZRegisterInfo *RI = STI.getRegisterInfo();
    const ANGRYZInstrInfo *TII = STI.getInstrInfo();
    MachineBasicBlock::iterator MBBI = MBB.begin();

    Register FPReg = getFPReg(STI);
    Register SPReg = getSPReg(STI);
    DebugLoc DL;

    if (MF.getFunction().getCallingConv() == CallingConv::GHC)
        return ;
    emitSCSPrologue(MF, MBB, MBBI, DL);

    MachineBasicBlock::iterator FirstFrameSetup = MBBI;

    while (MBBI != MBB.end() && MBBI->getFlag(MachineInstr::FrameSetup))
        ++MBBI;
    
    determineFrameLayout(MF);
    //对于负帧索引，帧指针的偏移量将根据帧索引适用于哪些组而有所不同。
    //下面计算正确的偏移量，了解这两种方法溢出的被调用者保存的寄存器的数量。
    /*
        库调用相关设置
    */
    /*
    if (int LibCallRegs = getLibCallID(MF, MFI.getCalleeSavedInfo()) + 1) {

    }*/

    //riscv这里还考虑了v panding之后
    uint64_t StackSize = MFI.getStackSize();
    uint64_t RVPushStakcSize = 0;
    uint64_t LibCallStackSize = 0;
    uint64_t RealStackSize = StackSize + LibCallStackSize + RVPushStakcSize;
    uint64_t RVVStackSize = 0;

    // Early exit if there is no need to allocate on the stack
    if (RealStackSize == 0 && !MFI.adjustsStack())
        return;
    
    /*
        todo
        isRegisterReservedByUser
        getFirstSPAdjustAmount      对于sp的优化
        isPushable && CM_PUSH

    */
    if (StackSize != 0) {
        RI->adjustReg(MBB, MBBI, DL, SPReg, SPReg, 
                        StackOffset::getFixed(-StackSize), MachineInstr::FrameSetup, 
                        getStackAlign());
    }

    // Emit ".cfi_def_cfa_offset RealStackSize"
  unsigned CFIIndex = MF.addFrameInst(
      MCCFIInstruction::cfiDefCfaOffset(nullptr, RealStackSize));
  BuildMI(MBB, MBBI, DL, TII->get(TargetOpcode::CFI_INSTRUCTION))
      .addCFIIndex(CFIIndex)
      .setMIFlag(MachineInstr::FrameSetup);

  const auto &CSI = MFI.getCalleeSavedInfo();

  std::advance(MBBI, getUnmanagedCSI(MF, CSI).size());
  for (const auto &Entry : CSI) {
    int FrameIdx = Entry.getFrameIdx();
    int64_t Offset;
    if (FrameIdx < 0) {   //todo

    } else {
      Offset = MFI.getObjectOffset(FrameIdx) - LibCallStackSize;
    }
    Register Reg = Entry.getReg();
    //emit cfi instruction
    unsigned CFIIndex = MF.addFrameInst(MCCFIInstruction::createOffset(
        nullptr, RI->getDwarfRegNum(Reg, true), Offset));
    BuildMI(MBB, MBBI, DL, TII->get(TargetOpcode::CFI_INSTRUCTION))
        .addCFIIndex(CFIIndex)
        .setMIFlag(MachineInstr::FrameSetup);
  }

  // Generate new FP.
  if (hasFP(MF)) {
    //isRegisterReservedByUser

    // The frame pointer does need to be reserved from register allocation.
    assert(MF.getRegInfo().isReserved(FPReg) && "FP not reserved");

    RI->adjustReg(MBB, MBBI, DL, FPReg, SPReg, 
            StackOffset::getFixed(RealStackSize - 0),     //todo RVFI->getVarArgsSaveSize()
                  MachineInstr::FrameSetup, getStackAlign());

    // Emit ".cfi_def_cfa $fp, RVFI->getVarArgsSaveSize()"
    unsigned CFIIndex = MF.addFrameInst(MCCFIInstruction::cfiDefCfa(
        nullptr, RI->getDwarfRegNum(FPReg, true), 0));    //todo RVFI->getVarArgsSaveSize()
    BuildMI(MBB, MBBI, DL, TII->get(TargetOpcode::CFI_INSTRUCTION))
        .addCFIIndex(CFIIndex)
        .setMIFlag(MachineInstr::FrameSetup);
  }

  if (RVVStackSize) {
    //todo
  }

  if (hasFP(MF)) {
    const ANGRYZRegisterInfo *RI = STI.getRegisterInfo();
    if (RI->hasStackRealignment(MF)) {    //TODO 
      Align MaxAlignment = MFI.getMaxAlign();
      const ANGRYZInstrInfo *TII = STI.getInstrInfo();
    }
  }
}


static void emitSCSEpilogue(MachineFunction &MF, MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI,
                            const DebugLoc &DL) {
  if (!MF.getFunction().hasFnAttribute(Attribute::ShadowCallStack))
    return;
}

void ANGRYZFrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
  const ANGRYZRegisterInfo *RI = STI.getRegisterInfo();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  //auto *RVFI = MF.getInfo<RISCVMachineFunctionInfo>();
  Register FPReg = getFPReg(STI);
  Register SPReg = getSPReg(STI);

  // All calls are tail calls in GHC calling conv, and functions have no
  // prologue/epilogue.
  if (MF.getFunction().getCallingConv() == CallingConv::GHC)
    return;

    // Get the insert location for the epilogue. If there were no terminators in
  // the block, get the last instruction.
  MachineBasicBlock::iterator MBBI = MBB.end();
  DebugLoc DL;
  if (!MBB.empty()) {
    MBBI = MBB.getLastNonDebugInstr();
    if (MBBI != MBB.end())
      DL = MBBI->getDebugLoc();

    MBBI = MBB.getFirstTerminator();

    // If callee-saved registers are saved via libcall, place stack adjustment
    // before this call.
    while (MBBI != MBB.begin() &&
           std::prev(MBBI)->getFlag(MachineInstr::FrameDestroy))
      --MBBI;
  }

  const auto &CSI = getUnmanagedCSI(MF, MFI.getCalleeSavedInfo());

  // Skip to before the restores of callee-saved registers
  // FIXME: assumes exactly one instruction is used to restore each
  // callee-saved register.
  auto LastFrameDestroy = MBBI;
  if (!CSI.empty())
    LastFrameDestroy = std::prev(MBBI, CSI.size());

  //TODO
  uint64_t StackSize = MFI.getStackSize();  
  uint64_t RVPushStakcSize = 0;
  uint64_t LibCallStackSize = 0;
  uint64_t RealStackSize =
      StackSize + LibCallStackSize + RVPushStakcSize;
  uint64_t FPOffset = RealStackSize - 0;
  uint64_t RVVStackSize = 0;

  if (RI->hasStackRealignment(MF) || MFI.hasVarSizedObjects() ||
      !hasReservedCallFrame(MF)) {
    assert(hasFP(MF) && "frame pointer should not have been eliminated");
    RI->adjustReg(MBB, LastFrameDestroy, DL, SPReg, FPReg,
                  StackOffset::getFixed(-FPOffset),
                  MachineInstr::FrameDestroy, getStackAlign());
  } else {
    if (RVVStackSize){

    }
  }
  
  /*
    TODO :getFirstSPAdjustAmount
  */
    // Deallocate stack
  if (StackSize != 0) {
    RI->adjustReg(MBB, MBBI, DL, SPReg, SPReg, StackOffset::getFixed(StackSize),
                  MachineInstr::FrameDestroy, getStackAlign());
  }

  // Emit epilogue for shadow call stack.
  emitSCSEpilogue(MF, MBB, MBBI, DL);
}


/*
    仿照riscv写的
    帧指针消除是否被禁用：如果帧指针消除（Frame Pointer Elimination）被禁用，意味着函数需要保留帧指针，因此会返回 true。
    是否需要动态栈重新对齐：如果函数需要在运行时对栈进行动态重新对齐（Dynamic Stack Realignment），通常会要求使用帧指针，因此也会返回 true。
    函数是否包含可变大小的分配（Variable Sized Alloca）：如果函数内部包含可变大小的分配，这可能会导致栈帧大小动态变化，因此需要帧指针来访问栈上的局部变量，同样返回 true。
    是否获取了帧地址（Frame Address Taken）：如果函数中的某些部分需要访问帧地址，也就是栈帧的起始地址，通常需要帧指针来实现这一功能。
*/
bool ANGRYZFrameLowering::hasFP(const MachineFunction &MF) const {
    const TargetRegisterInfo *TargetRI = MF.getSubtarget().getRegisterInfo();

    const MachineFrameInfo &MFrameI = MF.getFrameInfo();
    return MF.getTarget().Options.DisableFramePointerElim(MF) || 
            TargetRI->hasStackRealignment(MF) ||
            MFrameI.hasVarSizedObjects() || 
            MFrameI.isFrameAddressTaken();
}


void ANGRYZFrameLowering::determineFrameLayout (MachineFunction &MF) const {
    MachineFrameInfo &MFI = MF.getFrameInfo();
    uint64_t FrameSize = MFI.getStackSize();

    Align StackAlign = getStackAlign();
    FrameSize = alignTo(FrameSize, StackAlign);

    MFI.setStackSize(FrameSize);

    /*
        riscv之后针对看是否是v扩展，而是否采取panding
    */
}

