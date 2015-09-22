// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_CONNECTION_GROUP_H__
#define __WN_NETWORKING_CONNECTION_GROUP_H__

#include "WNNetworking/inc/WNConnection.h"
#include "WNCore/inc/WNTypes.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non � DLL-interface used as base for DLL-interface
    #pragma warning(disable: 4996)
#endif

#include <vector>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace WNNetworking {
    class WNNetworkWriteBuffer;

    class WNConnectionGroup : public WNConnection {
    public:
        WNConnectionGroup(const wn_char* _name);
        virtual ~WNConnectionGroup();

        virtual wn_void SendBuffer(WNNetworkWriteBuffer& buffer);
        wn_void AddConnection(WNConnection* _connection);
        wn_void CleanConnection(WNConnection* _connection);

    private:
        wn_char* mConnectionName;
        std::vector<WNConnection*> mConnections;
    };
};

#endif//__WN_CONNECTION_GROUP_H__