//===----------------------------------------------------------------------===//
//
// This file hold the declarations for the Wony-specific passes for
// both the legacy and new pass managers.
//
//===----------------------------------------------------------------------===//
#pragma once

#include "llvm/IR/PassManager.h" // For PassInfoMixin.
#include "llvm/PassRegistry.h"

namespace llvm {

class Function;
class Pass;
class PassRegistry;
class WonyTargetMachine;

class WonySimpleConstantPropagationNewPass
    : public llvm::PassInfoMixin<WonySimpleConstantPropagationNewPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &AM);
};

void initializeWonySimpleConstantPropagationPass(PassRegistry &);
Pass *createWonySimpleConstantPropagationPassForLegacyPM();

void initializeWonyDAGToDAGISelLegacyPass(PassRegistry &);
Pass *createWonyISelDAG(WonyTargetMachine &TM);

} // end namespace llvm.
