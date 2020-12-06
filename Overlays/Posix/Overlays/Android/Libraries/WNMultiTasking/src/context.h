// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_ANDROID_CONTEXT_H__
#define __WN_MULTI_TASKING_ANDROID_CONTEXT_H__

#include <ucontext.h>

extern "C" {

int wn_get_context(ucontext_t* _context);
int __noreturn wn_set_context(ucontext_t* _context);
int wn_swap_context(ucontext_t* _from, ucontext_t* _to);
void wn_make_context(
    ucontext_t* _context, void (*_function)(void*), void* _data);

}  // extern "C"

#endif  // __WN_MULTI_TASKING_ANDROID_CONTEXT_H__
