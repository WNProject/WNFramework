// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_FACTORY_H__
#define __WN_GRAPHICS_FACTORY_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/WNAdapter.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMultiTasking/inc/call_once.h"

namespace wn {
namespace runtime {
namespace graphics {

// This factory must outlive any objects created by any of the returned phsycial
// devices
class factory {
public:
  inline factory(memory::allocator* _allocator, logging::log* _log)
    : m_adapters(_allocator), m_allocator(_allocator), m_log(_log) {}

  virtual ~factory();

  virtual containers::read_only_contiguous_range<adapter_ptr> query_adapters()
      const;

private:
  void query_devices() const;

  mutable containers::dynamic_array<adapter_ptr> m_adapters;
  mutable multi_tasking::once_flag m_query_adapter_once_flag;
  memory::allocator* m_allocator;
  logging::log* m_log;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // _WN_GRAPHICS_FACTORY_H__
