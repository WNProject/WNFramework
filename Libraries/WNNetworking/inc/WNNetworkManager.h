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

enum class available_network_kind_bits : uint32_t { remote = 0x1, local = 0x2 };

enum class ip_protocol { ipv4, ipv6, ip_any };

class WNNetworkManager {
public:
  explicit WNNetworkManager(memory::allocator* _allocator)
    : m_allocator(_allocator) {}

  virtual ~WNNetworkManager() {}

  virtual uint32_t get_available_network_kinds() const = 0;

  // not valid to call with ip_any
  virtual memory::unique_ptr<WNReliableAcceptConnection> listen_remote_sync(
      ip_protocol protocol, uint16_t port, network_error* _error = nullptr) = 0;

  virtual memory::unique_ptr<WNReliableAcceptConnection> listen_local_sync(
      const containers::string_view& name, network_error* _error = nullptr) = 0;

  virtual memory::unique_ptr<WNReliableConnection> connect_remote_sync(
      const containers::string_view& target, ip_protocol protocol,
      uint16_t port, network_error* _error = nullptr) = 0;

  virtual memory::unique_ptr<WNReliableConnection> connect_local_sync(
      const containers::string_view& target,
      network_error* _error = nullptr) = 0;

protected:
  friend class WNReliableConnection;
  friend class WNReliableAcceptConnection;

  memory::allocator* m_allocator;
};

class WNConcreteNetworkManager : public WNNetworkManager {
public:
  WNConcreteNetworkManager(memory::allocator* _allocator,
      multi_tasking::job_pool* _job_pool, WNLogging::WNLog* _log)
    : WNNetworkManager(_allocator),
      m_job_pool(_job_pool),
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

  // Returns a connection that is able to accept new
  // WNReliableConntections made on a local socket.
  // Returns nullptr if the connection could not be created.
  // If _error is not nullptr, it gets filled with the
  // reason for the failure
  memory::unique_ptr<WNReliableAcceptConnection> listen_local_sync(
      const containers::string_view&, network_error* = nullptr) override {
    return nullptr;
  };

  // Returns a connection that is able to send data to
  // a remote target.
  // Returns nullptr if the connection could not be created.
  // If _error is not nullptr, it gets filled with the reason
  // for the failure.
  memory::unique_ptr<WNReliableConnection> connect_remote_sync(
      const containers::string_view& target, ip_protocol protocol,
      uint16_t port, network_error* _error = nullptr) override;

  // Returns a connection that is able to send data to
  // a local target.
  // Returns nullptr if the connection could not be created.
  // If _error is not nullptr, it gets filled with the reason
  // for the failure.
  memory::unique_ptr<WNReliableConnection> connect_local_sync(
      const containers::string_view&, network_error* = nullptr) override {
    return nullptr;
  };

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
  multi_tasking::job_pool* m_job_pool;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_NETWORK_MANAGER_H__