//===----------------------------------------------------------------------===//
//
// This file defines all of the Wony-specific intrinsics.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Definitions for all Wony intrinsics.
//
// All Wony intrinsics start with "llvm.wony.".
//===----------------------------------------------------------------------===//

let TargetPrefix = "wony" in {
  /// Wony_Intrinsic - Base class for the intrinsics exposed to Clang.
  class Wony_Intrinsic<string suffix, list<LLVMType> ret_types,
                              list<LLVMType> param_types>
    : ClangBuiltin<!strconcat("__builtin_wony_", suffix)>,
      DefaultAttrsIntrinsic<ret_types, param_types>;

  def int_wony_widening_smul :
    Wony_Intrinsic<"widening_smul", [llvm_i32_ty], [llvm_i16_ty, llvm_i16_ty]>;
  def int_wony_widening_umul :
    Wony_Intrinsic<"widening_umul", [llvm_i32_ty], [llvm_i16_ty, llvm_i16_ty]>;
} // end TargetPrefix = "wony"
