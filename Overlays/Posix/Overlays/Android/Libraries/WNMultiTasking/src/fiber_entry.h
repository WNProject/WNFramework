// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <ucontext.h>

extern "C" {

void wn_fiber_entry(ucontext_t* c, void (*func)(void*), void* data);

}  // extern "C"
