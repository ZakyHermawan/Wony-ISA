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
tablegen(LLVM WonyGenCallingConv.inc -gen-callingconv)
# SDISel
tablegen(LLVM WonyGenDAGISel.inc -gen-dag-isel)
# FastISel
tablegen(LLVM WonyGenFastISel.inc -gen-fast-isel)
# GlobalISel
tablegen(LLVM WonyGenRegisterBank.inc -gen-register-bank)
tablegen(LLVM WonyGenGlobalISel.inc -gen-global-isel)
tablegen(LLVM WonyGenMandatoryPreLegalizeGICombiner.inc -gen-global-isel-combiner
              -combiners="WonyMandatoryPreLegalizerCombiner")
tablegen(LLVM WonyGenMandatoryPostLegalizeGICombiner.inc -gen-global-isel-combiner
              -combiners="WonyMandatoryPostLegalizerCombiner")

add_public_tablegen_target(WonyCommonTableGen)

add_llvm_target(WonyCodeGen
  WonyFastISel.cpp
  WonySubtarget.cpp
  WonyInstrInfo.cpp
  WonyAsmPrinter.cpp
  WonyISelLowering.cpp
  WonyRegisterInfo.cpp
  WonyISelDAGToDAG.cpp
  WonyFrameLowering.cpp
  WonyTargetMachine.cpp
  WonyTargetObjectFile.cpp
  WonyCallingConvention.cpp
  WonyTargetTransformInfo.cpp
  WonySimpleConstantPropagation.cpp

  # GlobalISel related files.
  GISel/WonyCallLowering.cpp
  GISel/WonyLegalizerInfo.cpp
  GISel/WonyRegisterBankInfo.cpp
  GISel/WonyInstructionSelector.cpp
  GISel/WonyMandatoryPreLegalizerCombiner.cpp
  GISel/WonyMandatoryPostLegalizerCombiner.cpp

  LINK_COMPONENTS
  WonyDesc
  WonyInfo

  Analysis
  CodeGen
  CodeGenTypes
  Core
  GlobalISel
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
