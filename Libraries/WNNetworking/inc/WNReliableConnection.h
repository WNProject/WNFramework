// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_RELIABLE_CONNECTION_H__
#define __WN_NETWORKING_RELIABLE_CONNECTION_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"

namespace wn {

namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace networking {
class WNReliableConnection {
public:
  WNReliableConnection(memory::allocator* _allocator)
    : m_allocator(_allocator) {}

  virtual ~WNReliableConnection() {}

public:
  memory::allocator* m_allocator;
};

class WNReliableAcceptConnection {
public:
  WNReliableAcceptConnection(memory::allocator* _allocator)
    : m_allocator(_allocator) {}

  virtual ~WNReliableAcceptConnection() {}

  virtual memory::unique_ptr<WNReliableConnection> accept_sync(
      network_error* _error = nullptr) = 0;

public:
  memory::allocator* m_allocator;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NETWORKING_RELIABLE_CONNECTION_H__