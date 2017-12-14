// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <ucontext.h>

extern "C" {

int wn_get_context(ucontext_t* _ucontext);
int __noreturn wn_set_context(ucontext_t* _ucontext);
int wn_swap_context(ucontext_t* from, ucontext_t* to);
void wn_make_context(ucontext_t* c, void (*func)(void*), void* data);

}  // extern "C"
