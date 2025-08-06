//===----------------------------------------------------------------------===//
//
// Holds the MC (Machine Code) target descriptions of the backend.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/Compiler.h" // For LLVM_EXTERNAL_VISIBILITY.

/**
 * This function registers the machine code (MC)
 * components of this backend in the instance of the Target class of this backend. The MC
 * components represent a low-level description of a target, such as the number of registers it
 * has or how to produce object files
 */
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeWonyTargetMC() {}
