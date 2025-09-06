//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the Machinelegalizer class for Wony
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"

namespace llvm {

class WonySubtarget;

/// This class provides the information for the Wony target legalizer for
/// GlobalISel.
class WonyLegalizerInfo : public LegalizerInfo {

public:
  WonyLegalizerInfo(const WonySubtarget &ST);
};

} // namespace llvm
