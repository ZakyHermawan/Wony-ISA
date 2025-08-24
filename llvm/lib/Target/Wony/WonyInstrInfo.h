//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "MCTargetDesc/WonyMCTargetDesc.h" // For all the opcodes' enum.
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "WonyGenInstrInfo.inc"

namespace llvm {

class WonyInstrInfo : public WonyGenInstrInfo {
public:
  WonyInstrInfo();


  /// Callback to materialize a register-to-regiter copy before \p MI in
  /// \p MBB. The copy to materialize is DestReg = COPY SrcReg. The opcode
  /// of the COPY needs to be the actual target-specific opcode.
  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc, bool RenamableDest,
                   bool RenamableSrc) const override;
};

} // namespace llvm
