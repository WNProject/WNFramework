// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/Internal/Windows/WNOutConnectionWindows.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNStringUtility.h"

#ifdef _WN_MSVC
#pragma warning(disable : 4996)
#endif

using namespace WNNetworking;

WNOutConnectionWindows::WNOutConnectionWindows(WNNetworkManager& _manager)
  : WNConnectionWindows(_manager) {}

WNNetworkManagerReturnCode::type WNOutConnectionWindows::Initialize(
    WNConnectionType::type& _type, const char* _target, uint16_t _port) {
  WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable,
      "WNNetworking does not support unreliable connections... YET");
  WN_RELEASE_ASSERT_DESC(_port <= 0xFFFF, "Error port must be within range");

  mSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

  if (INVALID_SOCKET == mSocket) {
    return (WNNetworkManagerReturnCode::eWNCannotCreateSocket);
  }

  struct sockaddr_in address;
  struct hostent* server;

  // This is blocking
  server = gethostbyname(_target);

  if (server == NULL) {
    return (WNNetworkManagerReturnCode::eWNBadHostName);
  }

  wn::memory::memory_zero(&address);

  address.sin_family = AF_INET;

  wn::memory::memcpy(
      &address.sin_addr.s_addr, server->h_addr, server->h_length);

  address.sin_port = htons(static_cast<u_short>(_port));

  size_t length =
      wn::memory::snprintf(NULL, 0, "%s:%d", _target, ntohs(address.sin_port));

  mConnectionName = wn::memory::heap_allocate<char>(length + 1);

  wn::memory::snprintf(
      mConnectionName, length + 1, "%s:%d", _target, ntohs(address.sin_port));

  if (SOCKET_ERROR == connect(mSocket,
                          reinterpret_cast<const struct sockaddr*>(&address),
                          sizeof(address))) {
    return (WNNetworkManagerReturnCode::eWNCannotConnect);
  }

  return (WNNetworkManagerReturnCode::ok);
}
