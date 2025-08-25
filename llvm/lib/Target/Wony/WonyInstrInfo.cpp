//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "WonyInstrInfo.h"
#include "Wony.h"
#include "WonyRegisterInfo.h"

#include "llvm/IR/DebugLoc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include <cassert>
#include <iterator>

#define GET_INSTRINFO_CTOR_DTOR
#include "WonyGenInstrInfo.inc"

using namespace llvm;

WonyInstrInfo::WonyInstrInfo()
    : WonyGenInstrInfo(Wony::ADJCALLSTACKDOWN, Wony::ADJCALLSTACKUP) {}

void WonyInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                         MachineBasicBlock::iterator MBBI,
                                         Register SrcReg, bool isKill, int FI,
                                         const TargetRegisterClass *RC,
                                         const TargetRegisterInfo *TRI,
                                         Register VReg,
                                         MachineInstr::MIFlag Flags) const {
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  MachinePointerInfo PtrInfo = MachinePointerInfo::getFixedStack(MF, FI);
  MachineMemOperand *MMO =
      MF.getMachineMemOperand(PtrInfo, MachineMemOperand::MOStore,
                              MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

  unsigned Opc = TRI->getSpillSize(*RC) == 2 ? Wony::STRSP16 : Wony::STRSP32;
  MFI.setStackID(FI, TargetStackID::Default);
  BuildMI(MBB, MBBI, DebugLoc(), get(Opc))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FI)
      .addImm(0)
      .addMemOperand(MMO);
}

void WonyInstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, Register DestReg,
    int FI, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI,
    Register VReg, MachineInstr::MIFlag Flags) const {
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachinePointerInfo PtrInfo = MachinePointerInfo::getFixedStack(MF, FI);
  MachineMemOperand *MMO =
      MF.getMachineMemOperand(PtrInfo, MachineMemOperand::MOLoad,
                              MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

  unsigned Opc = TRI->getSpillSize(*RC) == 2 ? Wony::LDRSP16 : Wony::LDRSP32;
  MFI.setStackID(FI, TargetStackID::Default);
  BuildMI(MBB, MBBI, DebugLoc(), get(Opc), DestReg)
      .addFrameIndex(FI)
      .addImm(0)
      .addMemOperand(MMO);
}

void WonyInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const DebugLoc &DL, MCRegister DestReg,
                                 MCRegister SrcReg, bool KillSrc,
                                 bool RenamableDest, bool RenamableSrc) const {
  const TargetRegisterInfo &TRI =
      *MBB.getParent()->getSubtarget().getRegisterInfo();
  unsigned Opc = TRI.getMinimalPhysRegClass(DestReg) == &Wony::GPR16RegClass
                     ? Wony::MOV16
                     : Wony::MOV32;
  if (SrcReg == Wony::SP) {
    assert(TRI.getMinimalPhysRegClass(DestReg) == &Wony::GPR16RegClass &&
           "Dest reg for stack must be 16-bit");
    Opc = Wony::MOVFROMSP;
  } else if (DestReg == Wony::SP) {
    assert(TRI.getMinimalPhysRegClass(SrcReg) == &Wony::GPR16RegClass &&
           "Src reg for stack must be 16-bit");
    Opc = Wony::MOVTOSP;
  }

  BuildMI(MBB, MI, MI->getDebugLoc(), get(Opc))
      .addReg(DestReg, RegState::Define | getRenamableRegState(RenamableDest))
      .addReg(SrcReg,
              getKillRegState(KillSrc) | getRenamableRegState(RenamableSrc));
}
