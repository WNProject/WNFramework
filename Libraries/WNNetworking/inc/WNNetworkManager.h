// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NEWORKING_NETWORK_MANAGER_H__
#define __WN_NEWORKING_NETWORK_MANAGER_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNMultiTasking/inc/WNSpinLock.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"
#include "WNNetworking/inc/WNReceiveBuffer.h"
#include "WNNetworking/inc/WNReliableConnection.h"

namespace wn {
namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace networking {
const uint32_t MAX_NETWORK_BUFFER_SIZE = 1024;
const uint32_t MAX_NETWORK_MESSAGE_SIZE = 1000;

enum class available_network_kind_bits : uint32_t { remote = 0x1, local = 0x2 };

enum class ip_protocol { ipv4, ipv6, ip_any };

// All asynchronous calls in WNNetworkManager are only valid
// inside of the job system. Specifically that means it is only
// valid to call them from within a job. And they may spin up
// other jobs as necessary.
class WNNetworkManager {
public:
  explicit WNNetworkManager(
      memory::allocator* _allocator, multi_tasking::job_pool* _job_pool)
    : m_allocator(_allocator), m_job_pool(_job_pool) {}

  virtual ~WNNetworkManager() {}

  virtual uint32_t get_available_network_kinds() const = 0;

  // not valid to call with ip_any
  virtual memory::unique_ptr<WNReliableAcceptConnection> listen_remote_sync(
      ip_protocol protocol, uint16_t port, network_error* _error = nullptr) = 0;

  virtual memory::unique_ptr<WNReliableConnection> connect_remote_sync(
      const containers::string_view& target, ip_protocol protocol,
      uint16_t port, network_error* _error = nullptr) = 0;

  void listen_remote_async(ip_protocol protocol, uint16_t port,
      multi_tasking::job_signal* _signal,
      memory::unique_ptr<WNReliableAcceptConnection>* _connection,
      network_error* _error) {
    m_job_pool->add_unsynchronized_job(_signal, [=] {
      *_connection = m_job_pool->call_blocking_function<
          memory::unique_ptr<WNReliableAcceptConnection>>(
          &WNNetworkManager::listen_remote_sync, this, protocol, port, _error);
    });
  }

  void connect_remote_async(const containers::string_view& target,
      ip_protocol protocol, uint16_t port, multi_tasking::job_signal* _signal,
      memory::unique_ptr<WNReliableConnection>* _connection,
      network_error* _error) {
    containers::string s = target.to_string(m_allocator);
    m_job_pool->add_unsynchronized_job(_signal, [=] {
      *_connection = m_job_pool->call_blocking_function<
          memory::unique_ptr<WNReliableConnection>>(
          &WNNetworkManager::connect_remote_sync, this, s, protocol, port,
          _error);
    });
  }

protected:
  friend class WNReliableConnection;
  friend class WNReliableAcceptConnection;

  memory::allocator* m_allocator;
  multi_tasking::job_pool* m_job_pool;
};

// The _job_pool here may be different than the job_pool used
// to call asynchronous functions. This job_pool will be the one
// used by asynchronous calls to invoke "call_blocking_function",
// and as such may have a large number of blocking calls
// sitting in it simultaneously.
// In practice it is expected that there will be a total of 2
// job_pools in any system. One to handle system_calls and
// low_priority synchronous work, and another to handle
// non-blocking jobs.
class WNConcreteNetworkManager : public WNNetworkManager {
public:
  WNConcreteNetworkManager(memory::allocator* _allocator,
      multi_tasking::job_pool* _job_pool, WNLogging::WNLog* _log)
    : WNNetworkManager(_allocator, _job_pool),
      m_log(_log),
      m_buffer_manager(_allocator) {
    initialize();
  }

  ~WNConcreteNetworkManager();

  // Returns a bitmask of available_network_kind_bits.
  uint32_t get_available_network_kinds() const override;

  // Returns a connection that is able to accept new
  // WNReliableConntections made from a remote source.
  // Returns nullptr if the connection could not be created.
  // If _error is not nullptr, it gets filled with the
  // reason for the failure
  memory::unique_ptr<WNReliableAcceptConnection> listen_remote_sync(
      ip_protocol protocol, uint16_t port,
      network_error* _error = nullptr) override;

  // Returns a connection that is able to send data to
  // a remote target.
  // Returns nullptr if the connection could not be created.
  // If _error is not nullptr, it gets filled with the reason
  // for the failure.
  memory::unique_ptr<WNReliableConnection> connect_remote_sync(
      const containers::string_view& target, ip_protocol protocol,
      uint16_t port, network_error* _error = nullptr) override;

protected:
  class WNCachedBufferManager : public WNBufferManager {
  public:
    WNCachedBufferManager(memory::allocator* _allocator)
      : m_allocator(_allocator),
        m_buffers(_allocator),
        m_available_buffers(_allocator) {}
    WNReceiveBuffer acquire_buffer() override {
      multi_tasking::lock_guard<multi_tasking::spin_lock> lock(m_cache_lock);
      if (m_available_buffers.size() > 0) {
        char* buff = m_available_buffers.front();
        m_available_buffers.pop_front();
        return construct_buffer(static_cast<const void*>(buff),
            containers::contiguous_range<char>(buff, MAX_NETWORK_BUFFER_SIZE));
      } else {
        m_buffers.push_back(containers::array<char, MAX_NETWORK_BUFFER_SIZE>());
        return construct_buffer(m_buffers.back().data(), m_buffers.back());
      }
    }
    void release_buffer(const void* _token) override {
      multi_tasking::lock_guard<multi_tasking::spin_lock> lock(m_cache_lock);
      m_available_buffers.push_back(
          static_cast<char*>(const_cast<void*>(_token)));
    }

  private:
    multi_tasking::spin_lock m_cache_lock;
    memory::allocator* m_allocator;
    containers::deque<containers::array<char, MAX_NETWORK_BUFFER_SIZE>>
        m_buffers;
    containers::deque<char*> m_available_buffers;
  };

  WNCachedBufferManager m_buffer_manager;

private:
  void initialize();
  WNLogging::WNLog* m_log;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_NETWORK_MANAGER_H__