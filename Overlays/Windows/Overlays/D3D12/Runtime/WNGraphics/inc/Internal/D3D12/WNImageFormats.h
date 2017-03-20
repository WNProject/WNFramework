// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__

#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

static WN_FORCE_INLINE DXGI_FORMAT image_format_to_dxgi_format(
    data_format _format) {
  static const DXGI_FORMAT formats[] = {
      DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT};
  static_assert(sizeof(formats) / sizeof(formats[0]) ==
                    static_cast<uint32_t>(data_format::max),
      "Expected the number of dxgi formats and image formats to match");
  WN_DEBUG_ASSERT_DESC(
      _format < data_format::max, "Image format out of bounds");

  return formats[static_cast<uint32_t>(_format)];
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__