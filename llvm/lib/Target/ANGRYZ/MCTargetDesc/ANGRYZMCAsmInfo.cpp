#include "ANGRYZMCAsmInfo.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

void ANGRYZMCAsmInfo::anchor() {}

ANGRYZMCAsmInfo::ANGRYZMCAsmInfo(const Triple &TT) {
    /*
        todo以下是riscv asmInfo的设置，retargetable如何根据自定义更改呢？   ABI？EFI？
    */
  CodePointerSize = CalleeSaveStackSlotSize = TT.isArch64Bit() ? 8 : 4;
  CommentString = "#";
  AlignmentIsInBytes = false;
  SupportsDebugInformation = true;
  ExceptionsType = ExceptionHandling::DwarfCFI;
  Data16bitsDirective = "\t.half\t";
  Data32bitsDirective = "\t.word\t";
}
