//===----------------------------------------------------------------------===//
//
/// Provides Wony specific target descriptions.
//
//===----------------------------------------------------------------------===//
//

#pragma once

#include "llvm/MC/MCInstrInfo.h"
#include <cstdint> // For int16_t and so on used in the .inc files.

namespace llvm {

class MCContext;
class MCCodeEmitter;
MCCodeEmitter *createWonyMCCodeEmitter(const MCInstrInfo &MCII,
                                        MCContext &Ctx);

} // end namespace llvm.


// Defines symbolic names for Wony registers.  This defines a mapping from
// register name to register number.

#define GET_REGINFO_ENUM
#include "WonyGenRegisterInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "WonyGenSubtargetInfo.inc"

#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "WonyGenInstrInfo.inc"
