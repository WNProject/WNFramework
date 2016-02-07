// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_ALLOCATIONS_H__
#define __WN_SCRIPTING_ALLOCATIONS_H__

#include "WNCore/inc/WNBase.h"

#define WN_SCRIPTNODE_NEW(T) (new T)
#define WN_SCRIPTNODE_MALLOC(T) (malloc(T))

#ifdef _WN_WINDOWS
#define COPY_STRING(src, dest)                                                 \
  {                                                                            \
    if (src) {                                                                 \
      size_t allocSize = strnlen_s(src, 256) + 1;                              \
      dest = static_cast<char*>(WN_SCRIPTNODE_MALLOC(allocSize));              \
      strncpy(dest, src, allocSize - 1);                                       \
      dest[allocSize - 1] = '\0';                                              \
    } else {                                                                   \
      dest = nullptr;                                                          \
    }                                                                          \
  }
#else
#define COPY_STRING(src, dest)                                                 \
  {                                                                            \
    if (src) {                                                                 \
      size_t allocSize = strnlen(src, 256) + 1;                                \
      dest = static_cast<char*>(WN_SCRIPTNODE_MALLOC(allocSize));              \
      strncpy(dest, src, allocSize - 1);                                       \
      dest[allocSize - 1] = '\0';                                              \
    } else {                                                                   \
      dest = nullptr;                                                          \
    }                                                                          \
  }
#endif

#endif  //__WN_SCRIPTING_ALLOCATIONS_H__
