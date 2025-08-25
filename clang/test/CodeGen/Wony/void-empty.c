// REQUIRES: wony-registered-target
// RUN: %clang --target=wony %s -emit-llvm -o - -S | FileCheck %s

// Check that we can connect clang from the driver (as opposed to cc1) all
// the way to assembly code.

// CHECK-LABEL: empty
// CHECK: ret
void empty() {
}
