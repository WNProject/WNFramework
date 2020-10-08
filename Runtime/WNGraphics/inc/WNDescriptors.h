// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_DESCRIPTOR_SET_H__
#define __WN_GRAPHICS_DESCRIPTOR_SET_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNGraphicsObjectBase.h"
#include "WNMemory/inc/manipulation.h"

WN_GRAPHICS_FORWARD(command_list);

namespace wn {
namespace runtime {
namespace graphics {

// This just describes a descriptor set
class descriptor_set_layout final : public base_object<2> {
public:
  WN_FORCE_INLINE descriptor_set_layout(descriptor_set_layout&& _other)
    : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }
  ~descriptor_set_layout() {
    if (m_device) {
      m_device->destroy_descriptor_set_layout(this);
    }
  }
  descriptor_set_layout() : m_device(nullptr) {}

  descriptor_set_layout& operator=(descriptor_set_layout&& _other) {
    if (m_device) {
      m_device->destroy_descriptor_set_layout(this);
    }
    m_device = _other.m_device;
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
    return *this;
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);
  WN_GRAPHICS_ADD_FRIENDS(command_list);
  WN_FORCE_INLINE descriptor_set_layout(device* _device) : m_device(_device) {}
  device* m_device;
};

class descriptor_set final : public base_object<2> {
public:
  WN_FORCE_INLINE descriptor_set(descriptor_set&& _other)
    : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  ~descriptor_set() {
    if (m_device) {
      m_device->destroy_descriptor_set(this);
    }
  }

  descriptor_set() : m_device(nullptr) {}
  descriptor_set& operator=(descriptor_set&& _other) {
    if (m_device) {
      m_device->destroy_descriptor_set(this);
    }
    m_device = _other.m_device;
    _other.m_device = nullptr;
    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
    return *this;
  }

  void update_descriptors(
      const containers::contiguous_range<buffer_descriptor>& _buffer_updates,
      const containers::contiguous_range<image_descriptor>& _image_updates,
      const containers::contiguous_range<sampler_descriptor>&
          _sampler_updates) {
    m_device->update_descriptors(
        this, _buffer_updates, _image_updates, _sampler_updates);
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(command_list);
  WN_FORCE_INLINE descriptor_set(device* _device) : m_device(_device) {}
  device* m_device;
  friend class descriptor_pool;
};

// We can allocate descriptors out of here. NOTE: Only one descriptor pool
// of each "type" can exist at the same time.

// In fact, for d3d12 its a bit more subtle:
//    1 descriptor pool of RO_buffer/RW_buffer/texture all come from the same
//    pool, these are all the same "type"

class descriptor_pool final : public base_object<2> {
public:
  descriptor_set create_descriptor_set(descriptor_set_layout* _layout) {
    descriptor_set set(m_device);
    m_device->initialize_descriptor_set(&set, this, _layout);
    return core::move(set);
  }
  WN_FORCE_INLINE descriptor_pool(descriptor_pool&& _other)
    : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  descriptor_pool() : m_device(nullptr) {}
  descriptor_pool& operator=(descriptor_pool&& _other) {
    if (m_device) {
      m_device->destroy_descriptor_pool(this);
    }
    m_device = _other.m_device;
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
    return *this;
  }

  ~descriptor_pool() {
    if (m_device) {
      m_device->destroy_descriptor_pool(this);
    }
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);
  WN_FORCE_INLINE descriptor_pool(device* _device) : m_device(_device) {}

  device* m_device;
};

class pipeline_layout final : public base_object<2> {
public:
  WN_FORCE_INLINE pipeline_layout(pipeline_layout&& _other)
    : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  ~pipeline_layout() {
    if (m_device) {
      m_device->destroy_pipeline_layout(this);
    }
  }

  pipeline_layout() : m_device(nullptr) {}

  pipeline_layout& operator=(pipeline_layout&& _other) {
    if (m_device) {
      m_device->destroy_pipeline_layout(this);
    }
    m_device = _other.m_device;
    _other.m_device = nullptr;
    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
    return *this;
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);
  WN_GRAPHICS_ADD_FRIENDS(command_list);
  WN_FORCE_INLINE pipeline_layout(device* _device) : m_device(_device) {}
  device* m_device;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_DESCRIPTOR_SET_H__
