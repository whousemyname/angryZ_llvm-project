
//insturction def
let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class RVPBinary<bits<7> funct7, bits<3> funct3, string opcodestr,
                DAGOperand rd_kind = GPR, DAGOperand rs1_kind = GPR,
                DAGOperand rs2_kind = GPR>
    : RVInstR<funct7, funct3, OPC_OP_P,
              (outs rd_kind:$rd), (ins rs1_kind:$rs1, rs2_kind:$rs2),
              opcodestr, "$rd, $rs1, $rs2">;

let Predicates = [HasStdExtZpn] in {
def ADD8     : RVPBinary<0b0100100, 0b000, "add8">,
               Sched<[]>;
def ADD16    : RVPBinary<0b0100000, 0b000, "add16">,
               Sched<[]>;

let isCommutable = 1 in 
def KADDW    : RVPBinary<0b0000000, 0b001, "kaddw">,
               Sched<[]>;
def KSUBW    : RVPBinary<0b0000001, 0b001, "ksubw">,
               Sched<[]>;
}// Predicates = [HasStdExtZpn]



// match pattern
class RVPBinaryIntPat<RVInst Inst, string IntID>
    : Pat<(XLenVT (!cast<Intrinsic>("int_riscv_" # IntID)
                   XLenVT:$rs1, XLenVT:$rs2)),
          (Inst GPR:$rs1, GPR:$rs2)>;

let Predicates = [HasStdExtZpn] in {
    def : RVPBinaryIntPat<ADD8,     "add8">;
    def : RVPBinaryIntPat<ADD16,    "add16">;
    def : RVPBinaryIntPat<KADDW,     "kaddw">;
    def : RVPBinaryIntPat<KSUBW,     "ksubw">;
def : PatGprGpr<add, KADDW>;
def : PatGprGpr<sub, KSUBW>;
}



//20230817
def simm8 : Operand<XLenVT>, ImmLeaf<XLenVT, [{return isInt<8>(Imm);}]> {
  let ParserMatchClass = SImmAsmOperand<8>;
  let EncoderMethod = "getImmOpValue";          //用于MCemit的时候解析，用于MC  llvm/lib/Target/RISCV/MCTargetDesc/RISCVMCCodeEmitter.cpp定义的
  let DecoderMethod = "decodeSImmOperand<8>";   //用于反汇编
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isInt<8>(Imm);
    return MCOp.isBareSymbolRef();
  }];
  let OperandType = "OPERAND_SIMM8";
  let OperandNamespace = "RISCVOp";
}

