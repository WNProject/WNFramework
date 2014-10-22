// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_MANAGER_H__
#define __WN_NETWORKING_NETWORK_MANAGER_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNResourcePointer.h"
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
        enum Type {
            eWNReliable,
            eWNUnreliable
        };
    }

    namespace WNNetworkManagerReturnCode {
        enum Type {
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

    typedef WNConcurrency::WNResourcePointer<WNConnection> WNConnectionPtr;
    typedef WNConcurrency::WNResourcePointer<WNConnectionGroup> WNConnectionGroupPtr;

    typedef WNContainers::WNCallback1<WN_BOOL, const WN_CHAR*> WNConnectedCallback;
    typedef WNContainers::WNCallback2<WN_VOID, WNConnection*, WNNetworkReadBuffer&> WNMessageCallback;

    class WNNetworkManager {
    public:
        WN_FORCE_INLINE WNNetworkManager() {}
        virtual WN_FORCE_INLINE ~WNNetworkManager() {}

        virtual WNNetworkManagerReturnCode::Type Initialize(WN_UINT32 _numWorkerThreads) = 0;
        //You do not own this connection, you must ask the network manager to destroy it for you
        virtual WNNetworkManagerReturnCode::Type ConnectTo(WNConnection*& _outHandle, WNConnectionType::Type _type, const WN_CHAR* _target, WN_UINT16 _port) = 0;
        //You do not own this connection, you must ask the network manager to destroy it for you
        virtual WNNetworkManagerReturnCode::Type CreateListener(WNConnection*& _outHandle, WNConnectionType::Type _type, WN_UINT16 _port, const WNConnectedCallback& _callback) = 0;
        virtual WN_VOID DestroyConnection(WNConnection* _connection) = 0;

        virtual WN_VOID Cleanup();
        WN_VOID InitializeBuffer(WNNetworkWriteBuffer& _buffer, WN_UINT32 _number);
        WN_VOID SetCallback(WN_UINT32 _identifier, const WNMessageCallback& _callback);
        WN_VOID FireCallback(WN_UINT32 _callbackNumber, WNConnection* _connection, WNNetworkReadBuffer& _buffer);

        //You do not own this connection, you must ask the network manager to destroy it for you
        WNNetworkManagerReturnCode::Type CreateConnectionGroup(WNConnectionGroup*& _outHandle, const WN_CHAR* _groupName);
        WN_VOID DestroyConnectionGroup(WNConnectionGroup* _group);

    protected:
        enum WNCallbackMessage {
            eWNCallbackNone,
            eWNCallbackListener
        };

    protected:
        WN_VOID UnregisterConnection(WNConnection* _connection);

    private:
        WN_UNORDERED_MAP<WN_UINT32, WNMessageCallback> mCallbackMap;
        std::list<WNConnectionGroup*> mGroupList;
    };
}

#endif // __WN_NETWORKING_NETWORK_MANAGER_H__
