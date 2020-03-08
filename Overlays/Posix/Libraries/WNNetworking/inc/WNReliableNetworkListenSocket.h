// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__
#define __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__

#include <sys/socket.h>
#include <unistd.h>

#include "WNCore/inc/base.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNReliableConnection.h"

namespace wn {
namespace networking {

class WNReliableConnectListenSocket : public WNReliableAcceptConnection {
public:
  network_error initialize(logging::log* _log, int _protocol, uint16_t _port);
  WNReliableConnectListenSocket(
      memory::allocator* _allocator, WNBufferManager* _manager)
    : WNReliableAcceptConnection(_allocator, _manager), m_sock_fd(-1) {}
  ~WNReliableConnectListenSocket() {
    if (m_sock_fd != -1) {
      ::close(m_sock_fd);
      m_sock_fd = -1;
    }
  }

  memory::unique_ptr<WNConnection> accept_sync(
      network_error* _error = nullptr) override;

  void close() override {
    if (m_sock_fd != -1) {
      ::shutdown(m_sock_fd, SHUT_RDWR);
    }
  }

private:
  int m_sock_fd;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__