#include "ANGRYZInstrInfo.h"
#include "ANGRYZSubtarget.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "ANGRYZGenInstrInfo.inc"

ANGRYZInstrInfo::ANGRYZInstrInfo(ANGRYZSubtarget &STI)
    :   ANGRYZGenInstrInfo(), 
        STI(STI){

}