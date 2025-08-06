//===----------------------------------------------------------------------===//
//
// This file defines the Wony-specific builtin function database. Users of
// this file must define the BUILTIN macro to make use of this information.
//
//===----------------------------------------------------------------------===//

include "clang/Basic/BuiltinsBase.td"

def WideningSignedMultiply : TargetBuiltin {
  let Spellings = ["__builtin_wony_widening_smul"];
  let Attributes = [NoThrow, Const];
  let Prototype = "int(short, short)";
}

def WideningUnsignedMultiply : TargetBuiltin {
  let Spellings = ["__builtin_wony_widening_umul"];
  let Attributes = [NoThrow, Const];
  let Prototype = "unsigned int(unsigned short, unsigned short)";
}
