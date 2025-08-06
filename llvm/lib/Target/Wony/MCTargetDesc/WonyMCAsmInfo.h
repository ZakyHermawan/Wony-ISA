//===----------------------------------------------------------------------===//
//
/// Declaration of the Wony MCAsmInfos.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/MC/MCAsmInfoDarwin.h"
#include "llvm/MC/MCAsmInfoELF.h"
namespace llvm {

class Triple;

class WonyMCAsmInfoELF : public MCAsmInfoELF {
public:
  explicit WonyMCAsmInfoELF(const Triple &TT, const MCTargetOptions &Options);
};

class WonyMCAsmInfoDarwin : public MCAsmInfoDarwin {
public:
  explicit WonyMCAsmInfoDarwin(const Triple &TT,
                                const MCTargetOptions &Options);
};

} // end namespace llvm
