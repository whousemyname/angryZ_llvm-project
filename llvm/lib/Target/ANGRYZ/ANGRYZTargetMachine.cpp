#include "ANGRYZ.h"
#include "TargetInfo/ANGRYZTargetInfo.h"
#include "ANGRYZTargetMachine.h"
#include "ANGRYZSubtarget.h"
#include "ANGRYZTargetObjectFile.h"
#include "TargetInfo/ANGRYZTargetInfo.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/GlobalISel/IRTranslator.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"
#include "llvm/CodeGen/GlobalISel/Legalizer.h"
#include "llvm/CodeGen/GlobalISel/RegBankSelect.h"
#include "llvm/CodeGen/MIRParser/MIParser.h"
#include "llvm/CodeGen/MIRYamlMapping.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/InitializePasses.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/IPO.h"
#include <memory>
#include <optional>

using namespace llvm;

static StringRef computeDataLayout(const Triple &TT) {
    return "e-m:e-p:32:32-i64:64-n32-S128";
}


static Reloc::Model getEffectiveRelocModel(const Triple &TT, std::optional<Reloc::Model> RM) {
    return RM.value_or(Reloc::Static);
}

ANGRYZTargetMachine::ANGRYZTargetMachine (const Target &T, const Triple &TT, StringRef CPU, 
                                          StringRef FS, const TargetOptions &Options, 
                                          std::optional<Reloc::Model> RM, 
                                          std::optional<CodeModel::Model> CM, 
                                          CodeGenOpt::Level OL, 
                                          bool JIT) 
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options, 
                        getEffectiveRelocModel(TT, RM), 
                        getEffectiveCodeModel(CM, CodeModel::Small), OL), 
      Subtarget(TT, CPU, CPU, FS, Options.MCOptions.getABIName(), *this), 
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()){

    initAsmInfo();
    //todo

}


const ANGRYZSubtarget *ANGRYZTargetMachine::getSubtargetImpl(const Function &F) const {
    return &Subtarget;
}

/*
const ANGRYZSubtarget *
ANGRYZTargetMachine:: getSubtargetImpl(const Function &F) const {
    static ANGRYZSubtarget subtarget;
    return &subtarget;
}
*/

/*
    * PassConfig的实现
*/
namespace {
class ANGRYZPassConfig : public TargetPassConfig {
public :
    ANGRYZPassConfig(ANGRYZTargetMachine &TM, PassManagerBase &PM)
        : TargetPassConfig(TM, PM) {}

    ANGRYZTargetMachine &getANGRYZTargetMachine() const {
        return getTM<ANGRYZTargetMachine>();
    }

bool addInstSelector() override;

};
}   // namespace

TargetPassConfig *ANGRYZTargetMachine::createPassConfig(PassManagerBase &PM) {
    return new ANGRYZPassConfig(*this, PM);
}

bool ANGRYZPassConfig::addInstSelector() {
    addPass(createANGRYZISelDag(getANGRYZTargetMachine(), getOptLevel()));
    return false;
}


extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeANGRYZTarget() {
    RegisterTargetMachine<ANGRYZTargetMachine> X(getANGRYZTarget());
}