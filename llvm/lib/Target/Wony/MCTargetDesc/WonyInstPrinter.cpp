//===----------------------------------------------------------------------===//
//
// This class prints an Wony MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "WonyInstPrinter.h"

#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#define GET_INSTRUCTION_NAME
#define PRINT_ALIAS_INSTR
#include "WonyGenAsmWriter.inc"


WonyInstPrinter::WonyInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                                   const MCRegisterInfo &MRI)
    : MCInstPrinter(MAI, MII, MRI) {}


void WonyInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                 StringRef Annot, const MCSubtargetInfo &STI,
                                 raw_ostream &O) {
  if (!PrintAliases || !printAliasInstr(MI, Address, O))
    printInstruction(MI, Address, O);

  printAnnotation(O, Annot);
}

void WonyInstPrinter::printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    unsigned Reg = Op.getReg();
    printRegName(O, Reg);
  } else if (Op.isImm()) {
    printImm(MI, OpNo, O);
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    Op.getExpr()->print(O, &MAI);
  }
}
void WonyInstPrinter::printImm(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  markup(O, Markup::Immediate) << formatImm(Op.getImm());
}
void WonyInstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) {
  markup(OS, Markup::Register) << getRegisterName(Reg);
}
