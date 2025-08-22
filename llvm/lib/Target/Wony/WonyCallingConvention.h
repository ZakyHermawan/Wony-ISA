//===----------------------------------------------------------------------===//
//
// This file declares the entry points for Wony calling convention analysis.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/CallingConvLower.h"

namespace llvm {

bool CC_Wony_Common(unsigned ValNo, MVT ValVT, MVT LocVT,
                     CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                     CCState &State);
bool RetCC_Wony_Common(unsigned ValNo, MVT ValVT, MVT LocVT,
                        CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                        CCState &State);

} // namespace llvm
