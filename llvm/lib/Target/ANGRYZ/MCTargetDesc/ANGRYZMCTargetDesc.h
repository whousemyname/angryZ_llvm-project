#ifndef LLVM_LIB_TARGET_ANGRYZ_MCTARGETDESC_ANGRYZMCTARGETDESC_H
#define LLVM_LIB_TARGET_ANGRYZ_MCTARGETDESC_ANGRYZMCTARGETDESC_H

namespace llvm {



}   // namespace llvm


// Defines symbolic names for RISC-V registers.
#define GET_REGINFO_ENUM
#include "ANGRYZGenRegisterInfo.inc"

// Defines symbolic names for RISC-V instructions.
#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "ANGRYZGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "ANGRYZGenSubtargetInfo.inc"

#endif