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

/* int wn_set_context(ucontext_t* ucp) */

  .text
  .global wn_set_context
  .hidden wn_set_context
  .type wn_set_context, #function
  .align 0
  .fnstart
wn_set_context:
  /* Save ucontext_t* pointer across next call */
  mov   r4, r0

  /* Call sigprocmask(SIG_SETMASK, &(ucontext->uc_sigmask), NULL) */
  mov   r0, #2 /* SIG_SETMASK */
  add   r1, r4, #UCONTEXT_SIGMASK_OFFSET
  mov   r2, #0 /* NULL */
  bl    sigprocmask(PLT)

  /* Restore sp and lr registers */
  /* - sp can't be restored with stmia in Thumb-2 */
  mov   r12, r4
  ldr   sp, [r12, #MCONTEXT_GREGS_R13_OFFSET]
  ldr   lr, [r12, #MCONTEXT_GREGS_R14_OFFSET]

  /* Restore general registers */
  add   r12, r12, #MCONTEXT_GREGS_R0_OFFSET
  ldm   r12, {r0-r11}

  /* Restore the caller's address in 'pc'. */
  /* Note this offset is the offset from the base of the gregs element in the
     struct not the struct base, hence why this is different */
  ldr   pc, [r12, #MCONTEXT_GREGS_R15_OFFSET_GREGS]
  .fnend
  .size wn_set_context, . - wn_set_context
