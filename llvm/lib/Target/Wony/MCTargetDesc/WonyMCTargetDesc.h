//===----------------------------------------------------------------------===//
//
/// Provides Wony specific target descriptions.
//
//===----------------------------------------------------------------------===//
//

#pragma once

#include <cstdint> // For int16_t and so on used in the .inc files.

// Defines symbolic names for Wony registers.  This defines a mapping from
// register name to register number.

#define GET_SUBTARGETINFO_ENUM
#include "WonyGenSubtargetInfo.inc"

#define GET_REGINFO_ENUM
#include "WonyGenRegisterInfo.inc"
