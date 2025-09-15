//===----------------------------------------------------------------------===//
//
// This is the top level entry point for the Wony target.
//
//===----------------------------------------------------------------------===//

// Include the generic target definitions.
include "llvm/Target/Target.td"

include "WonyScheduleDefs.td"
include "WonyRegisterInfo.td"
include "WonyCallingConvention.td"
include "WonyInstrInfo.td"
include "WonySchedModel.td"

include "GISel/WonyCombine.td"
include "GISel/WonyRegisterBanks.td"

def : ProcessorModel<"generic", WonyDefaultModel, /*Features=*/[]>;

def WonyInstrInfo : InstrInfo;

def WonyAsmParser : AsmParser;

def Wony : Target {
  let InstructionSet = WonyInstrInfo;
  let AssemblyParsers = [WonyAsmParser];
}
