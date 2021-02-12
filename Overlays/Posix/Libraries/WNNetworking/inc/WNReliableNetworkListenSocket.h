// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__
#define __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNReliableConnection.h"
#include "core/inc/base.h"

#include <sys/socket.h>
#include <unistd.h>

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
#ifdef _WN_MACOS
      ::close(m_sock_fd);
      m_sock_fd = -1;
#else
      ::shutdown(m_sock_fd, SHUT_RDWR);
#endif
      m_sock_fd = -1;
    }
  }

private:
  int m_sock_fd;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_RELIABLE_CONNECTION_LISTEN_SOCKET_H__
