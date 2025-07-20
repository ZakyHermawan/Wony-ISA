#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Compiler.h" // For LLVM_LIBRARY_VISIBILITY.
#include "llvm/TargetParser/Triple.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY WonyTargetInfo : public TargetInfo {
public:
  WonyTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    resetDataLayout(
        // Little-endian.
        "e-"
        // Pointer size is 16-bit and the alignment matches.
        "p:16:16:16-"
        // Supports natively 16-bit and 32-bit integer.
        "n16:32-"
        // i32 are aligned on 32, i16 on 16 and i1 on 8.
        "i32:32:32-i16:16:16-i1:8:8-"
        // f32 aligned on 32-bit.
        "f32:32:32-"
        // v32 aligned on 32-bit.
        "v32:32:32");
  }

  /// Appends the target-specific \#define values for this
  /// target set to the specified buffer.
  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  /// Return information about target-specific builtins for
  /// the current primary target, and info about which builtins are non-portable
  /// across the current set of primary and secondary targets.
  ArrayRef<Builtin::Info> getTargetBuiltins() const override {
    return std::nullopt;
  }

  /// Returns the kind of __builtin_va_list type that should be used
  /// with this target.
  BuiltinVaListKind getBuiltinVaListKind() const override {
    return CharPtrBuiltinVaList;
  }

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &info) const override {
    return false;
  }

  /// Returns a string of target-specific clobbers, in LLVM format.
  std::string_view getClobbers() const override { return ""; }

  ArrayRef<const char *> getGCCRegNames() const override {
    return std::nullopt;
  }
  ArrayRef<GCCRegAlias> getGCCRegAliases() const override {
    return std::nullopt;
  }
};
} // namespace targets
} // namespace clang
