// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_LINUX_CONNECTION_H__
#define __WN_NETWORKING_LINUX_CONNECTION_H__

#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"
#include "WNNetworking/inc/WNNetworkReadBuffer.h"

namespace WNNetworking {
    class WNNetworkManagerLinux;
    class WNNetworkWriteBuffer;

    class WNConnectionLinux : public WNConnection {
    public:
        WNConnectionLinux(WNNetworkManager& _manager);
        virtual ~WNConnectionLinux();

        virtual wn_void Invalidate();
        virtual wn_void SendBuffer(WNNetworkWriteBuffer& _buffer);
        wn_int32 GetLinuxSocket();
        wn_void NotifyReadyToSend(wn_bool socketFree);
        wn_void NotifyReadReady();

    protected:
        wn_void ReadReady();
        wn_bool Send();
        WNNetworkManager& mManager;
        wn_bool mReadyToWrite;
        wn_int32 mSockFD;
        wn_size_t mBufferChunkCount;
        wn_size_t mBufferWritten;
        wn_size_t mTotalSent;
        wn_atom_t mReadAtomic;
        wn_atom_t mWriteAtomic;
        wn::spin_lock mReadLock;
        wn::spin_lock mWriteLock;
        wn::spin_lock mSendBufferLock;
        std::deque<WNNetworkWriteBuffer> mSendBuffers;

        WNNetworkReadBuffer mCurrentReadBuffer;
        wn_size_t mInProcessedBytes;
        wn_size_t mReadHead;
        wn_size_t mBufferBase;
        wn::intrusive_ptr<WNBufferResource> mReadLocation;
        wn_size_t mOverflowAmount;
        wn_char mOverflowLocation[8];
    };
};

#endif // __WN_NETWORKING_LINUX_CONNECTION_H__
