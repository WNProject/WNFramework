// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/src/fiber_entry.h"

#include "WNMultiTasking/src/context.h"

#include <pthread.h>

extern "C" {

void wn_fiber_entry(
    ucontext_t* _context, void (*_function)(void*), void* _data) {
  _function(_data);

  if (!_context) {
    ::pthread_exit(0);
  }

  wn_set_context(_context);
}

}  // extern "C"
