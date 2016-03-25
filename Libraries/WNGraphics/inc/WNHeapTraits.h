// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_HEAP_TRAITS_H__
#define __WN_GRAPHICS_HEAP_TRAITS_H__

#include "WNContainers/inc/WNContiguousRange.h"

namespace wn {
namespace graphics {

template <typename HeapTraits>
class heap;

struct upload_heap_traits {
  template <typename T>
  using range_type = containers::write_only_contiguous_range<T>;
};

template <typename T, typename HeapTraits>
class heap_buffer;

using upload_heap = heap<upload_heap_traits>;

template <typename T>
using upload_heap_buffer = heap_buffer<T, upload_heap_traits>;

struct download_heap_traits {
  template <typename T>
  using range_type = containers::read_only_contiguous_range<T>;
};

using download_heap = heap<download_heap_traits>;

template <typename T>
using download_heap_buffer = heap_buffer<T, download_heap_traits>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_HEAP_TRAITS_H__