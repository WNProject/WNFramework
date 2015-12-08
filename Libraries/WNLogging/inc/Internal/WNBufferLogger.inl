// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_BUFFER_LOGGER_INL__
#define __WN_BUFFER_LOGGER_INL__
#include "WNLogging/inc/WNBufferLogger.h"

template <
    wn_void (*T_FlushCB)(wn_void*, const wn_char*, wn_size_t,
                         const std::vector<WNLogging::WNLogColorElement>&)>
WNLogging::WNBufferLogger<T_FlushCB>::WNBufferLogger(wn_void* _context)
    : mContext(_context) {}

template <
    wn_void (*T_FlushCB)(wn_void*, const wn_char*, wn_size_t,
                         const std::vector<WNLogging::WNLogColorElement>&)>
WNLogging::WNBufferLogger<T_FlushCB>::~WNBufferLogger() {}

template <
    wn_void (*T_FlushCB)(wn_void*, const wn_char*, wn_size_t,
                         const std::vector<WNLogging::WNLogColorElement>&)>
wn_void WNLogging::WNBufferLogger<T_FlushCB>::FlushBuffer(
    const wn_char* _buffer, wn_size_t _bufferSize,
    const std::vector<WNLogColorElement>& _colors) {
  T_FlushCB(mContext, _buffer, _bufferSize, _colors);
}

#endif  //__WN_BUFFER_LOGGER_INL__
