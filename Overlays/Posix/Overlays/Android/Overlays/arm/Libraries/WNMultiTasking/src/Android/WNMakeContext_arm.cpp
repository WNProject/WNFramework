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
      4 * 16 < c->uc_stack.ss_size, "The stack must be larger than 16 words");

  void** sp = static_cast<void**>(c->uc_stack.ss_sp) + c->uc_stack.ss_size / 4;

  // set input parameters
  c->uc_mcontext.arm_r0 = reinterpret_cast<greg_t>(c->uc_link);
  c->uc_mcontext.arm_r1 = reinterpret_cast<greg_t>(func);
  c->uc_mcontext.arm_r2 = reinterpret_cast<greg_t>(data);

  // arm frame pointer
  c->uc_mcontext.arm_fp = reinterpret_cast<greg_t>(sp);

  // stack pointer
  c->uc_mcontext.arm_sp = reinterpret_cast<greg_t>(sp);

  // program counter
  c->uc_mcontext.arm_pc = reinterpret_cast<greg_t>(&wn_fiber_func);
}

}  // extern "C"
