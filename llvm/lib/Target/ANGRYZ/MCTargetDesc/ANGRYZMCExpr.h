#ifndef LLVM_LIB_TARGET_ANGRYZ_MCTARGETDESC_ANGRYZMCEXPR_H
#define LLVM_LIB_TARGET_ANGRYZ_MCTARGETDESC_ANGRYZMCEXPR_H

#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm{

class ANGRYZMCExpr : public MCTargetExpr {

public :
  enum VariantKind {
    VK_ANGRYZ_None, 
    VK_ANGRYZ_CALL, 
    VK_ANGRYZ_CALL_PLT, 
    VK_ANGRYZ_Invalid   // Must be the last item
  };  // end enum VariantKind
private:
  const MCExpr *Expr;
  const VariantKind Kind;

  explicit ANGRYZMCExpr(const MCExpr *Expr, VariantKind Kind)
            : Expr(Expr), 
              Kind(Kind){

  }

public :

  static const ANGRYZMCExpr *create(const MCExpr *Expr, VariantKind Kind, MCContext &Ctx);
  VariantKind getKind() const {return Kind;}
  const MCExpr *getSubExpr() const {return Expr;}

  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;

  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;

  void visitUsedExpr(MCStreamer &Streamer) const override;

  MCFragment *findAssociatedFragment() const override {
    return getSubExpr()->findAssociatedFragment();
  }

  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;

  static StringRef getVariantKindName(VariantKind Kind) ; 
}; //end class ANGRYZMCExpr

} // end namespace llvm
#endif