//关于该指令调度信息的解释
/*
  * WriteIMul 是微操作(与输入输出操作数operand有关)，由RISCVSchedule.td定义
  * 微操作需要与处理器资源resource绑定到一起，用于表示该操作时延为多少，占用res多少个周期
    * def : WriteRes<WriteIDiv, [SBPort0, SBDivider]> {let Latency = 25; let ResourceCycles = [1, 10]}
    * 代表WriteIDiv占用SBPort0/SBDivider这两个port25个时延
*/
def MULI : Instruction, Sched<[WriteIMul, ReadIMul, ReadIMul]> {

    let Namespace = "RISCV";
    let Size = 4;
    dag OutOperandList = (outs HPR:$hd);
    dag InOperandList = (ins HPR:$hs1, simm8:$imm8);
    let AsmString = "muli $hd, $hs1, $imm8";
    //let Pattern = ;

    bits<8> imm8;
    bits<7> hd;
    bits<7> hs1;
    field bits<32> Inst;
    let Inst{31-24} = imm8;
    let Inst{23-17} = hs1;
    let Inst{16-14} = 0b100;
    let Inst{13-7} = hd;
    let Inst{6-0} = 0b1111111;

    let mayLoad = 0;
    let mayStore = 0;
    let hasSideEffects = 0;

    
    //指令格式的设置
    bits<5> format_Value = 24;
    let TSFlags{4-0} = format_Value;

  // Defaults
  field bits<32> SoftFail = 0;
  RISCVVConstraint RVVConstraint = NoConstraint;
  let TSFlags{7-5} = RVVConstraint.Value;

  bits<3> VLMul = 0;
  let TSFlags{10-8} = VLMul;

  bit ForceTailAgnostic = false;
  let TSFlags{11} = ForceTailAgnostic;

  bit IsTiedPseudo = 0;
  let TSFlags{12} = IsTiedPseudo;

  bit HasSEWOp = 0;
  let TSFlags{13} = HasSEWOp;

  bit HasVLOp = 0;
  let TSFlags{14} = HasVLOp;

  bit HasVecPolicyOp = 0;
  let TSFlags{15} = HasVecPolicyOp;

  bit IsRVVWideningReduction = 0;
  let TSFlags{16} = IsRVVWideningReduction;

  bit UsesMaskPolicy = 0;
  let TSFlags{17} = UsesMaskPolicy;

  // Indicates that the result can be considered sign extended from bit 31. Some
  // instructions with this flag aren't W instructions, but are either sign
  // extended from a smaller size, always outputs a small integer, or put zeros
  // in bits 63:31. Used by the SExtWRemoval pass.
  bit IsSignExtendingOpW = 0;
  let TSFlags{18} = IsSignExtendingOpW;

  bit HasRoundModeOp = 0;
  let TSFlags{19} =  HasRoundModeOp;

  // This is only valid when HasRoundModeOp is set to 1. HasRoundModeOp is set
  // to 1 for vector fixed-point or floating-point intrinsics. This bit is
  // processed under pass 'RISCVInsertReadWriteCSR' pass to distinguish between
  // fixed-point / floating-point instructions and emit appropriate read/write
  // to the correct CSR.
  bit UsesVXRM = 0;
  let TSFlags{20} =  UsesVXRM;

}
def ADDIGH : Instruction, Sched<[WriteIALU, ReadIALU]> {

    let Namespace = "RISCV";
    let Size = 4;
    dag OutOperandList = (outs GPR:$rd);
    dag InOperandList = (ins HPR:$hs1, simm8:$imm8);
    let AsmString = "addigd $rd, $hs1, $imm8";
    //let Pattern = ;

    bits<8> imm8;
    bits<7> rd;
    bits<7> hs1;
    field bits<32> Inst;
    let Inst{31-24} = imm8;
    let Inst{23-17} = hs1;
    let Inst{16-14} = 0b011;
    let Inst{13-7} = rd;
    let Inst{6-0} = 0b1111111;

    let mayLoad = 0;
    let mayStore = 0;
    let hasSideEffects = 0;

    
    //指令格式的设置
    bits<5> format_Value = 24;
    let TSFlags{4-0} = format_Value;

  // Defaults
  field bits<32> SoftFail = 0;
  RISCVVConstraint RVVConstraint = NoConstraint;
  let TSFlags{7-5} = RVVConstraint.Value;

  bits<3> VLMul = 0;
  let TSFlags{10-8} = VLMul;

  bit ForceTailAgnostic = false;
  let TSFlags{11} = ForceTailAgnostic;

  bit IsTiedPseudo = 0;
  let TSFlags{12} = IsTiedPseudo;

  bit HasSEWOp = 0;
  let TSFlags{13} = HasSEWOp;

  bit HasVLOp = 0;
  let TSFlags{14} = HasVLOp;

  bit HasVecPolicyOp = 0;
  let TSFlags{15} = HasVecPolicyOp;

  bit IsRVVWideningReduction = 0;
  let TSFlags{16} = IsRVVWideningReduction;

  bit UsesMaskPolicy = 0;
  let TSFlags{17} = UsesMaskPolicy;

  // Indicates that the result can be considered sign extended from bit 31. Some
  // instructions with this flag aren't W instructions, but are either sign
  // extended from a smaller size, always outputs a small integer, or put zeros
  // in bits 63:31. Used by the SExtWRemoval pass.
  bit IsSignExtendingOpW = 0;
  let TSFlags{18} = IsSignExtendingOpW;

  bit HasRoundModeOp = 0;
  let TSFlags{19} =  HasRoundModeOp;

  // This is only valid when HasRoundModeOp is set to 1. HasRoundModeOp is set
  // to 1 for vector fixed-point or floating-point intrinsics. This bit is
  // processed under pass 'RISCVInsertReadWriteCSR' pass to distinguish between
  // fixed-point / floating-point instructions and emit appropriate read/write
  // to the correct CSR.
  bit UsesVXRM = 0;
  let TSFlags{20} =  UsesVXRM;

}


