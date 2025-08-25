//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "WonyFrameLowering.h"
#include "WonySubtarget.h"
#include "llvm/Support/Error.h"
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
                                      MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  // Get the number of bytes to allocate from the FrameInfo.
  unsigned NumBytes = MFI.getStackSize();

  if (NumBytes > 0) {
    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
    BuildMI(MBB, MBB.begin(), DebugLoc(), TII->get(Wony::SUBSP), Wony::SP)
        .addReg(Wony::SP)
        .addImm(NumBytes);
  }
}

// mock implementation
void WonyFrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  // Get the number of bytes to allocate from the FrameInfo.
  unsigned NumBytes = MFI.getStackSize();

  if (NumBytes > 0) {
    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
    BuildMI(MBB, MBB.getFirstTerminator(), DebugLoc(), TII->get(Wony::ADDSP),
            Wony::SP)
        .addReg(Wony::SP)
        .addImm(NumBytes);
  }
}

MachineBasicBlock::iterator WonyFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {
  const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
  unsigned Opc = MI->getOpcode();

  // The call frame should always be included in the stack frame in the
  // prologue.
  assert(hasReservedCallFrame(MF) && "Wony doesn't have a FP register");

  if (Opc != TII->getCallFrameSetupOpcode() &&
      Opc != TII->getCallFrameDestroyOpcode())
    report_fatal_error("Unexpected frame pseudo instruction");

  if (MI->getOperand(1).getImm() != 0)
    report_fatal_error("Callee pop count not supported");

  return MBB.erase(MI);
}
