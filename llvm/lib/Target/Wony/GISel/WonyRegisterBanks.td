//===----------------------------------------------------------------------===//
///
/// \file
/// Define theWony register banks used for GlobalISel.
///
//===----------------------------------------------------------------------===//

/// General Purpose Registers
def GPRBRegBank : RegisterBank<"GPRB", [GPR32, GPR16sp]>;
