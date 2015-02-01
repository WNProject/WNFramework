// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/Internal/Linux/WNNetworkManagerLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNListenConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNInConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNOutConnectionLinux.h"
#include "WNMemory/inc/WNMemory.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNBasic.h"
#include "WNConcurrency/inc/thread.h"
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
    mListenThread(wn_nullptr),
    mShuttingDown(wn_false) {
}

WNNetworkManagerLinux::~WNNetworkManagerLinux() {
    Cleanup();
}

wn_void WNNetworkManagerLinux::Cleanup() {
    WNNetworkManager::Cleanup();

    mShuttingDown = wn_true;

    if(mInitializationState > eWNEPollListenCreated) {
        close(mListenEPollInstance);
        if (mListenThread != wn_nullptr) {
            mListenThread->WaitForCompletion();
            WN_DELETE(mListenThread);
            mListenThread = wn_nullptr;
        }
    }

    if(mInitializationState > eWNEPollReadCreated) {
        close(mReadEPollInstance);
        for(wn_size_t i = 0; i < mReadThreads.size(); ++i) {
            mReadThreads[i]->WaitForCompletion();
            WN_DELETE(mReadThreads[i]);
        }
        mReadThreads.clear();
    }

    if(mInitializationState > eWNEPollWriteCreated) {
        close(mWriteEPollInstance);
        for(wn_size_t i = 0; i < mWriteThreads.size(); ++i) {
            mWriteThreads[i]->WaitForCompletion();
            WN_DELETE(mWriteThreads[i]);
        }
        mWriteThreads.clear();
    }
    CleanAllConnections();
}

