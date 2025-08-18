include "WonyInstrFormats.td"

def ADDi16rr : WonyBinaryInstruction<"addi16",
                                      /*opcode=*/0,
                                      /*is32Bit=*/false>;

let Uses = [R0], isReturn = 1, isTerminator = 1, isBarrier = 1 in
def RETURN : WonyNoOperandInstruction<"ret",
                                       /*opcode=*/0b00001,
                                       /*is32Bit=*/false>;
