add_llvm_component_group(Wony)

set(LLVM_TARGET_DEFINITIONS Wony.td)

tablegen(LLVM WonyGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM WonyGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM WonyGenInstrInfo.inc -gen-instr-info)
# MCInst to .s
tablegen(LLVM WonyGenAsmWriter.inc -gen-asm-writer)
# Encoding.
tablegen(LLVM WonyGenMCCodeEmitter.inc -gen-emitter)
# .s to MCInst
tablegen(LLVM WonyGenAsmMatcher.inc -gen-asm-matcher)

add_public_tablegen_target(WonyCommonTableGen)

add_llvm_target(WonyCodeGen
  WonySubtarget.cpp
  WonyISelLowering.cpp
  WonyRegisterInfo.cpp
  WonyFrameLowering.cpp
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

add_subdirectory(AsmParser)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
