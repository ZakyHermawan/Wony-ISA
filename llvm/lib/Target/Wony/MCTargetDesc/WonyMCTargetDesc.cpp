//===----------------------------------------------------------------------===//
//
// Holds the MC (Machine Code) target descriptions of the backend.
//
//===----------------------------------------------------------------------===//

#include "WonyMCTargetDesc.h"
#include "WonyMCAsmInfo.h"
#include "TargetInfo/WonyTargetInfo.h" // For getTheWonyTarget.
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"  // For LLVM_EXTERNAL_VISIBILITY.
#include "llvm/TargetParser/Triple.h"
#include "llvm/Support/ErrorHandling.h"


using namespace llvm;

#define GET_SUBTARGETINFO_MC_DESC
#include "WonyGenSubtargetInfo.inc"

static MCSubtargetInfo *
createWonyMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createWonyMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCAsmInfo *createWonyMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TheTriple,
                                       const MCTargetOptions &Options) {
  MCAsmInfo *MAI;
  if (TheTriple.isOSBinFormatMachO()) {
    MAI = new WonyMCAsmInfoDarwin(TheTriple, Options);
  }
  else if (TheTriple.isOSBinFormatELF()) {
    MAI = new WonyMCAsmInfoELF(TheTriple, Options);
  }
  else {
    report_fatal_error("Binary format not supported");
  }

  return MAI;
}

static MCRegisterInfo *createWonyMCRegisterInfo(const Triple &Triple) {
  MCRegisterInfo *X = new MCRegisterInfo();
  // TODO: Fill out the register info.
  return X;
}

static MCInstrInfo *createWonyMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  // TODO: Fill out the instr info.
  return X;
}


/**
 * This function registers the machine code (MC)
 * components of this backend in the instance of the Target class of this backend. The MC
 * components represent a low-level description of a target, such as the number of registers it
 * has or how to produce object files
 */
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeWonyTargetMC() {
  Target &TheTarget = getTheWonyTarget();

  // Register the MC asm info.
  RegisterMCAsmInfoFn X(TheTarget, createWonyMCAsmInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheTarget, createWonyMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheTarget, createWonyMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheTarget,
                                          createWonyMCSubtargetInfo);
}
