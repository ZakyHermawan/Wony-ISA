//==- WonyTargetTransformInfo.cpp - Wony specific TTI pass -*- C++ -*-==//
//
/// \file
/// This file implements a TargetTransformInfo analysis pass specific to the
/// Wony target machine. It uses the target's detailed information to provide
/// more precise answers to certain TTI queries, while letting the target
/// independent and default TTI implementations handle the rest.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "WonySubtarget.h"
#include "WonyTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

namespace llvm {

class WonyTTIImpl : public BasicTTIImplBase<WonyTTIImpl> {
  using BaseT = BasicTTIImplBase<WonyTTIImpl>;
  using TTI = TargetTransformInfo;
  
  friend BaseT;

  // Supply the minimum required APIs.
  const WonySubtarget &ST;
  const WonyTargetLowering &TLI;

  const WonySubtarget *getSt() const { return &ST; }
  const WonyTargetLowering *getTLI() const { return &TLI; }

public:
  explicit WonyTTIImpl(const WonyTargetMachine *TM, const Function &F)
    : BaseT(TM, F.getDataLayout()),
      ST(*TM->getSubtargetImpl(F)),
      TLI(*ST.getTargetLowering()) {}
  
  unsigned getLoadVectorFactor(
    unsigned VF, unsigned LoadSize,
    unsigned ChainSizeInBytes,
    VectorType *VecTy) const;

  InstructionCost getIntrinsicInstrCost(const IntrinsicCostAttributes &ICA,
                                        TTI::TargetCostKind CostKind);
};

} // end namespace llvm
