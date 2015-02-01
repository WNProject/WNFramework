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

WNNetworkManagerReturnCode::Type WNOutConnectionWindows::Initialize(WNConnectionType::Type& _type, const WN_CHAR* _target, WN_UINT16 _port) {
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

    WNMemory::WNMemClrT(&address);

    address.sin_family = AF_INET;

    WNMemory::WNMemCpy(&address.sin_addr.s_addr, server->h_addr, server->h_length);

    address.sin_port = htons(static_cast<u_short>(_port));

    WN_SIZE_T length = WN_SNPRINTF(NULL,0, "%s:%d", _target, ntohs(address.sin_port));

    mConnectionName = WNMemory::WNMallocT<WN_CHAR>(length + 1);

    WN_SNPRINTF(mConnectionName,length +1, "%s:%d", _target, ntohs(address.sin_port));

    if (SOCKET_ERROR == connect(mSocket, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address))) {
        return(WNNetworkManagerReturnCode::eWNCannotConnect);
    }

    return(WNNetworkManagerReturnCode::eWNOK);
}
