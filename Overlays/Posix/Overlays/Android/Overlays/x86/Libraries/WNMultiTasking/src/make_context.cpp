// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNAssert.h"
#include "WNMultiTasking/src/context.h"
#include "WNMultiTasking/src/fiber_entry.h"

#include <pthread.h>

extern "C" {

void wn_make_context(ucontext_t* c, void (*func)(void*), void* data) {
  WN_RELEASE_ASSERT_DESC(
      NULL != c->uc_stack.ss_sp, "stack pointer must be non-null");
  WN_RELEASE_ASSERT_DESC(
      4 * 64 < c->uc_stack.ss_size, "The stack must be larger than 64 words");

  uintptr_t top_of_stack =
      reinterpret_cast<uintptr_t>(c->uc_stack.ss_sp) + c->uc_stack.ss_size;
  top_of_stack = (top_of_stack - 1) & ~31;

  void** sp = reinterpret_cast<void**>(top_of_stack);

  c->uc_mcontext.gregs[REG_EBP] = reinterpret_cast<greg_t>(sp);
  c->uc_mcontext.gregs[REG_ESP] = reinterpret_cast<greg_t>(sp - 3);
  c->uc_mcontext.gregs[REG_EIP] = reinterpret_cast<greg_t>(&wn_fiber_entry);

  sp[0] = data;
  sp[-1] = reinterpret_cast<void*>(func);
  sp[-2] = c->uc_link;
}

}  // extern "C"
