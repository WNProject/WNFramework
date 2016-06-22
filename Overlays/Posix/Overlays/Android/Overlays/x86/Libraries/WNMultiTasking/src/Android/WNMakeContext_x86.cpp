// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <pthread.h>
#include "WNCore/inc/WNAssert.h"
#include "WNMultiTasking/src/Android/WNContext.h"

extern "C" {

void wn_makecontext(ucontext_t* c, void (*func)(void*), void* data) {
  WN_RELEASE_ASSERT_DESC(
      NULL != c->uc_stack.ss_sp, "stack pointer must be non-null");
  WN_RELEASE_ASSERT_DESC(
      4 * 16 < c->uc_stack.ss_size, "The stack must be larger than 16 words");

  void** sp = static_cast<void**>(c->uc_stack.ss_sp) + c->uc_stack.ss_size / 4;

  c->uc_mcontext.gregs[REG_EBP] = reinterpret_cast<greg_t>(sp);
  c->uc_mcontext.gregs[REG_ESP] = reinterpret_cast<greg_t>(sp - 3);
  c->uc_mcontext.gregs[REG_EIP] = reinterpret_cast<greg_t>(&wn_fiber_func);

  sp[0] = data;
  sp[-1] = reinterpret_cast<void*>(func);
  sp[-2] = c->uc_link;
}
}