def ADDIHG : Instruction, Sched<[WriteIALU, ReadIALU]> {

    let Namespace = "RISCV";
    let Size = 4;
    dag OutOperandList = (outs HPR:$hd);
    dag InOperandList = (ins GPR:$rs1, simm8:$imm8);
    let AsmString = "addihg $hd, $rs1, $imm8";
    //let Pattern = ;

    bits<8> imm8;
    bits<7> hd;
    bits<7> rs1;
    field bits<32> Inst;
    let Inst{31-24} = imm8;
    let Inst{23-17} = rs1;
    let Inst{16-14} = 0b010;
    let Inst{13-7} = hd;
    let Inst{6-0} = 0b1111111;

    let mayLoad = 0;
    let mayStore = 0;
    let hasSideEffects = 0;

    
    //指令格式的设置
    bits<5> format_Value = 24;
    let TSFlags{4-0} = format_Value;

  // Defaults
  field bits<32> SoftFail = 0;
  RISCVVConstraint RVVConstraint = NoConstraint;
  let TSFlags{7-5} = RVVConstraint.Value;

  bits<3> VLMul = 0;
  let TSFlags{10-8} = VLMul;

  bit ForceTailAgnostic = false;
  let TSFlags{11} = ForceTailAgnostic;

  bit IsTiedPseudo = 0;
  let TSFlags{12} = IsTiedPseudo;

  bit HasSEWOp = 0;
  let TSFlags{13} = HasSEWOp;

  bit HasVLOp = 0;
  let TSFlags{14} = HasVLOp;

  bit HasVecPolicyOp = 0;
  let TSFlags{15} = HasVecPolicyOp;

  bit IsRVVWideningReduction = 0;
  let TSFlags{16} = IsRVVWideningReduction;

  bit UsesMaskPolicy = 0;
  let TSFlags{17} = UsesMaskPolicy;

  // Indicates that the result can be considered sign extended from bit 31. Some
  // instructions with this flag aren't W instructions, but are either sign
  // extended from a smaller size, always outputs a small integer, or put zeros
  // in bits 63:31. Used by the SExtWRemoval pass.
  bit IsSignExtendingOpW = 0;
  let TSFlags{18} = IsSignExtendingOpW;

  bit HasRoundModeOp = 0;
  let TSFlags{19} =  HasRoundModeOp;

  // This is only valid when HasRoundModeOp is set to 1. HasRoundModeOp is set
  // to 1 for vector fixed-point or floating-point intrinsics. This bit is
  // processed under pass 'RISCVInsertReadWriteCSR' pass to distinguish between
  // fixed-point / floating-point instructions and emit appropriate read/write
  // to the correct CSR.
  bit UsesVXRM = 0;
  let TSFlags{20} =  UsesVXRM;

}


