#include "WonyTargetMachine.h"
#include "TargetInfo/WonyTargetInfo.h"
#include "llvm/MC/TargetRegistry.h" // For RegisterTargetMachine.
#include "llvm/Support/Compiler.h" // For LLVM_EXTERNAL_VISIBILITY.
#include "llvm/Support/CodeGen.h"  // For CodeGenOptLevel.

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeWonyTarget() {
  RegisterTargetMachine<WonyTargetMachine> X(getTheWonyTarget());
}

static const char *WonyDataLayoutStr =
    "e-p:16:16:16-n16:32-i32:32:32-i16:16:16-i1:8:8-f32:32:32-v32:32:32";

WonyTargetMachine::WonyTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, WonyDataLayoutStr, TT, CPU, FS, Options,
                               // Use the simplest relocation by default.
                               RM ? *RM : Reloc::Static,
                               CM ? *CM : CodeModel::Small, OL) {}

WonyTargetMachine::~WonyTargetMachine() = default;
