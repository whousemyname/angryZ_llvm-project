add_llvm_component_group(ANGRYZ)

add_llvm_target(ANGRYZCodeGen
    ANGRYZTargetMachine.cpp

    LINK_COMPONENTS
    Target
    CodeGen

    ADD_TO_COMPONENT
    ANGRYZ
)




add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)