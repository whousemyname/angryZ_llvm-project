add_llvm_component_group(ANGRYZ)

set(LLVM_TARGET_DEFINITIONS ANGRYZ.td)

tablegen(LLVM ANGRYZGenCallingConv.inc -gen-callingconv)
tablegen(LLVM ANGRYZGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ANGRYZGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ANGRYZGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ANGRYZGenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(ANGRYZCommonTableGen)

add_llvm_target(ANGRYZCodeGen
    ANGRYZTargetMachine.cpp
    ANGRYZTargetObjectFile.cpp
    ANGRYZISelDAGToDAG.cpp

    ANGRYZSubtarget.cpp
    ANGRYZISelLowering.cpp
    ANGRYZInstrInfo.cpp
    ANGRYZRegisterInfo.cpp
    ANGRYZFrameLowering.cpp

    LINK_COMPONENTS
    Target
    CodeGen
    Core
    Support
    SelectionDAG
    ANGRYZInfo

    ADD_TO_COMPONENT
    ANGRYZ
)




add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)