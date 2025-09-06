//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the RegisterBankInfo class for Wony
//===----------------------------------------------------------------------===//

#include "WonyRegisterBankInfo.hpp"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "wony-reg-bank-info"

#define GET_TARGET_REGBANK_IMPL
#include "WonyGenRegisterBank.inc"

using namespace llvm;

WonyRegisterBankInfo::WonyRegisterBankInfo(const TargetRegisterInfo &TRI)
    : WonyGenRegisterBankInfo() {}
