//===-- WonyTargetObjectFile.h - Wony Object Info -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#pragma once

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

/// This implementation is used for Wony ELF targets (Linux in particular).
class Wony_ELFTargetObjectFile : public TargetLoweringObjectFileELF {
public:
  Wony_ELFTargetObjectFile();
};

/// This TLOF implementation is used for Darwin.
class Wony_MachoTargetObjectFile : public TargetLoweringObjectFileMachO {
public:
  Wony_MachoTargetObjectFile();
};

} // end namespace llvm
