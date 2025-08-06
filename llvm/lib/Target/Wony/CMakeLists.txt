add_llvm_component_group(Wony)

set(LLVM_TARGET_DEFINITIONS Wony.td)

tablegen(LLVM WonyGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(WonyCommonTableGen)

add_llvm_target(WonyCodeGen
  WonySubtarget.cpp
  WonyISelLowering.cpp
  WonyTargetMachine.cpp
  WonyTargetObjectFile.cpp
  WonyTargetTransformInfo.cpp
  WonySimpleConstantPropagation.cpp
  LINK_COMPONENTS

  WonyDesc
  WonyInfo

  Analysis
  CodeGen
  Core
  MC
  Passes
  SelectionDAG
  Support
  Target
  TargetParser
  TransformUtils

  ADD_TO_COMPONENT
  Wony
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
