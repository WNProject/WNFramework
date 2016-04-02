// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNAssert.h"
#include "WNMultiTasking/src/Android/WNContext.h"

extern "C" {

void wn_makecontext(ucontext_t* c, void (*func)(void*), void* data) {
  WN_RELEASE_ASSERT_DESC(
      NULL != c->uc_stack.ss_sp, "stack pointer must be non-null");
  WN_RELEASE_ASSERT_DESC(
      8 * 16 < c->uc_stack.ss_size, "The stack must be larger than 16 words");

  void** sp = static_cast<void**>(c->uc_stack.ss_sp) + c->uc_stack.ss_size / 8;
  sp -= 16;

  // set input parameters
  c->uc_mcontext.regs[0] = reinterpret_cast<__u64>(c->uc_link);
  c->uc_mcontext.regs[1] = reinterpret_cast<__u64>(func);
  c->uc_mcontext.regs[2] = reinterpret_cast<__u64>(data);

  // arm stack pointer
  c->uc_mcontext.sp = reinterpret_cast<__u64>(sp);

  // arm frame pointer
  c->uc_mcontext.regs[29] = reinterpret_cast<__u64>(sp + 2);

  // program counter
  c->uc_mcontext.pc = reinterpret_cast<__u64>(&wn_fiber_func);
  c->uc_mcontext.regs[30] = reinterpret_cast<__u64>(&wn_fiber_func);
}

}  // extern "C"