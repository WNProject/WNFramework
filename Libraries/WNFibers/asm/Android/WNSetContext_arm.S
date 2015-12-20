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

/* int wn_setcontext(ucontext_t* ucp) */

  .text
  .global wn_setcontext
  .hidden wn_setcontext
  .type wn_setcontext, #function
  .align 0
  .fnstart
wn_setcontext:
  /* Save ucontext_t* pointer across next call */
  mov   r4, r0

  /* Call sigprocmask(SIG_SETMASK, &(ucontext->uc_sigmask), NULL) */
  mov   r0, #2  /* SIG_SETMASK */
  add   r1, r4, #UCONTEXT_SIGMASK_OFFSET
  mov   r2, #0 /* NULL */
  bl    sigprocmask(PLT)

  mov   r0, r4

  /* Restore sp and lr explicitly. */
  /* - sp can't be restored with stmia in Thumb-2 */
  ldr   sp, [r0, #(MCONTEXT_GREGS_OFFSET + 13 * 4)]
  ldr   lr, [r0, #(MCONTEXT_GREGS_OFFSET + 14 * 4)]

  add   r1, r0, #(MCONTEXT_GREGS_OFFSET + 4 * 4)

  ldm   r1, {r4-r11}

  /* Restore the caller's address in 'pc' */
  ldr   pc, [r0, #(MCONTEXT_GREGS_OFFSET + 15 * 4)]
  .fnend
  .size wn_setcontext, . - wn_setcontext