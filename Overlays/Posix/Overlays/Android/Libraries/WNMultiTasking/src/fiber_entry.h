// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#pragma once

#ifndef __WN_MULTI_TASKING_ANDROID_FIBER_ENTRY_H__
#define __WN_MULTI_TASKING_ANDROID_FIBER_ENTRY_H__

#include <ucontext.h>

extern "C" {

void wn_fiber_entry(ucontext_t* c, void (*func)(void*), void* data);

}  // extern "C"

#endif  // __WN_MULTI_TASKING_ANDROID_FIBER_ENTRY_H__
