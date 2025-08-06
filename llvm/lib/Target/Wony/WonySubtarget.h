//===----------------------------------------------------------------------===//
//
// This file declares the Wony specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "WonyISelLowering.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

namespace llvm {

class TargetMachine;
class Triple;

class WonySubtarget : public TargetSubtargetInfo {
  virtual void anchor();
  WonyTargetLowering TLInfo;

public:
  WonySubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                 const TargetMachine &TM);
  const WonyTargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
};

} // end namespace llvm
