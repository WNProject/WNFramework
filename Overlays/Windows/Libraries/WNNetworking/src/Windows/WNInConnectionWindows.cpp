// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/Internal/Windows/WNInConnectionWindows.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNStringUtility.h"

#ifdef _WN_MSVC
#pragma warning(disable : 4996)
#endif

using namespace WNNetworking;

WNInConnectionWindows::WNInConnectionWindows(WNNetworkManager& _manager)
  : WNConnectionWindows(_manager) {}

WNNetworkManagerReturnCode::type WNInConnectionWindows::Initialize(
    SOCKET _listenSocket, WNConnectedCallback _callback) {
  sockaddr_in address;

  wn::memory::memory_zero(&address);

  int sockAddrSize = sizeof(address);

  mSocket = accept(
      _listenSocket, reinterpret_cast<sockaddr*>(&address), &sockAddrSize);

  if (INVALID_SOCKET == mSocket) {
    return (WNNetworkManagerReturnCode::eWNCannotCreateSocket);
  }

  size_t length = wn::memory::snprintf(
      NULL, 0, "%s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

  mConnectionName = wn::memory::heap_allocate<char>(length + 1);

  wn::memory::snprintf(mConnectionName, length + 1, "%s:%d",
      inet_ntoa(address.sin_addr), ntohs(address.sin_port));

  mCallback = _callback;

  return (WNNetworkManagerReturnCode::ok);
}

bool WNInConnectionWindows::FireCallback() {
  return (mCallback(mConnectionName));
}
