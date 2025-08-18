//===----------------------------------------------------------------------===//
//
// This is the top level entry point for the Wony target.
//
//===----------------------------------------------------------------------===//

// Include the generic target definitions.
include "llvm/Target/Target.td"

include "WonyRegisterInfo.td"
include "WonyCallingConvention.td"
include "WonyInstrInfo.td"

def : ProcessorModel<"generic", NoSchedModel, /*Features=*/[]>;

def WonyInstrInfo : InstrInfo;

def WonyAsmParser : AsmParser;

def Wony : Target {
  let InstructionSet = WonyInstrInfo;
  let AssemblyParsers = [WonyAsmParser];
}
