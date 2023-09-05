#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZINSTRINFO_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZINSTRINFO_H

#include "ANGRYZRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/DiagnosticInfo.h"

#define GET_INSTRINFO_HEADER
#define GET_INSTRINFO_OPERAND_ENUM
#include "ANGRYZGenInstrInfo.inc"

namespace llvm {

class ANGRYZSubtarget;

class ANGRYZInstrInfo : public ANGRYZGenInstrInfo {
protected:
    const ANGRYZSubtarget &STI;
public:
    explicit ANGRYZInstrInfo(ANGRYZSubtarget &STI);
};

}   //namespace llvm

#endif