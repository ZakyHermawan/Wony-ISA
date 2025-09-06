//===----------------------------------------------------------------------===//
//
// This file implements the Wony specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#include "WonySubtarget.h"
#include "GISel/WonyCallLowering.h"
#include "GISel/WonyLegalizerInfo.h"
#include "GISel/WonyRegisterBankInfo.h"
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
    : WonyGenSubtargetInfo(TT, CPU, /*TuneCPU=*/"", FS), FrameLowering(*this),
      TLInfo(TM, *this) {
  CallLoweringInfo.reset(new WonyCallLowering(*getTargetLowering()));
  Legalizer.reset(new WonyLegalizerInfo(*this));
  RegBankInfo.reset(new WonyRegisterBankInfo(*getRegisterInfo()));
}

const CallLowering *WonySubtarget::getCallLowering() const {
  return CallLoweringInfo.get();
}

const LegalizerInfo *WonySubtarget::getLegalizerInfo() const {
  return Legalizer.get();
}

const RegisterBankInfo *WonySubtarget::getRegBankInfo() const {
  return RegBankInfo.get();
}
