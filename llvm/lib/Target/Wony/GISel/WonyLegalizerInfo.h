//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the Machinelegalizer class for Wony
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"

namespace llvm {
class MachineIRBuilder;
class GISelChangeObserver;

class WonySubtarget;

/// This class provides the information for the Wony target legalizer for
/// GlobalISel.
class WonyLegalizerInfo : public LegalizerInfo {
  const WonySubtarget &ST;

  bool legalizeMul(MachineInstr &MI, MachineRegisterInfo &MRI,
                   MachineIRBuilder &MIRBuilder,
                   GISelChangeObserver &Observer) const;
public:
  WonyLegalizerInfo(const WonySubtarget &ST);

  bool legalizeCustom(LegalizerHelper &Helper, MachineInstr &MI,
                      LostDebugLocObserver &LocObserver) const override;
};

} // namespace llvm
