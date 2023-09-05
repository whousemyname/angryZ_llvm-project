#include "ANGRYZInstrInfo.h"
#include "ANGRYZ.h"
#include "ANGRYZSubtarget.h"
#include "ANGRYZTargetMachine.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#define GET_INSTRINFO_NAMED_OPS
#include "ANGRYZGenInstrInfo.inc"

ANGRYZInstrInfo::ANGRYZInstrInfo(ANGRYZSubtarget &STI)
    :   ANGRYZGenInstrInfo(), 
        STI(STI){

}