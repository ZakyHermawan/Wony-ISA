//===----------------------------------------------------------------------===//
//
// Holds the implementation of the target-specific TargetMachine instance.
//
//===----------------------------------------------------------------------===//


#include "WonyTargetMachine.h"
#include "TargetInfo/WonyTargetInfo.h"
#include "llvm/MC/TargetRegistry.h" // For RegisterTargetMachine.
#include "llvm/Support/Compiler.h" // For LLVM_EXTERNAL_VISIBILITY.
#include "llvm/Support/CodeGen.h"  // For CodeGenOptLevel.

using namespace llvm;

/**
 * This function registers the target-specific TargetMachine
 * instance, from the Target library, in the instance of the Target class of this backend.
 * TargetMachine provides methods to enable and control the various pieces of the LLVM codegen
 * infrastructure
 */
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeWonyTarget() {
  RegisterTargetMachine<WonyTargetMachine> X(getTheWonyTarget());
}

static const char *WonyDataLayoutStr =
    "e-p:16:16:16-n16:32-i32:32:32-i16:16:16-i1:8:8-f32:32:32-v32:32:32";

/**
 * • T is the Target singleton of our backend.
 * • TT is an instance that represents the Triple object for our backend and, based on its value, we
 *   may do something differently for instance based on the target OS, and so on.
 * • CPU is the string that represents the name of the CPU we instantiate our backend for. For
 *   instance, you may instantiate the X86 backend for different generations of CPUs such as
 *   Skylake or Haswell. Different generations of CPUs may have access to different features, so
 *   your TargetMachine may need to be slightly different to accommodate that. For instance, an
 *   old GPU may use 32-bit pointers, whereas a more recent one may use 64-bit pointers.
 * • FS stands for feature string. This is a list of different features that the caller of this constructor
 *   may want to enable (prefixed with +) or disable (prefixed with -). For instance, from the Clang
 *   command line, -Xclang -target-feature -Xclang +sse2,-sse would tell the TargetMachine
 *   to disable the SSE instruction set while enabling the SSE2 instruction set, which are two different
 *   X86 instruction extensions of the base instruction set.
 * • Options sets the default behavior for the target, for things such as how are math instructions
 *   interpreted (remember our discussion about fast math flags in Chapter 4).
 * • RM represents the relocation model for the binary. In a nutshell, this affects how symbols are
 *   accessed. For instance, to call the foo function, we may have the address of foo available directly,
 *   which is the static mode. Alternatively, the foo function may be placed in memory at runtime,
 *   and we need to rely on a dynamically populated relocation table to resolve the address of foo.
 *   You can imagine that, depending on these two different modes, we could produce different
 *   sequences of instructions. In Clang, this is controlled with the -mrelocation-model option.
 * • CM represents the code model that codegen targets. This essentially tells the compiler how
 *   big you expect your final binary to be. Most targets default to the small mode, which means
 *   that the program and its symbols must fit in the lower 2 GB of the final executable. This has
 *   some implications on the kind of address computation you must generate. For instance, in
 *   small mode, it may be fine to use relative addressing from the program counter, whereas in
 *   large mode you must fully compute the address. You can play with this parameter by using
 *   -mcmodel in Clang.
 * • OL represents the optimization level. In Clang, that would be -O0, -O1, and so on.
 * • JIT indicates whether codegen is being run in a just-in-time (JIT) setting. When using JIT, for
 * instance, the target may want to scale down its pass pipeline to have a better compile time.
 */
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

/**
 * collects the CPU and feature string requirements from the instance of the instance of
 * the Function class passed as an argument. Then, it creates an instance of our WonySubtarget class
 * with this Function instance. We save this instance in a mutable unique pointer in our TargetMachine
 * object to avoid creating it for each input Function objects. This method is called for each Function
 * object that is compiled in a module.
 */
const WonySubtarget *
WonyTargetMachine::getSubtargetImpl(const Function& F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  StringRef CPU = CPUAttr.isValid() ? CPUAttr.getValueAsString() : TargetCPU;
  StringRef FS = FSAttr.isValid() ? FSAttr.getValueAsString() : TargetFS;

  if (!SubtargetSingleton) {
    SubtargetSingleton = std::make_unique<WonySubtarget>(TargetTriple, CPU, FS, *this);
  }
  return SubtargetSingleton.get();
}
