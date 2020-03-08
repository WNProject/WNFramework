// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNReliableNetworkListenSocket.h"

#include "WNNetworking/inc/WNReliableNetworkTransportSocket.h"

static const uint32_t kIncommingConnectionBacklog = 10;

namespace wn {
namespace networking {

network_error WNReliableConnectListenSocket::initialize(
    logging::log* _log, int _protocol, uint16_t _port) {
  m_socket = WSASocketW(
      _protocol, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
  if (m_socket == INVALID_SOCKET) {
    return network_error::invalid_parameters;
  }

  char port_array[11] = {0};
  memory::writeuint32(port_array, _port, 10);

  addrinfo* result;
  addrinfo* ptr;

  if (0 != GetAddrInfoA(NULL, port_array, nullptr, &result)) {
    _log->log_error("Could not resolve local port ", port_array);
    return network_error::could_not_resolve;
  }

  // TODO(awoloszyn)
  ptr = result;
  for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
    if (ptr->ai_family == _protocol) {
      break;
    }
  }

  if (ptr == nullptr) {
    _log->log_error("Could not resolve local port ", port_array);
    FreeAddrInfoA(result);
    return network_error::could_not_resolve;
  }

  int error = bind(m_socket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
  FreeAddrInfoA(result);
  if (SOCKET_ERROR == error) {
    _log->log_error("Could not bind to port ", port_array);
    return network_error::could_not_bind;
  }

  if (0 != listen(m_socket, kIncommingConnectionBacklog)) {
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
  char accept_addr[64] = {0};
  int client_size = 64;
  SOCKET accepted =
      WSAAccept(m_socket, (SOCKADDR*)&accept_addr, &client_size, nullptr, NULL);
  if (INVALID_SOCKET == accepted && m_socket != -1) {
    *_error = network_error::invalid_parameters;
    return nullptr;
  }
  if (m_socket == -1) {
    *_error = network_error::closed;
    return nullptr;
  }
  return memory::make_unique<WNReliableNetworkTransportSocket>(
      m_allocator, m_allocator, accepted, m_manager);
}

void WNReliableConnectListenSocket::close() {
  if (m_socket != INVALID_SOCKET) {
    closesocket(m_socket);
    m_socket = INVALID_SOCKET;
  }
}

}  // namespace networking
}  // namespace wn
