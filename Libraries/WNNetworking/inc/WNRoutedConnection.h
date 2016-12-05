// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_ROUTED_CONNECTION_H__
#define __WN_NETWORKING_ROUTED_CONNECTION_H__

#include "WNContainers/inc/WNFunction.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"
#include "WNNetworking/inc/WNReceiveBuffer.h"
#include "WNNetworking/inc/WNReliableConnection.h"

namespace wn {

namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace networking {
struct RoutedMessage {
  RoutedMessage(RoutedMessage&& _other)
    : message(core::move(_other.message)),
      message_size(_other.message_size),
      message_offset(_other.message_offset) {}

  // This section of message. It may be the whole
  // message or just a part
  WNReceiveBuffer message;
  // The total size of the message, 0 if unknown
  uint32_t message_size;
  // This message sections offset into the entire
  // message
  uint32_t message_offset;
};

class WNRoutedConnection {
public:
  using route = uint32_t;
  // Any messaged directed at route 0 will get routed to _default_route.
  // Any messages for which no route exists will also get routed to
  // _default_route.
  WNRoutedConnection(memory::allocator* _allocator,
      containers::function<void(route, RoutedMessage _message)>& _default_route,
      WNConnection* _underlying_connection, WNBufferManager* _manager)
    : m_allocator(_allocator),
      m_underlying_connection(_underlying_connection),
      m_manager(_manager),
      m_default_route(_default_route) {}

  // Returns the next unused routing number.
  route next_route() {
    // TODO(awoloszyn): Fill this in.
  }

  // Stops routing messages directed at the given route, they will
  // subsequently be routed to _default_rotue until someone else
  // starts listening again.
  void remove_route(route _route) {}

  // When a message is receieved on the given route, a job will be created
  // with the given callback. This over-writes any previous notification
  // with the same _route.
  void notify_on_receive(
      route _route, containers::function<void(RoutedMessage _message)>& v) {
    // TODO(awoloszyn): Fill this in
  }

  // Increments *signal, fills _returned_buffer with the returned message. This
  // over-writes any previous notification with the same _route.
  void signal_on_receive(route _route, RoutedMessage* _returned_buffer,
      multi_tasking::job_signal* signal) {
    // TODO(awoloszyn): Fill this in.
  }

  // Sends a single message asynchronously.
  template <typename It>
  void send_async(route, const It& begin, size_t size, ) {}

  // Starts a new multi-part message, the message will be considered
  // complete when the MessageToken is destroyed.
  template <typename It>
  MessageToken send_first_part_async(route, const It& begin, size_t size) {}

  // Continues a multi-part message.
  template <typename It>
  MessageToken send_subsequent_part_async(
      MessageToken token, const It& begin, size_t size) {}

protected:
  memory::allocator* m_allocator;
  WNConnection* m_underlying_connection;
  WNBufferManager* m_manager;
  containers::function<void(route, RoutedMessage _message)> m_default_route;
};

// When a new connection is established, this will kick off a job with the
// given function. The function will be given ownership of a
// WNRoutedConnection that is connected to the endpoint.
class WNRoutedAcceptConnection {
public:
  WNRoutedAcceptConnection(memory::allocator* _allocator,
      WNAcceptConnection* _underlying_connection,
      const containers::function<void(
          memory::unique_ptr<WNRoutedConnection>&&)>& _new_connection_job)
    : m_allocator(_allocator),
      m_underlying_connection(_underlying_connection) {}

  virtual ~WNRoutedAcceptConnection() {}

protected:
  memory::allocator* m_allocator;
  WNAcceptConnection* m_underlying_connection;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NETWORKING_ROUTED_CONNECTION_H__