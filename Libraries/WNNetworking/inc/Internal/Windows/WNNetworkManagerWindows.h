// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__
#define __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNConcurrency/inc/WNSpinLock.h"

#include <vector>

namespace wn {
    template <typename Return>
    class thread;
}

namespace WNNetworking {
    class WNListenConnectionWindows;
    class WNInConnectionWindows;
    class WNConnectionWindows;
    class WNOutConnectionWindows;

    class WNNetworkManagerWindows : public WNNetworkManager {
    public:
        WNNetworkManagerWindows();
        virtual ~WNNetworkManagerWindows();

        virtual WNNetworkManagerReturnCode::type Initialize(wn_uint32 _numWorkerThreads);
        virtual WNNetworkManagerReturnCode::type ConnectTo(WNConnection*& _outHandle, WNConnectionType::type _type, const wn_char* _target, wn_uint16 _port);
        virtual WNNetworkManagerReturnCode::type CreateListener(WNConnection*& _outHandle, WNConnectionType::type _type, wn_uint16 _port, const WNConnectedCallback& _callback);
        virtual wn_void Cleanup();
        virtual wn_void DestroyConnection(WNConnection* _connection);

    private:
        wn_void IOCPThread();
        wn_void ListenThread();

        enum eWNWindowsInitializationState {
            eWNNotStarted,
            eWNWSAStartup,
            eWNAcceptEventCreated,
            eWNShutdownEventCreated,
            eWNIOCPCreated,
            eWNThreadsCreated,
            eWNInitializationCompleted
        };

        wn_uint32 mMaxThreads;
        HANDLE mIOCP;
        HANDLE mAcceptEvent;
        HANDLE mShutdownEvent;
        eWNWindowsInitializationState mInitializationState;
        wn::concurrency::thread<wn_void>* mListenThread;
        wn::concurrency::spin_lock mListenerMutex;
        wn::concurrency::spin_lock mRecievedMutex;
        wn::concurrency::spin_lock mInvalidMutex;
        wn::concurrency::spin_lock mOutgoingMutex;

        std::vector<wn::concurrency::thread<wn_void>*> mThreads;
        std::list<WNListenConnectionWindows*> mIncommingConnections;
        std::list<WNOutConnectionWindows*> mOutgoingConnections;
        std::list<WNInConnectionWindows*> mReceivedConnections;
        std::list<WNConnection*> mInvalidConnections;
    };
};

#endif // __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__
