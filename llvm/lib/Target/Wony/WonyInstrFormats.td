class WonyInstruction<string asm, string operands,
                       bits<5> opcode = 0,
                       bit is32Bit = 0,
                       dag oops = (outs), dag iops = (ins)>
          : Instruction<> {
  let Namespace = "Wony";
  let AsmString = !strconcat(asm, "\t", operands);
  let OutOperandList = oops;
  let InOperandList = iops;
  // Encoding information.
  // +---------------------------------------------------------------+
  // | Bit index                                                     |
  // +--------+------------+--------+---------+---------+------------+
  // | 15 - 11|     10     |  9 - 7 |   6 - 4 |   3 - 1 |     0      |
  // +--------+------------+--------+---------+---------+------------+
  // | <5>opc | <1>is32Bit | <3>dst | <3>src0 | <3>src1 | <1><spare> |
  // +--------+------------+--------+---------+---------+------------+
  bits<16> Inst;
  let Inst{15-11} = opcode;
  let Inst{10} = is32Bit;
  let Inst{0} = 0;
  // oops and iops are handled by the other classes.
}

class WonyBinaryInstruction<string asm,
                             bits<5> opcode = 0,
                             bit is32Bit = 0,
                             RegisterClass SrcRC = !if(is32Bit, GPR32, GPR16),
                             RegisterClass DstRC = SrcRC>
          : WonyInstruction<asm, "$dst, $src0, $src1", opcode, is32Bit,
                             (outs DstRC:$dst), (ins SrcRC:$src0, SrcRC:$src1)> {
  bits<3> dst;
  bits<3> src0;
  bits<3> src1;
  let Inst{9-7} = dst;
  let Inst{6-4} = src0;
  let Inst{3-1} = src1;
}

class WonyNoOperandInstruction<string asm,
                             bits<5> opcode = 0,
                             bit is32Bit = 0>
          : WonyInstruction<asm, /*operands=*/"", opcode, is32Bit,
                             (outs), (ins)> {
  let Inst{9-1} = 0;
}


// classes for operand types.
class AsmImmRange<int Low, int High> : AsmOperandClass {
  let Name = "Imm" # Low # "_" # High;
  let DiagnosticType = "InvalidImm" # Low # "_" # High;
  let RenderMethod = "addImmOperands";
  let PredicateMethod = "isImmInRange<" # Low # "," # High # ">";
}

def uimm7 : Operand<i16>, ImmLeaf<i16, [{return Imm >= 0 && Imm < 128;}]> {
  let ParserMatchClass = AsmImmRange<0, 127>;
}
