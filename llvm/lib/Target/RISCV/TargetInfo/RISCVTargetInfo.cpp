//===-- RISCVTargetInfo.cpp - RISC-V Target Implementation ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/RISCVTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheRISCV32Target() {
  static Target TheRISCV32Target;
  return TheRISCV32Target;
}

Target &llvm::getTheRISCV64Target() {
  static Target TheRISCV64Target;
  return TheRISCV64Target;
}


/*
  * LLVM_EXTERNAL_VISIBILITY宏。定义在angryZ_llvm-project/llvm/include/llvm/Support/Compiler.h
  * 共享库外部可见
  * LLVM_LIBRARY_VISIBILITY 外部不可见，库可见，也就是库私有
*/
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeRISCVTargetInfo() {
  RegisterTarget<Triple::riscv32, /*HasJIT=*/true> X(
      getTheRISCV32Target(), "riscv32", "32-bit RISC-V", "RISCV");
  RegisterTarget<Triple::riscv64, /*HasJIT=*/true> Y(
      getTheRISCV64Target(), "riscv64", "64-bit RISC-V", "RISCV");
}
