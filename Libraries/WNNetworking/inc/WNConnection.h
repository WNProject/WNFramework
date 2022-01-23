// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_CONNECTION_H__
#define __WN_NETWORKING_CONNECTION_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"
#include "WNNetworking/inc/WNReceiveBuffer.h"

namespace wn {
namespace networking {

using send_range = containers::contiguous_range<const uint8_t>;
using send_ranges = containers::contiguous_range<const send_range>;

class WNConnection : public core::non_copyable {
public:
  // We want to synchronize both the sending and receiving of data
  // but we want them to be synchronized independently of each other.
  class send_pipe {
  public:
    send_pipe(WNConnection* _connection) : m_connection(_connection) {}

    network_error send_sync(const send_ranges& _ranges) {
      network_error err;
      internal_send_sync(_ranges, &err);
      return err;
    }

    template <typename It>
    network_error send_sync(const It& _begin, size_t _size) {
      send_range* ranges =
          static_cast<send_range*>(WN_STACK_ALLOC(sizeof(send_range) * _size));
      auto it = _begin;
      for (size_t i = 0; i < _size; ++i, ++it) {
        ranges[i] = *it;
      }
      return send_sync(send_ranges(ranges, _size));
    }

    network_error send_sync(
        const std::initializer_list<const send_range>& _list) {
      return send_sync(_list.begin(), _list.size());
    }

  private:
    void internal_send_sync(const send_ranges& _ranges, network_error* _err) {
      if (_err) {
        *_err = m_connection->do_send(_ranges);
      } else {
        m_connection->do_send(_ranges);
      }
    }

    WNConnection* m_connection;
  };

  class recv_pipe {
  public:
    recv_pipe(WNConnection* _connection) : m_connection(_connection) {}

    WNReceiveBuffer recv_sync() {
      return m_connection->recv_sync();
    }

  private:
    WNConnection* m_connection;
  };

  WNConnection(memory::allocator* _allocator, WNBufferManager* _manager)
    : m_send_pipe(this),
      m_recv_pipe(this),
      m_allocator(_allocator),
      m_manager(_manager) {}

  virtual ~WNConnection() {}
  virtual void close() = 0;

  send_pipe* get_send_pipe() {
    return &m_send_pipe;
  }
  recv_pipe* get_recv_pipe() {
    return &m_recv_pipe;
  }

  WNBufferManager* get_buffer_manager() {
    return m_manager;
  }

protected:
  send_pipe m_send_pipe;
  recv_pipe m_recv_pipe;
  memory::allocator* m_allocator;
  WNBufferManager* m_manager;

  friend class WNRoutedConnection;
  virtual WNReceiveBuffer recv_sync() = 0;
  virtual network_error do_send(const send_ranges& buffers) = 0;
};

const uint32_t infinite = 0xFFFFFFFF;

class WNAcceptConnection {
public:
  explicit WNAcceptConnection(WNBufferManager* _manager)
    : m_manager(_manager) {}
  virtual ~WNAcceptConnection() {}

  virtual memory::unique_ptr<WNConnection> accept_sync(
      network_error* _error = nullptr) = 0;

  virtual void close() = 0;
  WNBufferManager* m_manager;
  friend class WNRoutedAcceptConnection;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NETWORKING_CONNECTION_H__
