// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_WINDOWS_CONNECTION_H__
#define __WN_NETWORKING_WINDOWS_CONNECTION_H__

#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkReadBuffer.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"
#include "WNNetworking/inc/WNBufferResource.h"
#include "WNConcurrency/inc/WNMutex.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non – DLL-interface used as base for DLL-interface
    #pragma warning(disable: 4996)
#endif

#include <deque>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace WNNetworking {
    class WNNetworkManagerWindows;

    class WNConnectionWindows : public WNConnection {
    public:
        WNConnectionWindows(WNNetworkManager& _manager);
        virtual ~WNConnectionWindows();

        virtual WN_VOID Invalidate();
        virtual WN_VOID SendBuffer(WNNetworkWriteBuffer& _buffer);

    protected:
        enum eWNNetworkOperation {
            eWNWriteOp,
            eWNReadOp,
            eWNNoOp
        };

    protected:
        friend class WNNetworkManagerWindows;

    protected:
        SOCKET GetWindowsSocket();
        WN_BOOL Receive();
        eWNNetworkOperation GetOperation(LPOVERLAPPED _overlapped);
        WN_BOOL ProcessRead(WNNetworkManagerWindows* _windowsManager, DWORD _bytesTransferred);
        WN_VOID AppendSendBuffer(WNNetworkWriteBuffer& _buff);
        WN_VOID Send();
        WN_VOID ProcessWrite();

    protected:
        WNNetworkManager& mManager;
        WNNetworkReadBuffer mCurrentReadBuffer;
        WNConcurrency::WNMutex mSendMutex;
        std::deque<WNNetworkWriteBuffer> mWriteBuffers;
        std::vector<WSABUF> mWSAWriteBuffers;
        WN_SIZE_T mInProcessedBytes;
        WN_SIZE_T mReadHead;
        WN_SIZE_T mBufferBase;
        WSAOVERLAPPED mReceiveOverlap;
        WSAOVERLAPPED mSendOverlap;
        WNConcurrency::WNResourcePointer<WNBufferResource> mReadLocation;
        SOCKET mSocket;
        WN_SIZE_T mOverflowAmount;
        WN_CHAR mOverflowLocation[8];
    };
};

#endif // __WN_NETWORKING_WINDOWS_CONNECTION_H__
