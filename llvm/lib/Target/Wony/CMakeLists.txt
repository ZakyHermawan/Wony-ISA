add_llvm_component_group(Wony)

add_llvm_target(WonyCodeGen
  WonyTargetMachine.cpp
  LINK_COMPONENTS
  WonyDesc
  WonyInfo
  Support
  ADD_TO_COMPONENT
  Wony
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
