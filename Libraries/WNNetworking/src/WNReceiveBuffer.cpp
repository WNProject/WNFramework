// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNReceiveBuffer.h"
#include "WNContainers/inc/WNContiguousRange.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNReliableConnection.h"

namespace wn {
namespace networking {

WNReceiveBuffer::~WNReceiveBuffer() {
  if (m_owner) {
    m_owner->release_buffer(m_token);
  }
}

WNReceiveBuffer WNBufferManager::construct_buffer(
    const void* _token, const containers::contiguous_range<char>& _data) {
  return WNReceiveBuffer(_token, this, _data);
}

}  // namespace networking
}  // namespace wn