//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the Machinelegalizer class for Wony
//===----------------------------------------------------------------------===//

#include "WonyLegalizerInfo.hpp"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/CodeGenTypes/LowLevelType.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "Wony-legalinfo"

using namespace llvm;
using namespace LegalizeActions;

WonyLegalizerInfo::WonyLegalizerInfo(const WonySubtarget &ST) {
  const LLT p0 = LLT::pointer(0, 16);
  const LLT s8 = LLT::scalar(8);
  const LLT s16 = LLT::scalar(16);
  const LLT s32 = LLT::scalar(32);

  // Constants
  getActionDefinitionsBuilder(
      {TargetOpcode::G_CONSTANT, TargetOpcode::G_IMPLICIT_DEF})
      .legalFor({p0, s16, s32})
      .widenScalarToNextPow2(0)
      .clampScalar(0, s16, s32);

  // Load and store.
  getActionDefinitionsBuilder({TargetOpcode::G_LOAD, TargetOpcode::G_STORE})
      .legalForTypesWithMemDesc({{s8, p0, s8, 8},
                                 {s16, p0, s8, 8}, // anyext/truncstore
                                 {s16, p0, s16, 8},
                                 {s32, p0, s32, 8}})
      .clampScalar(0, s16, s32);

  // Pointer-handling.
  getActionDefinitionsBuilder(TargetOpcode::G_FRAME_INDEX).legalFor({p0});
  getActionDefinitionsBuilder(TargetOpcode::G_PTR_ADD).legalFor({{p0, s16}});

  // Arithmetic.
  getActionDefinitionsBuilder(TargetOpcode::G_ADD).legalFor({s16});

  getLegacyLegalizerInfo().computeTables();
}
