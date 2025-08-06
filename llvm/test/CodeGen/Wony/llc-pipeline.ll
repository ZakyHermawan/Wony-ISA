; RUN: llc -O0 -mtriple wony %s -debug-pass=Structure --stop-before=peephole-opt -o /dev/null 2>&1 | FileCheck %s --check-prefix=CHECK-O0
; RUN: llc -O1 -mtriple wony %s -debug-pass=Structure --stop-before=peephole-opt -o /dev/null 2>&1 | FileCheck %s --check-prefix=CHECK-Opt
; RUN: llc -O2 -mtriple wony %s -debug-pass=Structure --stop-before=peephole-opt -o /dev/null 2>&1 | FileCheck %s --check-prefix=CHECK-Opt
; RUN: llc -O3 -mtriple wony %s -debug-pass=Structure --stop-before=peephole-opt -o /dev/null 2>&1 | FileCheck %s --check-prefix=CHECK-Opt

; Check that the Wony custom codegen pipeline includes the target specific
; simple constant propagation pass, but only when the optimizations are enabled.

; CHECK-O0-NOT: Wony simple constant propagation
; CHECK-Opt: Wony simple constant propagation
