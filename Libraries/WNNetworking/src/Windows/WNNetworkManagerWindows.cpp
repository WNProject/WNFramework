// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/Internal/Windows/WNNetworkManagerWindows.h"
#include "WNNetworking/inc/Internal/Windows/WNListenConnectionWindows.h"
#include "WNNetworking/inc/Internal/Windows/WNInConnectionWindows.h"
#include "WNNetworking/inc/Internal/Windows/WNOutConnectionWindows.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNThreading/inc/WNThread.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non – DLL-interface used as base for DLL-interface
    #pragma warning(disable: 4996)
#endif

#include <algorithm>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

#include <mutex>

namespace WNNetworking {
    class WNWSAManager {
    public:
        WNWSAManager() :
            mCount(0) {
        }

        wn_bool AddToCount() {
            std::lock_guard<wn::threading::spin_lock> guard(mLock);

            if (mCount == 0) {
                if (::WSAStartup(MAKEWORD(2, 2), &mWSAData) != NO_ERROR) {
                    return(wn_false);
                }

                ++mCount;
            }

            return(wn_true);
        }

        wn_void RemoveFromCount() {
            std::lock_guard<wn::threading::spin_lock> guard(mLock);

            if (mCount > 0) {
                --mCount;

                if (mCount == 0) {
                    WSACleanup();
                }
            }
        }
    private:
        wn_size_t mCount;
        wn::threading::spin_lock mLock;
        WSAData mWSAData;
    };
}

wn_void WNNetworking::WNNetworkManagerWindows::IOCPThread() {
    LPOVERLAPPED overlapped = NULL;
    DWORD bytesTransferred = 0;
    ULONG_PTR completionKey = 0;

    for (;;) {
        const BOOL ret = ::GetQueuedCompletionStatus(mIOCP, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        WNConnectionWindows* connection = reinterpret_cast<WNConnectionWindows*>(completionKey);

        if (!ret) {
            //SOME ERROR OCCURRED HERE, not sur whats up
            continue;
        } else if (completionKey != 0 && overlapped == wn_nullptr && bytesTransferred != 0) {
            //USER MESSAGE!
            if (bytesTransferred == WNNetworkManager::eWNCallbackListener) {
                WNInConnectionWindows* conn = reinterpret_cast<WNInConnectionWindows*>(connection);

                if (!conn->FireCallback() || !conn->Receive()) {
                   DestroyConnection(conn);
                }
            }
        } else if (completionKey == 0 && overlapped == NULL && bytesTransferred == 0) {
            //We got our shutdown request.. lets leave
            return;
        } else if (overlapped == wn_nullptr && bytesTransferred == 0) {
            //CONNECTION CLOSED
        } else {
            switch (connection->GetOperation(overlapped)) {
                case WNConnectionWindows::eWNWriteOp:
                    connection->ProcessWrite();

                    break;
                case WNConnectionWindows::eWNReadOp:
                    connection->ProcessRead(this, bytesTransferred);

                    if (!connection->Receive()) {
                        DestroyConnection(connection);
                    }

                    break;
                case WNConnectionWindows::eWNNoOp:
                default:
                    WN_RELEASE_ASSERT_DESC(wn_false, "Bad operation on this socket");
            }
        }
    }
}

wn_void WNNetworking::WNNetworkManagerWindows::ListenThread() {
    WSANETWORKEVENTS events;

    while (WAIT_OBJECT_0 != WaitForSingleObject(mShutdownEvent, 0)) {
        if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &mAcceptEvent, FALSE, 1, FALSE)) {
            std::lock_guard<wn::threading::spin_lock> lock(mListenerMutex);

            for (std::list<WNListenConnectionWindows*>::iterator i = mIncommingConnections.begin(); i != mIncommingConnections.end();) {
                WSAEnumNetworkEvents((*i)->GetWindowsSocket(), mAcceptEvent, &events);

                if (events.lNetworkEvents & FD_ACCEPT && (0 == events.iErrorCode[FD_ACCEPT_BIT])) {
                    WNInConnectionWindows* conn = (*i)->AcceptConnection();

                    if (wn_nullptr != conn) {
                        const HANDLE temp = CreateIoCompletionPort((HANDLE)conn->GetWindowsSocket(), mIOCP, reinterpret_cast<ULONG_PTR>(conn), 0);

                        if (NULL == temp) {
                            wn::memory::destroy(conn);

                            continue;
                        }

                        {
                            std::lock_guard<wn::threading::spin_lock> guard(mRecievedMutex);
                            mReceivedConnections.push_back(conn);
                        }

                        PostQueuedCompletionStatus(mIOCP, WNNetworkManager::eWNCallbackListener, reinterpret_cast<ULONG_PTR>(conn), wn_nullptr);
                    }

                    ++i;
                } else if (events.lNetworkEvents & FD_ACCEPT && (0 != events.iErrorCode[FD_ACCEPT_BIT])) {
                    i = mIncommingConnections.erase(i);
                    (*i)->Invalidate();

                    std::lock_guard<wn::threading::spin_lock> guard(mInvalidMutex);

                    mInvalidConnections.push_back(*i);
                } else {
                    ++i;
                }
            }
        }
    }
}

