//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Wony uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class WonySubtarget;
class WonyTargetMachine;

class WonyTargetLowering: public TargetLowering {
public:
  explicit WonyTargetLowering(const TargetMachine& TM);
};

} // end namespace llvm
