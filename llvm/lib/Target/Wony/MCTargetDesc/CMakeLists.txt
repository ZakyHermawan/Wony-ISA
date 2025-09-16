add_llvm_component_library(LLVMWonyDesc
  WonyMCAsmInfo.cpp
  WonyMCTargetDesc.cpp
  WonyInstPrinter.cpp
  WonyMCCodeEmitter.cpp
  WonyMachObjectWriter.cpp

  LINK_COMPONENTS
  Support
  WonyInfo
  MC

  ADD_TO_COMPONENT
  Wony
)
