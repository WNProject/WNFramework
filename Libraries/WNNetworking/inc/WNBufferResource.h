// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_BUFFER_RESOURCE_H__
#define __WN_NETWORKING_BUFFER_RESOURCE_H__

#include "WNMemory/inc/WNIntrusivePtr.h"

namespace WNNetworking {
    class WNNetworkManager;

    class WNBufferResource : public wn::memory::intrusive_ptr_base {
    public:
        WNBufferResource(const WNNetworkManager& _manager);
        virtual ~WNBufferResource();

        char* GetPointer() const;
        char* GetBaseLocation() const;
        size_t GetSize() const;
        size_t GetWritten() const;
        void FlushWrite();
        void PrepareRead();
        void AddData(size_t _data);
        void FillData();
        void Clear();

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

        char* mBuffer;
        size_t mBaseLocation;
        size_t mCurLocation;
    };
}

#ifdef _WN_WINDOWS
    #include "WNNetworking/inc/Internal/Windows/WNBufferResource.inl"
#endif

#endif // __WN_NETWORKING_BUFFER_RESOURCE_H__
