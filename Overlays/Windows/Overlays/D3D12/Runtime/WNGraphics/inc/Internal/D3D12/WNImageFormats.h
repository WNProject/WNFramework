// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__

#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

static WN_FORCE_INLINE DXGI_FORMAT image_format_to_dxgi_format(
    data_format _format) {
  static const DXGI_FORMAT formats[] = {
      DXGI_FORMAT_R8G8B8A8_UNORM,
      DXGI_FORMAT_R32G32B32A32_FLOAT,
      DXGI_FORMAT_R32G32B32_FLOAT,
      DXGI_FORMAT_R32G32_FLOAT,
      DXGI_FORMAT_D16_UNORM,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_D32_FLOAT,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_D24_UNORM_S8_UINT,
      DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_UNKNOWN,
      DXGI_FORMAT_BC1_UNORM,
      DXGI_FORMAT_BC1_UNORM,
      DXGI_FORMAT_BC2_UNORM,
      DXGI_FORMAT_BC3_UNORM,
  };
  static_assert(sizeof(formats) / sizeof(formats[0]) ==
                    static_cast<uint32_t>(data_format::max),
      "Expected the number of dxgi formats and image formats to match");
  WN_DEBUG_ASSERT(_format < data_format::max, "Image format out of bounds");

  return formats[static_cast<uint32_t>(_format)];
}

static WN_FORCE_INLINE image_components valid_components(data_format _format) {
  image_components components;
  switch (_format) {
    case data_format::d24_unorm_s8_uint:
    case data_format::d32_float_s8_uint:
      components |= static_cast<image_components>(image_component::stencil);
    case data_format::d16_unorm:
    case data_format::d32_float:
      components |= static_cast<image_components>(image_component::depth);
      break;
    default:
      components = static_cast<image_components>(image_component::color);
  }
  return components;
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__
