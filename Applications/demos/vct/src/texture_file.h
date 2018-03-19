// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef WN_APPLICATIONS_DEMOS_VCT_TEXTURE_HEADER_H__
#define WN_APPLICATIONS_DEMOS_VCT_TEXTURE_HEADER_H__
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/endian.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMemory/inc/allocator.h"

const uint32_t k_astc_format = 1;
const uint32_t k_bc1_format = 2;
const uint32_t k_bc1n_format = 3;

struct mip_data {
  uint32_t mip_offset;
  uint32_t mip_size;
};

const uint32_t k_header_fixed_size = 24;
static_assert(sizeof(mip_data) == 8, "The packing has messed up our mip data");

struct texture_header {
  uint32_t image_format;
  uint32_t image_width;
  uint32_t image_height;
  uint32_t block_size_x;
  uint32_t block_size_y;
  uint32_t num_mips;
  mip_data mips[1];
};

using namespace wn;
class texture_file {
public:
  texture_file(memory::allocator* _allocator, const file_system::file_ptr _file)
    : m_mip_levels(_allocator), m_file(_file) {
    WN_DEBUG_ASSERT(
        _file->range().size() > sizeof(texture_header), "Invalid texture file");
    WN_DEBUG_ASSERT((reinterpret_cast<uintptr_t>(_file->data()) & 0xF) == 0,
        "Invalid base file offset");
    const texture_header* header =
        reinterpret_cast<const texture_header*>(_file->data());
    runtime::graphics::data_format format = runtime::graphics::data_format::max;

    switch (core::from_little_endian(header->image_format)) {
      case k_astc_format: {
        uint32_t xblock = core::from_little_endian(header->block_size_x);
        uint32_t yblock = core::from_little_endian(header->block_size_y);
        m_block_height = yblock;
        m_block_width = xblock;
        if (xblock == 4 && yblock == 4) {
          format = runtime::graphics::data_format::astc_4x4;
        } else if (xblock == 5 && yblock == 4) {
          format = runtime::graphics::data_format::astc_5x4;
        } else if (xblock == 5 && yblock == 5) {
          format = runtime::graphics::data_format::astc_5x5;
        } else if (xblock == 6 && yblock == 5) {
          format = runtime::graphics::data_format::astc_6x5;
        } else if (xblock == 6 && yblock == 6) {
          format = runtime::graphics::data_format::astc_6x6;
        } else if (xblock == 8 && yblock == 5) {
          format = runtime::graphics::data_format::astc_8x5;
        } else if (xblock == 8 && yblock == 6) {
          format = runtime::graphics::data_format::astc_8x6;
        } else if (xblock == 8 && yblock == 8) {
          format = runtime::graphics::data_format::astc_8x8;
        } else if (xblock == 10 && yblock == 5) {
          format = runtime::graphics::data_format::astc_10x5;
        } else if (xblock == 10 && yblock == 6) {
          format = runtime::graphics::data_format::astc_10x6;
        } else if (xblock == 10 && yblock == 8) {
          format = runtime::graphics::data_format::astc_10x8;
        } else if (xblock == 10 && yblock == 10) {
          format = runtime::graphics::data_format::astc_10x10;
        } else if (xblock == 12 && yblock == 10) {
          format = runtime::graphics::data_format::astc_12x10;
        } else if (xblock == 12 && yblock == 12) {
          format = runtime::graphics::data_format::astc_12x12;
        }
      } break;
      case k_bc1_format:
        format = runtime::graphics::data_format::bc1_rgb;
        m_block_height = 4;
        m_block_width = 4;
        break;
      case k_bc1n_format:
        format = runtime::graphics::data_format::bc1_rgb;
        m_block_height = 4;
        m_block_width = 4;
        break;
      default:
        m_block_height = 1;
        m_block_width = 1;
        break;
    }
    WN_DEBUG_ASSERT(
        format != runtime::graphics::data_format::max, "Unknown image format");
    uint32_t num_mips = core::from_little_endian(header->num_mips);
    m_mip_levels.reserve(num_mips);
    WN_DEBUG_ASSERT(
        _file->size() >= k_header_fixed_size + sizeof(mip_data) * num_mips,
        "Input file has an invalid size");
    m_texture_format = format;
    m_width = core::from_little_endian(header->image_width);
    m_height = core::from_little_endian(header->image_height);

    for (size_t i = 0; i < num_mips; ++i) {
      uint32_t data_offset =
          core::from_little_endian(header->mips[i].mip_offset);
      uint32_t data_size = core::from_little_endian(header->mips[i].mip_size);
      WN_DEBUG_ASSERT(data_offset < _file->size(), "Offset out of bounds");
      WN_DEBUG_ASSERT(
          data_offset + data_size <= _file->size(), "Data runs out of bounds");
      const uint8_t* base = _file->typed_data<const uint8_t>() + data_offset;

      m_mip_levels.push_back(
          containers::contiguous_range<const uint8_t>(base, base + data_size));
    }
  }

  uint32_t width() const {
    return m_width;
  }
  uint32_t height() const {
    return m_height;
  }
  uint32_t block_width() const {
    return m_block_width;
  }
  uint32_t block_height() const {
    return m_block_height;
  }
  uint32_t num_mips() const {
    return static_cast<uint32_t>(m_mip_levels.size());
  }
  runtime::graphics::data_format format() const {
    return m_texture_format;
  }
  const containers::dynamic_array<containers::contiguous_range<const uint8_t>>&
  data() const {
    return m_mip_levels;
  }

private:
  containers::dynamic_array<containers::contiguous_range<const uint8_t>>
      m_mip_levels;
  uint32_t m_width = 0;
  uint32_t m_height = 0;
  uint32_t m_block_height = 0;
  uint32_t m_block_width = 0;

  runtime::graphics::data_format m_texture_format =
      runtime::graphics::data_format::r8g8b8a8_unorm;

  file_system::file_ptr m_file;
};
#endif  // WN_APPLICATIONS_DEMOS_VCT_TEXTURE_HEADER_H__
