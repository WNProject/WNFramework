// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/Internal/Windows/WNInConnectionWindows.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNStrings/inc/WNStrings.h"

using namespace WNNetworking;

WNInConnectionWindows::WNInConnectionWindows(WNNetworkManager& _manager) :
    WNConnectionWindows(_manager) {
}

WNNetworkManagerReturnCode::type WNInConnectionWindows::Initialize(SOCKET _listenSocket, WNConnectedCallback _callback) {
    sockaddr_in address;

    wn::memory::memory_zero(&address);

    int sockAddrSize = sizeof(address);

    mSocket = accept(_listenSocket, reinterpret_cast<sockaddr*>(&address), &sockAddrSize);

    if (INVALID_SOCKET == mSocket) {
        return(WNNetworkManagerReturnCode::eWNCannotCreateSocket);
    }

    wn_size_t length = WN_SNPRINTF(NULL, 0, "%s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    mConnectionName = wn::memory::heap_allocate<wn_char>(length + 1);

    WN_SNPRINTF(mConnectionName, length + 1, "%s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    mCallback = _callback;

    return(WNNetworkManagerReturnCode::ok);
}

wn_bool WNInConnectionWindows::FireCallback() {
    return(mCallback.Execute(mConnectionName));
}
