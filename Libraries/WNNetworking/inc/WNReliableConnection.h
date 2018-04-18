// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_RELIABLE_CONNECTION_H__
#define __WN_NETWORKING_RELIABLE_CONNECTION_H__

#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"
#include "WNNetworking/inc/WNReceiveBuffer.h"

namespace wn {

namespace networking {

class WNReliableConnection : public WNConnection {
public:
  WNReliableConnection(memory::allocator* _allocator, WNBufferManager* _manager)
    : WNConnection(_allocator, _manager) {}

  virtual ~WNReliableConnection() {}

protected:
};

class WNReliableAcceptConnection : public WNAcceptConnection {
public:
  WNReliableAcceptConnection(
      memory::allocator* _allocator, WNBufferManager* _manager)
    : WNAcceptConnection(_manager), m_allocator(_allocator) {}

  virtual ~WNReliableAcceptConnection() {}

protected:
  memory::allocator* m_allocator;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NETWORKING_RELIABLE_CONNECTION_H__