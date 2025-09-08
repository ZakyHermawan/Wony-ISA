//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the InstructionSelector class for
/// Wony.
//===----------------------------------------------------------------------===//

#include "Wony.h"
#include "WonyInstrInfo.h"
#include "WonyRegisterBankInfo.h"
#include "WonySubtarget.h"
#include "WonyTargetMachine.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/IR/IntrinsicsWony.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "wony-gisel"

using namespace llvm;

namespace {

#define GET_GLOBALISEL_PREDICATE_BITSET
#include "WonyGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATE_BITSET

class WonyInstructionSelector : public InstructionSelector {
public:
  WonyInstructionSelector(const WonyTargetMachine &TM,
                           const WonySubtarget &STI,
                           const WonyRegisterBankInfo &RBI);

  bool select(MachineInstr &I) override;
  static const char *getName() { return DEBUG_TYPE; }

private:
  /// tblgen generated 'select' implementation that is used as the initial
  /// selector for the patterns that do not require complex C++.
  bool selectImpl(MachineInstr &I, CodeGenCoverage &CoverageInfo) const;

  const WonyInstrInfo &TII;
  const WonyRegisterInfo &TRI;
  const WonyRegisterBankInfo &RBI;

#define GET_GLOBALISEL_PREDICATES_DECL
#include "WonyGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_DECL

#define GET_GLOBALISEL_TEMPORARIES_DECL
#include "WonyGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_DECL
};

} // namespace

#define GET_GLOBALISEL_IMPL
#include "WonyGenGlobalISel.inc"
#undef GET_GLOBALISEL_IMPL

WonyInstructionSelector::WonyInstructionSelector(
    const WonyTargetMachine &TM, const WonySubtarget &STI,
    const WonyRegisterBankInfo &RBI)
    : TII(*STI.getInstrInfo()), TRI(*STI.getRegisterInfo()), RBI(RBI),
#define GET_GLOBALISEL_PREDICATES_INIT
#include "WonyGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_INIT
#define GET_GLOBALISEL_TEMPORARIES_INIT
#include "WonyGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_INIT
{
}

bool WonyInstructionSelector::select(MachineInstr &I) {
  unsigned Opc = I.getOpcode();
  if (!isPreISelGenericOpcode(Opc) && Opc != TargetOpcode::PHI &&
      Opc != TargetOpcode::COPY) {
    return true;
  }

  MachineBasicBlock &MBB = *I.getParent();
  MachineFunction &MF = *MBB.getParent();
  MachineRegisterInfo &MRI = MF.getRegInfo();

  switch (Opc) {
  case TargetOpcode::G_PHI:
    I.setDesc(TII.get(TargetOpcode::PHI));
    [[fallthrough]];
  case TargetOpcode::PHI:
  case TargetOpcode::COPY:
    // For PHIs and COPYs, we only need to assigned a register class.
    for (MachineOperand &MO : I.operands()) {
      Register Reg = MO.getReg();
      if (Reg.isPhysical())
        continue;
      const TargetRegisterClass *RC = MRI.getRegClassOrNull(Reg);
      if (RC)
        continue;
      unsigned Size = MRI.getType(Reg).getSizeInBits();
      MRI.setRegClass(Reg, Size == 16 ? &Wony::GPR16spRegClass
                                      : &Wony::GPR32RegClass);
    }
    return true;
  default:
    if (selectImpl(I, *CoverageInfo)) {
      return true;
    }
  }

  return false;
}

namespace llvm {

InstructionSelector *
Wony::createInstructionSelector(const WonyTargetMachine &TM,
                                 const WonySubtarget &Subtarget,
                                 const WonyRegisterBankInfo &RBI) {
  return new WonyInstructionSelector(TM, Subtarget, RBI);
}

} // namespace llvm
