//===----------------------------------------------------------------------===//
//
// The implementation of the Target information for this backend.
//
//===----------------------------------------------------------------------===//


#include "WonyTargetInfo.h"
#include "llvm/MC/TargetRegistry.h" // For RegisterTarget.
#include "llvm/Support/Compiler.h"  // For LLVM_EXTERNAL_VISIBILITY.
#include "llvm/TextAPI/Target.h"    // For Target class.

using namespace llvm;

Target &llvm::getTheWonyTarget() {
  static Target TheWonyTarget;
  return TheWonyTarget;
}

// Populate the Target instance with the constructors of the related components
// This function registers the instance of the Target class, from the MC library, for this backend
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeWonyTargetInfo() {
  RegisterTarget<Triple::wony, /*HasJIT=*/false> X(
      getTheWonyTarget(), /*Name=*/"wony",
      /*Desc=*/"Wonyoung Instruction Set Architecture",
      /*BackendName=*/"Wony");
}
