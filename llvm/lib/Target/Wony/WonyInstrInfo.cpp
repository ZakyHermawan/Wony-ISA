//===----------------------------------------------------------------------===//
//
// This file contains the Wony implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "WonyInstrInfo.h"
#include "Wony.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <iterator>

#define GET_INSTRINFO_CTOR_DTOR
#include "WonyGenInstrInfo.inc"

using namespace llvm;

WonyInstrInfo::WonyInstrInfo() : WonyGenInstrInfo() {}
