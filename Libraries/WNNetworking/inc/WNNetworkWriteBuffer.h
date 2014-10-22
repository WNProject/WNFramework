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
#include "WNConcurrency/inc/WNResourcePointer.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non – DLL-interface used as base for DLL-interface
#endif

#include <vector>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace WNContainers {
    template <typename Type>
    class WNResourcePointer;
    class WNSerializerBase;
}

namespace WNNetworking {
    class WNNetworkManager;
    class WNBufferResource;

    class WNNetworkWriteBuffer : public WNContainers::WNDataBuffer {
    public:
        typedef std::vector<WNConcurrency::WNResourcePointer<WNNetworking::WNBufferResource> > WNBufferQueue;

    public:
        WNNetworkWriteBuffer(WNNetworkManager& _manager, WN_UINT32 _number);
        WNNetworkWriteBuffer(const WNNetworkWriteBuffer& _other);
        virtual WN_FORCE_INLINE ~WNNetworkWriteBuffer() {}

        WNNetworkWriteBuffer& operator = (const WNNetworkWriteBuffer& _other);

        virtual WN_BOOL Serialize(const WN_UINT32 _flags, const WNContainers::WNSerializerBase& _serializer);
        virtual WN_CHAR* ReserveBytes(const WN_SIZE_T _numBytes, WN_SIZE_T& _returnedBytes);
        virtual WNContainers::WNDataBufferType GetType();
        const WNBufferQueue& GetChunks() const;
        WN_VOID FlushWrite();

    private:
        WNNetworkManager& mManager;
        WNBufferQueue mChunks;

        WN_BOOL mFlushed;
        WN_SIZE_T mTotalWritten;
        WN_SIZE_T mBufferPointer;
    };
}

#endif // __WN_NETWORKING_NETWORK_WRITE_BUFFER__
