//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "WonyFrameLowering.h"
#include "WonySubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

// mock implementation
bool WonyFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  return false;
}

// mock implementation
void WonyFrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {}

// mock implementation
void WonyFrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {}
