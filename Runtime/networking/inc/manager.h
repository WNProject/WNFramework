// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_NETWORKING_MANAGER_H__
#define __WN_RUNTIME_NETWORKING_MANAGER_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "networking/inc/connection.h"

namespace wn {
namespace runtime {
namespace networking {
using wn::networking::ip_protocol;
using wn::networking::network_error;

class manager : public wn::multi_tasking::synchronized<> {
public:
  explicit manager(memory::allocator* _allocator,
      memory::unique_ptr<wn::networking::WNNetworkManager> _manager)
    : m_allocator(_allocator), m_base_manager(core::move(_manager)) {}

  uint32_t get_available_network_kinds() const {
    return m_base_manager->get_available_network_kinds();
  };

  void listen_with_callback(multi_tasking::async_blocking_function,
      ip_protocol _protocol, uint16_t _port,
      multi_tasking::callback<multi_tasking::sync_ptr<accept_connection>,
          networking::network_error>
          _callback) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _protocol, _port, _callback]() {
          networking::network_error err = networking::network_error::ok;
          auto connection =
              m_base_manager->listen_remote_sync(_protocol, _port, &err);
          if (err != networking::network_error::ok) {
            _callback->enqueue_job(
                multi_tasking::job_pool::this_job_pool(), nullptr, err);
            return;
          }
          _callback->enqueue_job(multi_tasking::job_pool::this_job_pool(),
              multi_tasking::make_sync<accept_connection>(
                  m_allocator, m_allocator, core::move(connection)),
              err);

        });
  }

  void listen(multi_tasking::async_blocking_function, ip_protocol _protocol,
      uint16_t _port, multi_tasking::sync_ptr<accept_connection>* _connection,
      networking::network_error* _error) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _protocol, _port, _connection, _error]() {
          networking::network_error err = networking::network_error::ok;
          auto connection =
              m_base_manager->listen_remote_sync(_protocol, _port, &err);
          if (_error != nullptr) {
            *_error = err;
          }
          *_connection = multi_tasking::make_sync<accept_connection>(
              m_allocator, m_allocator, core::move(connection));
        });
  }

  void connect_with_callback(multi_tasking::async_blocking_function,
      containers::string _target, ip_protocol _protocol, uint16_t _port,
      multi_tasking::callback<multi_tasking::sync_ptr<connection>,
          networking::network_error>
          _callback) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _target, _protocol, _port, _callback]() {
          networking::network_error err = networking::network_error::ok;
          auto connected = m_base_manager->connect_remote_sync(
              _target, _protocol, _port, &err);
          if (err != networking::network_error::ok) {
            _callback->enqueue_job(
                multi_tasking::job_pool::this_job_pool(), nullptr, err);
            return;
          }
          _callback->enqueue_job(multi_tasking::job_pool::this_job_pool(),
              multi_tasking::make_sync<connection>(
                  m_allocator, core::move(connected)),
              err);

        });
  }

  void connect(multi_tasking::async_blocking_function,
      containers::string _target, ip_protocol _protocol, uint16_t _port,
      multi_tasking::sync_ptr<connection>* _connection,
      networking::network_error* _error) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _target, _protocol, _port, _connection, _error]() {
          networking::network_error err = networking::network_error::ok;
          auto connected = m_base_manager->connect_remote_sync(
              _target, _protocol, _port, &err);
          if (_error != nullptr) {
            *_error = err;
          }
          *_connection = multi_tasking::make_sync<connection>(
              m_allocator, core::move(connected));
        });
  }

private:
  memory::allocator* m_allocator;
  memory::unique_ptr<wn::networking::WNNetworkManager> m_base_manager;
};
}  // namespace networking
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_NETWORKING_MANAGER_H__