def SUBI : Instruction, Sched<[WriteIALU, ReadIALU]> {

    let Namespace = "RISCV";
    let Size = 4;
    dag OutOperandList = (outs HPR:$hd);
    dag InOperandList = (ins HPR:$hs1, simm8:$imm8);
    let AsmString = "subi $hd, $hs1, $imm8";
    //let Pattern = ;

    bits<8> imm8;
    bits<7> hd;
    bits<7> hs1;
    field bits<32> Inst;
    let Inst{31-24} = imm8;
    let Inst{23-17} = hs1;
    let Inst{16-14} = 0b001;
    let Inst{13-7} = hd;
    let Inst{6-0} = 0b1111111;

    let mayLoad = 0;
    let mayStore = 0;
    let hasSideEffects = 0;

    
    //指令格式的设置
    bits<5> format_Value = 24;
    let TSFlags{4-0} = format_Value;

  // Defaults
  field bits<32> SoftFail = 0;
  RISCVVConstraint RVVConstraint = NoConstraint;
  let TSFlags{7-5} = RVVConstraint.Value;

  bits<3> VLMul = 0;
  let TSFlags{10-8} = VLMul;

  bit ForceTailAgnostic = false;
  let TSFlags{11} = ForceTailAgnostic;

  bit IsTiedPseudo = 0;
  let TSFlags{12} = IsTiedPseudo;

  bit HasSEWOp = 0;
  let TSFlags{13} = HasSEWOp;

  bit HasVLOp = 0;
  let TSFlags{14} = HasVLOp;

  bit HasVecPolicyOp = 0;
  let TSFlags{15} = HasVecPolicyOp;

  bit IsRVVWideningReduction = 0;
  let TSFlags{16} = IsRVVWideningReduction;

  bit UsesMaskPolicy = 0;
  let TSFlags{17} = UsesMaskPolicy;

  // Indicates that the result can be considered sign extended from bit 31. Some
  // instructions with this flag aren't W instructions, but are either sign
  // extended from a smaller size, always outputs a small integer, or put zeros
  // in bits 63:31. Used by the SExtWRemoval pass.
  bit IsSignExtendingOpW = 0;
  let TSFlags{18} = IsSignExtendingOpW;

  bit HasRoundModeOp = 0;
  let TSFlags{19} =  HasRoundModeOp;

  // This is only valid when HasRoundModeOp is set to 1. HasRoundModeOp is set
  // to 1 for vector fixed-point or floating-point intrinsics. This bit is
  // processed under pass 'RISCVInsertReadWriteCSR' pass to distinguish between
  // fixed-point / floating-point instructions and emit appropriate read/write
  // to the correct CSR.
  bit UsesVXRM = 0;
  let TSFlags{20} =  UsesVXRM;

}


def LWH : Instruction, Sched<[WriteLDB, ReadMemBase]> {

    let Namespace = "RISCV";
    let Size = 4;
    dag OutOperandList = (outs HPR:$hd);
    dag InOperandList = (ins HPR:$hs1, simm8:$imm8);
    let AsmString = "lwh $hd, $hs1, $imm8";
    //let Pattern = ;

    bits<8> imm8;
    bits<7> hd;
    bits<7> hs1;
    field bits<32> Inst;
    let Inst{31-24} = imm8;
    let Inst{23-17} = hs1;
    let Inst{16-14} = 0b101;
    let Inst{13-7} = hd;
    let Inst{6-0} = 0b1111111;

    let mayLoad = 1;
    let mayStore = 0;
    let hasSideEffects = 0;

    
    //指令格式的设置
    bits<5> format_Value = 24;
    let TSFlags{4-0} = format_Value;

  // Defaults
  field bits<32> SoftFail = 0;
  RISCVVConstraint RVVConstraint = NoConstraint;
  let TSFlags{7-5} = RVVConstraint.Value;

  bits<3> VLMul = 0;
  let TSFlags{10-8} = VLMul;

  bit ForceTailAgnostic = false;
  let TSFlags{11} = ForceTailAgnostic;

  bit IsTiedPseudo = 0;
  let TSFlags{12} = IsTiedPseudo;

  bit HasSEWOp = 0;
  let TSFlags{13} = HasSEWOp;

  bit HasVLOp = 0;
  let TSFlags{14} = HasVLOp;

  bit HasVecPolicyOp = 0;
  let TSFlags{15} = HasVecPolicyOp;

  bit IsRVVWideningReduction = 0;
  let TSFlags{16} = IsRVVWideningReduction;

  bit UsesMaskPolicy = 0;
  let TSFlags{17} = UsesMaskPolicy;

  // Indicates that the result can be considered sign extended from bit 31. Some
  // instructions with this flag aren't W instructions, but are either sign
  // extended from a smaller size, always outputs a small integer, or put zeros
  // in bits 63:31. Used by the SExtWRemoval pass.
  bit IsSignExtendingOpW = 1;
  let TSFlags{18} = IsSignExtendingOpW;

  bit HasRoundModeOp = 0;
  let TSFlags{19} =  HasRoundModeOp;

  // This is only valid when HasRoundModeOp is set to 1. HasRoundModeOp is set
  // to 1 for vector fixed-point or floating-point intrinsics. This bit is
  // processed under pass 'RISCVInsertReadWriteCSR' pass to distinguish between
  // fixed-point / floating-point instructions and emit appropriate read/write
  // to the correct CSR.
  bit UsesVXRM = 0;
  let TSFlags{20} =  UsesVXRM;

}

