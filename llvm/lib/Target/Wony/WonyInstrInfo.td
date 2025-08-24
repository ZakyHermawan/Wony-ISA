//===----------------------------------------------------------------------===//
//
//  Describe Wony instructions
//
//===----------------------------------------------------------------------===//

include "WonyInstrFormats.td"

// SelectionDAG target specific nodes.
def Wonyreturnglue : SDNode<"WonyISD::RETURN_GLUE", SDTNone,
                             [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def Wonycallseq_start : SDNode<"ISD::CALLSEQ_START",
                                // Two arguments:
                                // Number of bytes allocated within the
                                // callseq_start/end region and the number
                                // of bytes allocated outside.
                                SDCallSeqStart<[ SDTCisVT<0, i32>,
                                                 SDTCisVT<1, i32> ]>,
                                [SDNPHasChain, SDNPOutGlue]>;

def Wonycallseq_end : SDNode<"ISD::CALLSEQ_END",
                              // Same arguments as callseq_start.
                              SDCallSeqEnd<[ SDTCisVT<0, i32>,
                                             SDTCisVT<1, i32> ]>,
                              [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def Wonycall : SDNode<"WonyISD::CALL",
                       // Type profile:
                       // 0: no result.
                       // -1: a variadic number of inputs.
                       // The first operand must be a pointer (to the call
                       // target)
                       SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>,
                       [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                        SDNPVariadic]>;

let Pattern = [(set GPR16:$dst, (add GPR16:$src0, GPR16:$src1))] in
def ADDi16rr : WonyBinaryInstruction<"addi16",
                                      /*opcode=*/0,
                                      /*is32Bit=*/false>;

let hasSideEffects = 1, isCodeGenOnly = 1 in {
  let Defs = [SP], Uses = [SP] in {
  def ADJCALLSTACKDOWN : WonyPseudoInstruction<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                [(Wonycallseq_start timm:$amt1, timm:$amt2)]>;
  def ADJCALLSTACKUP : WonyPseudoInstruction<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(Wonycallseq_end timm:$amt1, timm:$amt2)]>;
  }
}

let Uses = [R0], isReturn = 1, isTerminator = 1, isBarrier = 1,
Pattern = [(Wonyreturnglue)] in
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
  let Pattern = [(set GPR16:$dst, uimm7:$imm7)];
}

def LD16imm16 : WonyPseudoInstruction<(outs GPR16:$dst), (ins i16imm:$src),
                                       [(set GPR16:$dst, imm:$src)]>;

// Load a register.
def LDR16 : WonyLoadInstruction<"ldr16",
                                 /*is32Bit=*/0> {
  let Pattern = [(set GPR16:$dst, (load (addrmode GPR16:$addr, uimm4:$offset)))];
}

// Store a register.
def STR16 : WonyStoreInstruction<"str16",
                                 /*is32Bit=*/0> {
  let Pattern = [(store (i16 GPR16:$src), (addrmode GPR16:$addr, uimm4:$offset))];
}

def STR32 : WonyStoreInstruction<"str32",
                                 /*is32Bit=*/1> {
  let Pattern = [(store (i32 GPR32:$src), (addrmode GPR16:$addr, uimm4:$offset))];
}

def LDR32 : WonyLoadInstruction<"ldr32",
                                 /*is32Bit=*/1> {
  let Pattern = [(set GPR32:$dst, (load (addrmode GPR16:$addr, uimm4:$offset)))];
}

let isCall = 1, Defs = [R0], Uses = [SP] in
def CALL : WonyInstruction<"call", "$tgt",
                                 /*opcode=*/0b00100,
                                 /*is32Bit=*/0,
                                 /*oops=*/(outs),
                                 /*iops=*/(ins i16imm:$tgt)> {
  bits<11> tgt;
  let Inst{10-0} = tgt;
}

def MOV16 : WonyMoveInstruction<"mov16", /*is32Bit=*/0>;
def MOV32 : WonyMoveInstruction<"mov32", /*is32Bit=*/1>;
