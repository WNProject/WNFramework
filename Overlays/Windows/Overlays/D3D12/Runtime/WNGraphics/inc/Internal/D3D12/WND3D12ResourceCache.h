// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_CACHE_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_CACHE_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNFunctional/inc/WNUniqueFunction.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNMultiTasking/inc/lock_guard.h"
#include "WNMultiTasking/inc/spin_lock.h"

#include "WNGraphics/inc/Internal/D3D12/WNLockedHeap.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

struct blit_image_data {
  Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline_state;
  Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature;
};

struct d3d12_resource_cache {
  memory::allocator* m_allocator;
  multi_tasking::spin_lock m_blit_lock;
  containers::hash_map<DXGI_FORMAT, memory::unique_ptr<blit_image_data>>
      m_blit_pipelines;
  functional::unique_function<void(DXGI_FORMAT, blit_image_data*)> m_blit_func;
  locked_heap* m_csv_heap;
  locked_heap* m_rtv_heap;
  void initialize(memory::allocator* _allocator,
      functional::unique_function<void(DXGI_FORMAT, blit_image_data*)>
          _blit_func,
      locked_heap* _csv_heap, locked_heap* _rtv_heap) {
    m_allocator = _allocator;
    m_blit_func = core::move(_blit_func);
    m_blit_pipelines.set_allocator(_allocator);
    m_csv_heap = _csv_heap;
    m_rtv_heap = _rtv_heap;
  }

  blit_image_data* get_blit_pipeline(DXGI_FORMAT _format) {
    {
      multi_tasking::spin_lock_guard lg(m_blit_lock);
      auto it = m_blit_pipelines.find(_format);
      if (it != m_blit_pipelines.end()) {
        return it->second.get();
      }
    }
    blit_image_data img_data;
    m_blit_func(_format, &img_data);
    multi_tasking::spin_lock_guard lg(m_blit_lock);
    auto it = m_blit_pipelines.find(_format);
    if (it != m_blit_pipelines.end()) {
      return it->second.get();
    }

    auto retit = m_blit_pipelines.insert(
        std::make_pair(_format, memory::make_unique<blit_image_data>(
                                    m_allocator, core::move(img_data))));
    return retit.first->second.get();
  }

  locked_heap::token get_temporary_csv(size_t num) {
    return m_csv_heap->get_partition(num);
  }

  locked_heap::token get_temporary_rtv(size_t num) {
    return m_rtv_heap->get_partition(num);
  }
};
}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_CACHE_H__
