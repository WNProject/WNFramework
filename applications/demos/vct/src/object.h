// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_APPLICATIONS_DEMOS_OBJECT_H__
#define __WN_APPLICATIONS_DEMOS_OBJECT_H__
#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "core/inc/endian.h"
#include "texture_file.h"
#include "texture_manager.h"

using namespace wn;

struct packed_vertex {
  float x;
  float y;
  float z;
  float nx;
  float ny;
  float nz;
  float u;
  float v;
  float tx;
  float ty;
  float tz;
  float tw;
};

struct object_header {
  uint32_t name_offset;
  uint32_t name_length;
  uint32_t vertex_offset;
  uint32_t vertex_count;
  uint32_t index_offset;
  uint32_t index_count;
  uint32_t diffuse_tex_offset;
  uint32_t diffuse_tex_length;
  uint32_t spec_tex_offset;
  uint32_t spec_tex_length;
  uint32_t mask_tex_offset;
  uint32_t mask_tex_length;
  uint32_t normal_tex_offset;
  uint32_t normal_tex_length;
  float pos_x;
  float pos_y;
  float pos_z;
  uint32_t dummy;
};

static_assert(
    sizeof(object_header) == 18 * 4, "Object header unrecognized size");

static_assert(
    sizeof(packed_vertex) == 12 * 4, "The packed vector is not the right size");

class entity_in_file {
public:
  entity_in_file(file_system::file_ptr _file, uint32_t _file_header_offset,
      texture_manager* _manager, logging::log* _log)
    : m_diffuse(nullptr),
      m_spec(nullptr),
      m_mask(nullptr),
      m_normal(nullptr),
      m_file(_file) {
    const uint8_t* data = _file->data();
    const object_header* header =
        reinterpret_cast<const object_header*>(data + _file_header_offset);

    auto handle_texture = [this, &_file, _log, _manager](
                              uint32_t offset, uint32_t size) -> texture_file* {
      offset = core::from_little_endian(offset);
      size = core::from_little_endian(size);
      if (size == 0) {
        return nullptr;
      }
      WN_RELEASE_ASSERT(_file->size() > offset + size, "Invalid scene file");
      containers::string_view name(_file->typed_data<char>() + offset, size);
      _log->log_debug("Found texture in file: ", name);

      return _manager->load_file(name);
    };

    uint32_t name_offset = core::from_little_endian(header->name_offset);
    uint32_t name_length = core::from_little_endian(header->name_length);

    m_x = core::from_little_endian(header->pos_x);
    m_y = core::from_little_endian(header->pos_y);
    m_z = core::from_little_endian(header->pos_z);

    WN_RELEASE_ASSERT(
        _file->size() >= name_offset + name_length, "Invalid scene file.");
    m_name = containers::string_view(
        reinterpret_cast<const char*>(data) + name_offset, name_length);

    m_diffuse =
        handle_texture(header->diffuse_tex_offset, header->diffuse_tex_length);
    m_spec = handle_texture(header->spec_tex_offset, header->spec_tex_length);
    m_mask = handle_texture(header->mask_tex_offset, header->mask_tex_length);
    m_normal =
        handle_texture(header->normal_tex_offset, header->normal_tex_length);

    if (header->vertex_count == 0 || header->index_count == 0) {
      _log->log_warning("Empty object");
      return;
    }
    uint32_t n_verts = core::from_little_endian(header->vertex_count);
    uint32_t vert_offset = core::from_little_endian(header->vertex_offset);
    WN_RELEASE_ASSERT(
        _file->size() > vert_offset + sizeof(packed_vertex) * n_verts,
        "Invalid scene file.");

    uint32_t n_indices = core::from_little_endian(header->index_count);
    uint32_t index_offset = core::from_little_endian(header->index_offset);
    WN_RELEASE_ASSERT(
        _file->size() >= index_offset + sizeof(uint32_t) * n_indices,
        "Invalid scene file.");

    vertices = containers::contiguous_range<const packed_vertex>(
        reinterpret_cast<const packed_vertex*>(data + vert_offset), n_verts);

    indices = containers::contiguous_range<const uint32_t>(
        reinterpret_cast<const uint32_t*>(data + index_offset), n_indices);
  }

  const containers::contiguous_range<const packed_vertex>& get_vertices()
      const {
    return vertices;
  }
  const containers::contiguous_range<const uint32_t>& get_indices() const {
    return indices;
  }

  const containers::string_view& get_name() const {
    return m_name;
  }

  const texture_file* diffuse() const {
    return m_diffuse;
  }

  const texture_file* spec() const {
    return m_spec;
  }

  const texture_file* mask() const {
    return m_mask;
  }

  const texture_file* normal() const {
    return m_normal;
  }

  float x() const {
    return m_x;
  }
  float y() const {
    return m_y;
  }
  float z() const {
    return m_z;
  }

private:
  texture_file* m_diffuse;
  texture_file* m_spec;
  texture_file* m_mask;
  texture_file* m_normal;

  float m_x;
  float m_y;
  float m_z;
  containers::contiguous_range<const packed_vertex> vertices;
  containers::contiguous_range<const uint32_t> indices;
  containers::string_view m_name;

  file_system::file_ptr m_file;
};

#endif  // __WN_APPLICATIONS_DEMOS_OBJECT_H__
