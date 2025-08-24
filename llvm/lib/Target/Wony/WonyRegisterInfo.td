//===----------------------------------------------------------------------===//
//
// Register description of the Wony target.
//
// The register hierarchy looks like:
//        +-----------+-----+-----+
// 32-bit |  d0 |  d1 |  d2 |  d3 |
//        +--+--+--+--+--+--+--+--+
// 16-bit |r0|r1|r2|r3|r4|r5|r6|r7|
//        +--+--+--+--+--+--+--+--+
//===----------------------------------------------------------------------===//


let Namespace = "Wony" in {

class WonyReg<bits<16> enc, string name, list<Register> subregs = []>
        : Register<name> {
  let HWEncoding = enc;
  let SubRegs = subregs;
  // When the subregs are specified, they cover the full register.
  // Note: Remember that !empty means the function empty, not "not empty".
  let CoveredBySubRegs = !if(!empty(subregs), false, true);
}

// Subreg definitions.
def sub_low16 : SubRegIndex<16>;
def sub_high16 : SubRegIndex<16, 16>;

// Register definitions.
def R0 : WonyReg<0, "r0">;
def R1 : WonyReg<1, "r1">;
def R2 : WonyReg<2, "r2">;
def R3 : WonyReg<3, "r3">;
def R4 : WonyReg<4, "r4">;
def R5 : WonyReg<5, "r5">;
def R6 : WonyReg<6, "r6">;
def R7 : WonyReg<7, "r7">;

let SubRegIndices = [sub_low16, sub_high16] in {
  def D0 : WonyReg<0, "d0", [R0, R1]>;
  def D1 : WonyReg<1, "d1", [R2, R3]>;
  def D2 : WonyReg<2, "d2", [R4, R5]>;
  def D3 : WonyReg<3, "d3", [R6, R7]>;
}

// Stack register, it is its own thing.
def SP : WonyReg<0, "sp">;

// Register classes.
def GPR16 : RegisterClass<"Wony", [i16], 16, (sequence "R%u", 0, 7)>;
def GPR32 : RegisterClass<"Wony", [i32], 32, (sequence "D%u", 0, 3)>;

} // end namespace Wony.
