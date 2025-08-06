//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that Wony uses to lower LLVM code
// into a selection DAG.
//
//===----------------------------------------------------------------------===//

#include "WonySubtarget.h"
#include "WonyISelLowering.h"
#include "WonyTargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "wony-lowering"

WonyTargetLowering::WonyTargetLowering(const TargetMachine &TM)
    : TargetLowering(TM) {}
