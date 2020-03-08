// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_POSIX_INTERNAL_STACK_ALLOCATION_H__
#define __WN_MEMORY_POSIX_INTERNAL_STACK_ALLOCATION_H__

#include <alloca.h>

#include "WNCore/inc/types.h"

#define __WN_INTERNAL_STACK_ALLOC(_size) ::alloca(_size)

#endif  // __WN_MEMORY_POSIX_INTERNAL_STACK_ALLOCATION_H__
