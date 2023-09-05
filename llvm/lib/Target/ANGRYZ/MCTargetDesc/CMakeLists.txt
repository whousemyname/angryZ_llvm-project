add_llvm_component_library(LLVMANGRYZDesc
    ANGRYZMCTargetDesc.cpp
    ANGRYZMCAsmInfo.cpp
    ANGRYZInstPrinter.cpp

    LINK_COMPONENTS
    MC
    ANGRYZInfo
    Support
    TargetParser

    ADD_TO_COMPONENT
    ANGRYZ
)