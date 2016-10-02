// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNReliableNetworkTransportSocket.h"
#include "WNNetworking/inc/WNReliableNetworkListenSocket.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

namespace wn {
namespace networking {

network_error WNReliableNetworkTransportSocket::connect_to(
    WNLogging::WNLog* _log, const containers::string_view& target,
    uint32_t _connection_type, uint16_t _port) {
  char port_array[11] = {0};
  memory::writeuint32(port_array, _port, 10);

  addrinfo* result;
  addrinfo* ptr;

  if (0 != getaddrinfo(target.to_string(m_allocator).c_str(), port_array,
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
      m_sock_fd = socket(ptr->ai_family, SOCK_STREAM, IPPROTO_TCP);
      if (m_sock_fd == -1) {
        continue;
      }
      if (0 != connect(m_sock_fd, ptr->ai_addr, ptr->ai_addrlen)) {
        close(m_sock_fd);
        m_sock_fd = -1;
        continue;
      }
      break;
    }
  }
  freeaddrinfo(result);
  if (ptr == nullptr) {
    _log->Log(WNLogging::eError, 0, "Could not resolve target ", target, ":",
        port_array);
    return network_error::could_not_resolve;
  }
  return network_error::ok;
}

network_error WNReliableNetworkTransportSocket::do_send(
    const containers::contiguous_range<const send_buffer*>& buffers) {
  iovec* send_buffers =
      static_cast<iovec*>(WN_STACK_ALLOC(sizeof(iovec) * buffers.size()));

  size_t total_bytes = 0;
  for (size_t i = 0; i < buffers.size(); ++i) {
    total_bytes += buffers[i]->size;
    send_buffers[i].iov_base = const_cast<void*>(buffers[i]->data);
    send_buffers[i].iov_len = buffers[i]->size;
  }

  msghdr header = {nullptr,  // msg_name
      0,                     // msg_namelen
      send_buffers,          // msg_iov
      buffers.size(),        // msg_iovlen
      nullptr,               // msg_control
      0,                     //  msg_controllen
      0};

  size_t num_sent = 0;
  if (0 > (num_sent = sendmsg(m_sock_fd, &header, 0))) {
    return network_error::could_not_send;
  }
  if (num_sent != total_bytes) {
    return network_error::could_not_send;
    close(m_sock_fd);
    m_sock_fd = -1;
  }
  return network_error::ok;
}

WNReceiveBuffer WNReliableNetworkTransportSocket::recv_sync() {
  WNReceiveBuffer buffer(m_manager->acquire_buffer());
  ssize_t received = 0;
  if (0 >=
      (received = recv(m_sock_fd, buffer.data.data(), buffer.data.size(), 0))) {
    return WNReceiveBuffer(network_error::could_not_receive);
  }
  buffer.data =
      containers::contiguous_range<char>(buffer.data.data(), received);

  return core::move(buffer);
}

}  // namespace networking
}  // wn
