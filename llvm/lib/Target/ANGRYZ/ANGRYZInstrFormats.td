
class angryz32 : Instruction {
    let Size = 4;
    let Namespace = "ANGRYZ";
    bits<32> Inst;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : angryz32 {
  let isPseudo = 1;
  let isCodeGenOnly = 1;

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # !if(!empty(argstr), "", "\t" # argstr);
  let Pattern = pattern;
}