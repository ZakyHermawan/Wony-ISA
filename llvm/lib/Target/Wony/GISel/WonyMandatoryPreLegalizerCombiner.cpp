//===----------------------------------------------------------------------===//
//
// This pass does combining of machine instructions at the generic MI level,
// before the legalizer.
//
//===----------------------------------------------------------------------===//

#include "Wony.h"
#include "WonyTargetMachine.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelKnownBits.h"
#include "llvm/CodeGen/GlobalISel/MIPatternMatch.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Debug.h"

#define GET_GICOMBINER_DEPS
#include "WonyGenMandatoryPreLegalizeGICombiner.inc"
#undef GET_GICOMBINER_DEPS

#define DEBUG_TYPE "wony-mandatory-prelegalizer-combiner"

using namespace llvm;
using namespace MIPatternMatch;
namespace {

#define GET_GICOMBINER_TYPES
#include "WonyGenMandatoryPreLegalizeGICombiner.inc"
#undef GET_GICOMBINER_TYPES

bool matchInsertVectorElt(MachineInstr &MI,
                          SmallVectorImpl<Register> &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_INSERT_VECTOR_ELT);
  // Try to find each element of the vector to produce a BUILD_VECTOR.
  Register DstReg = MI.getOperand(0).getReg();
  const MachineRegisterInfo &MRI = MI.getParent()->getParent()->getRegInfo();
  uint16_t NumElts = MRI.getType(DstReg).getNumElements();
  MatchInfo.resize(NumElts);
  BitVector IndicesFound(NumElts);
  MachineInstr *CurMI = &MI;
  do {
    switch (CurMI->getOpcode()) {
    case TargetOpcode::G_INSERT_VECTOR_ELT: {
      std::optional<ValueAndVReg> MaybeVectorLane;
      if (!mi_match(CurMI->getOperand(3).getReg(), MRI,
                    m_GCst(MaybeVectorLane)))
        return false;
      uint64_t VectorLaneIdx = MaybeVectorLane->Value.getZExtValue();
      // If we already found the contain of this lane, move on.
      // We're following the use-def chain so by construction the previous
      // definition that set this lane is the right one.
      if (!IndicesFound.test(VectorLaneIdx)) {
        IndicesFound.set(VectorLaneIdx);
        MatchInfo[VectorLaneIdx] = CurMI->getOperand(2).getReg();
      }
      CurMI = MRI.getUniqueVRegDef(CurMI->getOperand(1).getReg());
      break;
    }
    case TargetOpcode::G_BUILD_VECTOR: {
      for (const auto &[Idx, MO] : enumerate(CurMI->operands())) {
        // Skip the definition
        if (Idx == 0)
          continue;
        // Adjust Idx to match the vector lane index.
        --Idx;
        if (!IndicesFound.test(Idx)) {
          MatchInfo[Idx] = MO.getReg();
          IndicesFound.set(Idx);
        }
      }
      CurMI = nullptr;
      break;
    }
    case TargetOpcode::IMPLICIT_DEF:
    case TargetOpcode::G_IMPLICIT_DEF: {
      BitVector MissingIndices = IndicesFound.flip();
      for (unsigned Idx : MissingIndices.set_bits()) {
        MatchInfo[Idx] = Wony::NoRegister;
      }
      IndicesFound = BitVector(NumElts, true);
    }
      [[fallthrough]];
    default:
      CurMI = nullptr;
      break;
    }
  } while (CurMI != nullptr && !IndicesFound.all());
  return IndicesFound.all();
}

void applyInsertVectorElt(MachineInstr &MI,
                          SmallVectorImpl<Register> &MatchInfo) {
  assert(MI.getOpcode() == TargetOpcode::G_INSERT_VECTOR_ELT);
  MachineIRBuilder MIB(MI);
  Register UndefReg;
  Register DstReg = MI.getOperand(0).getReg();
  for (Register &Reg : MatchInfo) {
    if (Reg == Wony::NoRegister) {
      if (UndefReg == Wony::NoRegister) {
        LLT EltTy = MIB.getMRI()->getType(DstReg).getElementType();
        UndefReg = MIB.buildUndef(EltTy).getReg(0);
      }
      Reg = UndefReg;
    }
  }

  MIB.buildBuildVector(DstReg, MatchInfo);
  MI.eraseFromParent();
}

