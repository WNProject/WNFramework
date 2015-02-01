// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_BUFFER_LOGGER_INL__
#define __WN_BUFFER_LOGGER_INL__
#include "WNLogging/inc/WNBufferLogger.h"

template<WN_VOID (*T_FlushCB)(WN_VOID*, const WN_CHAR*, WN_SIZE_T, const std::vector<WNLogging::WNLogColorElement>&)>
WNLogging::WNBufferLogger<T_FlushCB>::WNBufferLogger(WN_VOID* _context):mContext(_context){
}

template<WN_VOID (*T_FlushCB)(WN_VOID*, const WN_CHAR*, WN_SIZE_T, const std::vector<WNLogging::WNLogColorElement>&)>
WNLogging::WNBufferLogger<T_FlushCB>::~WNBufferLogger() {
}

template<WN_VOID (*T_FlushCB)(WN_VOID*, const WN_CHAR*, WN_SIZE_T, const std::vector<WNLogging::WNLogColorElement>&)>
WN_VOID WNLogging::WNBufferLogger<T_FlushCB>::FlushBuffer(const WN_CHAR* _buffer, WN_SIZE_T _bufferSize, const std::vector<WNLogColorElement>& _colors) {
    T_FlushCB(mContext, _buffer, _bufferSize, _colors);
}

#endif//__WN_BUFFER_LOGGER_INL__
