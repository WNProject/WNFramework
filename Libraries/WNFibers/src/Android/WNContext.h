// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <ucontext.h>

extern "C" {
int wn_getcontext(ucontext_t* _ucontext);
int wn_setcontext(ucontext_t* _ucontext);
}