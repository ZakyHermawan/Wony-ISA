//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the RegisterBankInfo class for Wony.
//===----------------------------------------------------------------------===//

#pragma once

#include "MCTargetDesc/WonyMCTargetDesc.h"
#include "llvm/CodeGen/RegisterBankInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGenTypes/LowLevelType.h"

#define GET_REGBANK_DECLARATIONS
#include "WonyGenRegisterBank.inc"

namespace llvm {

class TargetRegisterInfo;

class WonyGenRegisterBankInfo : public RegisterBankInfo {
protected:
#define GET_TARGET_REGBANK_CLASS
#include "WonyGenRegisterBank.inc"
};

class WonyRegisterBankInfo final : public WonyGenRegisterBankInfo {
public:
  WonyRegisterBankInfo(const TargetRegisterInfo &TRI);
  const InstructionMapping &
  getInstrMapping(const MachineInstr &MI) const override;

  const RegisterBank &getRegBankFromRegClass(const TargetRegisterClass &RC,
                                             LLT Ty) const override;
};

} // end namespace llvm
