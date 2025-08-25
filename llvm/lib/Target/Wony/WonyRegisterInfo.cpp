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
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_REGINFO_TARGET_DESC
#include "WonyGenRegisterInfo.inc"

using namespace llvm;

WonyRegisterInfo::WonyRegisterInfo() : WonyGenRegisterInfo(Wony::R0) {}

// mock implementation
const MCPhysReg *
WonyRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

// mock implementation
BitVector WonyRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  // Reserve the stack register so that the register allocator doesn't
  // touch it.
  markSuperRegs(Reserved, Wony::SP);
  return Reserved;
}

// mock implementation
bool WonyRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {


  assert(SPAdj == 0 && "unhandled SP adjustment in call sequence?");

  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  const MachineFrameInfo &MFI = MF.getFrameInfo();

  DebugLoc DL = MI.getDebugLoc();

  MachineOperand &FIOp = MI.getOperand(FIOperandNum);
  int Index = MI.getOperand(FIOperandNum).getIndex();

  // The offset is from the incoming stack pointer.
  // Since our stack pointer is adjusted to the current frame before we
  // issue the load and store, we need to adjust the offset accordingly.
  // I.e., incoming sp pointer + offset
  //       == (current sp pointer + framesize) + offset
  // Note: incoming sp pointer is current sp pointer + framesize, because the
  // stack grows down.
  int64_t Offset = MFI.getObjectOffset(Index);
  Offset += MFI.getStackSize();
  assert(FIOperandNum == 1 && "Stack argument is expected to be the second "
                              "operand for both loads and stores");
  switch (MI.getOpcode()) {
  case Wony::STRSP16:
  case Wony::LDRSP16:
  case Wony::STRSP32:
  case Wony::LDRSP32:
    FIOp.ChangeToRegister(Wony::SP, /*IsDef=*/false);
    Offset += MI.getOperand(2).getImm();
    assert(Offset >= -64 && Offset < 63 && "Offset must fit 7 bits for now");
    MI.getOperand(2).setImm(Offset);
    break;
  default:
    llvm_unreachable("frame index used on unknown instruction");
  }

  return false;
}

// mock implementation
Register WonyRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Register();
}
