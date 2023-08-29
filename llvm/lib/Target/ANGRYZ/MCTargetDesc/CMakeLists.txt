add_llvm_component_library(LLVMANGRYZDesc
    ANGRYZMCTargetDesc.cpp

    LINK_COMPONENTS
    MC


    ADD_TO_COMPONENT
    ANGRYZ
)