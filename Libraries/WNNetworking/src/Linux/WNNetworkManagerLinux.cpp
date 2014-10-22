////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNNetworking/inc/Internal/Linux/WNNetworkManagerLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNListenConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNInConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNOutConnectionLinux.h"
#include "WNMemory/inc/WNMemory.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNBasic.h"
#include "WNConcurrency/inc/WNThread.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNConcurrency/inc/WNLockGuard.h"

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <algorithm>

using namespace WNMemory;
using namespace WNMath;
using namespace WNNetworking;
using namespace WNConcurrency;

WNNetworkManagerLinux::WNNetworkManagerLinux() :
    WNNetworkManager(),
    mInitializationState(eWNNotStarted),
    mWriteEPollInstance(-1), 
    mReadEPollInstance(-1),
    mListenThread(WN_NULL),
    mShuttingDown(WN_FALSE) {
}

WNNetworkManagerLinux::~WNNetworkManagerLinux() {
    Cleanup();
}

WN_VOID WNNetworkManagerLinux::Cleanup() {
    WNNetworkManager::Cleanup();

    mShuttingDown = WN_TRUE;

    if(mInitializationState > eWNEPollListenCreated) {
        close(mListenEPollInstance);
        if (mListenThread != WN_NULL) {
            mListenThread->WaitForCompletion();
            WN_DELETE(mListenThread);
            mListenThread = WN_NULL;
        }
    }

    if(mInitializationState > eWNEPollReadCreated) {
        close(mReadEPollInstance);
        for(WN_SIZE_T i = 0; i < mReadThreads.size(); ++i) {
            mReadThreads[i]->WaitForCompletion();
            WN_DELETE(mReadThreads[i]);
        }
        mReadThreads.clear();
    }

    if(mInitializationState > eWNEPollWriteCreated) {
        close(mWriteEPollInstance);
        for(WN_SIZE_T i = 0; i < mWriteThreads.size(); ++i) {
            mWriteThreads[i]->WaitForCompletion();
            WN_DELETE(mWriteThreads[i]);
        }
        mWriteThreads.clear();
    }
    CleanAllConnections();
}

WNNetworkManagerReturnCode::Type WNNetworkManagerLinux::Initialize(WN_UINT32 _numWorkerThreads) {

    mShuttingDown = WN_FALSE;
#ifdef _WN_ANDROID
    mWriteEPollInstance = epoll_create(2048);
#else
    mWriteEPollInstance = epoll_create1(0);
#endif

    if (mWriteEPollInstance == -1) {
       Cleanup();

       return(WNNetworkManagerReturnCode::eWNSystemInitializationFailed);
    }

    mInitializationState = eWNEPollWriteCreated;

    for (WN_UINT32 i = 0; i < WNCeil(static_cast<WN_FLOAT32>(_numWorkerThreads) / 2.0f); ++i) {
        WNThread<WN_VOID>* thread = WN_NEW WNThread<WN_VOID>();

        if (thread->Initialize(this, &WNNetworkManagerLinux::WriteThread) != WNConcurrency::WNThreadReturnCode::eWNOK) {
            WN_DELETE(thread);

            mInitializationState = eWNWriteThreadsCreated;

            Cleanup();

            return(WNNetworkManagerReturnCode::eWNThreadCreationError);
        }

        mReadThreads.push_back(thread);
    }

    mInitializationState = eWNWriteThreadsCreated;
#ifdef _WN_ANDROID 
    mReadEPollInstance = epoll_create(2048);
#else
    mReadEPollInstance = epoll_create1(0);
#endif

    if (mReadEPollInstance == -1) {
        Cleanup();

        return(WNNetworkManagerReturnCode::eWNSystemInitializationFailed);
    }

    mInitializationState = eWNEPollReadCreated;

    for (WN_UINT32 i = 0; i < WNCeil(static_cast<WN_FLOAT32>(_numWorkerThreads) / 2.0f); ++i) {
        WNThread<WN_VOID>* thread = WN_NEW WNThread<WN_VOID>();

        if (thread->Initialize(this, &WNNetworkManagerLinux::ReadThread) != WNConcurrency::WNThreadReturnCode::eWNOK) {
            WN_DELETE(thread);

            mInitializationState = eWNReadThreadsCreated;

            Cleanup();

            return(WNNetworkManagerReturnCode::eWNThreadCreationError);
        }

        mReadThreads.push_back(thread);
    }

    mInitializationState = eWNReadThreadsCreated;
#ifdef _WN_ANDROID 
    mListenEPollInstance = epoll_create(2048); // afaik the size isn't even used any more (android does not have epoll_create1
#else
    mListenEPollInstance = epoll_create1(0);
#endif
    if (mListenEPollInstance == -1) {
        Cleanup();

        return(WNNetworkManagerReturnCode::eWNSystemInitializationFailed);
    }

    mInitializationState = eWNEPollListenCreated;
    mListenThread = WN_NEW WNThread<WN_VOID>();

    if (mListenThread->Initialize(this, &WNNetworkManagerLinux::ListenThread) != WNConcurrency::WNThreadReturnCode::eWNOK) {
        WN_DELETE(mListenThread);

        mListenThread = WN_NULL;

        Cleanup();

        return(WNNetworkManagerReturnCode::eWNThreadCreationError);
    }

    mInitializationState = eWNListenThreadCreated;
    mInitializationState = eWNInitializationComplete;

    return(WNNetworkManagerReturnCode::eWNOK);
}

