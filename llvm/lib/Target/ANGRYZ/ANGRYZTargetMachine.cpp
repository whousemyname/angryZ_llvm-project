
#include "ANGRYZTargetMachine.h"
#include "ANGRYZSubtarget.h"
#include "ANGRYZTargetObjectFile.h"
#include "TargetInfo/ANGRYZTargetInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"
#include "ANGRYZ.h"
#include <memory>
#include <optional>

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeANGRYZTarget() {
    RegisterTargetMachine<ANGRYZTargetMachine> X(getANGRYZTarget());
}


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
      TLOF(std::make_unique<ANGRYZELFTargetObjectFile>()){

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