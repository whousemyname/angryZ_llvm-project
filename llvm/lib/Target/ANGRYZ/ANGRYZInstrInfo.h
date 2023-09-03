#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZINSTRINFO_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZINSTRINFO_H

#include "ANGRYZSubtarget.h"
#define GET_INSTRINFO_HEADER
#include "ANGRYZGenInstrInfo.inc"

namespace llvm {

class ANGRYZSubtarget Subtarget;

class ANGRYZInstrInfo : public ANGRYZGenInstrInfo {
protected:
    const ANGRYZSubtarget &STI;
public:
    explicit ANGRYZInstrInfo(ANGRYZSubtarget &STI);
};

}   //namespace llvm

#endif