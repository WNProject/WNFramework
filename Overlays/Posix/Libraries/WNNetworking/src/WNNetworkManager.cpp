// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNNetworkManager.h"

#include "WNNetworking/inc/WNReliableNetworkListenSocket.h"
#include "WNNetworking/inc/WNReliableNetworkTransportSocket.h"

#include <sys/socket.h>

namespace wn {
namespace networking {

uint32_t WNConcreteNetworkManager::get_available_network_kinds() const {
  return static_cast<uint32_t>(available_network_kind_bits::remote);
}

void WNConcreteNetworkManager::initialize() {}

WNConcreteNetworkManager::~WNConcreteNetworkManager() {}

const uint32_t PROTOCOL_MAPPING[] = {AF_INET, AF_INET6, 0xFFFFFFFF};

memory::unique_ptr<WNReliableAcceptConnection>
WNConcreteNetworkManager::listen_remote_sync(
    ip_protocol protocol, uint16_t _port, network_error* _error) {
  network_error tmp;
  _error = _error ? _error : &tmp;
  auto socket = memory::make_unique<WNReliableConnectListenSocket>(
      m_allocator, m_allocator, &m_buffer_manager);
  if (network_error::ok !=
      (*_error = socket->initialize(
           m_log, PROTOCOL_MAPPING[static_cast<uint32_t>(protocol)], _port))) {
    return nullptr;
  }
  return core::move(socket);
}

memory::unique_ptr<WNReliableConnection>
WNConcreteNetworkManager::connect_remote_sync(
    const containers::string_view& _target, ip_protocol protocol,
    uint16_t _port, network_error* _error) {
  network_error tmp;
  _error = _error ? _error : &tmp;
  auto socket = memory::make_unique<WNReliableNetworkTransportSocket>(
      m_allocator, m_allocator, &m_buffer_manager);
  if (network_error::ok !=
      (*_error = socket->connect_to(m_log, _target,
           PROTOCOL_MAPPING[static_cast<uint32_t>(protocol)], _port))) {
    return nullptr;
  }
  return core::move(socket);
}

}  // namespace networking
}  // namespace wn
