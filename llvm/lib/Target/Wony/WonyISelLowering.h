//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Wony uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class WonySubtarget;
class WonyTargetMachine;

class WonyTargetLowering: public TargetLowering {
public:
  explicit WonyTargetLowering(const TargetMachine& TM);

  /// This method returns a target specific FastISel object, or null if the
  /// target does not support "fast" ISel.
  FastISel *createFastISel(FunctionLoweringInfo &funcInfo,
                           const TargetLibraryInfo *libInfo) const override;

  /// This hook must be implemented to lower the incoming (formal) arguments,
  /// described by the Ins array, into the specified DAG. The implementation
  /// should fill in the InVals array with legal-type argument values, and
  /// return the resulting token chain value.
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

};

} // end namespace llvm
