// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/Internal/Linux/WNNetworkManagerLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNListenConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNInConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNOutConnectionLinux.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMultiTasking/inc/WNThread.h"
#include "WNMultiTasking/inc/WNSpinLock.h"

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <algorithm>
#include <mutex>

using namespace wn;
using namespace WNNetworking;

WNNetworkManagerLinux::WNNetworkManagerLinux() :
    WNNetworkManager(),
    mInitializationState(eWNNotStarted),
    mWriteEPollInstance(-1),
    mReadEPollInstance(-1),
    mListenThread(nullptr),
    mShuttingDown(false) {
}

WNNetworkManagerLinux::~WNNetworkManagerLinux() {
    Cleanup();
}

void WNNetworkManagerLinux::Cleanup() {
    WNNetworkManager::Cleanup();

    mShuttingDown = true;

    if(mInitializationState > eWNEPollListenCreated) {
        close(mListenEPollInstance);
        if (mListenThread != nullptr) {
            mListenThread->join();
            wn::memory::destroy(mListenThread);
            mListenThread = nullptr;
        }
    }

    if(mInitializationState > eWNEPollReadCreated) {
        close(mReadEPollInstance);
        for(size_t i = 0; i < mReadThreads.size(); ++i) {
            mReadThreads[i]->join();
            wn::memory::destroy(mReadThreads[i]);
        }
        mReadThreads.clear();
    }

    if(mInitializationState > eWNEPollWriteCreated) {
        close(mWriteEPollInstance);
        for(size_t i = 0; i < mWriteThreads.size(); ++i) {
            mWriteThreads[i]->join();
            wn::memory::destroy(mWriteThreads[i]);
        }
        mWriteThreads.clear();
    }
    CleanAllConnections();
}

WNNetworkManagerReturnCode::type WNNetworkManagerLinux::Initialize(uint32_t _numWorkerThreads) {

    mShuttingDown = false;
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

    for (uint32_t i = 0; i < wn::ceil(static_cast<float>(_numWorkerThreads) / 2.0f); ++i) {
        wn::multi_tasking::thread<void>* thread = wn::memory::construct<wn::multi_tasking::thread<void> >(&allocator, &WNNetworkManagerLinux::WriteThread, this);

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

    for (uint32_t i = 0; i < wn::ceil(static_cast<float>(_numWorkerThreads) / 2.0f); ++i) {
        wn::multi_tasking::thread<void>* thread = wn::memory::construct<wn::multi_tasking::thread<void> >(&allocator, &WNNetworkManagerLinux::ReadThread, this);

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
    mListenThread = wn::memory::construct<wn::multi_tasking::thread<void> >(&allocator, &WNNetworkManagerLinux::ListenThread, this);
    mInitializationState = eWNListenThreadCreated;
    mInitializationState = eWNInitializationComplete;

    return(WNNetworkManagerReturnCode::ok);
}

void WNNetworkManagerLinux::ListenThread() {
    int32_t numEvents;
    static int32_t MAX_EPOLL_EVENTS=1;
    epoll_event events[MAX_EPOLL_EVENTS];
    memset(events, 0, sizeof(epoll_event) * MAX_EPOLL_EVENTS);
    while(1){
        numEvents = epoll_wait(mListenEPollInstance, events, MAX_EPOLL_EVENTS, 100);

        if(mShuttingDown) {
            return;
        }

        for(int32_t i = 0; i < numEvents; ++i) {
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
            while ((inConn = conn->ReceiveConnection()) != nullptr) {
                if (AddToReadEPoll(inConn) && AddToWriteEPoll(inConn)) {
                  std::lock_guard<wn::multi_tasking::spin_lock> guard(mIncommingMutex);

                    mIncommingConnections.push_back(inConn);
                } else {
                    wn::memory::destroy(inConn);
                }
            }
        }
    }

}

void WNNetworkManagerLinux::ReadThread() {
    int32_t numEvents;
    static int32_t MAX_EPOLL_EVENTS = 1;

    epoll_event events[MAX_EPOLL_EVENTS];

    memset(events, 0, sizeof(epoll_event) * MAX_EPOLL_EVENTS);

    for (;;) {
        numEvents = epoll_wait(mReadEPollInstance, events, MAX_EPOLL_EVENTS, 100);

        if (mShuttingDown) {
            return;
        }

        for (int32_t i = 0; i < numEvents; ++i) {
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

void WNNetworkManagerLinux::WriteThread() {
    int32_t numEvents;
    static int32_t MAX_EPOLL_EVENTS = 1;
    static int32_t MAX_EPOLL_TIME = 5;

    epoll_event events[MAX_EPOLL_EVENTS];

    for (;;) {
        numEvents = epoll_wait(mWriteEPollInstance, events, MAX_EPOLL_EVENTS, 100);

        if (mShuttingDown) {
            return;
        }

        for (int32_t i = 0; i < numEvents; ++i) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !((events[i].events & EPOLLOUT))) {
                //HANDLE CLOSING CONNECTION
                continue;
            }

            WNConnectionLinux* conn = reinterpret_cast<WNConnectionLinux*>(events[i].data.ptr);

            conn->NotifyReadyToSend(true);
        }
    }
}

WNNetworkManagerReturnCode::type WNNetworkManagerLinux::CreateListener(WNConnection*& _outHandle, WNConnectionType::type _type, uint16_t _port, const WNConnectedCallback& _callback) {
    WNListenConnectionLinux* conn = wn::memory::construct<WNListenConnectionLinux>(*this, _type, _port, _callback);
    const WNNetworkManagerReturnCode::type err = conn->Initialize();

    if (err != WNNetworkManagerReturnCode::ok) {
        wn::memory::destroy(conn);

        return(err);
    }

    struct epoll_event event;

    wn::memory::memzero(&event, sizeof(epoll_event));

    event.data.ptr = conn;
    event.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(mListenEPollInstance, EPOLL_CTL_ADD, conn->GetLinuxSocket(), &event) != 0) {
        wn::memory::destroy(conn);

        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mListenMutex);

        mListenConnections.push_back(conn);
    }

    _outHandle = conn;

    return(WNNetworkManagerReturnCode::ok);
}

bool WNNetworkManagerLinux::AddToReadEPoll(WNConnectionLinux* _conn) {
    struct epoll_event event;

    wn::memory::memzero(&event, sizeof(epoll_event));

    event.data.ptr = _conn;
    event.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(mReadEPollInstance, EPOLL_CTL_ADD, _conn->GetLinuxSocket(), &event) == -1) {
        return(false);
    }

    return(true);
}

