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

// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_ANDROID_ARM64_CONSTANTS_H__
#define __WN_MULTI_TASKING_ANDROID_ARM64_CONSTANTS_H__

#define UCONTEXT_SIGMASK_OFFSET 40

#define MCONTEXT_GREGS_OFFSET 184
#define MCONTEXT_SP_OFFSET 432
#define MCONTEXT_PC_OFFSET 440
#define MCONTEXT_PSTATE_OFFSET 448
#define MCONTEXT_EXTENSION_OFFSET 464

#define FPSIMD_MAGIC 0x46508001

#define FPSIMD_CONTEXT_MAGIC_OFFSET 0
#define FPSIMD_CONTEXT_SIZE_OFFSET 4
#define FPSIMD_CONTEXT_FPSR_OFFSET 8
#define FPSIMD_CONTEXT_FPCR_OFFSET 12
#define FPSIMD_CONTEXT_VREGS_OFFSET 16
#define FPSIMD_CONTEXT_SIZE 528

#define REGISTER_SIZE 8
#define SIMD_REGISTER_SIZE 16

#define NSIG 64
#define NR_rt_sigprocmask 135

#endif  // __WN_MULTI_TASKING_ANDROID_ARM64_CONSTANTS_H__
