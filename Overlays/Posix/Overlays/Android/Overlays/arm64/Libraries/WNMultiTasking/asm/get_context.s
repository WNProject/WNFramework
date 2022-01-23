// This file is derived from the assembly code found in google breakpad
// (https://chromium.googlesource.com/breakpad/breakpad/). See license below

// Copyright (c) 2012, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// A minimalistic implementation of getcontext() to be used by
// Google Breakpad on Android.

// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMultiTasking/src/constants.h"

/* int wn_get_context(ucontext_t* ucp) */

  .text
  .global wn_get_context
  .hidden wn_get_context
  .type wn_get_context, #function
  .align 4
  .cfi_startproc
wn_get_context:

  // The saved context will return to the wn_get_context() call point
  //   with a return value of 0
  stp     x0,  x1,  [x0, MCONTEXT_GREGS_OFFSET +  0 * REGISTER_SIZE]
  stp     x2,  x3,  [x0, MCONTEXT_GREGS_OFFSET +  2 * REGISTER_SIZE]
  stp     x18, x19, [x0, MCONTEXT_GREGS_OFFSET + 18 * REGISTER_SIZE]
  stp     x20, x21, [x0, MCONTEXT_GREGS_OFFSET + 20 * REGISTER_SIZE]
  stp     x22, x23, [x0, MCONTEXT_GREGS_OFFSET + 22 * REGISTER_SIZE]
  stp     x24, x25, [x0, MCONTEXT_GREGS_OFFSET + 24 * REGISTER_SIZE]
  stp     x26, x27, [x0, MCONTEXT_GREGS_OFFSET + 26 * REGISTER_SIZE]
  stp     x28, x29, [x0, MCONTEXT_GREGS_OFFSET + 28 * REGISTER_SIZE]
  str     x30,      [x0, MCONTEXT_GREGS_OFFSET + 30 * REGISTER_SIZE]
  // Place LR into the saved PC, this will ensure that when
  // switching to this saved context with setcontext() control
  //  will pass back to the caller of wn_get_context(), we have
  // already arranged to return the appropriate return value in x0
  // above.
  str     x30, [x0, MCONTEXT_PC_OFFSET]
  // Save the current SP
  mov     x2, sp
  str     x2, [x0, MCONTEXT_SP_OFFSET]
  // Initialize the pstate.
  str     xzr, [x0, MCONTEXT_PSTATE_OFFSET]
  // Figure out where to place the first context extension
  // block.
  add     x2, x0, #MCONTEXT_EXTENSION_OFFSET
  // Write the context extension fpsimd header.
  mov     w3, #(FPSIMD_MAGIC & 0xffff)
  movk    w3, #(FPSIMD_MAGIC >> 16), lsl #16
  str     w3, [x2, #FPSIMD_CONTEXT_MAGIC_OFFSET]
  mov     w3, #FPSIMD_CONTEXT_SIZE
  str     w3, [x2, FPSIMD_CONTEXT_SIZE_OFFSET]
  // Fill in the FP SIMD context.
  add     x3, x2, #(FPSIMD_CONTEXT_VREGS_OFFSET + 8 * SIMD_REGISTER_SIZE)
  stp     d8,  d9, [x3], #(2 * SIMD_REGISTER_SIZE)
  stp     d10, d11, [x3], #(2 * SIMD_REGISTER_SIZE)
  stp     d12, d13, [x3], #(2 * SIMD_REGISTER_SIZE)
  stp     d14, d15, [x3], #(2 * SIMD_REGISTER_SIZE)
  add     x3, x2, FPSIMD_CONTEXT_FPSR_OFFSET
  mrs     x4, fpsr
  str     w4, [x3]
  mrs     x4, fpcr
  str     w4, [x3, FPSIMD_CONTEXT_FPCR_OFFSET - FPSIMD_CONTEXT_FPSR_OFFSET]
  // Write the termination context extension header.
  add     x2, x2, #FPSIMD_CONTEXT_SIZE
  str     xzr, [x2, FPSIMD_CONTEXT_MAGIC_OFFSET]
  str     xzr, [x2, FPSIMD_CONTEXT_SIZE_OFFSET]
  // Grab the signal mask
  // rt_sigprocmask (SIG_BLOCK, NULL, &ucp->uc_sigmask, NSIG8)
  add     x2, x0, #UCONTEXT_SIGMASK_OFFSET
  mov     x0, #0  // SIG_BLOCK
  mov     x1, #0  // NULL
  mov     x3, #(NSIG / 8)
  mov     x8, #NR_rt_sigprocmask
  svc     0
  // Return x0 for success
  mov     x0, 0
  ret
  .cfi_endproc
  .size wn_get_context, . - wn_get_context
