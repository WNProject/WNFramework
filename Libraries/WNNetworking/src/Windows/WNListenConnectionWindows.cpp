// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/Internal/Windows/WNListenConnectionWindows.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNStringUtility.h"
#include "WNNetworking/inc/Internal/Windows/WNInConnectionWindows.h"
#include "WNNetworking/inc/WNNetworkManager.h"

using namespace WNNetworking;

WNListenConnectionWindows::WNListenConnectionWindows(WNNetworkManager& _manager,
    WNConnectionType::type _type, uint16_t _port,
    WNConnectedCallback _connected)
  : WNConnectionWindows(_manager),
    mPort(_port),
    mConnectedCallback(_connected),
    mType(_type),
    mInitialized(false) {
  WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable,
      "We do not support unreliable connections yet");
}

WNNetworkManagerReturnCode::type WNListenConnectionWindows::Initialize() {
  mSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

  if (INVALID_SOCKET == mSocket) {
    return (WNNetworkManagerReturnCode::eWNCannotCreateSocket);
  }

  sockaddr_in ServerAddress;

  wn::memory::memory_zero(&ServerAddress);

  ServerAddress.sin_family = AF_INET;
  ServerAddress.sin_addr.s_addr = INADDR_ANY;
  ServerAddress.sin_port = htons(mPort);

  if (SOCKET_ERROR ==
      bind(mSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress))) {
    closesocket(mSocket);

    return (WNNetworkManagerReturnCode::eWNCannotCreateSocket);
  }

  if (SOCKET_ERROR == listen(mSocket, SOMAXCONN)) {
    closesocket(mSocket);

    return (WNNetworkManagerReturnCode::eWNCannotCreateSocket);
  }

  const uint32_t nameLen = wn::memory::snprintf(NULL, 0, "Listen:%d", mPort);
  char* name = wn::memory::heap_allocate<char>(nameLen + 1);

  wn::memory::snprintf(name, nameLen + 1, "Listen:%d", mPort);

  mConnectionName = name;
  mInitialized = true;

  return (WNNetworkManagerReturnCode::ok);
}

WNInConnectionWindows* WNListenConnectionWindows::AcceptConnection() {
  WNInConnectionWindows* connection =
      wn::memory::construct<WNInConnectionWindows>(mManager);

  if (connection->Initialize(mSocket, mConnectedCallback) !=
      WNNetworkManagerReturnCode::ok) {
    wn::memory::destroy(connection);
  }

  return (connection);
}
