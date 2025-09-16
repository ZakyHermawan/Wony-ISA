add_llvm_component_library(LLVMWonyDesc
  WonyMCAsmInfo.cpp
  WonyAsmBackend.cpp
  WonyInstPrinter.cpp
  WonyMCTargetDesc.cpp
  WonyMCCodeEmitter.cpp
  WonyMachObjectWriter.cpp

  LINK_COMPONENTS
  Support
  WonyInfo
  MC

  ADD_TO_COMPONENT
  Wony
)
