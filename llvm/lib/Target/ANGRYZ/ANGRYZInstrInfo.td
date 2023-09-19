



/*
  SDTypeProfile
*/
def SDT_ANGRYZCall : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                       SDTCisVT<1, i32>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                     SDTCisVT<1, i32>]>;
/*
    自定义 SDNode
*/
def ANGRYZRETFlag : SDNode<"ANGRYZISD::RET_FLAG", SDTNone, 
                            [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def angryz_call : SDNode<"ANGRYZISD::CALL", SDT_ANGRYZCall, 
                         [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, 
                         SDNPVariadic]>;
// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
/*
    自定义 Operand
*/
def simm13 : Operand<i32>, ImmLeaf<i32, [{
                                            return isInt<13>(Imm);
                                        }]
                                  >;

def simm12 : Operand<i32>, ImmLeaf<i32, [{
                                            return isInt<12>(Imm);
                                        }]
                                  >;

def uimmlog2xlen : Operand<i32>, ImmLeaf<i32, [{
  if (Subtarget->is64Bit())
    return isUInt<6>(Imm);
  return isUInt<5>(Imm);
}]>{
  let MCOperandPredicate = [{
    int64_t Imm;
    if (!MCOp.evaluateAsConstantImm(Imm))
      return false;
    if (STI.getTargetTriple().isArch64Bit())
      return isUInt<6>(Imm);
    return isUInt<5>(Imm);
  }];
  let OperandType = "OPERAND_UIMMLOG2XLEN";
  let OperandNamespace = "ANGRYZOp";
}

def CallSymbol : AsmOperandClass {
  let Name = "CallSymbol";
  let RenderMethod = "addImmOperands";
  let DiagnosticType = "InvalidCallSymbol";
  let ParserMethod = "parseCallSymbol";
}

// A bare symbol used in call/tail only.
def call_symbol : Operand<i32> {
  // TODO : let ParserMatchClass = CallSymbol;
}


include "ANGRYZInstrFormats.td"
/*
    自定义指令
*/

/*
  shifts    //shift指令的定义虽然属于传统的I格式，但是对于imm的处理 ，它实际上只有5/6bit
*/
def SRAI : angryz32 {

  let OutOperandList = (outs HPR:$rd1);
  let InOperandList = (ins HPR:$rs1, uimmlog2xlen:$shamt);
  let AsmString = "srai $rd1, $rs1, $shamt";
  
  bits<6> opcode = 0b000111;
  bits<7> rd1;    
  bits<7> rs1;
  //bits<12> shamt;
  bits<6> shamt;

  let Inst{31-27} = 0b01000;      //01000 for arithmetic; 00000 for logic;
  let Inst{26} = 0;
  let Inst{25-20} = shamt;
  let Inst{19-13} = rs1;
  let Inst{12-6} = rd1;
  let Inst{5-0} = opcode;
}

/*
    arithmetic  
*/
def ADDI : angryz32 {

    let OutOperandList = (outs HPR:$rd1);
    let InOperandList = (ins HPR:$rs1, simm12:$imm12);
    let AsmString = "addi $rd1, $rs1, $imm12";
    let Pattern = [(set HPR:$rd1, (add HPR:$rs1, simm12:$imm12))];
    
    bits<6> opcode = 0b000000;
    bits<7> rd1;    
    bits<7> rs1;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs1;
    let Inst{12-6} = rd1;
    let Inst{5-0} = opcode;
}



def ADD : angryz32 {
    

    let OutOperandList = (outs HPR:$rd1);
    let InOperandList = (ins HPR:$rs1, HPR:$rs2);
    let AsmString = "add $rd1, $rs1, $rs2";
    let Pattern = [(set HPR:$rd1, (add HPR:$rs1, HPR:$rs2))];

    bits<11> opcode = 0b00000000000;
    bits<7> rd1;    
    bits<7> rs1;
    bits<7> rs2;
    let Inst{31-25} = rs2;
    let Inst{24-18} = rs1;
    let Inst{17-11} = rd1;
    let Inst{10-0} = opcode;
}

def SUB : angryz32 {
    

    let OutOperandList = (outs HPR:$rd1);
    let InOperandList = (ins HPR:$rs1, HPR:$rs2);
    let AsmString = "SUB $rd1, $rs1, $rs2";

    bits<11> opcode = 0b00000000001;
    bits<7> rd1;    
    bits<7> rs1;
    bits<7> rs2;
    let Inst{31-25} = rs2;
    let Inst{24-18} = rs1;
    let Inst{17-11} = rd1;
    let Inst{10-0} = opcode;
}

/*
  logic
*/
def XORI : angryz32 {
    //let isReMaterializable = 1, isAsCheapAsAMove = 1
    let OutOperandList = (outs HPR:$rd1);
    let InOperandList = (ins HPR:$rs1, simm12:$imm12);
    let AsmString = "XORI $rd1, $rs1, $imm12";
    
    bits<6> opcode = 0b001000;
    bits<7> rd1;    
    bits<7> rs1;
    bits<12> imm12;
    let Inst{31-20} = imm12;
    let Inst{19-13} = rs1;
    let Inst{12-6} = rd1;
    let Inst{5-0} = opcode;
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

// Pessimistically assume the stack pointer will be clobbered
let Defs = [H2], Uses = [H2] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(callseq_end timm:$amt1, timm:$amt2)]>;
} // Defs = [X2], Uses = [X2]


let isCall = 1, Defs = [H1], isCodeGenOnly = 0, Size = 8 in
def PseudoCALL : Pseudo<(outs), (ins call_symbol:$func), [],
                        "call", "$func">;

/*
    complexPattern
*/
def AddrRegImm : ComplexPattern <iPTR, 2, "SelectAddrRegImm">;

// pattern
/*
def : Pat<(add HPR:$rs, simm13:$imm13), (ADDI HPR:$rs, simm13:$imm13)>;
def : Pat<(ANGRYZRETFlag), (PseudoRET)>;
*/

def: Pat<(i32 (sra (i32 HPR:$rs1), uimmlog2xlen:$shamt)), 
         (SRAI HPR:$rs1, uimmlog2xlen:$shamt)>;

def : Pat<(add HPR:$rs1, HPR:$rs2), (ADD HPR:$rs1, HPR:$rs2)>;

def : Pat<(i32 (sub (i32 HPR:$rs1), (i32 HPR:$rs2))), 
          (SUB HPR:$rs1, HPR:$rs2)>;

def : Pat<(i32 (xor (i32 HPR:$rs1), simm12:$imm12)), 
          (XORI HPR:$rs1, simm12:$imm12)>;

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

def : Pat<(angryz_call tglobaladdr:$func), (PseudoCALL tglobaladdr:$func)>;