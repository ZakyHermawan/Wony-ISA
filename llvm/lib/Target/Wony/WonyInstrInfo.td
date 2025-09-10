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

let Pattern = [(set (i32 GPR32:$dst), (add (i32 GPR32:$src0),
                                           (i32 GPR32:$src1)))] in
def ADDi32rr : WonyBinaryInstruction<"addi32",
                                      /*opcode=*/0,
                                      /*is32Bit=*/true>;

let hasSideEffects = 1, isCodeGenOnly = 1 in {
  let Defs = [SP], Uses = [SP] in {
  def ADJCALLSTACKDOWN : WonyPseudoInstruction<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                [(Wonycallseq_start timm:$amt1, timm:$amt2)]>;
  def ADJCALLSTACKUP : WonyPseudoInstruction<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(Wonycallseq_end timm:$amt1, timm:$amt2)]>;
  }
}

let FastISelShouldIgnore = 1, usesCustomInserter = 1 in
def PTR_ADD16rr :
  WonyPseudoInstruction<(outs GPR16:$dst), (ins GPR16:$src0, GPR16:$src1),
                         [(set GPR16:$dst, (ptradd GPR16:$src0, GPR16:$src1))]>;

let Uses = [R0], isReturn = 1, isTerminator = 1, isBarrier = 1,
Pattern = [(Wonyreturnglue)] in
def RETURN : WonyNoOperandInstruction<"ret",
                                       /*opcode=*/0b00001,
                                       /*is32Bit=*/false>;

// Pseudo return instruction to make the machine verifier happy during
// GlobalISel lowering. Indeed, the live-range of R0 will only be correct
// after we run FinalizeISel.
let isReturn = 1, isTerminator = 1, isBarrier = 1, usesCustomInserter = 1 in
def RET_PSEUDO : WonyPseudoInstruction<(outs), (ins), []>;

def LD16imm7 : WonyLoadImmediate<"ldi16",

                                 /*opcode=*/0b00010,
                                 /*is32Bit=*/0>;
def LD32imm7 : WonyLoadImmediate<"ldi32",
                                 /*opcode=*/0b00010,
                                 /*is32Bit=*/1>;

def LD16imm16 : WonyPseudoInstruction<(outs GPR16:$dst), (ins i16imm:$src),
                                       [(set GPR16:$dst, imm:$src)]>;

def LD32imm32 : WonyPseudoInstruction<(outs GPR32:$dst), (ins i32imm:$src),
                                       [(set (i32 GPR32:$dst), imm:$src)]>;

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
  let Pattern = [(set (i32 GPR32:$dst), (load (addrmode GPR16:$addr, uimm4:$offset)))];
}

let isCall = 1, Defs = [R0], Uses = [SP] in
def CALL : WonyInstruction<"call", "$tgt",
                                 /*opcode=*/0b00100,
                                 /*is32Bit=*/0,
                                 /*oops=*/(outs),
                                 /*iops=*/(ins i16imm:$tgt)> {
  bits<11> tgt;
  let Inst{10-0} = tgt;
  let Pattern = [(Wonycall tglobaladdr:$tgt)];
}

def TRUNCSTR8: WonyInstruction<"truncstr8", "$src, $addr, $offset",
                                 /*opcode=*/0b01110,
                                 /*unusedBit=*/0,
                                 /*oops=*/(outs),
                                 /*iops=*/(ins GPR16:$src, GPR16:$addr, uimm4:$offset)> {
  let mayStore = 1;
  bits<3> src;
  bits<3> addr;
  bits<4> offset;
  let Inst{9-7} = src;
  let Inst{6-4} = addr;
  let Inst{3-0} = offset;
  let Pattern = [(truncstorei8 (i16 GPR16:$src), (addrmode GPR16:$addr, uimm4:$offset))];
}

def : Pat<(store (i8 (trunc GPR16:$src)), (addrmode GPR16:$addr, uimm4:$offset)), (TRUNCSTR8 GPR16:$src, GPR16:$addr, uimm4:$offset)>;

def : Pat<(i16 (trunc (i32 GPR32:$src))),
          (i16 (EXTRACT_SUBREG GPR32:$src, sub_low16))>;

def MOV16 : WonyMoveInstruction<"mov16", /*is32Bit=*/0>;
def MOV32 : WonyMoveInstruction<"mov32", /*is32Bit=*/1>;

