// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef WN_APPLICATIONS_DEMOS_VCT_TEXTURE_MANAGER_H__
#define WN_APPLICATIONS_DEMOS_VCT_TEXTURE_MANAGER_H__
#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNMemory/inc/allocator.h"
#include "texture_file.h"

using namespace wn;

class texture_manager {
public:
  texture_manager(memory::allocator* _allocator, file_system::mapping* _mapping)
    : m_files(_allocator),
      m_file_names(_allocator),
      m_mapping(_mapping),
      m_allocator(_allocator) {}

  texture_file* load_file(containers::string_view _view) {
    auto it = m_files.find(_view);

    if (it != m_files.end()) {
      return it->second.get();
    }

    file_system::result res;
    file_system::file_ptr ptr = m_mapping->open_file(_view, res);
    WN_RELEASE_ASSERT(
        res == file_system::result::ok, "Cannot open texture file");

    memory::unique_ptr<texture_file> f =
        memory::make_unique<texture_file>(m_allocator, m_allocator, ptr);

    texture_file* file = f.get();
    m_file_names.push_back(_view.to_string(m_allocator));
    m_files[m_file_names.back()] = core::move(f);
    return file;
  }

  const containers::hash_map<containers::string_view,
      memory::unique_ptr<texture_file>>&
  files() const {
    return m_files;
  }

private:
  containers::hash_map<containers::string_view,
      memory::unique_ptr<texture_file>>
      m_files;
  containers::deque<containers::string> m_file_names;
  file_system::mapping* m_mapping;
  memory::allocator* m_allocator;
};

#endif  // WN_APPLICATIONS_DEMOS_VCT_TEXTURE_MANAGER_H__
