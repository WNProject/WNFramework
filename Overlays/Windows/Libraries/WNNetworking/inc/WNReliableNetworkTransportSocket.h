// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NEWORKING_RELIABLE_CONNECTION_COMM_SOCKET_H__
#define __WN_NEWORKING_RELIABLE_CONNECTION_COMM_SOCKET_H__

#include "WNCore/inc/WNBase.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNReliableConnection.h"

namespace wn {
namespace networking {

class WNReliableNetworkTransportSocket : public WNReliableConnection {
public:
  WNReliableNetworkTransportSocket(memory::allocator* _allocator,
      multi_tasking::job_pool* _job_pool, WNBufferManager* _manager)
    : WNReliableConnection(_allocator, _job_pool, _manager),
      m_socket(INVALID_SOCKET) {}

  WNReliableNetworkTransportSocket(memory::allocator* _allocator,
      multi_tasking::job_pool* _job_pool, SOCKET _socket,
      WNBufferManager* _manager)
    : WNReliableConnection(_allocator, _job_pool, _manager),
      m_socket(_socket) {}

  network_error connect_to(WNLogging::WNLog* _log,
      const containers::string_view& target, uint32_t connection_type,
      uint16_t _port);

  ~WNReliableNetworkTransportSocket() {
    close();
  }

  void close() override {
    if (m_socket != INVALID_SOCKET) {
      closesocket(m_socket);
      m_socket = INVALID_SOCKET;
    }
  }

  WNReceiveBuffer recv_sync() override;

private:
  network_error do_send(const send_ranges& buffers) override;
  SOCKET m_socket;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_RELIABLE_CONNECTION_COMM_SOCKET_H__