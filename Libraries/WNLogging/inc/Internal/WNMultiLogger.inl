// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_MULTI_LOGGER_INL__
#define __WN_MULTI_LOGGER_INL__
#include "WNLogging/inc/WNMultiLogger.h"

template <typename T_Logger1, typename T_Logger2, int T_BufferSize>
WNLogging::WNMultiLogger<T_Logger1, T_Logger2, T_BufferSize>::~WNMultiLogger() {
}

template <typename T_Logger1, typename T_Logger2, int T_BufferSize>
void WNLogging::WNMultiLogger<T_Logger1, T_Logger2,
    T_BufferSize>::FlushExternal(const char* _buffer, size_t _bufferSize,
    std::vector<WNLogColorElement>* _colors) {
  mLogger1.FlushExternal(_buffer, _bufferSize, _colors);
  mLogger2.FlushExternal(_buffer, _bufferSize, _colors);
}
#endif  //__WN_MULTI_LOGGER_INL__
