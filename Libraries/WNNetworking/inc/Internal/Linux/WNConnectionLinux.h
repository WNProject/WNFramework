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

        virtual WN_VOID Invalidate();
        virtual WN_VOID SendBuffer(WNNetworkWriteBuffer& _buffer);
        WN_INT32 GetLinuxSocket();
        WN_VOID NotifyReadyToSend(WN_BOOL socketFree);
        WN_VOID NotifyReadReady();

    protected:
        WN_VOID ReadReady();
        WN_BOOL Send();
        WNNetworkManager& mManager;
        WN_BOOL mReadyToWrite;
        WN_INT32 mSockFD;
        WN_SIZE_T mBufferChunkCount;
        WN_SIZE_T mBufferWritten;
        WN_SIZE_T mTotalSent;
        WN_ATOM_T mReadAtomic;
        WN_ATOM_T mWriteAtomic;
        WNConcurrency::WNSpinLock mReadLock;
        WNConcurrency::WNSpinLock mWriteLock;
        WNConcurrency::WNSpinLock mSendBufferLock;
        std::deque<WNNetworkWriteBuffer> mSendBuffers;

        WNNetworkReadBuffer mCurrentReadBuffer;
        WN_SIZE_T mInProcessedBytes;
        WN_SIZE_T mReadHead;
        WN_SIZE_T mBufferBase;
        WNConcurrency::WNResourcePointer<WNBufferResource> mReadLocation;
        WN_SIZE_T mOverflowAmount;
        WN_CHAR mOverflowLocation[8];
    };
};

#endif // __WN_NETWORKING_LINUX_CONNECTION_H__
