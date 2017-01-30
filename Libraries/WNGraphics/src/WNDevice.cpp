// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace graphics {

upload_heap device::create_upload_heap(const size_t _num_bytes) {
  upload_heap new_upload_heap(this);

  initialize_upload_heap(&new_upload_heap, _num_bytes);

  return core::move(new_upload_heap);
}

download_heap device::create_download_heap(const size_t _num_bytes) {
  download_heap new_download_heap(this);

  initialize_download_heap(&new_download_heap, _num_bytes);

  return core::move(new_download_heap);
}

command_allocator device::create_command_allocator() {
  command_allocator new_command_allocator(this);

  initialize_command_allocator(&new_command_allocator);

  return core::move(new_command_allocator);
}

fence device::create_fence() {
  fence new_fence(this);

  initialize_fence(&new_fence);

  return core::move(new_fence);
}

image device::create_image(const image_create_info& _info) {
  image new_image(this);
  initialize_image(_info, &new_image);
  return core::move(new_image);
}

}  // namespace graphics
}  // namespace wn