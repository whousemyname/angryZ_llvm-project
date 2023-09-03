#ifndef LLVM_LIB_TARGET_ANGRYZ_ANGRYZASMPRINTER_H
#define LLVM_LIB_TARGET_ANGRYZ_ANGRYZASMPRINTER_H


#include "ANGRYZSubtarget.h"
#include "llvm/CodeGen/AsmPrinter.h"
namespace llvm {

class ANGRYZAsmPrinter : public AsmPrinter {
private:
    const ANGRYZSubtarget *Subtarget;

public:
    explicit 
};

}   //namespace llvm

#endif
