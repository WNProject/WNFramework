// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_READ_BUFFER_H__
#define __WN_NETWORKING_NETWORK_READ_BUFFER_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNDataBuffer.h"
#include "WNNetworking/inc/WNBufferResource.h"
#include "WNConcurrency/inc/WNResourcePointer.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non – DLL-interface used as base for DLL-interface
#endif

#include <deque>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace WNContainers {
    class WNSerializerBase;
}

namespace WNConcurrency {
    template <typename Type>
    class WNResourcePointer;
}

namespace WNNetworking {
    class WNNetworkManager;

    class WNNetworkReadBuffer : public WNContainers::WNDataBuffer {
    public:
        typedef std::deque<WNConcurrency::WNResourcePointer<WNNetworking::WNBufferResource> > WNBufferQueue;

    public:
        WNNetworkReadBuffer(WNNetworkManager& _manager);
        virtual WN_FORCE_INLINE ~WNNetworkReadBuffer() {}

        virtual WN_BOOL Serialize(const WN_UINT32 _flags, const WNContainers::WNSerializerBase& _serializer);
        virtual WN_CHAR* ReserveBytes(const WN_SIZE_T _numBytes, WN_SIZE_T& _returnedBytes);
        virtual WNContainers::WNDataBufferType GetType();
        WN_VOID AppendBuffer(WNConcurrency::WNResourcePointer<WNBufferResource>& _buffer, WN_SIZE_T _dataCount, WN_SIZE_T _dataOffset);
        WN_BOOL Initialized();
        WN_VOID Clear();
        WN_CHAR* GetLastBuffer();
        WN_VOID PrepareRead();

    private:
        WNNetworkReadBuffer& operator = (const WNNetworkReadBuffer&);

    private:
        WNNetworkManager& mManager;
        WNConcurrency::WNResourcePointer<WNNetworking::WNBufferResource> mDataOverflow;
        WNBufferQueue mChunks;

        WNBufferQueue::iterator mCurrentChunk;
        WN_SIZE_T mBufferPointer;
        WN_SIZE_T mWriteOffset;
        WN_SIZE_T mTotalSize;
        WN_SIZE_T mTotalRead;
        WN_BOOL   mLastChunk;
        WN_BOOL   mInitialized;
    };
}

#endif // __WN_NETWORKING_NETWORK_READ_BUFFER_H__
