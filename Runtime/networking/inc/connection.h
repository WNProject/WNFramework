// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_NETWORKING_CONNECTION_H__
#define __WN_RUNTIME_NETWORKING_CONNECTION_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNNetworking/inc/WNConnection.h"

namespace wn {
namespace runtime {
namespace networking {

uint32_t k_infinite = 0xFFFFFFFF;

class connection : public multi_tasking::synchronized<> {
public:
  explicit connection(
      memory::unique_ptr<wn::networking::WNConnection> _connection)
    : m_base_connection(core::move(_connection)) {}

  // send_object sends the given bytes through this connection.
  // The given object must support begin(), end() and size().
  // Furthermore the bytes must be contiguous in memory. The object
  // will be destroyed on completion.
  template <typename T>
  void send_object(multi_tasking::async_blocking_function, T&& t) {
    WN_DEBUG_ASSERT(static_cast<size_t>(t.end() - t.begin()) == t.size());
    wn::networking::send_range range(&(*t.begin()), t.size());
    wn::networking::send_ranges ranges(&range, 1);
    wn::networking::network_error (
        wn::networking::WNConnection::send_pipe::*func)(
        const wn::networking::send_ranges&) =
        &wn::networking::WNConnection::send_pipe::send_sync;

    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        func, m_base_connection->get_send_pipe(), ranges);
  }

  // send_async sends the given bytes through this connection.
  //   the given contiguous_range must remain valid until
  //   the send_async job is completed.
  // NB: You can use the signal on the add_job to notify
  //     you once the send has completed.
  void send(multi_tasking::async_blocking_function f,
      containers::contiguous_range<uint8_t> _bytes) {
    // This is valid: We are calling another async function
    //    but not using the job system. This is fine HOWEVER:
    //    we have to be very careful to never call an async function
    //    on a different object in this way, since it will avoid
    //    the synchronization.
    send_object(f, _bytes);
  }

  // recv_with_callback receives a series of num_messages messages from the
  // socket.
  //    When a message is recieved it is sent to teh given callback.
  // If and error occurs, or the connection is closed, then
  //    the callback will be called, and the ReceiveBuffer will
  //    contain the status.
  void recv_with_callback(multi_tasking::async_blocking_function,
      multi_tasking::callback<wn::networking::WNReceiveBuffer> _callback,
      uint32_t _num_messages = k_infinite) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _callback, _num_messages]() {
          for (size_t i = 0; i < _num_messages; ++i) {
            if (_num_messages == k_infinite) {
              i = 0;
            }
            wn::networking::WNReceiveBuffer buffer =
                m_base_connection->get_recv_pipe()->recv_sync();
            wn::networking::network_error err = buffer.get_status();
            if (err != wn::networking::network_error::ok &&
                scheduled_for_destruction()) {
              buffer.set_status(wn::networking::network_error::aborted);
            }

            _callback->enqueue_job(
                multi_tasking::job_pool::this_job_pool(), core::move(buffer));
            if (err != wn::networking::network_error::ok) {
              break;
            }
          }
        });
  }

  // recv receieves a single message from the socket.
  //  This function is less recommended, as it forces more jobs for every packet
  //    that is sent.
  void recv(multi_tasking::async_blocking_function,
      wn::networking::WNReceiveBuffer* _buffer) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _buffer]() {
          wn::networking::WNReceiveBuffer buffer =
              m_base_connection->get_recv_pipe()->recv_sync();
          *_buffer = core::move(buffer);
        });
  }

private:
  void on_destroy() {
    wn::multi_tasking::job_signal signal(0);
    wn::multi_tasking::job_pool::this_job_pool()->call_blocking_function_in_job(
        &signal, &wn::networking::WNConnection::close, m_base_connection.get());
    signal.wait_until(1);
  }

  memory::unique_ptr<wn::networking::WNConnection> m_base_connection;
};

class accept_connection : public multi_tasking::synchronized<> {
public:
  explicit accept_connection(memory::allocator* _allocator,
      memory::unique_ptr<wn::networking::WNAcceptConnection> _accept)
    : m_base_connection(core::move(_accept)), m_allocator(_allocator) {}

  // This will return aborted if this connection has been closed
  //  while the callback is still being run.
  void accept_with_callback(multi_tasking::async_blocking_function,
      multi_tasking::callback<multi_tasking::sync_ptr<connection>,
          wn::networking::network_error>
          _callback,
      size_t _num_connections = k_infinite) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _callback, _num_connections]() {
          for (size_t i = 0; i < _num_connections; ++i) {
            if (_num_connections == k_infinite) {
              i = 0;
            }
            wn::networking::network_error err =
                wn::networking::network_error::ok;
            memory::unique_ptr<wn::networking::WNConnection> base_connection =
                m_base_connection->accept_sync(&err);
            if (err != wn::networking::network_error::ok) {
              if (scheduled_for_destruction()) {
                // The decision here is, if we are being closed, due to
                // this object being destroyed, then we should
                // simply return aborted.
                err = wn::networking::network_error::aborted;
              }
              _callback->enqueue_job(
                  multi_tasking::job_pool::this_job_pool(), nullptr, err);
              break;
            } else {
              _callback->enqueue_job(multi_tasking::job_pool::this_job_pool(),
                  multi_tasking::make_sync<connection>(
                      m_allocator, core::move(base_connection)),
                  err);
            }
          }
        });
  }

  void accept(multi_tasking::async_blocking_function,
      multi_tasking::sync_ptr<connection>* _connection,
      wn::networking::network_error* _error) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _connection, _error]() {
          wn::networking::network_error err = wn::networking::network_error::ok;
          memory::unique_ptr<wn::networking::WNConnection> base_connection =
              m_base_connection->accept_sync(&err);
          if (_error) {
            *_error = err;
          }
          *_connection = multi_tasking::make_sync<connection>(
              m_allocator, core::move(base_connection));
        });
  }

private:
  void on_destroy() {
    wn::multi_tasking::job_signal signal(0);
    wn::multi_tasking::job_pool::this_job_pool()->call_blocking_function_in_job(
        &signal, &wn::networking::WNAcceptConnection::close,
        m_base_connection.get());
    signal.wait_until(1);
  }

  memory::unique_ptr<wn::networking::WNAcceptConnection> m_base_connection;
  memory::allocator* m_allocator;
};

}  // namespace networking
}  // namespace runtime
}  // namespace wn

#endif