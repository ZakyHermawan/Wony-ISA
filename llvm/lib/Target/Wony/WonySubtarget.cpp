//===----------------------------------------------------------------------===//
//
// This file implements the Wony specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#include "WonySubtarget.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "wony-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "WonyGenSubtargetInfo.inc"

// Pin the vtable to this file.
void WonySubtarget::anchor() {}

WonySubtarget::WonySubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                               const TargetMachine &TM)
    : WonyGenSubtargetInfo(TT, CPU, /*TuneCPU=*/"", FS), TLInfo(TM) {}
