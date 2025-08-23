; RUN: llc -O0  -o - %s -fast-isel -fast-isel-abort=3 | FileCheck %s
target triple="wony--"

define void @empty() {
; CHECK-LABEL: empty:
; CHECK:       # %bb.0:
; CHECK-NEXT:    ret
  ret void
}
