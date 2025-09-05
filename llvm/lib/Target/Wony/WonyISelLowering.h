//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Wony uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace WonyISD {

enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  CALL,
  RETURN_GLUE,
};

} // end WonyISD

class WonySubtarget;
class WonyTargetMachine;

class WonyTargetLowering: public TargetLowering {
  const WonySubtarget &Subtarget;

public:
  explicit WonyTargetLowering(const TargetMachine &TM,
                               const WonySubtarget &STI);

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

  /// Check whether the return values described by \p Outs can fit into the
  /// return registers.
  /// If false is returned, an sret-demotion is performed.
  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      LLVMContext &Context, const Type *RetTy) const override;

  // Lower return instruction, return WonyISD::RETURN_GLUE SDNode
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  /// Lower the arguments and resutls of a call (from the caller perspective)
  /// as described by \p CLI.
  /// The resulting values from the callee are fed in \p InVals.
  /// The lowering consists in the classic:
  /// CALLSEQ_START
  /// arg setup (as described by CLI.Outs and CLI.OutVals)
  /// CALL to callee (as described by CLI.Callee)
  /// CALLSEQ_END
  /// grab callee resulting values..
  SDValue LowerCall(CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  /// Perform the last clean-ups after finishing instruction selection.
  void finalizeLowering(MachineFunction &MF) const override;

  // This method returns the name of a target specific DAG node.
  const char *getTargetNodeName(unsigned Opcode) const override;
};

} // end namespace llvm
