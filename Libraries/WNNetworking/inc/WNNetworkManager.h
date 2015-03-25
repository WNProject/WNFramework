// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_MANAGER_H__
#define __WN_NETWORKING_NETWORK_MANAGER_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNContainers/inc/WNCallback.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)
#endif

#include <deque>
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <list>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

#ifndef WN_UNORDERED_MAP
    #if (defined _WN_MSVC && _WN_MSVC_MAJOR < 16)
        #define WN_UNORDERED_MAP std::tr1::unordered_map
        #define WN_UNORDERED_MULTIMAP std::tr1::unordered_multimap
        #define WN_UNORDERED_SET std::tr1::unordered_set
    #else
        #define WN_UNORDERED_MAP std::unordered_map
        #define WN_UNORDERED_MULTIMAP std::unordered_multimap
        #define WN_UNORDERED_SET std::unordered_set
    #endif
#endif

namespace WNNetworking {
    namespace WNConnectionType {
        enum type {
            eWNReliable,
            eWNUnreliable
        };
    }

    namespace WNNetworkManagerReturnCode {
        enum type {
            #include "WNCore/inc/Internal/WNErrors.inc"
            eWNTooManyThreads,
            eWNSystemInitializationFailed,
            eWNThreadCreationError,
            eWNCannotCreateSocket,
            eWNCannotListen,
            eWNBadHostName,
            eWNCannotConnect,
            eWNCannotAssociate,
            eWNCannotUnblock
        };
    }

    class WNNetworkWriteBuffer;
    class WNNetworkReadBuffer;
    class WNConnection;
    class WNConnectionGroup;

    typedef wn::memory::intrusive_ptr<WNConnection> WNConnectionPtr;
    typedef wn::memory::intrusive_ptr<WNConnectionGroup> WNConnectionGroupPtr;

    typedef WNContainers::WNCallback1<wn_bool, const wn_char*> WNConnectedCallback;
    typedef WNContainers::WNCallback2<wn_void, WNConnection*, WNNetworkReadBuffer&> WNMessageCallback;

    class WNNetworkManager {
    public:
        WN_FORCE_INLINE WNNetworkManager() {}
        virtual WN_FORCE_INLINE ~WNNetworkManager() {}

        virtual WNNetworkManagerReturnCode::type Initialize(wn_uint32 _numWorkerThreads) = 0;
        //You do not own this connection, you must ask the network manager to destroy it for you
        virtual WNNetworkManagerReturnCode::type ConnectTo(WNConnection*& _outHandle, WNConnectionType::type _type, const wn_char* _target, wn_uint16 _port) = 0;
        //You do not own this connection, you must ask the network manager to destroy it for you
        virtual WNNetworkManagerReturnCode::type CreateListener(WNConnection*& _outHandle, WNConnectionType::type _type, wn_uint16 _port, const WNConnectedCallback& _callback) = 0;
        virtual wn_void DestroyConnection(WNConnection* _connection) = 0;

        virtual wn_void Cleanup();
        wn_void InitializeBuffer(WNNetworkWriteBuffer& _buffer, wn_uint32 _number);
        wn_void SetCallback(wn_uint32 _identifier, const WNMessageCallback& _callback);
        wn_void FireCallback(wn_uint32 _callbackNumber, WNConnection* _connection, WNNetworkReadBuffer& _buffer);

        //You do not own this connection, you must ask the network manager to destroy it for you
        WNNetworkManagerReturnCode::type CreateConnectionGroup(WNConnectionGroup*& _outHandle, const wn_char* _groupName);
        wn_void DestroyConnectionGroup(WNConnectionGroup* _group);

    protected:
        enum WNCallbackMessage {
            eWNCallbackNone,
            eWNCallbackListener
        };

    protected:
        wn_void UnregisterConnection(WNConnection* _connection);

    private:
        WN_UNORDERED_MAP<wn_uint32, WNMessageCallback> mCallbackMap;
        std::list<WNConnectionGroup*> mGroupList;
    };
}

#endif // __WN_NETWORKING_NETWORK_MANAGER_H__
