#pragma once

#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"


namespace llvm {

class WonyTargetMachine: public CodeGenTargetMachineImpl {
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
};

} // end namespace llvm
