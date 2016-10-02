// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__
#define __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__

#include "WNCore/inc/WNBase.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNReliableConnection.h"

#include <unistd.h>

namespace wn {
namespace networking {

class WNReliableConnectListenSocket : public WNReliableAcceptConnection {
public:
  network_error initialize(
      WNLogging::WNLog* _log, int _protocol, uint16_t _port);
  WNReliableConnectListenSocket(
      memory::allocator* _allocator, WNBufferManager* _manager)
    : WNReliableAcceptConnection(_allocator, _manager), m_sock_fd(-1) {}
  ~WNReliableConnectListenSocket() {
    if (m_sock_fd != -1) {
      close(m_sock_fd);
    }
  }

  memory::unique_ptr<WNReliableConnection> accept_sync(
      network_error* _error = nullptr) override;

private:
  int m_sock_fd;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__