#ifndef LLVM_LIB_TARGET_ANGRYZ_MCTARGETDESC_ANGRYZBASEINFO_H
#define LLVM_LIB_TARGET_ANGRYZ_MCTARGETDESC_ANGRYZBASEINFO_H

#include "MCTargetDesc/ANGRYZMCTargetDesc.h"
#include "llvm/MC/MCInstrDesc.h"
namespace llvm {

namespace ANGRYZII {
enum {
  MO_None = 0, 
  MO_CALL = 1, 

  
};

} // end namespace ANGRYZII 

namespace ANGRYZABI {
enum ABI{
  ABI_ILP32,
  ABI_ILP32F,
  ABI_ILP32D,
  ABI_ILP32E,
  ABI_LP64,
  ABI_LP64F,
  ABI_LP64D,
  ABI_LP64E,
  ABI_Unknown 
};  // end enum ABI

}  //end namespace ANGRYZABI


namespace ANGRYZOp {
enum OperandType : unsigned {
  OPERAND_FIRST_ANGRYZ_IMM = MCOI::OPERAND_FIRST_TARGET, 
  OPERAND_UIMMLOG2XLEN, 
  OPERAND_LAST_ANGRYZ_IMM = OPERAND_UIMMLOG2XLEN,  
};  // end enum Operantype
} //end namespace ANGRYZOp

} // end namespace llvm

#endif
