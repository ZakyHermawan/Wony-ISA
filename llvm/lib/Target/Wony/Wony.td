//===----------------------------------------------------------------------===//
//
// This is the top level entry point for the Wony target.
//
//===----------------------------------------------------------------------===//

// Include the generic target definitions.
include "llvm/Target/Target.td"

def : ProcessorModel<"generic", NoSchedModel, /*Features=*/[]>;

def WonyInstrInfo : InstrInfo;

def Wony : Target {
  let InstructionSet = WonyInstrInfo;
}
