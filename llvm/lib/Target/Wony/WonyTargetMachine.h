//===----------------------------------------------------------------------===//
//
// This file declares the Wony specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "WonySubtarget.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"

#include <memory>
#include <optional>

namespace llvm {

class WonyTargetMachine: public CodeGenTargetMachineImpl {
  mutable std::unique_ptr<WonySubtarget> SubtargetSingleton;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

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
  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;

  TargetLoweringObjectFile *getObjFileLowering() const override;

  // Register the target specific passes that this backend offers.
  void registerPassBuilderCallbacks(PassBuilder &PB) override;
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
};

class WonyPassConfig : public TargetPassConfig {
public:
  WonyPassConfig(TargetMachine &TM, PassManagerBase &PM);

  WonyTargetMachine &getWonyTargetMachine() const {
    return getTM<WonyTargetMachine>();
  }

  bool addIRTranslator() override;
  void addPreLegalizeMachineIR() override;
  bool addLegalizeMachineIR() override;
  bool addRegBankSelect() override;
  bool addGlobalInstructionSelect() override;
  bool addInstSelector() override;
  void addIRPasses() override;
};

} // end namespace llvm
