#include "WonyMachineScheduler.h"
#include "MCTargetDesc/WonyMCTargetDesc.h"

using namespace llvm;

bool WonyPreRASchedStrategy::tryCandidate(SchedCandidate &Cand,
                                           SchedCandidate &TryCand,
                                           SchedBoundary *Zone) const {
  bool BetterCand = GenericScheduler::tryCandidate(Cand, TryCand, Zone);
  // Try our specific heuristic only when TryCand isn't selected or
  // selected as node order.
  if (BetterCand && TryCand.Reason != NodeOrder && TryCand.Reason != NoCand)
    return true;

  // If we are in the same scheduling region use the fact that one of the
  // candidate is a widening mul to prioritize it.
  if (Zone != nullptr) {

    if (TryCand.SU->getInstr()->mayLoad()) {
      TryCand.Reason = Stall;
      return true;
    }
    unsigned Opc = TryCand.SU->getInstr()->getOpcode();
    if (Opc == Wony::WIDENING_SMUL) {
      TryCand.Reason = Stall;
      return true;
    }
  }

  return TryCand.Reason != NoCand;
}
