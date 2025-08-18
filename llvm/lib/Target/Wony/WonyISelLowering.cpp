//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that Wony uses to lower LLVM code
// into a selection DAG.
//
//===----------------------------------------------------------------------===//

#include "Wony.h"
#include "WonySubtarget.h"
#include "WonyISelLowering.h"
#include "WonyTargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "wony-lowering"

WonyTargetLowering::WonyTargetLowering(const TargetMachine &TM)
    : TargetLowering(TM) {}

// Calling Convention Implementation
#include "WonyGenCallingConv.inc"

FastISel *
WonyTargetLowering::createFastISel(FunctionLoweringInfo &funcInfo,
                                    const TargetLibraryInfo *libInfo) const {
  return Wony::createFastISel(funcInfo, libInfo);
}

