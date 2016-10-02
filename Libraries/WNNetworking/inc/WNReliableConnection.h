// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_RELIABLE_CONNECTION_H__
#define __WN_NETWORKING_RELIABLE_CONNECTION_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"
#include "WNNetworking/inc/WNReceiveBuffer.h"

namespace wn {

namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace networking {
struct send_buffer {
  const void* data;
  size_t size;
};

class WNReliableConnection {
public:
  WNReliableConnection(memory::allocator* _allocator, WNBufferManager* _manager)
    : m_allocator(_allocator), m_manager(_manager) {}

  virtual ~WNReliableConnection() {}

  template <typename It>
  network_error send_sync(const It& begin, size_t size) {
    const send_buffer** send_buffers = static_cast<const send_buffer**>(
        WN_STACK_ALLOC(sizeof(send_buffer*) * size));
    auto it = begin;
    for (size_t i = 0; i < size; ++i, ++it) {
      send_buffers[i] = &(*it);
    }
    containers::contiguous_range<const send_buffer*> send_range(
        send_buffers, size);
    return do_send(send_range);
  }

  network_error send_sync(
      const std::initializer_list<const send_buffer>& list) {
    return send_sync(list.begin(), list.size());
  }

  virtual WNReceiveBuffer recv_sync() = 0;

protected:
  virtual network_error do_send(
      const containers::contiguous_range<const send_buffer*>& buffers) = 0;
  WNBufferManager* m_manager;
  memory::allocator* m_allocator;
};

class WNReliableAcceptConnection {
public:
  WNReliableAcceptConnection(
      memory::allocator* _allocator, WNBufferManager* _manager)
    : m_allocator(_allocator), m_manager(_manager) {}

  virtual ~WNReliableAcceptConnection() {}

  virtual memory::unique_ptr<WNReliableConnection> accept_sync(
      network_error* _error = nullptr) = 0;

protected:
  WNBufferManager* m_manager;
  memory::allocator* m_allocator;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NETWORKING_RELIABLE_CONNECTION_H__