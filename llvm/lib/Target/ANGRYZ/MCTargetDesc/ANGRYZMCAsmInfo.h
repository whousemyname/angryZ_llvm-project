#ifndef LLVM_LIB_TARGET_ANGRYZ_MCTARGETDESC_ANGRYZMCASMINFO_H
#define LLVM_LIB_TARGET_ANGRYZ_MCTARGETDESC_ANGRYZMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"
namespace llvm {
class Triple;

class ANGRYZMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit ANGRYZMCAsmInfo(const Triple &TargetTriple);

    /*  riscv重写了以下函数
  const MCExpr *getExprForFDESymbol(const MCSymbol *Sym, unsigned Encoding,
                                    MCStreamer &Streamer) const override;
    */

};

} // namespace llvm

#endif
