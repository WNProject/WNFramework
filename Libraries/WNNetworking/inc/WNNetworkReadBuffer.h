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
#include "WNMemory/inc/WNIntrusivePtr.h"

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
    template <typename type>
    class intrusive_ptr;
}

namespace WNNetworking {
    class WNNetworkManager;

    class WNNetworkReadBuffer : public WNContainers::WNDataBuffer {
    public:
        typedef std::deque<wn::memory::intrusive_ptr<WNNetworking::WNBufferResource> > WNBufferQueue;

    public:
        WNNetworkReadBuffer(WNNetworkManager& _manager);
        virtual WN_FORCE_INLINE ~WNNetworkReadBuffer() {}

        virtual wn_bool Serialize(const wn_uint32 _flags, const WNContainers::WNSerializerBase& _serializer);
        virtual wn_char* ReserveBytes(const wn_size_t _numBytes, wn_size_t& _returnedBytes);
        virtual WNContainers::WNDataBufferType GetType();
        wn_void AppendBuffer(wn::memory::intrusive_ptr<WNBufferResource>& _buffer, wn_size_t _dataCount, wn_size_t _dataOffset);
        wn_bool Initialized();
        wn_void Clear();
        wn_char* GetLastBuffer();
        wn_void PrepareRead();

    private:
        WNNetworkReadBuffer& operator = (const WNNetworkReadBuffer&);

    private:
        WNNetworkManager& mManager;
        wn::memory::intrusive_ptr<WNNetworking::WNBufferResource> mDataOverflow;
        WNBufferQueue mChunks;

        WNBufferQueue::iterator mCurrentChunk;
        wn_size_t mBufferPointer;
        wn_size_t mWriteOffset;
        wn_size_t mTotalSize;
        wn_size_t mTotalRead;
        wn_bool   mLastChunk;
        wn_bool   mInitialized;
    };
}

#endif // __WN_NETWORKING_NETWORK_READ_BUFFER_H__
