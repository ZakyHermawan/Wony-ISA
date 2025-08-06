#pragma once

#include "WonySubtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"

namespace llvm {

class WonyTargetMachine: public CodeGenTargetMachineImpl {
  mutable std::unique_ptr<WonySubtarget> SubtargetSingleton;

public:
  WonyTargetMachine(const Target &T, const Triple &TT,
    StringRef CPU,
    StringRef FS,
    const TargetOptions &Options,
    std::optional<Reloc::Model> RM,
    std::optional<CodeModel::Model> CM,
    CodeGenOptLevel OL,
    bool JIT);
  ~WonyTargetMachine() override;

  const WonySubtarget *getSubtargetImpl(const Function &F) const override;
};

} // end namespace llvm
