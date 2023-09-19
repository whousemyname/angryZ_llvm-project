

def RetCC_ANGRYZ : CallingConv<[
  CCIfType<[i1, i8, i16, i32], CCAssignToReg<[H10, H11]>>, 
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def CC_ANGRYZ : CallingConv<[
  CCIfType<[i1, i8, i16, i32], CCAssignToReg<[H10, H11, H12, H13, H14]>>, 
  CCIfType<[i32], CCAssignToStack<4, 4>> 
]>;


/*
  inc文件中的CC_Save_SaveList会与CC_Save对应起来 ： 被调用者保存的寄存器
*/
def CSR_ILP32_LP64 : CalleeSavedRegs<(add H1, H3, H4, H8, H9, (sequence "H%u", 18, 27))>;

// Needed for implementation of RISCVRegisterInfo::getNoPreservedMask()
def CSR_NoRegs : CalleeSavedRegs<(add)>;