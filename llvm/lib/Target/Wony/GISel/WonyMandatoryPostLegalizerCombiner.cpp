//===----------------------------------------------------------------------===//
///
/// \file
/// Post-legalization lowering for instructions.
///
/// This is used to offload pattern matching from the selector.
///
/// For example, this combiner will notice that a G_UNMERGE is actually
/// a G_EXTRACT_VECTOR_ELT, etc.
///
//===----------------------------------------------------------------------===//

#include "Wony.h"
#include "WonySubtarget.h"
#include "WonyTargetMachine.h"
#include "MCTargetDesc/WonyMCTargetDesc.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelChangeObserver.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/CodeGen/GlobalISel/MIPatternMatch.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include <optional>

#define GET_GICOMBINER_DEPS
#include "WonyGenMandatoryPostLegalizeGICombiner.inc"
#undef GET_GICOMBINER_DEPS

#define DEBUG_TYPE "wony-mandatory-postlegalizer-combiner"

using namespace llvm;
using namespace MIPatternMatch;

namespace {

#define GET_GICOMBINER_TYPES
#include "WonyGenMandatoryPostLegalizeGICombiner.inc"
#undef GET_GICOMBINER_TYPES

bool matchScalarizeVectorUnmerge(MachineInstr &MI, MachineRegisterInfo &MRI) {
  auto &Unmerge = cast<GUnmerge>(MI);
  Register Src1Reg = Unmerge.getReg(Unmerge.getNumOperands() - 1);
  const LLT SrcTy = MRI.getType(Src1Reg);
  if (SrcTy.getSizeInBits() != 32) {
    return false;
  }

  return SrcTy.isVector() && !SrcTy.isScalable() &&
         Unmerge.getNumOperands() == (unsigned)SrcTy.getNumElements() + 1;
}

void applyScalarizeVectorUnmerge(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 MachineIRBuilder &B) {
  auto &Unmerge = cast<GUnmerge>(MI);
  Register Src1Reg = Unmerge.getReg(Unmerge.getNumOperands() - 1);
  const LLT SrcTy = MRI.getType(Src1Reg);
  assert((SrcTy.isVector() && !SrcTy.isScalable()) &&
         "Expected a fixed length vector");

  for (int I = 0; I < SrcTy.getNumElements(); ++I) {
    B.buildExtractVectorElementConstant(Unmerge.getReg(I), Src1Reg, I);
  }
  MI.eraseFromParent();
}

class WonyMandatoryPostLegalizerCombinerImpl : public Combiner {
protected:
  mutable CombinerHelper Helper;
  const WonyMandatoryPostLegalizerCombinerImplRuleConfig &RuleConfig;
  const WonySubtarget &STI;

public:
  WonyMandatoryPostLegalizerCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelCSEInfo *CSEInfo,
      const WonyMandatoryPostLegalizerCombinerImplRuleConfig &RuleConfig,
      const WonySubtarget &STI);

  static const char *getName() { return "WonyMandatoryPostLegalizerCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "WonyGenMandatoryPostLegalizeGICombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "WonyGenMandatoryPostLegalizeGICombiner.inc"
#undef GET_GICOMBINER_IMPL

WonyMandatoryPostLegalizerCombinerImpl::
    WonyMandatoryPostLegalizerCombinerImpl(
        MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
        GISelCSEInfo *CSEInfo,
        const WonyMandatoryPostLegalizerCombinerImplRuleConfig &RuleConfig,
        const WonySubtarget &STI)
    : Combiner(MF, CInfo, TPC, /*KB*/ nullptr, CSEInfo),
      Helper(Observer, B, /*IsPreLegalize*/ false), RuleConfig(RuleConfig),
      STI(STI),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "WonyGenMandatoryPostLegalizeGICombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}

class WonyMandatoryPostLegalizerCombiner : public MachineFunctionPass {
public:
  static char ID;

  WonyMandatoryPostLegalizerCombiner();

  StringRef getPassName() const override {
    return "WonyMandatoryPostLegalizerCombiner";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;
  void getAnalysisUsage(AnalysisUsage &AU) const override;

private:
  WonyMandatoryPostLegalizerCombinerImplRuleConfig RuleConfig;
};
} // end anonymous namespace

void WonyMandatoryPostLegalizerCombiner::getAnalysisUsage(
    AnalysisUsage &AU) const {
  AU.addRequired<TargetPassConfig>();
  AU.setPreservesCFG();
  getSelectionDAGFallbackAnalysisUsage(AU);
  MachineFunctionPass::getAnalysisUsage(AU);
}

WonyMandatoryPostLegalizerCombiner::WonyMandatoryPostLegalizerCombiner()
    : MachineFunctionPass(ID) {
  if (!RuleConfig.parseCommandLineOption())
    report_fatal_error("Invalid rule identifier");
}

bool WonyMandatoryPostLegalizerCombiner::runOnMachineFunction(
    MachineFunction &MF) {
  if (MF.getProperties().hasProperty(
          MachineFunctionProperties::Property::FailedISel)) {
    return false;
  }
  assert(MF.getProperties().hasProperty(
             MachineFunctionProperties::Property::Legalized) &&
         "Expected a legalized function?");
  auto *TPC = &getAnalysis<TargetPassConfig>();
  const Function &F = MF.getFunction();

  const WonySubtarget &ST = MF.getSubtarget<WonySubtarget>();
  CombinerInfo CInfo(/*AllowIllegalOps*/ false, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, /*OptEnabled=*/true,
                     F.hasOptSize(), F.hasMinSize());
  WonyMandatoryPostLegalizerCombinerImpl Impl(
      MF, CInfo, TPC, /*CSEInfo*/ nullptr, RuleConfig, ST);
  return Impl.combineMachineInstrs();
}

char WonyMandatoryPostLegalizerCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(WonyMandatoryPostLegalizerCombiner, DEBUG_TYPE,
                      "Lower Wony MachineInstrs after legalization", false,
                      false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_END(WonyMandatoryPostLegalizerCombiner, DEBUG_TYPE,
                    "Lower Wony MachineInstrs after legalization", false,
                    false)

namespace llvm {
Pass *createWonyMandatoryPostLegalizerCombiner() {
  return new WonyMandatoryPostLegalizerCombiner();
}
} // end namespace llvm