def SWH : Instruction, Sched<[WriteSTD, ReadStoreData, ReadMemBase]> {

    let Namespace = "RISCV";
    let Size = 4;
    dag OutOperandList = (outs);
    dag InOperandList = (ins HPR:$hs2, HPR:$hs1, simm8:$imm8);
    let AsmString = "swh $hs2, $hs1, $imm8";
    //let Pattern = ;

    bits<8> imm8;
    bits<7> hs2;
    bits<7> hs1;
    field bits<32> Inst;
    let Inst{31-24} = imm8;
    let Inst{23-17} = hs2;
    let Inst{16-14} = 0b110;
    let Inst{13-7} = hs1;
    let Inst{6-0} = 0b1111111;

    let mayLoad = 0;
    let mayStore = 1;
    let hasSideEffects = 0;

    
    //指令格式的设置
    bits<5> format_Value = 24;
    let TSFlags{4-0} = format_Value;

  // Defaults
  field bits<32> SoftFail = 0;
  RISCVVConstraint RVVConstraint = NoConstraint;
  let TSFlags{7-5} = RVVConstraint.Value;

  bits<3> VLMul = 0;
  let TSFlags{10-8} = VLMul;

  bit ForceTailAgnostic = false;
  let TSFlags{11} = ForceTailAgnostic;

  bit IsTiedPseudo = 0;
  let TSFlags{12} = IsTiedPseudo;

  bit HasSEWOp = 0;
  let TSFlags{13} = HasSEWOp;

  bit HasVLOp = 0;
  let TSFlags{14} = HasVLOp;

  bit HasVecPolicyOp = 0;
  let TSFlags{15} = HasVecPolicyOp;

  bit IsRVVWideningReduction = 0;
  let TSFlags{16} = IsRVVWideningReduction;

  bit UsesMaskPolicy = 0;
  let TSFlags{17} = UsesMaskPolicy;

  // Indicates that the result can be considered sign extended from bit 31. Some
  // instructions with this flag aren't W instructions, but are either sign
  // extended from a smaller size, always outputs a small integer, or put zeros
  // in bits 63:31. Used by the SExtWRemoval pass.
  bit IsSignExtendingOpW = 0;
  let TSFlags{18} = IsSignExtendingOpW;

  bit HasRoundModeOp = 0;
  let TSFlags{19} =  HasRoundModeOp;

  // This is only valid when HasRoundModeOp is set to 1. HasRoundModeOp is set
  // to 1 for vector fixed-point or floating-point intrinsics. This bit is
  // processed under pass 'RISCVInsertReadWriteCSR' pass to distinguish between
  // fixed-point / floating-point instructions and emit appropriate read/write
  // to the correct CSR.
  bit UsesVXRM = 0;
  let TSFlags{20} =  UsesVXRM;

}



