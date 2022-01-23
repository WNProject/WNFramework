// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NEWORKING_RELIABLE_CONNECTION_COMM_SOCKET_H__
#define __WN_NEWORKING_RELIABLE_CONNECTION_COMM_SOCKET_H__

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNReliableConnection.h"
#include "core/inc/base.h"

#include <sys/socket.h>
#include <unistd.h>

namespace wn {
namespace networking {

class WNReliableNetworkTransportSocket : public WNReliableConnection {
public:
  WNReliableNetworkTransportSocket(
      memory::allocator* _allocator, WNBufferManager* _manager)
    : WNReliableConnection(_allocator, _manager), m_sock_fd(-1) {}

  WNReliableNetworkTransportSocket(
      memory::allocator* _allocator, int _socket, WNBufferManager* _manager)
    : WNReliableConnection(_allocator, _manager), m_sock_fd(_socket) {}

  network_error connect_to(logging::log* _log,
      const containers::string_view& target, uint32_t connection_type,
      uint16_t _port);

  ~WNReliableNetworkTransportSocket() {
    if (m_sock_fd != -1) {
      ::close(m_sock_fd);
      m_sock_fd = -1;
    }
  }
  WNReceiveBuffer recv_sync() override;
  void close() override {
    if (m_sock_fd != -1) {
      ::shutdown(m_sock_fd, SHUT_RDWR);
    }
  }

private:
  network_error do_send(const send_ranges& buffers) override;
  int m_sock_fd;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_RELIABLE_CONNECTION_COMM_SOCKET_H__