bool WNNetworkManagerLinux::AddToWriteEPoll(WNConnectionLinux* _conn) {
    struct epoll_event event;

    wn::memory::memzero(&event, sizeof(epoll_event));

    event.data.ptr = _conn;
    event.events = EPOLLOUT | EPOLLET;

    if (epoll_ctl(mWriteEPollInstance, EPOLL_CTL_ADD, _conn->GetLinuxSocket(), &event) == -1) {
        return(false);
    }

    return(true);
}

WNNetworkManagerReturnCode::type WNNetworkManagerLinux::ConnectTo(WNConnection*& _outHandle, WNConnectionType::type _type, const char* _target, uint16_t _port) {
    WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable, "WNNetworking does not support unreliable connections .... YET");

    WNOutConnectionLinux* conn = wn::memory::construct<WNOutConnectionLinux>(*this);
    const WNNetworkManagerReturnCode::type err = conn->Initialize(_type, _target, _port);

    if (err != WNNetworkManagerReturnCode::ok) {
        wn::memory::destroy(conn);

        return(err);
    }

    if (!AddToReadEPoll(conn)) {
        wn::memory::destroy(conn);

        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    if (!AddToWriteEPoll(conn)) {
        wn::memory::destroy(conn);

        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    {
        mOutgoingMutex.lock();
        mOutgoingConnections.push_back(conn);
        mOutgoingMutex.unlock();
    }

    _outHandle = conn;

    return(WNNetworkManagerReturnCode::ok);
}

void WNNetworkManagerLinux::DestroyConnection(WNConnection* _connection) {
     {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mIncommingMutex);

        std::list<WNConnectionLinux*>::iterator i = std::find(mIncommingConnections.begin(), mIncommingConnections.end(), _connection);

        if (i != mIncommingConnections.end()) {
             (*i)->Invalidate();
            mIncommingConnections.erase(i);
        }
    }

    {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mOutgoingMutex);

        std::list<WNConnectionLinux*>::iterator i = std::find(mOutgoingConnections.begin(), mOutgoingConnections.end(), _connection);

        if (i != mOutgoingConnections.end()) {
            (*i)->Invalidate();
            mOutgoingConnections.erase(i);
        }
    }

    {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mListenMutex);

        std::list<WNListenConnectionLinux*>::iterator i = std::find(mListenConnections.begin(), mListenConnections.end(), _connection);

        if (i != mListenConnections.end()) {
             (*i)->Invalidate();
            mListenConnections.erase(i);
        }
    }

    {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mInvalidMutex);

        mInvalidConnections.push_back(_connection);
    }
}

void WNNetworkManagerLinux::CleanAllConnections() {
    {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mIncommingMutex);

        for (std::list<WNConnectionLinux*>::iterator i = mIncommingConnections.begin(); i != mIncommingConnections.end(); ++i) {
            wn::memory::destroy((*i));
        }

        mIncommingConnections.clear();
    }

    {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mOutgoingMutex);

        for (std::list<WNConnectionLinux*>::iterator i = mOutgoingConnections.begin(); i != mOutgoingConnections.end(); ++i) {
            wn::memory::destroy((*i));
        }

        mOutgoingConnections.clear();
    }

    {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mListenMutex);

        for (std::list<WNListenConnectionLinux*>::iterator i = mListenConnections.begin(); i != mListenConnections.end(); ++i) {
            wn::memory::destroy((*i));
        }

        mListenConnections.clear();
    }

    {
        std::lock_guard<wn::multi_tasking::spin_lock> guard(mInvalidMutex);

        mInvalidConnections.clear();
    }
}