WNNetworkManagerReturnCode::type WNNetworkManagerLinux::Initialize(wn_uint32 _numWorkerThreads) {

    mShuttingDown = wn_false;
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

    for (wn_uint32 i = 0; i < WNCeil(static_cast<wn_float32>(_numWorkerThreads) / 2.0f); ++i) {
        thread<wn_void>* thread = WN_NEW thread<wn_void>();

        if (thread->Initialize(this, &WNNetworkManagerLinux::WriteThread) != wn::thread_result::ok) {
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

    for (wn_uint32 i = 0; i < WNCeil(static_cast<wn_float32>(_numWorkerThreads) / 2.0f); ++i) {
        thread<wn_void>* thread = WN_NEW thread<wn_void>();

        if (thread->Initialize(this, &WNNetworkManagerLinux::ReadThread) != wn::thread_result::ok) {
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
    mListenThread = WN_NEW thread<wn_void>();

    if (mListenThread->Initialize(this, &WNNetworkManagerLinux::ListenThread) != wn::thread_result::ok) {
        WN_DELETE(mListenThread);

        mListenThread = wn_nullptr;

        Cleanup();

        return(WNNetworkManagerReturnCode::eWNThreadCreationError);
    }

    mInitializationState = eWNListenThreadCreated;
    mInitializationState = eWNInitializationComplete;

    return(WNNetworkManagerReturnCode::ok);
}

wn_void WNNetworkManagerLinux::ListenThread() {
    wn_int32 numEvents;
    static wn_int32 MAX_EPOLL_EVENTS=1;
    epoll_event events[MAX_EPOLL_EVENTS];
    WNMemSet(events, 0, sizeof(epoll_event) * MAX_EPOLL_EVENTS);
    while(1){
        numEvents = epoll_wait(mListenEPollInstance, events, MAX_EPOLL_EVENTS, 100);
        
        if(mShuttingDown) {
            return;
        }
        
        for(wn_int32 i = 0; i < numEvents; ++i) {
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
            while ((inConn = conn->ReceiveConnection()) != wn_nullptr) {
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

wn_void WNNetworkManagerLinux::ReadThread() {
    wn_int32 numEvents;
    static wn_int32 MAX_EPOLL_EVENTS = 1;

    epoll_event events[MAX_EPOLL_EVENTS];

    WNMemSet(events, 0, sizeof(epoll_event) * MAX_EPOLL_EVENTS);

    for (;;) {
        numEvents = epoll_wait(mReadEPollInstance, events, MAX_EPOLL_EVENTS, 100);

        if (mShuttingDown) {
            return;
        }

        for (wn_int32 i = 0; i < numEvents; ++i) {
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

wn_void WNNetworkManagerLinux::WriteThread() {
    wn_int32 numEvents;
    static wn_int32 MAX_EPOLL_EVENTS = 1;
    static wn_int32 MAX_EPOLL_TIME = 5;

    epoll_event events[MAX_EPOLL_EVENTS];

    for (;;) {
        numEvents = epoll_wait(mWriteEPollInstance, events, MAX_EPOLL_EVENTS, 100);

        if (mShuttingDown) {
            return;
        }

        for (wn_int32 i = 0; i < numEvents; ++i) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !((events[i].events & EPOLLOUT))) {
                //HANDLE CLOSING CONNECTION
                continue;
            }

            WNConnectionLinux* conn = reinterpret_cast<WNConnectionLinux*>(events[i].data.ptr);

            conn->NotifyReadyToSend(wn_true);
        }
    }
}

WNNetworkManagerReturnCode::type WNNetworkManagerLinux::CreateListener(WNConnection*& _outHandle, WNConnectionType::type _type, wn_uint16 _port, const WNConnectedCallback& _callback) {
    WNListenConnectionLinux* conn = WN_NEW WNListenConnectionLinux(*this, _type, _port, _callback);
    const WNNetworkManagerReturnCode::type err = conn->Initialize();

    if (err != WNNetworkManagerReturnCode::ok) {
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

    return(WNNetworkManagerReturnCode::ok);
}

wn_bool WNNetworkManagerLinux::AddToReadEPoll(WNConnectionLinux* _conn) {
    struct epoll_event event;

    WNMemory::WNMemClr(&event, sizeof(epoll_event));

    event.data.ptr = _conn;
    event.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(mReadEPollInstance, EPOLL_CTL_ADD, _conn->GetLinuxSocket(), &event) == -1) {
        return(wn_false);
    }

    return(wn_true);
}

wn_bool WNNetworkManagerLinux::AddToWriteEPoll(WNConnectionLinux* _conn) {
    struct epoll_event event;

    WNMemory::WNMemClr(&event, sizeof(epoll_event));

    event.data.ptr = _conn;
    event.events = EPOLLOUT | EPOLLET;

    if (epoll_ctl(mWriteEPollInstance, EPOLL_CTL_ADD, _conn->GetLinuxSocket(), &event) == -1) {
        return(wn_false);
    }

    return(wn_true);
}

WNNetworkManagerReturnCode::type WNNetworkManagerLinux::ConnectTo(WNConnection*& _outHandle, WNConnectionType::type _type, const wn_char* _target, wn_uint16 _port) {
    WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable, "WNNetworking does not support unreliable connections .... YET");

    WNOutConnectionLinux* conn = WN_NEW WNOutConnectionLinux(*this);
    const WNNetworkManagerReturnCode::type err = conn->Initialize(_type, _target, _port);

    if (err != WNNetworkManagerReturnCode::ok) {
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

    return(WNNetworkManagerReturnCode::ok);
}

wn_void WNNetworkManagerLinux::DestroyConnection(WNConnection* _connection) {
     {
        WNConcurrency::WNLockGuard<wn::spin_lock> guard(mIncommingMutex);

        std::list<WNConnectionLinux*>::iterator i = std::find(mIncommingConnections.begin(), mIncommingConnections.end(), _connection);

        if (i != mIncommingConnections.end()) {
             (*i)->Invalidate();
            mIncommingConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<wn::spin_lock> guard(mOutgoingMutex);

        std::list<WNConnectionLinux*>::iterator i = std::find(mOutgoingConnections.begin(), mOutgoingConnections.end(), _connection);

        if (i != mOutgoingConnections.end()) {
            (*i)->Invalidate();
            mOutgoingConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<wn::spin_lock> guard(mListenMutex);

        std::list<WNListenConnectionLinux*>::iterator i = std::find(mListenConnections.begin(), mListenConnections.end(), _connection);

        if (i != mListenConnections.end()) {
             (*i)->Invalidate();
            mListenConnections.erase(i);
        }
    }

    {
        WNConcurrency::WNLockGuard<wn::spin_lock> guard(mInvalidMutex);

        mInvalidConnections.push_back(_connection);
    }
}

wn_void WNNetworkManagerLinux::CleanAllConnections() {
    {
        WNConcurrency::WNLockGuard<wn::spin_lock> guard(mIncommingMutex);

        for (std::list<WNConnectionLinux*>::iterator i = mIncommingConnections.begin(); i != mIncommingConnections.end(); ++i) {
            WN_DELETE((*i));
        }

        mIncommingConnections.clear();
    }

    {
        WNConcurrency::WNLockGuard<wn::spin_lock> guard(mOutgoingMutex);

        for (std::list<WNConnectionLinux*>::iterator i = mOutgoingConnections.begin(); i != mOutgoingConnections.end(); ++i) {
            WN_DELETE((*i));
        }

        mOutgoingConnections.clear();
    }

    {
        WNConcurrency::WNLockGuard<wn::spin_lock> guard(mListenMutex);

        for (std::list<WNListenConnectionLinux*>::iterator i = mListenConnections.begin(); i != mListenConnections.end(); ++i) {
            WN_DELETE((*i));
        }

        mListenConnections.clear();
    }

    {
        WNConcurrency::WNLockGuard<wn::spin_lock> guard(mInvalidMutex);

        mInvalidConnections.clear();
    }
}
