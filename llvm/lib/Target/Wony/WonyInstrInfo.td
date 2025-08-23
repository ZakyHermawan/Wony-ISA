include "WonyInstrFormats.td"

// SelectionDAG target specific nodes.
def WonyReturnGlue : SDNode<"WonyISD::RETURN_GLUE", SDTNone,
                             [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def ADDi16rr : WonyBinaryInstruction<"addi16",
                                      /*opcode=*/0,
                                      /*is32Bit=*/false>;

let Uses = [R0], isReturn = 1, isTerminator = 1, isBarrier = 1 in
def RETURN : WonyNoOperandInstruction<"ret",
                                       /*opcode=*/0b00001,
                                       /*is32Bit=*/false>;

let isAsCheapAsAMove = 1, isMoveImm = 1, isReMaterializable = 1 in
def LD16imm7 : WonyInstruction<"ldi16", "$dst, $imm7",
                                 /*opcode=*/0b00010,
                                 /*is32Bit=*/0,
                                 /*oops=*/(outs GPR16:$dst),
                                 /*iops=*/(ins uimm7:$imm7)> {
  bits<3> dst;
  bits<7> imm7;
  let Inst{9-7} = dst;
  let Inst{6-0} = imm7;
}
