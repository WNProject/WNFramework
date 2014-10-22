// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_BUFFER_LOGGER_H__
#define __WN_BUFFER_LOGGER_H__
#include "WNCore/inc/WNBase.h"
#include "WNLogging/inc/WNLog.h"
#include <vector>
namespace WNLogging {
    
    template<WN_VOID (*T_FlushCB)(WN_VOID*, const WN_CHAR*, WN_SIZE_T, const std::vector<WNLogColorElement>&)>
    class WNBufferLogger: public WNLogger {
    public:
        WNBufferLogger(WN_VOID* context);
        ~WNBufferLogger();
        virtual WN_INLINE WN_VOID FlushBuffer(const WN_CHAR* _buffer, WN_SIZE_T bufferSize, const std::vector<WNLogColorElement>& mColors);
    private:
        WN_VOID* mContext;
    };
};

#include "WNLogging/inc/Internal/WNBufferLogger.inl"
#endif
