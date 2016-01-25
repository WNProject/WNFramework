// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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

namespace WNNetworking {
    class WNNetworkManager;

    class WNNetworkReadBuffer : public wn::containers::data_buffer {
    public:
        typedef std::deque<wn::memory::intrusive_ptr<WNNetworking::WNBufferResource>> WNBufferQueue;

    public:
        WNNetworkReadBuffer(WNNetworkManager& _manager);

        virtual WN_FORCE_INLINE ~WNNetworkReadBuffer() {}

        virtual wn_bool serialize(const wn::containers::serializer_base& _serializer, const wn_uint32 _flags) override;
        virtual wn_char* reserve(const wn_size_t _numBytes, wn_size_t& _returnedBytes) override;
        virtual wn::containers::data_buffer_type type() const override;

        wn_void AppendBuffer(wn::memory::intrusive_ptr<WNBufferResource>& _buffer, wn_size_t _dataCount, wn_size_t _dataOffset);
        wn_bool Initialized();
        wn_void Clear();
        wn_char* GetLastBuffer();
        wn_void PrepareRead();

    private:
        WNNetworkReadBuffer& operator = (const WNNetworkReadBuffer&);

    private:
        wn::memory::basic_allocator allocator;
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
