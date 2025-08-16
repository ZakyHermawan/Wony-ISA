//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "WonyGenInstrInfo.inc"

namespace llvm {

class WonyInstrInfo : public WonyGenInstrInfo {
public:
  WonyInstrInfo();
};

} // namespace llvm
