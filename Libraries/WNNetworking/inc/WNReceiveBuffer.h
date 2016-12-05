// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NEWORKING_NETWORK_RECEIVE_BUFFER_H__
#define __WN_NEWORKING_NETWORK_RECEIVE_BUFFER_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"

namespace wn {
namespace networking {

class WNNetworkManager;
class WNReceiveBuffer;
class WNBufferManager {
public:
  virtual ~WNBufferManager() {}
  virtual void release_buffer(const void* _token) = 0;
  virtual WNReceiveBuffer acquire_buffer() = 0;

protected:
  WNReceiveBuffer construct_buffer(
      const void* _token, const containers::contiguous_range<char>& _data);
};

class WNReceiveBuffer {
public:
  WNReceiveBuffer() = delete;
  WNReceiveBuffer(const WNReceiveBuffer&) = delete;
  WNReceiveBuffer& operator=(const WNReceiveBuffer&) = delete;

  WNReceiveBuffer(WNReceiveBuffer&& _other)
    : m_owner(_other.m_owner),
      m_token(_other.m_token),
      data(_other.data),
      m_error(_other.m_error) {
    _other.m_owner = nullptr;
  }

  WNReceiveBuffer& operator=(WNReceiveBuffer&& _other) {
    release();
    m_owner = _other.m_owner;
    _other.m_owner = nullptr;
    m_token = _other.m_token;
    _other.m_token = nullptr;
    m_error = _other.m_error;
    _other.m_error = network_error::error;
    data = core::move(_other.data);
    return *this;
  }

  containers::contiguous_range<char> data;

  ~WNReceiveBuffer();

  explicit WNReceiveBuffer(network_error _error)
    : m_error(_error), m_owner(nullptr) {
    WN_RELEASE_ASSERT_DESC(_error != network_error::ok,
        "Cannot create a recieve buffer with an ok error without data");
  }
  network_error get_status() {
    return m_error;
  }

  // TODO: Once we fix up core::function to have a move-only version,
  // then we can remove these. These are to pass a ReceiveBuffer through
  // a job.
  void decompose(const void** _token, WNBufferManager** _manager,
      containers::contiguous_range<char>* _data, network_error* _err) {
    *_token = m_token;
    *_manager = m_owner;
    *_data = data;
    *_err = m_error;
    m_owner = nullptr;
  }

  WNReceiveBuffer(const void* _token, WNBufferManager* _manager,
      const containers::contiguous_range<char>& _data,
      network_error _err = network_error::ok)
    : m_token(_token), m_owner(_manager), data(_data), m_error(_err) {}

private:
  void release();
  const void* m_token;
  WNBufferManager* m_owner;
  network_error m_error;
  friend class WNBufferManager;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NEWORKING_NETWORK_RECEIVE_BUFFER_H__