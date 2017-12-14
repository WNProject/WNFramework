// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/src/fiber_entry.h"
#include "WNMultiTasking/src/context.h"

#include <pthread.h>

extern "C" {

void wn_fiber_entry(ucontext_t* c, void (*func)(void*), void* data) {
  func(data);

  if (!c) {
    ::pthread_exit(0);
  }

  wn_set_context(c);
}

}  // extern "C"
