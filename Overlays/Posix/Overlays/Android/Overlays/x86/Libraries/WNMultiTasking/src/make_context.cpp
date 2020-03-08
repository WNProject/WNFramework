// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <pthread.h>

#include "WNCore/inc/assert.h"
#include "WNMultiTasking/src/context.h"
#include "WNMultiTasking/src/fiber_entry.h"

extern "C" {

void wn_make_context(
    ucontext_t* _context, void (*_function)(void*), void* _data) {
  WN_RELEASE_ASSERT(_context->uc_stack.ss_sp, "stack pointer must be non-null");
  WN_RELEASE_ASSERT(4 * 64 < _context->uc_stack.ss_size,
      "The stack must be larger than 64 words");

  uintptr_t top_of_stack =
      reinterpret_cast<uintptr_t>(_context->uc_stack.ss_sp) +
      _context->uc_stack.ss_size;
  top_of_stack = (top_of_stack - 1) & ~31;

  void** sp = reinterpret_cast<void**>(top_of_stack);

  _context->uc_mcontext.gregs[REG_EBP] = reinterpret_cast<greg_t>(sp);
  _context->uc_mcontext.gregs[REG_ESP] = reinterpret_cast<greg_t>(sp - 3);
  _context->uc_mcontext.gregs[REG_EIP] =
      reinterpret_cast<greg_t>(&wn_fiber_entry);

  sp[0] = _data;
  sp[-1] = reinterpret_cast<void*>(_function);
  sp[-2] = _context->uc_link;
}

}  // extern "C"
