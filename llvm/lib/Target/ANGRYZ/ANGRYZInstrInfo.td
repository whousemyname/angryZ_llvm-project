

class angryz32 : Instruction {
    let Size = 4;
    let Namespace = "ANGRYZ";
    bits<32> Inst;
}


/*
    自定义SDNode
*/
def ANGRYZRETFlag : SDNode<"ANGRYZISD::RET_FLAG", SDTNone, 
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

/*
    arithmetic  
*/
def ADDI : angryz32 {

    let OutOperandList = (outs HPR:$rd);
    let InOperandList = (ins HPR:$rs, simm12:$imm12);
    let AsmString = "addi $rd, $rs, $imm12";
    let Pattern = [(set HPR:$rd, (add HPR:$rs, simm12:$imm12))];
    
    bits<6> opcode = 0b000000;
    bits<7> rd;    
    bits<7> rs;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs;
    let Inst{12-6} = rd;
    let Inst{5-0} = opcode;
}



def ADD : angryz32 {
    

    let OutOperandList = (outs HPR:$rd);
    let InOperandList = (ins HPR:$rs1, HPR:$rs2);
    let AsmString = "add $rd, $rs1, $rs2";
    let Pattern = [(set HPR:$rd, (add HPR:$rs1, HPR:$rs2))];

    bits<11> opcode = 0b00000000000;
    bits<7> rd;    
    bits<7> rs1;
    bits<7> rs2;
    let Inst{31-25} = rs2;
    let Inst{24-18} = rs1;
    let Inst{17-11} = rd;
    let Inst{10-0} = opcode;
}



/*
    jump & link
*/
def JALR : angryz32 {

    let OutOperandList = (outs HPR:$rd);
    let InOperandList = (ins HPR:$rs, simm12:$imm12);
    let AsmString = "jalr $rd, $rs, $imm12";

    bits<6> opcode = 0b000001;
    bits<7> rd;    
    bits<7> rs;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs;
    let Inst{12-6} = rd;
    let Inst{5-0} = opcode;

}


/*
    load store
*/
def LW : angryz32 {
    let OutOperandList = (outs HPR:$rd);
    let InOperandList = (ins HPR:$rs1, simm12:$imm12);
    let AsmString = "LW $rd, ${imm12}(${rs1})";

    bits<6> opcode = 0b000010;
    bits<7> rd;    
    bits<7> rs1;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs1;
    let Inst{12-6} = rd;
    let Inst{5-0} = opcode;
}

def LB : angryz32 {
    let OutOperandList = (outs HPR:$rd);
    let InOperandList = (ins HPR:$rs1, simm12:$imm12);
    let AsmString = "LB $rd, ${imm12}(${rs1})";

    bits<6> opcode = 0b000100;
    bits<7> rd;    
    bits<7> rs1;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs1;
    let Inst{12-6} = rd;
    let Inst{5-0} = opcode;
}

def LBU : angryz32 {
    let OutOperandList = (outs HPR:$rd);
    let InOperandList = (ins HPR:$rs1, simm12:$imm12);
    let AsmString = "LB $rd, ${imm12}(${rs1})";

    bits<6> opcode = 0b000101;
    bits<7> rd;    
    bits<7> rs1;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs1;
    let Inst{12-6} = rd;
    let Inst{5-0} = opcode;
}


/*
  store
*/
def SW : angryz32 {
    let OutOperandList = (outs);
    let InOperandList = (ins HPR:$rs2, HPR:$rs1, simm12:$imm12);
    let AsmString = "SW  $rs2, ${imm12}(${rs1})";

    bits<6> opcode = 0b000011;
    bits<7> rs2;    
    bits<7> rs1;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs1;
    let Inst{12-6} = rs2;
    let Inst{5-0} = opcode;
}


def SB : angryz32 {
    let OutOperandList = (outs);
    let InOperandList = (ins HPR:$rs2, HPR:$rs1, simm12:$imm12);
    let AsmString = "SB  $rs2, ${imm12}(${rs1})";

    bits<6> opcode = 0b000110;
    bits<7> rs2;
    bits<7> rs1;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs1;
    let Inst{12-6} = rs2;
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


/*
    complexPattern
*/
def AddrRegImm : ComplexPattern <iPTR, 2, "SelectAddrRegImm">;

// pattern
/*
def : Pat<(add HPR:$rs, simm13:$imm13), (ADDI HPR:$rs, simm13:$imm13)>;
def : Pat<(ANGRYZRETFlag), (PseudoRET)>;
*/

def : Pat<(add HPR:$rs1, HPR:$rs2), (ADD HPR:$rs1, HPR:$rs2)>;

def : Pat<(load (AddrRegImm (i32 HPR:$rs1), simm12:$imm12)), 
          (LW HPR:$rs1, simm12:$imm12)>;

def : Pat<(i32 (extloadi8 (AddrRegImm (i32 HPR:$rs1), simm12:$imm12))),     //extloadi8 是任意ext类型
          (LBU HPR:$rs1, simm12:$imm12)>;

def : Pat<(i32 (zextloadi8 (AddrRegImm (i32 HPR:$rs1), simm12:$imm12))),    //zext 是零扩展
          (LBU HPR:$rs1, simm12:$imm12)>;

def : Pat<(truncstorei8 (i32 HPR:$rs2), 
                        (AddrRegImm (i32 HPR:$rs1), simm12:$imm12)), 
          (SB HPR:$rs2, HPR:$rs1, simm12:$imm12)>;

def : Pat<(store (i32 HPR:$rs2), 
                 (AddrRegImm (i32 HPR:$rs1), simm12:$imm12)), 
          (SW HPR:$rs2, HPR:$rs1, simm12:$imm12)>;