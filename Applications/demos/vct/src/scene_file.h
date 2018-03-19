// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_APPLICATIONS_DEMOS_VCT_SCENE_H__
#define __WN_APPLICATIONS_DEMOS_VCT_SCENE_H__
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/endian.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMemory/inc/allocator.h"
#include "object.h"
#include "texture_manager.h"

using namespace wn;
// Scene file layout

class scene_file {
public:
  scene_file(memory::allocator* _allocator, const file_system::file_ptr _file,
      texture_manager* _manager, logging::log* _log)
    : m_file(_file), m_entities(_allocator) {
    const uint8_t* file_data = _file->data();
    WN_RELEASE_ASSERT(_file->size() > 4, "Invalid scene file");
    WN_RELEASE_ASSERT(reinterpret_cast<uintptr_t>(_file->data()) % 4 == 0,
        "Expected: the data is aligned");

    const uint32_t num_objects =
        core::from_little_endian(*reinterpret_cast<const uint32_t*>(file_data));

    uint32_t offset = 4;
    for (size_t i = 0; i < num_objects; ++i) {
      WN_RELEASE_ASSERT(
          _file->size() > offset + sizeof(object_header), "Invalid scene file");
      const object_header* header =
          reinterpret_cast<const object_header*>(file_data + offset);
      uint32_t string_offset = core::from_little_endian(header->name_offset);
      uint32_t string_size = core::from_little_endian(header->name_length);

      containers::string_view v(
          reinterpret_cast<const char*>(file_data) + string_offset,
          string_size);

      _log->log_debug("Loading: ", v);

      m_entities[v] = memory::make_unique<entity_in_file>(
          _allocator, _file, offset, _manager, _log);

      offset += sizeof(object_header);
    }
  }

  const containers::hash_map<containers::string_view,
      memory::unique_ptr<entity_in_file>>&
  entities() const {
    return m_entities;
  }

private:
  file_system::file_ptr m_file;
  containers::hash_map<containers::string_view,
      memory::unique_ptr<entity_in_file>>
      m_entities;
};

#endif  // __WN_APPLICATIONS_DEMOS_VCT_SCENE_H__