static WNNetworking::WNWSAManager sWSAManager;

WNNetworking::WNNetworkManagerWindows::WNNetworkManagerWindows() :
    WNNetworkManager(),
    mMaxThreads(0),
    mIOCP(INVALID_HANDLE_VALUE),
    mAcceptEvent(INVALID_HANDLE_VALUE),
    mShutdownEvent(INVALID_HANDLE_VALUE),
    mInitializationState(eWNNotStarted),
    mListenThread(wn_nullptr) {
}

WNNetworking::WNNetworkManagerWindows::~WNNetworkManagerWindows() {
    Cleanup();
}

WNNetworking::WNNetworkManagerReturnCode::type WNNetworking::WNNetworkManagerWindows::ConnectTo(WNConnection*& _outHandle, WNConnectionType::type _type, const wn_char* _target, wn_uint16 _port) {
    WNOutConnectionWindows* conn = wn::memory::construct<WNOutConnectionWindows>(*this);

    if (conn->Initialize(_type, _target, _port) != WNNetworkManagerReturnCode::ok) {
        wn::memory::destroy(conn);
    }

    if (CreateIoCompletionPort((HANDLE)conn->GetWindowsSocket(), mIOCP, reinterpret_cast<ULONG_PTR>(conn), 0) == NULL) {
        wn::memory::destroy(conn);

        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    std::lock_guard<wn::threading::spin_lock> guard(mOutgoingMutex);

    mOutgoingConnections.push_back(conn);
    conn->Receive();
    _outHandle = conn;

    return(WNNetworkManagerReturnCode::ok);
}

WNNetworking::WNNetworkManagerReturnCode::type WNNetworking::WNNetworkManagerWindows::Initialize(wn_uint32 _numWorkerThreads) {
    if (!sWSAManager.AddToCount()) {
        Cleanup();

        return(WNNetworkManagerReturnCode::eWNSystemInitializationFailed);
    }

    mMaxThreads = _numWorkerThreads;
    mInitializationState = eWNWSAStartup;
    mAcceptEvent = WSACreateEvent();

    if (WSA_INVALID_EVENT == mAcceptEvent) {
        Cleanup();

        return(WNNetworkManagerReturnCode::eWNSystemInitializationFailed);
    }

    mInitializationState = eWNAcceptEventCreated;
    mShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if(INVALID_HANDLE_VALUE == mShutdownEvent) {
        Cleanup();

        return(WNNetworkManagerReturnCode::eWNSystemInitializationFailed);
    }

    mInitializationState = eWNShutdownEventCreated;
    mIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, wn_nullptr, 0, 0);

    if (INVALID_HANDLE_VALUE == mIOCP) {
        Cleanup();

        return(WNNetworkManagerReturnCode::eWNSystemInitializationFailed);
    }

    mInitializationState = eWNIOCPCreated;

    for (wn_uint32 i = 0; i < mMaxThreads; ++i) {
        mThreads.push_back(wn::memory::construct<wn::threading::thread<wn_void>>(&WNNetworkManagerWindows::IOCPThread, this));
    }

    mInitializationState = eWNThreadsCreated;
    mListenThread = wn::memory::construct<wn::threading::thread<wn_void>>(&WNNetworkManagerWindows::ListenThread, this);

    mInitializationState = eWNInitializationCompleted;

    return(WNNetworkManagerReturnCode::ok);
}

