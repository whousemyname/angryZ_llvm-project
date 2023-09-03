class angryz32 : Instruction {
    let Size = 4;
    let Namespace = "ANGRYZ";
    bits<32> Inst;
}


/*
    自定义SDNode
*/
def ANGRYZRETFlag : SDNode<"ANGRYZISD::RETFlag", SDTNone, 
                            [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

/*
    自定义Operand
*/
def simm13 : Operand<i32>, ImmLeaf<i32, [{
                                            return isInt<13>(Imm);
                                        }]
                                  >;

def simm12 : Operand<i32>, ImmLeaf<i32, [{
                                            return isInt<12>(Imm);
                                        }]
                                  >;

/*
    自定义指令
*/
def ADDI : angryz32 {
    bits<5> opcode = 0b00000;

    let OutOperandList = (outs HPR:$rd);
    let InOperandList = (ins HPR:$rs, simm13:$imm13);
    let AsmString = "addi $rd, $rs, $imm13";
    let Pattern = [(set HPR:$rd, (add HPR:$rs, simm13:$imm13))];

    bits<7> rd;    
    bits<7> rs;
    bits<13> imm13;
    let Inst{31-19} = imm13;
    let Inst{18-12} = rs;
    let Inst{11-5} = rd;
    let Inst{4-0} = opcode;
}


def JALR : angryz32 {
    bits<6> opcode = 0b000000;

    let OutOperandList = (outs HPR:$rd);
    let InOperandList = (ins HPR:$rs, simm12:$imm12);
    let AsmString = "jalr $rd, $rs, $imm12";
    bits<7> rd;    
    bits<7> rs;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs;
    let Inst{12-6} = rd;
    let Inst{5-0} = opcode;

}


/*
    自定义伪指令
*/
def PseudoRET : angryz32, PseudoInstExpansion<(JALR H0, H1, 0)> {
    let OutOperandList = (outs);
    let InOperandList = (ins);
    let Pattern = [(ANGRYZRETFlag)];

    let isPseudo = 1;
    let isReturn = 1;
    let isTerminator = 1;
}





// pattern
/*
def : Pat<(add HPR:$rs, simm13:$imm13), (ADDI HPR:$rs, simm13:$imm13)>;
def : Pat<(ANGRYZRETFlag), (PseudoRET)>;
*/