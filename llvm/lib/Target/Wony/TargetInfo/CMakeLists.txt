add_llvm_component_library(LLVMWonyInfo
  WonyTargetInfo.cpp
  LINK_COMPONENTS
  Support
  ADD_TO_COMPONENT
  Wony
)
