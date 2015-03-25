// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/Internal/Windows/WNOutConnectionWindows.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNStrings/inc/WNStrings.h"

using namespace WNNetworking;

WNOutConnectionWindows::WNOutConnectionWindows(WNNetworkManager& _manager) :
    WNConnectionWindows(_manager) {
}

WNNetworkManagerReturnCode::type WNOutConnectionWindows::Initialize(WNConnectionType::type& _type, const wn_char* _target, wn_uint16 _port) {
    WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable, "WNNetworking does not support unreliable connections... YET");
    WN_RELEASE_ASSERT_DESC(_port <= 0xFFFF, "Error port must be within range");

    mSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (INVALID_SOCKET == mSocket) {
        return(WNNetworkManagerReturnCode::eWNCannotCreateSocket);
    }

    struct sockaddr_in address;
    struct hostent* server;

    //This is blocking
    server = gethostbyname(_target);

    if (server == NULL) {
        return(WNNetworkManagerReturnCode::eWNBadHostName);
    }

    wn::memory::memzero(&address);

    address.sin_family = AF_INET;

    wn::memory::memcpy<wn_void>(&address.sin_addr.s_addr, server->h_addr, server->h_length);

    address.sin_port = htons(static_cast<u_short>(_port));

    wn_size_t length = WN_SNPRINTF(NULL,0, "%s:%d", _target, ntohs(address.sin_port));

    mConnectionName = wn::memory::malloc<wn_char>(length + 1);

    WN_SNPRINTF(mConnectionName,length +1, "%s:%d", _target, ntohs(address.sin_port));

    if (SOCKET_ERROR == connect(mSocket, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address))) {
        return(WNNetworkManagerReturnCode::eWNCannotConnect);
    }

    return(WNNetworkManagerReturnCode::ok);
}
