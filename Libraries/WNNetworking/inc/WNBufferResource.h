// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_BUFFER_RESOURCE_H__
#define __WN_NETWORKING_BUFFER_RESOURCE_H__

#include "WNConcurrency/inc/WNResourceBase.h"

namespace WNNetworking {
    class WNNetworkManager;

    class WNBufferResource : public WNConcurrency::WNResourceBase {
    public:
        WNBufferResource(const WNNetworkManager& _manager);
        virtual ~WNBufferResource();

        WN_CHAR* GetPointer() const;
        WN_CHAR* GetBaseLocation() const;
        WN_SIZE_T GetSize() const;
        WN_SIZE_T GetWritten() const;
        WN_VOID FlushWrite();
        WN_VOID PrepareRead();
        WN_VOID AddData(WN_SIZE_T _data);
        WN_VOID FillData();
        WN_VOID Clear();

        #ifdef _WN_WINDOWS
            WN_FORCE_INLINE const WSABUF* GetWriteWinBuf() const;
            WN_FORCE_INLINE WSABUF* GetReadWinBuf();
        #endif

    private:
        WNBufferResource& operator = (const WNBufferResource&);

    private:
        const WNNetworkManager& mManager;

        #ifdef _WN_WINDOWS
            WSABUF mWinBuf;
        #endif

        WN_CHAR* mBuffer;
        WN_SIZE_T mBaseLocation;
        WN_SIZE_T mCurLocation;
    };
}

#ifdef _WN_WINDOWS
    #include "WNNetworking/inc/Internal/Windows/WNBufferResource.inl"
#endif

#endif // __WN_NETWORKING_BUFFER_RESOURCE_H__
