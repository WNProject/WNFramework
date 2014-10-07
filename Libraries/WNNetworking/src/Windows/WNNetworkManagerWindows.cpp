////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNNetworking/inc/Internal/Windows/WNNetworkManagerWindows.h"
#include "WNNetworking/inc/Internal/Windows/WNListenConnectionWindows.h"
#include "WNNetworking/inc/Internal/Windows/WNInConnectionWindows.h"
#include "WNNetworking/inc/Internal/Windows/WNOutConnectionWindows.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNConcurrency/inc/WNLockGuard.h"


#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non – DLL-interface used as base for DLL-interface
    #pragma warning(disable: 4996)
#endif

#include <algorithm>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace WNNetworking {
    class WNWSAManager {
    public:
        WNWSAManager() :
            mCount(0) {
        }

        WN_BOOL AddToCount() {
            mLock.Lock();

            if (mCount == 0) {
                if (WSAStartup(MAKEWORD(2, 2), &mWSAData) != NO_ERROR) {
                    return(WN_FALSE);
                }

                ++mCount;
            }

            mLock.Unlock();

            return(WN_TRUE);
        }

        WN_VOID RemoveFromCount() {
            mLock.Lock();

            if (mCount > 0) {
                --mCount;

                if (mCount == 0) {
                    WSACleanup();
                }
            }

            mLock.Unlock();
        }
    private:
        WN_SIZE_T mCount;
        WNConcurrency::WNSpinLock mLock;
        WSAData mWSAData;
    };
}

WN_VOID WNNetworking::WNNetworkManagerWindows::IOCPThread() {
    LPOVERLAPPED overlapped = NULL;
    DWORD bytesTransferred = 0;
    ULONG_PTR completionKey = 0;

    for (;;) {
        const BOOL ret = GetQueuedCompletionStatus(mIOCP, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        WNConnectionWindows* connection = reinterpret_cast<WNConnectionWindows*>(completionKey);

        if (!ret) {
            //SOME ERROR OCCURRED HERE, not sur whats up
            continue;
        } else if (completionKey != 0 && overlapped == WN_NULL && bytesTransferred != 0) {
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
        } else if (overlapped == WN_NULL && bytesTransferred == 0) {
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
                    WN_RELEASE_ASSERT_DESC(WN_FALSE, "Bad operation on this socket");
            }
        }
    }
}

WN_VOID WNNetworking::WNNetworkManagerWindows::ListenThread() {
    WSANETWORKEVENTS events;

    while (WAIT_OBJECT_0 != WaitForSingleObject(mShutdownEvent, 0)) {
        if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &mAcceptEvent, FALSE, 1, FALSE)) {
            WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> lock(mListenerMutex);

            for (std::list<WNListenConnectionWindows*>::iterator i = mIncommingConnections.begin(); i != mIncommingConnections.end();) {
                WSAEnumNetworkEvents((*i)->GetWindowsSocket(), mAcceptEvent, &events);

                if (events.lNetworkEvents & FD_ACCEPT && (0 == events.iErrorCode[FD_ACCEPT_BIT])) {
                    WNInConnectionWindows* conn = (*i)->AcceptConnection();

                    if (WN_NULL != conn) {
                        const HANDLE temp = CreateIoCompletionPort((HANDLE)conn->GetWindowsSocket(), mIOCP, reinterpret_cast<ULONG_PTR>(conn), 0);

                        if (NULL == temp) {
                            WN_DELETE(conn);

                            continue;
                        }

                        {
                            WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mRecievedMutex);
                            mReceivedConnections.push_back(conn);
                        }

                        PostQueuedCompletionStatus(mIOCP, WNNetworkManager::eWNCallbackListener, reinterpret_cast<ULONG_PTR>(conn), WN_NULL);
                    }

                    ++i;
                } else if (events.lNetworkEvents & FD_ACCEPT && (0 != events.iErrorCode[FD_ACCEPT_BIT])) {
                    i = mIncommingConnections.erase(i);
                    (*i)->Invalidate();

                    WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mInvalidMutex);

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
    mListenThread(WN_NULL) {
}

WNNetworking::WNNetworkManagerWindows::~WNNetworkManagerWindows() {
    Cleanup();
}

WNNetworking::WNNetworkManagerReturnCode::Type WNNetworking::WNNetworkManagerWindows::ConnectTo(WNConnection*& _outHandle, WNConnectionType::Type _type, const WN_CHAR* _target, WN_UINT16 _port) {
    WNOutConnectionWindows* conn = WN_NEW WNOutConnectionWindows(*this);

    if (conn->Initialize(_type, _target, _port) != WNNetworkManagerReturnCode::eWNOK) {
        WN_DELETE(conn);
    }

    if (CreateIoCompletionPort((HANDLE)conn->GetWindowsSocket(), mIOCP, reinterpret_cast<ULONG_PTR>(conn), 0) == NULL) {
        WN_DELETE(conn);

        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mOutgoingMutex);

    mOutgoingConnections.push_back(conn);
    conn->Receive();
    _outHandle = conn;

    return(WNNetworkManagerReturnCode::eWNOK);
}

WNNetworking::WNNetworkManagerReturnCode::Type WNNetworking::WNNetworkManagerWindows::Initialize(WN_UINT32 _numWorkerThreads) { 
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
    mIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, WN_NULL, 0, 0);

    if (INVALID_HANDLE_VALUE == mIOCP) {
        Cleanup();

        return(WNNetworkManagerReturnCode::eWNSystemInitializationFailed);
    }

    mInitializationState = eWNIOCPCreated;

    for (WN_UINT32 i = 0; i < mMaxThreads; ++i) {
        WNConcurrency::WNThread<WN_VOID>* thread = WN_NEW WNConcurrency::WNThread<WN_VOID>();

        if (thread->Initialize(this, &WNNetworkManagerWindows::IOCPThread) != WNConcurrency::WNThreadReturnCode::eWNOK) {
            WN_DELETE(thread);

            mInitializationState = eWNThreadsCreated;

            Cleanup();

            return(WNNetworkManagerReturnCode::eWNThreadCreationError);
        } else {
            mThreads.push_back(thread);
        }
    }

    mInitializationState = eWNThreadsCreated;
    mListenThread = WN_NEW WNConcurrency::WNThread<WN_VOID>();

    if (mListenThread->Initialize(this, &WNNetworkManagerWindows::ListenThread) != WNConcurrency::WNThreadReturnCode::eWNOK) {
        WN_DELETE(mListenThread);

        Cleanup();

        return(WNNetworkManagerReturnCode::eWNThreadCreationError);
    } 

    mInitializationState = eWNInitializationCompleted;

    return(WNNetworkManagerReturnCode::eWNOK);
}

