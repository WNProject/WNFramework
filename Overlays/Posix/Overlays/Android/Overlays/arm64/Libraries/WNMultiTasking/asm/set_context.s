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

// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/src/constants.h"

/* int wn_set_context(ucontext_t* ucp) */

  .text
  .global wn_set_context
  .hidden wn_set_context
  .type wn_set_context, #function
  .align 4
  .cfi_startproc
wn_set_context:
//// Save x0, we will need it later
    mov      x18, x0
//// Restore the signal mask
//// rt_sigprocmask (SIG_BLOCK, &ucp->uc_sigmask, NULL, NSIG8)
    add     x1, x0, #UCONTEXT_SIGMASK_OFFSET
    mov     x0, #0  /* SIG_BLOCK */
    mov     x2, #0  /* NULL */
    mov     x3, #(NSIG / 8)
    mov     x8, #NR_rt_sigprocmask
    svc     0
    mov     x0, x18
//
//// Figure out where to place the first context extension
//// block.
    add     x2, x0, #MCONTEXT_EXTENSION_OFFSET
    add     x3, x2, #(FPSIMD_CONTEXT_VREGS_OFFSET + 8 * SIMD_REGISTER_SIZE)
    ldp     d8,  d9, [x3], #(2 * SIMD_REGISTER_SIZE)
    ldp     d10, d11, [x3], #(2 * SIMD_REGISTER_SIZE)
    ldp     d12, d13, [x3], #(2 * SIMD_REGISTER_SIZE)
    ldp     d14, d15, [x3], #(2 * SIMD_REGISTER_SIZE)

    add     x3, x2, FPSIMD_CONTEXT_FPSR_OFFSET
    ldr     w4, [x3]
    msr     fpsr, x4

    ldr     w4, [x3, FPSIMD_CONTEXT_FPCR_OFFSET - FPSIMD_CONTEXT_FPSR_OFFSET]
    msr     fpcr, x4

    ldr     x2, [x0, MCONTEXT_SP_OFFSET]
    mov     sp, x2

    ldr     x30,      [x0, MCONTEXT_GREGS_OFFSET + 30 * REGISTER_SIZE]
    ldp     x28, x29, [x0, MCONTEXT_GREGS_OFFSET + 28 * REGISTER_SIZE]
    ldp     x26, x27, [x0, MCONTEXT_GREGS_OFFSET + 26 * REGISTER_SIZE]
    ldp     x24, x25, [x0, MCONTEXT_GREGS_OFFSET + 24 * REGISTER_SIZE]
    ldp     x22, x23, [x0, MCONTEXT_GREGS_OFFSET + 22 * REGISTER_SIZE]
    ldp     x20, x21, [x0, MCONTEXT_GREGS_OFFSET + 20 * REGISTER_SIZE]
    ldp     x18, x19, [x0, MCONTEXT_GREGS_OFFSET + 18 * REGISTER_SIZE]
    ldp     x2,  x3,  [x0, MCONTEXT_GREGS_OFFSET +  2 * REGISTER_SIZE]
    ldp     x0,  x1,  [x0, MCONTEXT_GREGS_OFFSET +  0 * REGISTER_SIZE]
  ret
  .cfi_endproc
  .size wn_set_context, . - wn_set_context
