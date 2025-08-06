//===- WonyTargetTransformInfo.cpp - Wony specific TTI pass ---------===//
//
/// \file
/// This file implements a TargetTransformInfo analysis pass specific to the
/// Wony target machine. It uses the target's detailed information to provide
/// more precise answers to certain TTI queries, while letting the target
/// independent and default TTI implementations handle the rest.
///
//===----------------------------------------------------------------------===//

#include "WonyTargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/IntrinsicsWony.h"

using namespace llvm;

#define DEBUG_TYPE "wonytti"

unsigned WonyTTIImpl::getLoadVectorFactor(unsigned VF, unsigned LoadSize,
                                           unsigned ChainSizeInBytes,
                                           VectorType *VecTy) const {
  // We support <2 x i16> loads.
  unsigned ElemSize = VecTy->getScalarSizeInBits();
  if (ElemSize != 16)
    return 0;

  return std::min(VF, 2u);
}

InstructionCost
WonyTTIImpl::getIntrinsicInstrCost(const IntrinsicCostAttributes &ICA,
                                    TTI::TargetCostKind CostKind) {
  // Extending the input values of a widening multiply is more expensive than a
  // regular instruction.
  // For code size, though, this is the same.
  if (CostKind != TargetTransformInfo::TCK_CodeSize &&
      ICA.getID() == Intrinsic::wony_widening_smul)
    return TargetTransformInfo::TCC_Expensive;

  return BaseT::getIntrinsicInstrCost(ICA, CostKind);
}
