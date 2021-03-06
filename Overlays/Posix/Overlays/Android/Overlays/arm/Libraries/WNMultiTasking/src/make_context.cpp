// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMultiTasking/src/context.h"
#include "WNMultiTasking/src/fiber_entry.h"
#include "core/inc/assert.h"

extern "C" {

void wn_make_context(ucontext_t* c, void (*func)(void*), void* data) {
  WN_RELEASE_ASSERT(
      NULL != c->uc_stack.ss_sp, "stack pointer must be non-null");
  WN_RELEASE_ASSERT(
      4 * 16 < c->uc_stack.ss_size, "The stack must be larger than 16 words");

  uintptr_t top_of_stack =
      reinterpret_cast<uintptr_t>(c->uc_stack.ss_sp) + c->uc_stack.ss_size;
  top_of_stack = (top_of_stack - 1) & ~15;

  void** sp = reinterpret_cast<void**>(top_of_stack);

  // set input parameters
  c->uc_mcontext.arm_r0 = reinterpret_cast<greg_t>(c->uc_link);
  c->uc_mcontext.arm_r1 = reinterpret_cast<greg_t>(func);
  c->uc_mcontext.arm_r2 = reinterpret_cast<greg_t>(data);

  // arm frame pointer
  c->uc_mcontext.arm_fp = reinterpret_cast<greg_t>(sp);

  // stack pointer
  c->uc_mcontext.arm_sp = reinterpret_cast<greg_t>(sp);

  // program counter
  c->uc_mcontext.arm_pc = reinterpret_cast<greg_t>(&wn_fiber_entry);
}

}  // extern "C"