wn_void WNNetworking::WNNetworkManagerWindows::Cleanup() {
    switch(mInitializationState) {
        case eWNInitializationCompleted:
            SetEvent(mShutdownEvent);

            mListenThread->join();
        case eWNThreadsCreated:
            for (wn_size_t i = 0; i < mThreads.size(); ++i) {
                PostQueuedCompletionStatus(mIOCP, 0, NULL, 0);
            }

            for (wn_uint32 i = 0; i < static_cast<wn_uint32>(mThreads.size()); ++i) {
                mThreads[i]->join();
                wn::memory::destroy(mThreads[i]);
            }

            mThreads.clear();
        case eWNIOCPCreated:
            CloseHandle(mIOCP);

            mIOCP = INVALID_HANDLE_VALUE;
        case eWNShutdownEventCreated:
            CloseHandle(mShutdownEvent);

            mShutdownEvent = INVALID_HANDLE_VALUE;
        case eWNAcceptEventCreated:
            WSACloseEvent(mAcceptEvent);

            mAcceptEvent = INVALID_HANDLE_VALUE;
        case eWNWSAStartup:
            sWSAManager.RemoveFromCount();
        case eWNNotStarted:
            break;
    }

    mInitializationState = eWNNotStarted;

    WNNetworkManager::Cleanup();
}

WNNetworking::WNNetworkManagerReturnCode::type WNNetworking::WNNetworkManagerWindows::CreateListener(WNConnection*& _outHandle, WNConnectionType::type _type, wn_uint16 _port, const WNConnectedCallback& _callback) {
    WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable, "We do not support unreliable connections yet");

    WNListenConnectionWindows* listenConnection = wn::memory::construct<WNListenConnectionWindows>(*this, _type, _port, _callback);
    const WNNetworkManagerReturnCode::type err = listenConnection->Initialize();

    if (err != WNNetworkManagerReturnCode::ok) {
        wn::memory::destroy(listenConnection);

        return(err);
    }

    const SOCKET s = listenConnection->GetWindowsSocket();

    {
        std::lock_guard<wn::threading::spin_lock> guard(mListenerMutex);

        mIncommingConnections.push_back(listenConnection);

        if (SOCKET_ERROR == WSAEventSelect(s, mAcceptEvent, FD_ACCEPT)) {
               mIncommingConnections.pop_back();

               wn::memory::destroy(listenConnection);

               return(WNNetworkManagerReturnCode::eWNCannotListen);
        }
    }

    _outHandle = listenConnection;

    return(WNNetworkManagerReturnCode::ok);
}

wn_void WNNetworking::WNNetworkManagerWindows::DestroyConnection(WNConnection* _connection) {
     {
         std::lock_guard<wn::threading::spin_lock> guard(mRecievedMutex);
        std::list<WNInConnectionWindows*>::iterator i = std::find(mReceivedConnections.begin(), mReceivedConnections.end(), _connection);

        if (i != mReceivedConnections.end()) {
            (*i)->Invalidate();
            mReceivedConnections.erase(i);
        }
    }

    {
        std::lock_guard<wn::threading::spin_lock> guard(mOutgoingMutex);
        std::list<WNOutConnectionWindows*>::iterator i = std::find(mOutgoingConnections.begin(), mOutgoingConnections.end(), _connection);

        if (i != mOutgoingConnections.end()) {
             (*i)->Invalidate();
            mOutgoingConnections.erase(i);
        }
    }

    {
        std::lock_guard<wn::threading::spin_lock> guard(mListenerMutex);
        std::list<WNListenConnectionWindows*>::iterator i = std::find(mIncommingConnections.begin(), mIncommingConnections.end(), _connection);

        if (i != mIncommingConnections.end()) {
             (*i)->Invalidate();
            mIncommingConnections.erase(i);
        }
    }

    {
        std::lock_guard<wn::threading::spin_lock> guard(mInvalidMutex);

        mInvalidConnections.push_back(_connection);
    }
}
