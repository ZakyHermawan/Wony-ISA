//===----------------------------------------------------------------------===//
///
/// \file
/// This file describes how to lower LLVM calls to machine code calls.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/ADT/ArrayRef.h"
#include "llvm/CodeGen/GlobalISel/CallLowering.h"
#include "llvm/IR/CallingConv.h"
#include <cstdint>

namespace llvm {

class WonyTargetLowering;

class WonyCallLowering: public CallLowering {
public:
  WonyCallLowering(const WonyTargetLowering &TLI);

  /// Lower outgoing return values, described / by \p Val, into the specified
  /// virtual registers \p VRegs.
  ///
  /// \p FLI is required for sret demotion.
  ///
  /// \return True if the lowering succeeds, false otherwise.
  bool lowerReturn(MachineIRBuilder &MIRBuilder, const Value *Val,
                   ArrayRef<Register> VRegs,
                   FunctionLoweringInfo &FLI) const override;

  /// Check whether the return values described by \p Outs can fit into the
  /// return registers.
  /// If false is returned, an sret-demotion is performed.
  bool canLowerReturn(MachineFunction &MF, CallingConv::ID CallConv,
                      SmallVectorImpl<BaseArgInfo> &Outs,
                      bool IsVarArg) const override;

  /// Lowers the incoming (formal) arguments, described by \p VRegs.
  /// Each argument must end up in the related virtual registers described
  /// by \p VRegs.
  /// In other words, the first argument should end up in \c VRegs[0],
  /// the second in \c VRegs[1], and so on. For each argument, there will be one
  /// register for each non-aggregate type, as returned by \c computeValueLLTs.
  /// \p MIRBuilder is set to the proper insertion for the argument
  /// lowering. \p FLI is required for sret demotion.
  ///
  /// \return True if the lowering succeeded, false otherwise.
  bool lowerFormalArguments(MachineIRBuilder &MIRBuilder, const Function &F,
                            ArrayRef<ArrayRef<Register>> VRegs,
                            FunctionLoweringInfo &FLI) const override;

  /// Lower the given call instruction, including argument and return value
  /// marshalling.
  ///
  /// \return true if the lowering succeeded, false otherwise.
  bool lowerCall(MachineIRBuilder &MIRBuilder,
                 CallLoweringInfo &Info) const override;
};


} // end namespace llvm
