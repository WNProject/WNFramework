// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/Internal/Linux/WNInConnectionLinux.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNStringUtility.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace WNNetworking;

WNInConnectionLinux::WNInConnectionLinux(WNNetworkManager& _manager)
  : WNConnectionLinux(_manager) {}

bool WNInConnectionLinux::Initialize(
    int32_t _yourSocket, sockaddr& _addr, socklen_t addrLen) {
  mSockFD = _yourSocket;

  char name[NI_MAXHOST];
  char port[NI_MAXSERV];

  bool validName = (getnameinfo(&_addr, addrLen, name, NI_MAXHOST, port,
                           NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) == 0);
  int32_t flags;

  flags = fcntl(mSockFD, F_GETFL, 0);
  if (flags == -1) {
    // NOTIFY ERROR
    return (false);
  }
#ifdef _WN_ANDROID
  flags |= O_NONBLOCK;  // Android does not define SOCK_NONBLOCK
#else
  flags |= SOCK_NONBLOCK;
#endif
  if (fcntl(mSockFD, F_SETFL, flags) == -1) {
    // NOTIFY ERROR
    return (false);
  }
  sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(&_addr);
  size_t length = wn::memory::snprintf(
      NULL, 0, "%s:%d", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));

  mConnectionName = wn::memory::heap_allocate<char>(length + 1);
  wn::memory::snprintf(mConnectionName, length + 1, "%s:%d", inet_ntoa(sin->sin_addr),
      ntohs(sin->sin_port));

  return (true);
}
