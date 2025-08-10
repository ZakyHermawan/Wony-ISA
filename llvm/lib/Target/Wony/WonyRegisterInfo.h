//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "WonyGenRegisterInfo.inc"

namespace llvm {

struct WonyRegisterInfo : public WonyGenRegisterInfo {

  WonyRegisterInfo();

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // namespace llvm
