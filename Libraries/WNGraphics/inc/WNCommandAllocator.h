// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_COMMAND_ALLOCATOR_H__
#define __WN_GRAPHICS_COMMAND_ALLOCATOR_H__

#include "WNCore/inc/WNBase.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNCommandListForward.h"

namespace wn {
namespace graphics {

class command_allocator : public core::non_copyable {
public:
  WN_FORCE_INLINE command_allocator(device* _device)
    : m_device(_device), m_data({0}) {}

  // TODO(awoloszyn): Allow secondary command buffers.
  command_list_ptr create_command_list() {
    return m_device->create_command_list(this);
  }

  WN_FORCE_INLINE command_allocator(command_allocator&& _other) {
    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    m_device = _other.m_device;
    memory::memzero(&_other.m_data, sizeof(opaque_data));
    _other.m_device = nullptr;
  }

  ~command_allocator() {
    if (m_device) {
      m_device->destroy_command_allocator(this);
    }
  }

  bool WN_FORCE_INLINE is_valid() const {
    return m_device != nullptr;
  }
protected:

  template <typename T>
  T& data_as() {
    static_assert(sizeof(opaque_data) >= sizeof(T), "Invalid cast");
    return *reinterpret_cast<T*>(&m_data);
  }

  template <typename T>
  const T& data_as() const {
    static_assert(sizeof(opaque_data) >= sizeof(T), "Invalid cast");
    return *reinterpret_cast<const T*>(&m_data);
  }

#include "WNGraphics/inc/Internal/WNSetFriendDevices.h"
#include "WNGraphics/inc/Internal/WNSetFriendQueues.h"

  // The opaque_data must be trivially copyable.
  // It also must be considered uninitialized when
  // memset to 0.
  struct opaque_data {
    uint64_t _dummy[1];
  } m_data;

  device* m_device;

};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_COMMAND_ALLOCATOR_LIST_H__
