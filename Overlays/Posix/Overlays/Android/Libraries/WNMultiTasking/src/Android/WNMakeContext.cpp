// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/src/Android/WNContext.h"

#include <pthread.h>

extern "C" {

void wn_fiber_func(ucontext_t* c, void (*func)(void*), void* data) {
  func(data);

  if (c == 0) {
    pthread_exit(0);
  }

  wn_setcontext(c);
}

}  // extern "C"