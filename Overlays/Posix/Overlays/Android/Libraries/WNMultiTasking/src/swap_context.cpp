// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <ucontext.h>

#include "WNMultiTasking/src/context.h"

extern "C" {

int wn_swap_context(ucontext_t* from, ucontext_t* to) {
  volatile bool swapped = false;

  wn_get_context(from);

  if (!swapped) {
    swapped = true;

    wn_set_context(to);
  }

  return 0;
}

}  // extern "C"
