// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_CONNECTION_H__
#define __WN_NETWORKING_CONNECTION_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNSynchronized.h"
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
  class send_pipe : public multi_tasking::synchronized<> {
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

    void send_async(multi_tasking::job_signal* _signal, network_error* _error,
        containers::dynamic_array<send_range> _ranges) {
      m_connection->m_job_pool->add_job(_signal,
          &send_pipe::internal_send_async, this, core::move(_ranges), _error);
    }

    template <typename It>
    void send_async(multi_tasking::job_signal* _signal, network_error* _error,
        const It& _begin, size_t _size) {
      containers::dynamic_array<send_range> ranges(m_connection->m_allocator);
      ranges.reserve(_size);
      auto it = _begin;
      for (size_t i = 0; i < _size; ++i, ++it) {
        ranges.push_back(*it);
      }
      return send_async(_signal, _error, core::move(ranges));
    }

    void send_async(multi_tasking::job_signal* _signal, network_error* _error,
        const std::initializer_list<const send_range>& _list) {
      return send_async(_signal, _error, _list.begin(), _list.size());
    }

  private:
    struct send_sync_params {
      containers::dynamic_array<send_range> _params;
      network_error* _err;
    };
    void internal_send_async(send_sync_params& s) {
      if (s._err) {
        *s._err =
            m_connection->m_job_pool->call_blocking_function<network_error>(
                &WNConnection::do_send, m_connection,
                send_ranges(s._params.data(), s._params.size()));
      } else {
        m_connection->m_job_pool->call_blocking_function(&WNConnection::do_send,
            m_connection, send_ranges(s._params.data(), s._params.size()));
      }
    }

    void internal_send_sync(const send_ranges& _ranges, network_error* _err) {
      if (_err) {
        *_err = m_connection->do_send(_ranges);
      } else {
        m_connection->do_send(_ranges);
      }
    }

    WNConnection* m_connection;
  };

  class recv_pipe : public multi_tasking::synchronized<> {
  public:
    recv_pipe(WNConnection* _connection) : m_connection(_connection) {}

    WNReceiveBuffer recv_sync() {
      return m_connection->recv_sync();
    }

    void recv_async(multi_tasking::callback_ptr<WNReceiveBuffer&&> callback,
        multi_tasking::job_pool* _pool = nullptr) {
      m_connection->m_job_pool->add_job(nullptr, &recv_pipe::recv_internal,
          this, core::move(callback), _pool);
    }

  private:
    struct recv_internal_params {
      multi_tasking::callback_ptr<WNReceiveBuffer&&> callback;
      multi_tasking::job_pool* _pool;
    };

    void recv_internal(recv_internal_params& _r) {
      multi_tasking::job_pool* pool =
          _r._pool ? _r._pool : m_connection->m_job_pool;
      network_error err = network_error::ok;
      do {
        auto val =
            m_connection->m_job_pool->call_blocking_function<WNReceiveBuffer>(
                &recv_pipe::recv_sync, this);
        err = val.get_status();
        _r.callback->enqueue_job(pool, core::move(val));
      } while (err == network_error::ok);
    }

    WNConnection* m_connection;
  };

  WNConnection(memory::allocator* _allocator, multi_tasking::job_pool* job_pool)
    : m_send_pipe(this),
      m_recv_pipe(this),
      m_allocator(_allocator),
      m_job_pool(job_pool) {}

  virtual ~WNConnection() {}

  send_pipe* get_send_pipe() {
    return &m_send_pipe;
  }
  recv_pipe* get_recv_pipe() {
    return &m_recv_pipe;
  }

protected:
  send_pipe m_send_pipe;
  recv_pipe m_recv_pipe;
  memory::allocator* m_allocator;
  multi_tasking::job_pool* m_job_pool;

  virtual WNReceiveBuffer recv_sync() = 0;
  virtual network_error do_send(const send_ranges& buffers) = 0;
};

const uint32_t infinite = 0xFFFFFFFF;

class WNAcceptConnection : public multi_tasking::synchronized<> {
public:
  explicit WNAcceptConnection(multi_tasking::job_pool* _pool) : m_pool(_pool) {}
  virtual ~WNAcceptConnection() {}

  virtual memory::unique_ptr<WNConnection> accept_sync(
      network_error* _error = nullptr) = 0;

  // Sets up a callback to wait for connections.
  // if num_connections == infininte, then it will create a job for the
  // the callback for every connection, otherwise it will only call it for the
  // first num_connections, connections.
  // If pool is nullptr, then the jobs will be run on the same job pool
  // as the connection, otherwise they will be created on the given pool.
  // If the connection was closed, accepted_socket returns nullptr,
  // and no more callbacks will be created.
  void accept_async(
      functional::function<void(memory::unique_ptr<networking::WNConnection>)>
          callback = nullptr,
      multi_tasking::job_pool* _pool = nullptr,
      uint32_t num_connections = infinite) {
    m_pool->add_job(nullptr, &WNAcceptConnection::accept_async_internal_job,
        this, callback, _pool, num_connections);
  }

  void accept_async(multi_tasking::job_signal* _signal,
      memory::unique_ptr<networking::WNConnection>* _output_connection,
      network_error* _error) {
    m_pool->add_job(_signal, &WNAcceptConnection::accept_async_internal, this,
        _output_connection, _error);
  }

protected:
  struct accept_async_internal_params {
    memory::unique_ptr<networking::WNConnection>* _output_connection;
    network_error* _err;
  };

  void accept_async_internal(accept_async_internal_params& _a) {
    *_a._output_connection = m_pool->call_blocking_function<
        memory::unique_ptr<networking::WNConnection>>(
        &WNAcceptConnection::accept_sync, this, (network_error*)_a._err);
  }

  struct accept_async_internal_job_params {
    functional::function<void(memory::unique_ptr<networking::WNConnection>)>
        _callback;
    multi_tasking::job_pool* _pool;
    uint32_t _num_connections;
  };

  void accept_async_internal_job(accept_async_internal_job_params& _c) {
    multi_tasking::job_pool* pool = _c._pool ? _c._pool : m_pool;
    for (uint32_t i = 0; i < _c._num_connections; ++i) {
      auto accepted_socket =
          m_pool->call_blocking_function<memory::unique_ptr<WNConnection>>(
              &WNAcceptConnection::accept_sync, this, nullptr);
      memory::allocator* alloc = accepted_socket.get_allocator();

      pool->add_unsynchronized_job<functional::function<void(
          networking::WNConnection*, memory::allocator*)>>(nullptr,
          [_c](networking::WNConnection* _conn, memory::allocator* _alloc) {
            auto c =
                memory::unique_ptr<networking::WNConnection>(_alloc, _conn);
            _c._callback(core::move(c));
          },
          accepted_socket.release(), alloc);

      if (accepted_socket == nullptr) {
        return;
      }
    }
  }

  multi_tasking::job_pool* m_pool;
};

}  // namespace networking
}  // namesapce wn

#endif  // __WN_NETWORKING_CONNECTION_H__