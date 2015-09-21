// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_MULTI_LOGGER_H__
#define __WN_MULTI_LOGGER_H__
#include "WNLogging/inc/WNLog.h"
namespace WNLogging {

    template<typename T_Logger1, typename T_Logger2>
    class WNMultiLogger: public WNLogger {
    public:
        ~WNMultiLogger();
        wn_void FlushBuffer(const wn_char* _buffer, wn_size_t bufferSize, const std::vector<WNLogColorElement>* mColors);
    private:
        T_Logger1 mLogger1;
        T_Logger2 mLogger2;
    };
};

#include "WNLogging/inc/Internal/WNMultiLogger.inl"
#endif