def MOVTOSP: WonyInstruction<"movtosp", "$sp, $src",
                                 /*opcode=*/0b00111,
                                 /*toSP=*/1,
                                 /*oops=*/(outs OnlySP:$sp),
                                 /*iops=*/(ins GPR16:$src)> {
  bits<3> sp;
  bits<3> src;
  let Inst{9-7} = sp;
  let Inst{6-4} = src;
  let Inst{3-0} = 0;
}

def MOVFROMSP: WonyInstruction<"movfromsp", "$dst, $sp",
                                 /*opcode=*/0b00111,
                                 /*toSP=*/0,
                                 /*oops=*/(outs GPR16:$dst),
                                 /*iops=*/(ins OnlySP:$sp)> {
  bits<3> dst;
  bits<3> sp;
  let Inst{9-7} = dst;
  let Inst{6-4} = sp;
  let Inst{3-0} = 0;
}

def : Pat<(i16 (frameindex:$ptr)), (MOVFROMSP (i16 (to_tframeindex $ptr)))>;

let AddedComplexity = 1 in {

def STRSP16: WonyStoreSPInstruction<"strsp16", /*is32Bit=*/0> {
  let Pattern = [(store (i16 GPR16:$src), (spaddrmode OnlySP:$sp, simm7:$offset))];
}

def STRSP32: WonyStoreSPInstruction<"strsp32", /*is32Bit=*/1> {
  let Pattern = [(store (i32 GPR32:$src), (spaddrmode OnlySP:$sp, simm7:$offset))];
}

def LDRSP16: WonyLoadSPInstruction<"ldrsp16", /*is32Bit=*/0> {
  let Pattern = [(set GPR16:$dst, (load (spaddrmode OnlySP:$sp, simm7:$offset)))];
}

def LDRSP32: WonyLoadSPInstruction<"ldrsp32", /*is32Bit=*/1> {
  let Pattern = [(set (i32 GPR32:$dst), (load (spaddrmode OnlySP:$sp, simm7:$offset)))];
}

def LDRSEXTSP8: WonyExtLoadSPInstruction<"ldrsextsp8", /*isZExt=*/0> {
  let Pattern = [(set GPR16:$dst, (sextload (spaddrmode OnlySP:$sp, simm7:$offset)))];
}
def LDRZEXTSP8: WonyExtLoadSPInstruction<"ldrzextsp8", /*isZExt=*/1> {
  let Pattern = [(set GPR16:$dst, (zextload (spaddrmode OnlySP:$sp, simm7:$offset)))];
}

} // end AddedComplexity.

// Match any_ext(load) on zextload.
def : Pat<(i16 (extload (spaddrmode OnlySP:$sp, simm7:$offset))), (LDRZEXTSP8 OnlySP:$sp, simm7:$offset)>;

def ADDSP : WonyBinarySPInstruction<"addsp",
                                     /*opcode=*/0b01010,
                                     /*isAdd=*/true>;
def SUBSP : WonyBinarySPInstruction<"subsp",
                                     /*opcode=*/0b01010,
                                     /*isAdd=*/false>;

let Pattern = [(set GPR32:$dst, (int_wony_widening_smul GPR16:$src0, GPR16:$src1))] in
def WIDENING_SMUL : WonyWidenIMul<"wsmul", /*isSign=*/1>;

let Pattern = [(set GPR32:$dst, (int_wony_widening_umul GPR16:$src0, GPR16:$src1))] in
def WIDENING_UMUL : WonyWidenIMul<"wumul", /*isSign=*/0>;

def : Pat<(i32 (mul (sext GPR16:$src0), (sext GPR16:$src1))),
          (WIDENING_SMUL GPR16:$src0, GPR16:$src1)>;
def : Pat<(i32 (mul (zext GPR16:$src0), (zext GPR16:$src1))),
          (WIDENING_UMUL GPR16:$src0, GPR16:$src1)>;

let Pattern = [(set GPR16:$dst, (and GPR16:$src0, GPR16:$src1))] in
def AND16rr : WonyBinaryInstruction<"and16",
                                     /*opcode=*/0b01100,
                                     /*is32Bit=*/false>;
let Pattern = [(set GPR32:$dst, (and GPR32:$src0, GPR32:$src1))] in
def AND32rr : WonyBinaryInstruction<"and32",
                                     /*opcode=*/0b01100,
                                     /*is32Bit=*/true>;
