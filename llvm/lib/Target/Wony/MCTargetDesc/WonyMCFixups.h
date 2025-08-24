
//=======-- WonyMCFixups.h - Wony-specific fixup entries ------*- C++ -*-===//
//
// A fix-up is an indication left by the assembler to the linker that tells
// the linker that it needs to patch the final binary with something.
// In this specific case, the fix-up would tell the liker to replace the 11-bit
// of the immediate value with the actual address (PC-relative) of the related
// symbol.
// The fix-up is currently not emitted because we didn't connect a target
// specific MCAsmBackend yet.
//
//===----------------------------------------------------------------------===//



#pragma once
#include "llvm/MC/MCFixup.h"

namespace llvm {

namespace Wony {

enum FixupKind {
  // Wony specific relocations.
  FK_Wony_PCRel_11 = FirstTargetFixupKind,

  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};

} // end namespace Wony

} // end namespace llvm
