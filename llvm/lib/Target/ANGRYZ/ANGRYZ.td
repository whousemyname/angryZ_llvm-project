include "llvm/Target/Target.td"

/*
    * 包含了所有的td
*/
include "ANGRYZRegisterInfo.td"
include "ANGRYZInstrInfo.td"
include "ANGRYZCallingConv.td"

/*
    * processor model的定义
    * todo
*/
def : ProcessorModel<"angryz", NoSchedModel, []>;


//唯一实例化，用来给Target machine的global 赋值
def ANGRYZInstrInfo : InstrInfo;

def ANGRYZ : Target {

    let InstructionSet = ANGRYZInstrInfo;
}