WN_VOID WNNetworkManagerLinux::ListenThread() {
    WN_INT32 numEvents;
    static WN_INT32 MAX_EPOLL_EVENTS=1;
    epoll_event events[MAX_EPOLL_EVENTS];
    WNMemSet(events, 0, sizeof(epoll_event) * MAX_EPOLL_EVENTS);
    while(1){
        numEvents = epoll_wait(mListenEPollInstance, events, MAX_EPOLL_EVENTS, 100);
        
        if(mShuttingDown) {
            return;
        }
        
        for(WN_INT32 i = 0; i < numEvents; ++i) {
            if((events[i].events & EPOLLERR)) {
                //ERROR;
                continue;
            } else if ((events[i].events & EPOLLHUP)) {
                //internal error?
                continue;
            } else if (!(events[i].events & EPOLLIN)) {
                //UHH whats this?
                continue;
            }
            WNListenConnectionLinux* conn = reinterpret_cast<WNListenConnectionLinux*>(events[i].data.ptr);
            WNConnectionLinux * inConn;
            //Could be getting multiple connections at once
            while ((inConn = conn->ReceiveConnection()) != WN_NULL) {
                if (AddToReadEPoll(inConn) && AddToWriteEPoll(inConn)) {
                    WNLockGuard<WNSpinLock> guard(mIncommingMutex);

                    mIncommingConnections.push_back(inConn);
                } else {
                    WN_DELETE(inConn);
                }
            }
        }
    }
    
}

WN_VOID WNNetworkManagerLinux::ReadThread() {
    WN_INT32 numEvents;
    static WN_INT32 MAX_EPOLL_EVENTS = 1;

    epoll_event events[MAX_EPOLL_EVENTS];

    WNMemSet(events, 0, sizeof(epoll_event) * MAX_EPOLL_EVENTS);

    for (;;) {
        numEvents = epoll_wait(mReadEPollInstance, events, MAX_EPOLL_EVENTS, 100);

        if (mShuttingDown) {
            return;
        }

        for (WN_INT32 i = 0; i < numEvents; ++i) {
            if ((events[i].events & EPOLLERR)) {
                //ERROR;
                continue;
            } else if ((events[i].events & EPOLLHUP)) {
                //internal error?
                continue;
            } else if (!(events[i].events & EPOLLIN)) {
                //UHH whats this?
                continue;
            }

            WNConnectionLinux* conn = reinterpret_cast<WNConnectionLinux*>(events[i].data.ptr);

            conn->NotifyReadReady();
        }
    }
}

WN_VOID WNNetworkManagerLinux::WriteThread() {
    WN_INT32 numEvents;
    static WN_INT32 MAX_EPOLL_EVENTS = 1;
    static WN_INT32 MAX_EPOLL_TIME = 5;

    epoll_event events[MAX_EPOLL_EVENTS];

    for (;;) {
        numEvents = epoll_wait(mWriteEPollInstance, events, MAX_EPOLL_EVENTS, 100);

        if (mShuttingDown) {
            return;
        }

        for (WN_INT32 i = 0; i < numEvents; ++i) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !((events[i].events & EPOLLOUT))) {
                //HANDLE CLOSING CONNECTION
                continue;
            }

            WNConnectionLinux* conn = reinterpret_cast<WNConnectionLinux*>(events[i].data.ptr);

            conn->NotifyReadyToSend(WN_TRUE);
        }
    }
}

