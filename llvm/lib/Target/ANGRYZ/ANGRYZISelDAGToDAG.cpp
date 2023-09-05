#include "ANGRYZTargetMachine.h"
#include "ANGRYZ.h"
#include "ANGRYZISelDAGToDAG.h"

#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Pass.h"
#include "llvm/Support/CodeGen.h"
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "angryz-isel" 

char ANGRYZDAGToDAGISel::ID = 0;

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


FunctionPass *llvm::createANGRYZISelDag(ANGRYZTargetMachine &TM, CodeGenOpt::Level OptLevel) {
    return new ANGRYZDAGToDAGISel(TM, OptLevel);
}
