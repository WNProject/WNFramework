// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNReliableNetworkTransportSocket.h"
#include "WNNetworking/inc/WNReliableNetworkListenSocket.h"

namespace wn {
namespace networking {

network_error WNReliableNetworkTransportSocket::connect_to(WNLogging::WNLog* _log,
    const containers::string_view& target, uint32_t _connection_type,
    uint16_t _port) {
  char port_array[11] = {0};
  memory::writeuint32(port_array, _port, 10);

  addrinfo* result;
  addrinfo* ptr;

  if (0 != GetAddrInfoA(target.to_string(m_allocator).c_str(), port_array,
               nullptr, &result)) {
    _log->Log(
        WNLogging::eError, 0, "Could not resolve local port ", port_array);
    return network_error::could_not_resolve;
  }

  ptr = result;
  for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
    if ((_connection_type == 0xFFFFFFFF &&
            (ptr->ai_family == AF_INET || ptr->ai_family == AF_INET6)) ||
        ptr->ai_family == static_cast<int>(_connection_type)) {
      m_socket = WSASocketW(ptr->ai_family, SOCK_STREAM, IPPROTO_TCP, nullptr,
          0, WSA_FLAG_OVERLAPPED);
      if (m_socket == INVALID_SOCKET) {
        continue;
      }
      if (0 != WSAConnect(m_socket, ptr->ai_addr,
                   static_cast<int>(ptr->ai_addrlen), NULL, NULL, NULL, NULL)) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        continue;
      }
      break;
    }
  }
  FreeAddrInfoA(result);
  if (ptr == nullptr) {
    _log->Log(WNLogging::eError, 0, "Could not resolve target ", target, ":",
        port_array);
    return network_error::could_not_resolve;
  }
  return network_error::ok;
}
}  // namespace networking
}  // wn
