#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZTARGETMACHINE_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZTARGETMACHINE_H

//#include "ANGRYZ.h"
#include "ANGRYZSubtarget.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"
#include "ANGRYZSubtarget.h"
#include <memory>
namespace llvm {


//class ANGRYZTargetMachine : LLVMTargetMachine{};
/*
 * todo ： TargetMachine待实现
*/
class ANGRYZTargetMachine : public LLVMTargetMachine {
    std::unique_ptr<TargetLoweringObjectFile> TLOF;
    //mutable StringMap<std::unique_ptr<ANGRYZSubtarget>> SubtargetMap;
    ANGRYZSubtarget Subtarget;
    //std::unique_ptr<RISCVSubtarget> Subtarget;

public :
    ANGRYZTargetMachine(const Target &T, const Triple &TT, StringRef CPU, 
                        StringRef FS, const TargetOptions &Options, 
                        std::optional<Reloc::Model> RM, 
                        std::optional<CodeModel::Model> CM, CodeGenOpt::Level OL, 
                        bool JIT);

    const ANGRYZSubtarget *getSubtargetImpl(const Function &F) const override;
    const ANGRYZSubtarget *getSubtargetImpl() const = delete;
    
    TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

    TargetLoweringObjectFile *getObjFileLowering() const override {
        return TLOF.get();
    }

};



}   // namespace llvm

#endif