// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNReliableNetworkListenSocket.h"

#include "WNNetworking/inc/WNReliableNetworkTransportSocket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

static const uint32_t kIncommingConnectionBacklog = 10;

namespace wn {
namespace networking {

network_error WNReliableConnectListenSocket::initialize(
    logging::log* _log, int _protocol, uint16_t _port) {
  m_sock_fd = socket(_protocol, SOCK_STREAM, IPPROTO_TCP);
  if (m_sock_fd == -1) {
    return network_error::invalid_parameters;
  }

  int set_reuse = 1;
  if (setsockopt(m_sock_fd, SOL_SOCKET, SO_REUSEADDR, &set_reuse, sizeof(int)) <
      0) {
    WN_DEBUG_ASSERT(false, "setsockopt(SO_REUSEADDR) failed");
  }
  char port_array[11] = {0};
  memory::writeuint32(port_array, _port, 10);

  addrinfo* result;
  addrinfo* ptr;

  if (0 != getaddrinfo(NULL, port_array, nullptr, &result)) {
    _log->log_error("Could not resolve local port ", port_array);
    return network_error::could_not_resolve;
  }

  ptr = result;
  for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
    if (ptr->ai_family == _protocol) {
      break;
    }
  }

  if (ptr == nullptr) {
    _log->log_error("Could not resolve local port ", port_array);
    freeaddrinfo(result);
    return network_error::could_not_resolve;
  }

  int error = bind(m_sock_fd, ptr->ai_addr, ptr->ai_addrlen);
  freeaddrinfo(result);
  if (0 != error) {
    _log->log_error("Could not bind to port ", port_array);
    return network_error::could_not_bind;
  }

  if (0 != listen(m_sock_fd, kIncommingConnectionBacklog)) {
    _log->log_error("Could not listen on port ", port_array);
    return network_error::could_not_bind;
  }

  return network_error::ok;
}

memory::unique_ptr<WNConnection> WNReliableConnectListenSocket::accept_sync(
    network_error* _error) {
  network_error tmp;
  _error = _error ? _error : &tmp;
  *_error = network_error::ok;
  sockaddr_in accepted_client;
  socklen_t client_size = sizeof(sockaddr_in);
  int accepted = accept(
      m_sock_fd, reinterpret_cast<sockaddr*>(&accepted_client), &client_size);
  if (-1 == accepted) {
    *_error = network_error::invalid_parameters;
    return nullptr;
  }
  return memory::make_unique<WNReliableNetworkTransportSocket>(
      m_allocator, m_allocator, accepted, m_manager);
}

}  // namespace networking
}  // namespace wn
