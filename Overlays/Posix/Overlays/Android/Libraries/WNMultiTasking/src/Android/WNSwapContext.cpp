// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <ucontext.h>
#include "WNMultiTasking/src/Android/WNContext.h"

extern "C" {
int wn_swapcontext(ucontext_t* from, ucontext_t* to) {
  volatile bool swapped = false;
  wn_getcontext(from);
  if (!swapped) {
    swapped = true;
    wn_setcontext(to);
  }
  return 0;
}
}
