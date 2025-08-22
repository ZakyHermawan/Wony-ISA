; RUN: llc  -o - %s -fast-isel -fast-isel-abort=3 --stop-after=finalize-isel | FileCheck %s
target triple="wony--"

define void @empty() {
  ; CHECK-LABEL: name: empty
  ; CHECK: bb.0 (%ir-block.0):
  ; CHECK-NEXT:   RETURN implicit $r0
  ret void
}
