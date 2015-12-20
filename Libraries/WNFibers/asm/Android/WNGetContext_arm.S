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

// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFibers/src/Android/WNContext_arm.h"

/* int wn_getcontext(ucontext_t* ucp) */

  .text
  .global wn_getcontext
  .hidden wn_getcontext
  .type wn_getcontext, #function
  .align 0
  .fnstart
wn_getcontext:
  /* First, save r4-r11 */
  add   r1, r0, #(MCONTEXT_GREGS_OFFSET + 4 * 4)
  stm   r1, {r4-r11}

  /* r12 is a scratch register, don't save it */
  /* Save sp and lr explicitly. */
  /* - sp can't be stored with stmia in Thumb-2 */
  /* - STM instructions that store sp and pc are deprecated in ARM */
  str   sp, [r0, #(MCONTEXT_GREGS_OFFSET + 13 * 4)]
  str   lr, [r0, #(MCONTEXT_GREGS_OFFSET + 14 * 4)]

  /* Save the caller's address in 'pc' */
  str   lr, [r0, #(MCONTEXT_GREGS_OFFSET + 15 * 4)]

  /* Save ucontext_t* pointer across next call */
  mov   r4, r0

  /* Call sigprocmask(SIG_BLOCK, NULL, &(ucontext->uc_sigmask)) */
  mov   r0, #0  /* SIG_BLOCK */
  mov   r1, #0  /* NULL */
  add   r2, r4, #UCONTEXT_SIGMASK_OFFSET
  bl    sigprocmask(PLT)

  /* Intentionally do not save the FPU state here. This is because on
   * Linux/ARM, one should instead use ptrace(PTRACE_GETFPREGS) or
   * ptrace(PTRACE_GETVFPREGS) to get it.
   *
   * Note that a real implementation of getcontext() would need to save
   * this here to allow setcontext()/swapcontext() to work correctly.
   */
  /* Restore the values of r4 and lr */
  mov   r0, r4
  ldr   lr, [r0, #(MCONTEXT_GREGS_OFFSET + 14 * 4)]
  ldr   r4, [r0, #(MCONTEXT_GREGS_OFFSET +  4 * 4)]

  /* Return 0 */
  mov   r0, #0
  bx    lr
  .fnend
  .size wn_getcontext, . - wn_getcontext