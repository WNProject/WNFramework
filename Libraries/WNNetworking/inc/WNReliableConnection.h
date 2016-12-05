// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_RELIABLE_CONNECTION_H__
#define __WN_NETWORKING_RELIABLE_CONNECTION_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"
#include "WNNetworking/inc/WNReceiveBuffer.h"

namespace wn {

namespace networking {

class WNReliableConnection : public WNConnection {
public:
  WNReliableConnection(memory::allocator* _allocator,
      multi_tasking::job_pool* job_pool, WNBufferManager* _manager)
    : WNConnection(_allocator, job_pool), m_manager(_manager) {}

  virtual ~WNReliableConnection() {}

protected:
  WNBufferManager* m_manager;
};

class WNReliableAcceptConnection : public WNAcceptConnection {
public:
  WNReliableAcceptConnection(memory::allocator* _allocator,
      multi_tasking::job_pool* _pool, WNBufferManager* _manager)
    : WNAcceptConnection(_pool), m_allocator(_allocator), m_manager(_manager) {}

  virtual ~WNReliableAcceptConnection() {}

protected:
  WNBufferManager* m_manager;
  memory::allocator* m_allocator;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NETWORKING_RELIABLE_CONNECTION_H__