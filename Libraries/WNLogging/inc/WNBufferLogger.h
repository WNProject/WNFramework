// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_BUFFER_LOGGER_H__
#define __WN_BUFFER_LOGGER_H__
#include "WNCore/inc/WNBase.h"
#include "WNLogging/inc/WNLog.h"
#include <vector>
namespace WNLogging {

    template<void (*T_FlushCB)(void*, const char*, size_t, const std::vector<WNLogColorElement>&)>
    class WNBufferLogger: public WNLogger {
    public:
        WNBufferLogger(void* context);
        ~WNBufferLogger();
        virtual WN_INLINE void FlushBuffer(const char* _buffer, size_t bufferSize, const std::vector<WNLogColorElement>& mColors);
    private:
        void* mContext;
    };
};

#include "WNLogging/inc/Internal/WNBufferLogger.inl"
#endif
