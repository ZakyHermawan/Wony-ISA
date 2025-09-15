//===----------------------------------------------------------------------===//
//
// Custom Wony MI scheduler.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/MachineScheduler.h"

namespace llvm {

/// A MachineSchedStrategy implementation for Wony pre RA scheduling.
class WonyPreRASchedStrategy : public GenericScheduler {
public:
  WonyPreRASchedStrategy(const MachineSchedContext *C) : GenericScheduler(C) {}

protected:
  bool tryCandidate(SchedCandidate &Cand, SchedCandidate &TryCand,
                    SchedBoundary *Zone) const override;
};

} // end namespace llvm
