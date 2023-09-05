add_llvm_component_library(LLVMANGRYZInfo
  ANGRYZTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  ANGRYZ
  )