WNNetworkManagerReturnCode::Type WNNetworkManagerLinux::CreateListener(WNConnection*& _outHandle, WNConnectionType::Type _type, WN_UINT16 _port, const WNConnectedCallback& _callback) {
    WNListenConnectionLinux* conn = WN_NEW WNListenConnectionLinux(*this, _type, _port, _callback);
    const WNNetworkManagerReturnCode::Type err = conn->Initialize();

    if (err != WNNetworkManagerReturnCode::eWNOK) {
        WN_DELETE(conn);

        return(err);
    }

    struct epoll_event event;

    WNMemClr(&event, sizeof(epoll_event));

    event.data.ptr = conn;
    event.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(mListenEPollInstance, EPOLL_CTL_ADD, conn->GetLinuxSocket(), &event) != 0) {
        WN_DELETE(conn);

        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    {
        WNLockGuard<WNSpinLock> guard(mListenMutex);

        mListenConnections.push_back(conn);
    }

    _outHandle = conn;

    return(WNNetworkManagerReturnCode::eWNOK);
}

WN_BOOL WNNetworkManagerLinux::AddToReadEPoll(WNConnectionLinux* _conn) {
    struct epoll_event event;

    WNMemory::WNMemClr(&event, sizeof(epoll_event));

    event.data.ptr = _conn;
    event.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(mReadEPollInstance, EPOLL_CTL_ADD, _conn->GetLinuxSocket(), &event) == -1) {
        return(WN_FALSE);
    }

    return(WN_TRUE);
}

WN_BOOL WNNetworkManagerLinux::AddToWriteEPoll(WNConnectionLinux* _conn) {
    struct epoll_event event;

    WNMemory::WNMemClr(&event, sizeof(epoll_event));

    event.data.ptr = _conn;
    event.events = EPOLLOUT | EPOLLET;

    if (epoll_ctl(mWriteEPollInstance, EPOLL_CTL_ADD, _conn->GetLinuxSocket(), &event) == -1) {
        return(WN_FALSE);
    }

    return(WN_TRUE);
}

WNNetworkManagerReturnCode::Type WNNetworkManagerLinux::ConnectTo(WNConnection*& _outHandle, WNConnectionType::Type _type, const WN_CHAR* _target, WN_UINT16 _port) {
    WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable, "WNNetworking does not support unreliable connections .... YET");

    WNOutConnectionLinux* conn = WN_NEW WNOutConnectionLinux(*this);
    const WNNetworkManagerReturnCode::Type err = conn->Initialize(_type, _target, _port);

    if (err != WNNetworkManagerReturnCode::eWNOK) {
        WN_DELETE(conn);

        return(err);
    }

    if (!AddToReadEPoll(conn)) {
        WN_DELETE(conn);

        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    if (!AddToWriteEPoll(conn)) {
        WN_DELETE(conn);

        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    {
        mOutgoingMutex.Lock();
        mOutgoingConnections.push_back(conn);
        mOutgoingMutex.Unlock();
    }

    _outHandle = conn;

    return(WNNetworkManagerReturnCode::eWNOK);
}

WN_VOID WNNetworkManagerLinux::DestroyConnection(WNConnection* _connection) {
     {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mIncommingMutex);

        std::list<WNConnectionLinux*>::iterator i = std::find(mIncommingConnections.begin(), mIncommingConnections.end(), _connection);

        if (i != mIncommingConnections.end()) {
             (*i)->Invalidate();
            mIncommingConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mOutgoingMutex);

        std::list<WNConnectionLinux*>::iterator i = std::find(mOutgoingConnections.begin(), mOutgoingConnections.end(), _connection);

        if (i != mOutgoingConnections.end()) {
            (*i)->Invalidate();
            mOutgoingConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mListenMutex);

        std::list<WNListenConnectionLinux*>::iterator i = std::find(mListenConnections.begin(), mListenConnections.end(), _connection);

        if (i != mListenConnections.end()) {
             (*i)->Invalidate();
            mListenConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mInvalidMutex);

        mInvalidConnections.push_back(_connection);
    }
}

WN_VOID WNNetworkManagerLinux::CleanAllConnections() {
    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mIncommingMutex);

        for (std::list<WNConnectionLinux*>::iterator i = mIncommingConnections.begin(); i != mIncommingConnections.end(); ++i) {
            WN_DELETE((*i));
        }

        mIncommingConnections.clear();
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mOutgoingMutex);

        for (std::list<WNConnectionLinux*>::iterator i = mOutgoingConnections.begin(); i != mOutgoingConnections.end(); ++i) {
            WN_DELETE((*i));
        }

        mOutgoingConnections.clear();
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mListenMutex);

        for (std::list<WNListenConnectionLinux*>::iterator i = mListenConnections.begin(); i != mListenConnections.end(); ++i) {
            WN_DELETE((*i));
        }

        mListenConnections.clear();
    }

    {
        WNConcurrency::WNLockGuard<WNConcurrency::WNSpinLock> guard(mInvalidMutex);

        mInvalidConnections.clear();
    }
}
