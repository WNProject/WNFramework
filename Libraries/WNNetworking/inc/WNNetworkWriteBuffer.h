// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_WRITE_BUFFER_H__
#define __WN_NETWORKING_NETWORK_WRITE_BUFFER_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNDataBuffer.h"
#include "WNNetworking/inc/WNBufferResource.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non – DLL-interface used as base for DLL-interface
#endif

#include <vector>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace WNContainers {
    template <typename type>
    class intrusive_ptr;
    class WNSerializerBase;
}

namespace WNNetworking {
    class WNNetworkManager;
    class WNBufferResource;

    class WNNetworkWriteBuffer : public wn::containers::data_buffer {
    public:
        typedef std::vector<wn::memory::intrusive_ptr<WNNetworking::WNBufferResource>> WNBufferQueue;

    public:
        WNNetworkWriteBuffer(WNNetworkManager& _manager, wn_uint32 _number);
        WNNetworkWriteBuffer(const WNNetworkWriteBuffer& _other);
        virtual WN_FORCE_INLINE ~WNNetworkWriteBuffer() {}

        WNNetworkWriteBuffer& operator = (const WNNetworkWriteBuffer& _other);

        virtual wn_bool serialize(const wn::containers::serializer_base& _serializer, const wn_uint32 _flags) override;
        virtual wn_char* reserve(const wn_size_t _numBytes, wn_size_t& _returnedBytes) override;
        virtual wn::containers::data_buffer_type type() const override;

        const WNBufferQueue& GetChunks() const;
        wn_void FlushWrite();

    private:
        WNNetworkManager& mManager;
        WNBufferQueue mChunks;

        wn_bool mFlushed;
        wn_size_t mTotalWritten;
        wn_size_t mBufferPointer;
    };
}

#endif // __WN_NETWORKING_NETWORK_WRITE_BUFFER__
