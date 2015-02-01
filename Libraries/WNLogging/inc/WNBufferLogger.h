// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_BUFFER_LOGGER_H__
#define __WN_BUFFER_LOGGER_H__
#include "WNCore/inc/WNBase.h"
#include "WNLogging/inc/WNLog.h"
#include <vector>
namespace WNLogging {
    
    template<wn_void (*T_FlushCB)(wn_void*, const wn_char*, wn_size_t, const std::vector<WNLogColorElement>&)>
    class WNBufferLogger: public WNLogger {
    public:
        WNBufferLogger(wn_void* context);
        ~WNBufferLogger();
        virtual WN_INLINE wn_void FlushBuffer(const wn_char* _buffer, wn_size_t bufferSize, const std::vector<WNLogColorElement>& mColors);
    private:
        wn_void* mContext;
    };
};

#include "WNLogging/inc/Internal/WNBufferLogger.inl"
#endif
