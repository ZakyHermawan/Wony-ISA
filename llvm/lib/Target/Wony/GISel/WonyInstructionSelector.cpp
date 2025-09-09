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
#include "llvm/CodeGen/GlobalISel/MIPatternMatch.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/IR/IntrinsicsWony.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "wony-gisel"

using namespace llvm;
using namespace MIPatternMatch;

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

  ComplexRendererFns selectAddrMode(MachineOperand &Root) const;
  ComplexRendererFns selectSPAddrMode(MachineOperand &Root) const;

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

static void setRegisterClassForOperands(MachineInstr &I,
                                        MachineRegisterInfo &MRI) {
  for (MachineOperand &MO : I.operands()) {
    Register Reg = MO.getReg();
    if (Reg.isPhysical()) {
      continue;
    }
    const TargetRegisterClass *RC = MRI.getRegClassOrNull(Reg);
    if (RC) {
      continue;
    }
    unsigned Size = MRI.getType(Reg).getSizeInBits();
    MRI.setRegClass(Reg, Size == 16 ? &Wony::GPR16RegClass
                                    : &Wony::GPR32RegClass);
  }
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
  case TargetOpcode::G_IMPLICIT_DEF:
    I.setDesc(TII.get(TargetOpcode::IMPLICIT_DEF));
    setRegisterClassForOperands(I, MRI);
    return true;
  case TargetOpcode::G_PHI:
    I.setDesc(TII.get(TargetOpcode::PHI));
    [[fallthrough]];
  case TargetOpcode::PHI:
  case TargetOpcode::COPY:
    // For PHIs and COPYs, we only need to assigned a register class.
    setRegisterClassForOperands(I, MRI);
    return true;
  case TargetOpcode::G_FRAME_INDEX: {
    // Frame index are ultimately SP + something.
    // We'll materialize the ADD when we know the offset at frame lowering time.
    // This may require an emergency spill slot.
    I.setDesc(TII.get(Wony::MOVFROMSP));
    return constrainSelectedInstRegOperands(I, TII, TRI, RBI);
  }
  default:
    if (selectImpl(I, *CoverageInfo)) {
      return true;
    }
  }

  return false;
}

InstructionSelector::ComplexRendererFns
WonyInstructionSelector::selectAddrMode(MachineOperand &Root) const {
  if (!Root.isReg()) {
    return std::nullopt;
  }

  MachineRegisterInfo &MRI =
      Root.getParent()->getParent()->getParent()->getRegInfo();

  // Loads and stores through the stack need to go through the SP-based
  // addressing mode.
  MachineInstr *RootDef = MRI.getVRegDef(Root.getReg());
  if (RootDef->getOpcode() == TargetOpcode::G_FRAME_INDEX) {
    return std::nullopt;
  }

  Register BaseReg = RootDef->getOperand(0).getReg();
  uint64_t Offset = 0;
  // Do some matching of ADD + immediate and fold if it fits.
  if (RootDef->getOpcode() == TargetOpcode::G_ADD ||
      RootDef->getOpcode() == TargetOpcode::G_PTR_ADD) {
    std::optional<ValueAndVReg> MaybeConstantInt;
    if (mi_match(RootDef->getOperand(2).getReg(), MRI,
                 m_GCst(MaybeConstantInt))) {
      uint64_t CstImm = MaybeConstantInt->Value.getZExtValue();
      if (CstImm < 16) {
        BaseReg = RootDef->getOperand(1).getReg();
        Offset = CstImm;
      }
    }
  }

  return {{
      [=](MachineInstrBuilder &MIB) { MIB.addReg(BaseReg); },
      [=](MachineInstrBuilder &MIB) { MIB.addImm(Offset); },
  }};
}

InstructionSelector::ComplexRendererFns
WonyInstructionSelector::selectSPAddrMode(MachineOperand &Root) const {
  if (!Root.isReg())
    return std::nullopt;

  MachineRegisterInfo &MRI =
      Root.getParent()->getParent()->getParent()->getRegInfo();

  MachineInstr *RootDef = MRI.getVRegDef(Root.getReg());
  if (RootDef->getOpcode() == TargetOpcode::G_FRAME_INDEX)
    return {{
        [=](MachineInstrBuilder &MIB) {
          MIB.addFrameIndex(RootDef->getOperand(1).getIndex());
        },
        [=](MachineInstrBuilder &MIB) { MIB.addImm(0); },
    }};

  auto MaybeSPReg = [](MachineInstr &MI) -> bool {
    if (MI.getOpcode() == TargetOpcode::COPY) {
      Register Reg = MI.getOperand(1).getReg();
      return Reg == Wony::SP;
    }
    return false;
  };

  auto MaybeFrameIndex = [](MachineInstr &MI) -> std::optional<int> {
    if (MI.getOpcode() == TargetOpcode::G_FRAME_INDEX)
      return MI.getOperand(1).getIndex();
    return std::nullopt;
  };

  auto MatchSPRegOrIndex = [&MaybeFrameIndex,
                            &MaybeSPReg](MachineInstr &MI,
                                         std::optional<int> &Index) -> bool {
    std::optional<int> MaybeIndex = MaybeFrameIndex(MI);
    if (MaybeIndex) {
      Index = *MaybeIndex;
      return true;
    }
    return MaybeSPReg(MI);
  };

  // On a match, if MaybeIndex is set we are dealing with a FrameIndex,
  // otherwise we are dealing with SP.
  std::optional<int> MaybeIndex;
  bool Matched = false;
  int64_t CstImm = 0;

  switch (RootDef->getOpcode()) {
  case TargetOpcode::COPY:
  case TargetOpcode::G_FRAME_INDEX:
    Matched = MatchSPRegOrIndex(*RootDef, MaybeIndex);
    break;
  case TargetOpcode::G_PTR_ADD: {
    // Do some matching of ADD + immediate and fold if it fits.
    std::optional<ValueAndVReg> MaybeConstantInt;
    if (!mi_match(RootDef->getOperand(2).getReg(), MRI,
                  m_GCst(MaybeConstantInt)))
      return std::nullopt;

    CstImm = MaybeConstantInt->Value.getZExtValue();
    if (CstImm < -64 || CstImm >= 64)
      return std::nullopt;

    MachineInstr *BaseDef = MRI.getVRegDef(RootDef->getOperand(1).getReg());
    Matched = MatchSPRegOrIndex(*BaseDef, MaybeIndex);
    break;
  }
  default:
    Matched = false;
    break;
  }

  if (!Matched)
    return std::nullopt;

  return {{
      [=](MachineInstrBuilder &MIB) {
        if (MaybeIndex)
          MIB.addFrameIndex(*MaybeIndex);
        else
          MIB.addReg(Wony::SP);
      },
      [=](MachineInstrBuilder &MIB) { MIB.addImm(CstImm); },
  }};
}

namespace llvm {

InstructionSelector *
Wony::createInstructionSelector(const WonyTargetMachine &TM,
                                 const WonySubtarget &Subtarget,
                                 const WonyRegisterBankInfo &RBI) {
  return new WonyInstructionSelector(TM, Subtarget, RBI);
}

} // namespace llvm