def BGT : Instruction, Sched<[WriteJmp, ReadJmp, ReadJmp]> {

    let Namespace = "RISCV";
    let Size = 4;
    dag OutOperandList = (outs);
    dag InOperandList = (ins GPR:$rs1, GPR:$rs2, simm13_lsb0:$imm12);
    let AsmString = "bgt $rs1, $rs2, $imm12";
    //let Pattern = ;

    bits<12> imm12;
    bits<5> rs2;
    bits<5> rs1;
    field bits<32> Inst;

    let Inst{31} = imm12{11};
    let Inst{30-25} = imm12{9-4};
    let Inst{24-20} = rs2;
    let Inst{19-15} = rs1;
    let Inst{14-12} = 0b111;
    let Inst{11-8} = imm12{3-0};
    let Inst{7} = imm12{10};
    let Inst{6-0} = 0b1111111;

    let mayLoad = 0;
    let mayStore = 0;
    let hasSideEffects = 0;

    
    //指令格式的设置
    bits<5> format_Value = 24;
    let TSFlags{4-0} = format_Value;

    let isBranch = 1;
    let isTerminator  = 1;



  // Defaults
  field bits<32> SoftFail = 0;
  RISCVVConstraint RVVConstraint = NoConstraint;
  let TSFlags{7-5} = RVVConstraint.Value;

  bits<3> VLMul = 0;
  let TSFlags{10-8} = VLMul;

  bit ForceTailAgnostic = false;
  let TSFlags{11} = ForceTailAgnostic;

  bit IsTiedPseudo = 0;
  let TSFlags{12} = IsTiedPseudo;

  bit HasSEWOp = 0;
  let TSFlags{13} = HasSEWOp;

  bit HasVLOp = 0;
  let TSFlags{14} = HasVLOp;

  bit HasVecPolicyOp = 0;
  let TSFlags{15} = HasVecPolicyOp;

  bit IsRVVWideningReduction = 0;
  let TSFlags{16} = IsRVVWideningReduction;

  bit UsesMaskPolicy = 0;
  let TSFlags{17} = UsesMaskPolicy;

  // Indicates that the result can be considered sign extended from bit 31. Some
  // instructions with this flag aren't W instructions, but are either sign
  // extended from a smaller size, always outputs a small integer, or put zeros
  // in bits 63:31. Used by the SExtWRemoval pass.
  bit IsSignExtendingOpW = 0;
  let TSFlags{18} = IsSignExtendingOpW;

  bit HasRoundModeOp = 0;
  let TSFlags{19} =  HasRoundModeOp;

  // This is only valid when HasRoundModeOp is set to 1. HasRoundModeOp is set
  // to 1 for vector fixed-point or floating-point intrinsics. This bit is
  // processed under pass 'RISCVInsertReadWriteCSR' pass to distinguish between
  // fixed-point / floating-point instructions and emit appropriate read/write
  // to the correct CSR.
  bit UsesVXRM = 0;
  let TSFlags{20} =  UsesVXRM;

}

//pattern
def :Pat<(i32 (sub (i32 HPR:$hs1), (simm8:$imm))), (SUBI HPR:$hs1, simm8:$imm)>;
def :Pat<(i32 (mul (i32 HPR:$hs1), (simm8:$imm))), (MULI HPR:$hs1, simm8:$imm)>;
/*
def :Pat<(i32 (load (AddrRegImm (XLenVT HPR:$hs1), simm8:$imm8))),
          (LWH HPR:$hs1, simm8:$imm)>;
*/

class HLdPat<PatFrag LoadOp, Instruction Inst, RegisterClass resClass, ValueType vt>
    : Pat<(vt (LoadOp (AddrRegImm (vt resClass:$rs1), simm8:$imm8))),
          (Inst resClass:$rs1, simm8:$imm8)>;
def :HLdPat<load, LWH, HPR, i32>;

/*
def: Pat<(store (i32 HPR:$rs2), (AddrRegImm (i32 HPR:$rs1),
                  simm8:$imm8)),
        (Inst StTy:$rs2, HPR:$rs1, simm8:$imm8)>;
*/

class HStPat<PatFrag StoreOp, Instruction Inst, RegisterClass StTy,
            ValueType vt>
    : Pat<(StoreOp (vt StTy:$rs2), (AddrRegImm (XLenVT StTy:$rs1),
                   simm8:$imm8)),
          (Inst StTy:$rs2, StTy:$rs1, simm8:$imm8)>;
def :HStPat<store, SWH, HPR, i32>;


// Match `riscv_brcc` and lower to the appropriate RISC-V branch instruction.
multiclass HBccPat<CondCode Cond,  Instruction Inst> {
  def : Pat<(riscv_brcc (XLenVT GPR:$rs1), GPR:$rs2, Cond, bb:$imm12),
            (Inst GPR:$rs1, GPR:$rs2, simm13_lsb0:$imm12)>;
  // Explicitly select 0 to X0. The register coalescer doesn't always do it.
  def : Pat<(riscv_brcc (XLenVT GPR:$rs1), 0, Cond, bb:$imm12),
            (Inst GPR:$rs1, (XLenVT X0), simm13_lsb0:$imm12)>;
}
defm : HBccPat<SETGT, BGT>;