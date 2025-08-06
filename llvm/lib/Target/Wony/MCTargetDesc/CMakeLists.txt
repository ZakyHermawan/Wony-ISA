add_llvm_component_library(LLVMWonyDesc
  WonyMCAsmInfo.cpp
  WonyMCTargetDesc.cpp

  LINK_COMPONENTS
  Support
  WonyInfo
  MC

  ADD_TO_COMPONENT
  Wony
)
