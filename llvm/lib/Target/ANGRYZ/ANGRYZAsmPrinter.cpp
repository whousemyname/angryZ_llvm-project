#include "ANGRYZSubtarget.h"
#include "MCTargetDesc/ANGRYZMCTargetDesc.h"
#include "TargetInfo/ANGRYZTargetInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include <memory>
#include <utility>

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {

class ANGRYZAsmPrinter : public AsmPrinter {
private:
    const ANGRYZSubtarget *Subtarget;

public:
    explicit ANGRYZAsmPrinter(TargetMachine& TM, std::unique_ptr<MCStreamer> Streamer)
        :   AsmPrinter(TM, std::move(Streamer)){}

    bool runOnMachineFunction(MachineFunction &MF) override;
    StringRef getPassName() const override;
    void emitInstruction(const MachineInstr* MI) override; 
    /*
    void emitStartOfAsmFile(Module &M) override;
    void emitEndOfAsmFile(Module &M) override;
    void emitFunctionEntryLabel() override;
    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       const char *ExtraCode, raw_ostream &OS) override;
    bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                             const char *ExtraCode, raw_ostream &OS) override;

    bool isSameAttribute();
    
    void EmitToStreamer(MCStreamer &S, const MCInst &Inst) override ; 
    */
    //该函数由inc提供实现
    bool emitPseudoExpansionLowering(MCStreamer &OutStreamer, const MachineInstr *MI);
    //该函数由lowerToMCInst调用，用于对于operand的lower-MCInt
    bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp) const;
private:
    //该函数由重新实现的emitInstruction调用
    bool lowerToMCInst(const MachineInstr* MI, MCInst &OutMI);
};  //end class ANGRYZAsmPrinter

}   // namespace

bool ANGRYZAsmPrinter::runOnMachineFunction(MachineFunction &MF) {
    Subtarget = &MF.getSubtarget<ANGRYZSubtarget>();
/*
    todo
    RISCVTargetStreamer
*/
    SetupMachineFunction(MF);
    emitFunctionBody();

    return false;
}

StringRef ANGRYZAsmPrinter::getPassName () const {
    return "ANGRYZ Assembly Printer";
}

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "ANGRYZGenMCPseudoLowering.inc"

void ANGRYZAsmPrinter::emitInstruction(const MachineInstr *MI) {

  // td提供 Do any auto-generated pseudo lowerings.
  if (emitPseudoExpansionLowering(*OutStreamer, MI))
    return;

  MCInst OutInst;
  if (!lowerToMCInst(MI, OutInst))
    EmitToStreamer(*OutStreamer, OutInst);
}

bool ANGRYZAsmPrinter::lowerOperand(const MachineOperand &MO,
                                   MCOperand &MCOp) const {
  switch (MO.getType()) {
  default:
    report_fatal_error("lowerOperand: unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      return false;
    MCOp = MCOperand::createReg(MO.getReg());
    break;
  case MachineOperand::MO_RegisterMask:
    // Regmasks are like implicit defs.
    return false;
  case MachineOperand::MO_Immediate:
    MCOp = MCOperand::createImm(MO.getImm());
    break;
    /*lowerSymbolOperand没有实现
  case MachineOperand::MO_MachineBasicBlock:
    MCOp = lowerSymbolOperand(MO, MO.getMBB()->getSymbol(), *this);
    break;
  case MachineOperand::MO_GlobalAddress:
    MCOp = lowerSymbolOperand(MO, getSymbolPreferLocal(*MO.getGlobal()), *this);
    break;
  case MachineOperand::MO_BlockAddress:
    MCOp = lowerSymbolOperand(MO, GetBlockAddressSymbol(MO.getBlockAddress()),
                              *this);
    break;
  case MachineOperand::MO_ExternalSymbol:
    MCOp = lowerSymbolOperand(MO, GetExternalSymbolSymbol(MO.getSymbolName()),
                              *this);
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    MCOp = lowerSymbolOperand(MO, GetCPISymbol(MO.getIndex()), *this);
    break;
  case MachineOperand::MO_JumpTableIndex:
    MCOp = lowerSymbolOperand(MO, GetJTISymbol(MO.getIndex()), *this);
    break;
  case MachineOperand::MO_MCSymbol:
    MCOp = lowerSymbolOperand(MO, MO.getMCSymbol(), *this);
    break;
    */
  }
  
  return true;
}


bool ANGRYZAsmPrinter::lowerToMCInst(const MachineInstr *MI, MCInst &OutMI) {

  OutMI.setOpcode(MI->getOpcode());
  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    if (lowerOperand(MO, MCOp))
      OutMI.addOperand(MCOp);
  }
    /* 以下为riscv的参考代码
    对于MCInst的某些的特殊处理
  switch (OutMI.getOpcode()) {
  case TargetOpcode::PATCHABLE_FUNCTION_ENTER: {
    const Function &F = MI->getParent()->getParent()->getFunction();
    if (F.hasFnAttribute("patchable-function-entry")) {
      unsigned Num;
      if (F.getFnAttribute("patchable-function-entry")
              .getValueAsString()
              .getAsInteger(10, Num))
        return false;
      emitNops(Num);
      return true;
    }
    break;
  }
  }
  */
  return false;
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeANGRYZAsmPrinter() {
  RegisterAsmPrinter<ANGRYZAsmPrinter> X(getANGRYZTarget());
}
