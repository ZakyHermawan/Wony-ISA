//===----------------------------------------------------------------------===//
//
// This class implements Wony-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {

class WonySubtarget;

class WonyFrameLowering : public TargetFrameLowering {
protected:
  bool hasFPImpl(const MachineFunction &MF) const override;

public:
  explicit WonyFrameLowering(const WonySubtarget &sti)
      : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(8), 0) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
};

} // namespace llvm
