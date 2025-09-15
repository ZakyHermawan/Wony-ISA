//===----------------------------------------------------------------------===//
//
// This file implements the WonyMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "WonyMCFixups.h"
#include "MCTargetDesc/WonyMCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/EndianStream.h"
#include <cassert>
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

namespace {

class WonyMCCodeEmitter : public MCCodeEmitter {
  MCContext &MCCtxt;

public:
  WonyMCCodeEmitter(MCContext &MCCtxt) : MCCodeEmitter(), MCCtxt(MCCtxt) {}
  ~WonyMCCodeEmitter() override = default;

  // TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  // Return binary encoding of operand. If the machine
  // operand requires relocation, record the relocation and return zero.
  // This method is used in the TableGen'erated code.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;
};

} // end anonymous namespace

MCCodeEmitter *llvm::createWonyMCCodeEmitter(const MCInstrInfo &MCII,
                                              MCContext &MCCtxt) {
  return new WonyMCCodeEmitter(MCCtxt);
}

unsigned
WonyMCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return MCCtxt.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  // At this point we expect a symbol reference for the branches.
  assert(MO.isExpr());
  const MCExpr *Expr = MO.getExpr();
  assert(Expr->getKind() == MCExpr::SymbolRef);

  if (MI.getOpcode() == Wony::CALL) {
    Fixups.push_back(
        MCFixup::create(0, Expr, (MCFixupKind)Wony::FK_Wony_PCRel_11));
  } else
    llvm_unreachable("We don't have any operation with symbols");

  return 0;
}

void WonyMCCodeEmitter::encodeInstruction(const MCInst &MI,
                                           SmallVectorImpl<char> &CB,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  // Get instruction encoding and emit it
  uint64_t Encoding = getBinaryCodeForInstr(MI, Fixups, STI);
  assert(((Encoding & 0xffffffffffff0000) == 0) &&
         "Only the first 16-bit should be set");
  support::endian::write<uint16_t>(CB, Encoding, llvm::endianness::little);
}

#include "WonyGenMCCodeEmitter.inc"
