//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "WonyInstrInfo.h"
#include "Wony.h"
#include "WonyRegisterInfo.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <iterator>

#define GET_INSTRINFO_CTOR_DTOR
#include "WonyGenInstrInfo.inc"

using namespace llvm;

WonyInstrInfo::WonyInstrInfo() : WonyGenInstrInfo() {}

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
