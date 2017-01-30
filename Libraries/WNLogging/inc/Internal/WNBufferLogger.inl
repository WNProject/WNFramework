// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_BUFFER_LOGGER_INL__
#define __WN_BUFFER_LOGGER_INL__
#include "WNLogging/inc/WNBufferLogger.h"

template <void (*T_FlushCB)(void*, const char*, size_t,
    const std::vector<WNLogging::WNLogColorElement>&)>
WNLogging::WNBufferLogger<T_FlushCB>::WNBufferLogger(void* _context)
  : mContext(_context) {}

template <void (*T_FlushCB)(void*, const char*, size_t,
    const std::vector<WNLogging::WNLogColorElement>&)>
WNLogging::WNBufferLogger<T_FlushCB>::~WNBufferLogger() {}

template <void (*T_FlushCB)(void*, const char*, size_t,
    const std::vector<WNLogging::WNLogColorElement>&)>
void WNLogging::WNBufferLogger<T_FlushCB>::FlushBuffer(const char* _buffer,
    size_t _bufferSize, const std::vector<WNLogColorElement>& _colors) {
  T_FlushCB(mContext, _buffer, _bufferSize, _colors);
}

#endif  //__WN_BUFFER_LOGGER_INL__