class WonyMandatoryPreLegalizerCombinerImpl : public Combiner {
protected:
  mutable CombinerHelper Helper;
  const WonyMandatoryPreLegalizerCombinerImplRuleConfig &RuleConfig;
  const WonySubtarget &STI;

public:
  WonyMandatoryPreLegalizerCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
      const WonyMandatoryPreLegalizerCombinerImplRuleConfig &RuleConfig,
      const WonySubtarget &STI);

  static const char *getName() { return "WonyMandatoryPreLegalizerCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "WonyGenMandatoryPreLegalizeGICombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "WonyGenMandatoryPreLegalizeGICombiner.inc"
#undef GET_GICOMBINER_IMPL

WonyMandatoryPreLegalizerCombinerImpl::WonyMandatoryPreLegalizerCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
    const WonyMandatoryPreLegalizerCombinerImplRuleConfig &RuleConfig,
    const WonySubtarget &STI)
    : Combiner(MF, CInfo, TPC, &KB, CSEInfo),
      Helper(Observer, B, /*IsPreLegalize*/ true, &KB), RuleConfig(RuleConfig),
      STI(STI),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "WonyGenMandatoryPreLegalizeGICombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}

// Pass boilerplate
// ================

class WonyMandatoryPreLegalizerCombiner : public MachineFunctionPass {
public:
  static char ID;

  WonyMandatoryPreLegalizerCombiner();

  StringRef getPassName() const override {
    return "WonyMandatoryPreLegalizerCombiner";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override;

private:
  WonyMandatoryPreLegalizerCombinerImplRuleConfig RuleConfig;
};

} // end anonymous namespace

void WonyMandatoryPreLegalizerCombiner::getAnalysisUsage(
    AnalysisUsage &AU) const {
  AU.addRequired<TargetPassConfig>();
  AU.setPreservesCFG();
  getSelectionDAGFallbackAnalysisUsage(AU);
  AU.addRequired<GISelKnownBitsAnalysis>();
  AU.addPreserved<GISelKnownBitsAnalysis>();
  MachineFunctionPass::getAnalysisUsage(AU);
}

WonyMandatoryPreLegalizerCombiner::WonyMandatoryPreLegalizerCombiner()
    : MachineFunctionPass(ID) {
  initializeWonyMandatoryPreLegalizerCombinerPass(
      *PassRegistry::getPassRegistry());

  if (!RuleConfig.parseCommandLineOption())
    report_fatal_error("Invalid rule identifier");
}

bool WonyMandatoryPreLegalizerCombiner::runOnMachineFunction(
    MachineFunction &MF) {
  if (MF.getProperties().hasProperty(
          MachineFunctionProperties::Property::FailedISel))
    return false;
  auto &TPC = getAnalysis<TargetPassConfig>();

  const Function &F = MF.getFunction();
  GISelKnownBits *KB = &getAnalysis<GISelKnownBitsAnalysis>().get(MF);

  const WonySubtarget &ST = MF.getSubtarget<WonySubtarget>();

  CombinerInfo CInfo(/*AllowIllegalOps*/ true, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, /*EnableOpt*/ false,
                     F.hasOptSize(), F.hasMinSize());
  // Disable fixed-point iteration in the Combiner. This improves compile-time
  // at the cost of possibly missing optimizations. See PR#94291 for details.
  CInfo.MaxIterations = 1;

  WonyMandatoryPreLegalizerCombinerImpl Impl(MF, CInfo, &TPC, *KB,
                                              /*CSEInfo*/ nullptr, RuleConfig,
                                              ST);
  return Impl.combineMachineInstrs();
}

char WonyMandatoryPreLegalizerCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(WonyMandatoryPreLegalizerCombiner, DEBUG_TYPE,
                      "Combine Wony machine instrs before legalization", false,
                      false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelKnownBitsAnalysis)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(WonyMandatoryPreLegalizerCombiner, DEBUG_TYPE,
                    "Combine Wony machine instrs before legalization", false,
                    false)

namespace llvm {

Pass *createWonyMandatoryPreLegalizerCombiner() {
  return new WonyMandatoryPreLegalizerCombiner();
}

} // end namespace llvm
