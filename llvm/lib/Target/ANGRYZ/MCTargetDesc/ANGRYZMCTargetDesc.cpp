#include "ANGRYZMCTargetDesc.h"
#include "ANGRYZInstPrinter.h"
#include "ANGRYZMCAsmInfo.h"
#include "TargetInfo/ANGRYZTargetInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "ANGRYZGenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "ANGRYZGenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "ANGRYZGenSubtargetInfo.inc"

using namespace llvm;

static MCInstrInfo *createANGRYZMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitANGRYZMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createANGRYZMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  //todo
  InitANGRYZMCRegisterInfo(X, ANGRYZ::H1);
  return X;
}

static MCAsmInfo *createANGRYZMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new ANGRYZMCAsmInfo(TT);

  MCRegister SP = MRI.getDwarfRegNum(ANGRYZ::H2, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, SP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

/*
static MCObjectFileInfo *
createANGRYZMCObjectFileInfo(MCContext &Ctx, bool PIC,
                            bool LargeCodeModel = false) {
  MCObjectFileInfo *MOFI = new ANGRYZMCObjectFileInfo();
  MOFI->initMCObjectFileInfo(Ctx, PIC, LargeCodeModel);
  return MOFI;
}
*/

static MCSubtargetInfo *createANGRYZMCSubtargetInfo(const Triple &TT,
                                                   StringRef CPU, StringRef FS) {
  if (CPU.empty() || CPU == "generic")
    CPU = TT.isArch64Bit() ? "generic-rv64" : "generic-rv32";

    //调用td生成的genSubtargetInfo.inc
  return createANGRYZMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCInstPrinter *createANGRYZMCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new ANGRYZInstPrinter(MAI, MII, MRI);
}


extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeANGRYZTargetMC() {
    /*
     * TargetRegistry::RegisterMCInstrInfo(getANGRYZTarget(), createANGRYZMCInstrInfo);
       ......
       todo
    */
    Target *T = &getANGRYZTarget();
    TargetRegistry::RegisterMCRegInfo(*T, createANGRYZMCRegisterInfo);
    TargetRegistry::RegisterMCInstrInfo(*T, createANGRYZMCInstrInfo);
    TargetRegistry::RegisterMCAsmInfo(*T, createANGRYZMCAsmInfo);
    TargetRegistry::RegisterMCSubtargetInfo(*T, createANGRYZMCSubtargetInfo);
    
    TargetRegistry::RegisterMCInstPrinter(*T, createANGRYZMCInstPrinter);
    
}