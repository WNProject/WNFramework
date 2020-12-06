// This file is derived from the assembly code found in google breakpad
// (https://chromium.googlesource.com/breakpad/breakpad/).
// See licence below

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

// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMultiTasking/src/constants.h"

.text
.global wn_set_context
.align 4
.type wn_set_context, @function

wn_set_context:
movl 4(%esp), %eax   /* eax = uc */

push %eax // save off eax, for the function call will restore afterwards
/* Restore signal mask: sigprocmask(SIG_SETMASK, &uc->uc_sigmask, NULL) */
leal UCONTEXT_SIGMASK_OFFSET(%eax), %edx
xorl %ecx, %ecx

push %ecx   /* NULL */
push %edx   /* &uc->uc_sigmask */
movl $0, %ecx /* SIG_SETMASK == 2 on i386 */
push %ecx
call sigprocmask@PLT
addl $12, %esp

pop %eax // Restore EAX we will need it

/* Restore the floating point registers */
leal UCONTEXT_FPREGS_MEM_OFFSET(%eax), %ecx
fldenv  (%ecx)

/* Restore register values */
// Restore fs
movl  MCONTEXT_FS_OFFSET(%eax), %ecx
movw %cx, %fs

movl MCONTEXT_EBP_OFFSET(%eax), %ebp
movl MCONTEXT_ESI_OFFSET(%eax), %esi
movl MCONTEXT_EDI_OFFSET(%eax), %edi
movl MCONTEXT_EBX_OFFSET(%eax), %ebx
movl MCONTEXT_EDX_OFFSET(%eax), %edx
movl MCONTEXT_ECX_OFFSET(%eax), %ecx

// Restore eip and esp
movl  MCONTEXT_ESP_OFFSET(%eax), %esp
movl  MCONTEXT_EIP_OFFSET(%eax), %eax
jmp *%eax
movl $0, %eax
ret
.size wn_set_context, . - wn_set_context
