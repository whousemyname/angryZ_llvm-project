#include "ANGRYZTargetMachine.h"
#include "ANGRYZ.h"
#include "ANGRYZISelDAGToDAG.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineValueType.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CodeGen.h"
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "angryz-isel" 

StringRef ANGRYZDAGToDAGISel::getPassName() const {
    return  "ANGRYZ ISel " ;
}

void ANGRYZDAGToDAGISel::Select(SDNode *Node) {
    SDLoc DL(Node);
    switch (Node->getOpcode()) {
        case ISD::Constant : {
            int64_t Imm = cast<ConstantSDNode>(Node)->getSExtValue();
            if (Imm >= -2048 && Imm <= 2047) {
                SDValue SDImm = CurDAG->getTargetConstant(Imm, DL, MVT::i32);
                SDValue SrcReg = CurDAG->getRegister(ANGRYZ::H0, MVT::i32);
                SDNode *Result = 
                    CurDAG->getMachineNode(ANGRYZ::ADDI, DL, MVT::i32, SrcReg, SDImm);
                ReplaceNode(Node, Result);
                return ;
            }
        }
        default :
            break;
    }
    SelectCode(Node);
}

bool ANGRYZDAGToDAGISel::SelectAddrFrameIndex (SDValue Addr, SDValue &Base, SDValue &Offset) {
    if (auto *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
        Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), MVT::i32);
        return true;
    }
    
    return false;
}

bool ANGRYZDAGToDAGISel::SelectAddrRegImm(SDValue Addr, SDValue &Base, SDValue &Offset, 
                          bool IsINX) {
    if (SelectAddrFrameIndex(Addr, Base, Offset)) 
        return true;
    return false;

}


char ANGRYZDAGToDAGISel::ID = 0;

FunctionPass *llvm::createANGRYZISelDag(ANGRYZTargetMachine &TM, CodeGenOpt::Level OptLevel) {
    return new ANGRYZDAGToDAGISel(TM, OptLevel);
}