WN_VOID WNNetworking::WNNetworkManagerWindows::Cleanup() {
    switch(mInitializationState) {
        case eWNInitializationCompleted:
            SetEvent(mShutdownEvent);

            mListenThread->WaitForCompletion();
        case eWNThreadsCreated:
            for (WN_SIZE_T i = 0; i < mThreads.size(); ++i) {
                PostQueuedCompletionStatus(mIOCP, 0, NULL, 0);
            }

            for (WN_UINT32 i = 0; i < static_cast<WN_UINT32>(mThreads.size()); ++i) {
                mThreads[i]->WaitForCompletion();
                WN_DELETE(mThreads[i]);
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

WNNetworking::WNNetworkManagerReturnCode::Type WNNetworking::WNNetworkManagerWindows::CreateListener(WNConnection*& _outHandle, WNConnectionType::Type _type, WN_UINT16 _port, const WNConnectedCallback& _callback) {
    WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable, "We do not support unreliable connections yet");

    WNListenConnectionWindows* listenConnection = WN_NEW WNListenConnectionWindows(*this, _type, _port, _callback);
    const WNNetworkManagerReturnCode::Type err = listenConnection->Initialize();

    if (err != WNNetworkManagerReturnCode::eWNOK) {
        WN_DELETE(listenConnection);

        return(err);
    }

    const SOCKET s = listenConnection->GetWindowsSocket();

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mListenerMutex);

        mIncommingConnections.push_back(listenConnection);

        if (SOCKET_ERROR == WSAEventSelect(s, mAcceptEvent, FD_ACCEPT)) {
               mIncommingConnections.pop_back();

               WN_DELETE(listenConnection);

               return(WNNetworkManagerReturnCode::eWNCannotListen);
        }
    }

    _outHandle = listenConnection;

    return(WNNetworkManagerReturnCode::eWNOK);
}

WN_VOID WNNetworking::WNNetworkManagerWindows::DestroyConnection(WNConnection* _connection) {
     {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mRecievedMutex);
        std::list<WNInConnectionWindows*>::iterator i = std::find(mReceivedConnections.begin(), mReceivedConnections.end(), _connection);

        if (i != mReceivedConnections.end()) {
            (*i)->Invalidate();
            mReceivedConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mOutgoingMutex);
        std::list<WNOutConnectionWindows*>::iterator i = std::find(mOutgoingConnections.begin(), mOutgoingConnections.end(), _connection);

        if (i != mOutgoingConnections.end()) {
             (*i)->Invalidate();
            mOutgoingConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mListenerMutex);
        std::list<WNListenConnectionWindows*>::iterator i = std::find(mIncommingConnections.begin(), mIncommingConnections.end(), _connection);

        if (i != mIncommingConnections.end()) {
             (*i)->Invalidate();
            mIncommingConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mInvalidMutex);

        mInvalidConnections.push_back(_connection);
    }
}
