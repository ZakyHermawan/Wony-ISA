//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "WonyRegisterInfo.h"
#include "WonyFrameLowering.h"
#include "MCTargetDesc/WonyMCTargetDesc.h" // For the enum of the regclasses.
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_REGINFO_TARGET_DESC
#include "WonyGenRegisterInfo.inc"
using namespace llvm;

WonyRegisterInfo::WonyRegisterInfo() : WonyGenRegisterInfo(Register()) {}

// mock implementation
const MCPhysReg *
WonyRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return nullptr;
}

// mock implementation
BitVector WonyRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  return Reserved;
}

// mock implementation
bool WonyRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  return false;
}

// mock implementation
Register WonyRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Register();
}
