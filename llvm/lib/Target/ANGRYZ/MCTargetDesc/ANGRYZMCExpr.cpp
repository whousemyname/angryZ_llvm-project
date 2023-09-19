#include "ANGRYZMCExpr.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "angryz-mcexpr"

const ANGRYZMCExpr *ANGRYZMCExpr::create(const MCExpr *Expr, VariantKind Kind,
                                       MCContext &Ctx) {
  return new (Ctx) ANGRYZMCExpr(Expr, Kind);
}

void ANGRYZMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  VariantKind Kind = getKind();
  bool HasVariant = ((Kind != VK_ANGRYZ_None) && (Kind != VK_ANGRYZ_CALL) &&
                     (Kind != VK_ANGRYZ_CALL_PLT));

  if (HasVariant)
    OS << '%' << getVariantKindName(getKind()) << '(';
  Expr->print(OS, MAI);
  if (Kind == VK_ANGRYZ_CALL_PLT)
    OS << "@plt";
  if (HasVariant)
    OS << ')';
}

bool ANGRYZMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                            const MCAsmLayout *Layout,
                                            const MCFixup *Fixup) const {
  // Explicitly drop the layout and assembler to prevent any symbolic folding in
  // the expression handling.  This is required to preserve symbolic difference
  // expressions to emit the paired relocations.
  if (!getSubExpr()->evaluateAsRelocatable(Res, nullptr, nullptr))
    return false;

  Res =
      MCValue::get(Res.getSymA(), Res.getSymB(), Res.getConstant(), getKind());
  // Custom fixup types are not valid with symbol difference expressions.
  return Res.getSymB() ? getKind() == VK_ANGRYZ_None : true;
}

void ANGRYZMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}


// TODO : ANGRYZMCExpr::fixELFSymbolsInTLSFixups
void ANGRYZMCExpr::fixELFSymbolsInTLSFixups(MCAssembler &Asm) const {
  switch (getKind()) {
  default:
    return;
    /*
  case VK_ANGRYZ_TPREL_HI:
  case VK_ANGRYZ_TLS_GOT_HI:
  case VK_ANGRYZ_TLS_GD_HI:
    break;
    */
  }
  //fixELFSymbolsInTLSFixupsImpl(getSubExpr(), Asm);
}

StringRef ANGRYZMCExpr::getVariantKindName(VariantKind Kind) {
  switch (Kind) {
    case VK_ANGRYZ_Invalid:
    case VK_ANGRYZ_None : 
      llvm_unreachable("Invalid ELF symbol kind");
    case VK_ANGRYZ_CALL : 
      return "call";
    case VK_ANGRYZ_CALL_PLT : 
      return "call_plt";
  }
  llvm_unreachable("Invalid ELF symbol kind");
}
