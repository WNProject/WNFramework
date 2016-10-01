// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNReliableNetworkTransportSocket.h"
#include "WNContainers/inc/WNContiguousRange.h"
#include "WNNetworking/inc/WNReliableNetworkListenSocket.h"

namespace wn {
namespace networking {

network_error WNReliableNetworkTransportSocket::connect_to(
    WNLogging::WNLog* _log, const containers::string_view& target,
    uint32_t _connection_type, uint16_t _port) {
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

network_error WNReliableNetworkTransportSocket::do_send(
    const containers::contiguous_range<const send_buffer*>& buffers) {
  WSABUF* send_buffers =
      static_cast<WSABUF*>(WN_STACK_ALLOC(sizeof(WSABUF) * buffers.size()));
  for (size_t i = 0; i < buffers.size(); ++i) {
    send_buffers[i].buf =
        static_cast<char*>(const_cast<void*>(buffers[i]->data));
    send_buffers[i].len = static_cast<u_long>(buffers[i]->size);
  }
  DWORD number_of_bytes_sent = 0;
  if (0 != WSASend(m_socket, send_buffers, static_cast<DWORD>(buffers.size()),
               &number_of_bytes_sent, 0, NULL, NULL)) {
    return network_error::could_not_send;
  }
  return network_error::ok;
}

WNReceiveBuffer WNReliableNetworkTransportSocket::recv_sync() {
  WNReceiveBuffer buffer(get_receieve_buffer());

  WSABUF recv_buff{static_cast<ULONG>(buffer.data.size()), buffer.data.data()};
  DWORD recieved = 0;
  DWORD flags = 0;
  if (0 != WSARecv(m_socket, &recv_buff, 1, &recieved, &flags, NULL, NULL)) {
    return WNReceiveBuffer(network_error::could_not_receive);
  }
  buffer.data =
      containers::contiguous_range<char>(buffer.data.data(), recieved);

  return core::move(buffer);
}

}  // namespace networking
}  // wn
