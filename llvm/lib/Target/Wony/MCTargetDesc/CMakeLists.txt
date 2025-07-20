add_llvm_component_library(LLVMWonyDesc
  WonyMCTargetDesc.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  Wony
)
