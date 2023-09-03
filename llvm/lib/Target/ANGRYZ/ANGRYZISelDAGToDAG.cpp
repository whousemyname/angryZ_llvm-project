#include "ANGRYZTargetMachine.h"
#include "llvm/Pass.h"
#include "llvm/Support/CodeGen.h"
#include "ANGRYZ.h"
#include "ANGRYZISelDAGToDAG.h"

using namespace llvm;


void ANGRYZDAGToDAGISel::Select(SDNode *Node) {
    //SelectCode(Node);
}


FunctionPass *llvm::createANGRYZISelDag(ANGRYZTargetMachine &TM, CodeGenOpt::Level OptLevel) {
    return new ANGRYZDAGToDAGISel(TM, OptLevel);
}