class WonyInstruction<string asm, string operands, dag oops = (outs), dag iops = (ins)> : Instruction<> {
  let Namespace = "Wony";
  let AsmString = !strconcat(asm, "\t", operands);
  let OutOperandList = oops;
  let InOperandList = iops;
}
