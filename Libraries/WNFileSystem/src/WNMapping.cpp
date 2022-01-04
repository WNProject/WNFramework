// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNMapping.h"

#include "WNFileSystem/src/WNUtilities.h"

namespace wn {
namespace file_system {

bool mapping::sanitize_and_validate_path(
    const containers::string_view _path, containers::string& _full_path) const {
  containers::string path(m_allocator, _path);

  internal::sanitize_path(path);

  if (internal::validate_relative_path(path)) {
    if (!m_root_path.empty()) {
      _full_path.append(m_root_path);
    }

    internal::append_path(_full_path, path);

    return true;
  }

  return false;
}

result mapping::create_directory_recursive(containers::string_view _directory) {
  containers::string sanitized_path(m_allocator, _directory);

  internal::sanitize_path(sanitized_path);

  if (exists_directory(sanitized_path)) {
    return result::ok;
  }

  containers::dynamic_array<containers::string_view> subpaths(m_allocator);

  if (!internal::split_sanitized_path(sanitized_path, subpaths)) {
    return result::invalid_path;
  }

  containers::string built_up_path(m_allocator);

  for (auto& subpath : subpaths) {
    internal::append_path(built_up_path, subpath);

    if (!exists_directory(built_up_path)) {
      result res;

      if ((res = create_directory(built_up_path)) != result::ok) {
        return res;
      }
    }
  }

  return result::ok;
}

template <typename T>
result mapping::initialize_files_internal(const T& _files) {
  for (auto& file : _files) {
    containers::string sanitized_path(m_allocator, file.first);

    internal::sanitize_path(sanitized_path);

    containers::string_view directory;

    if (!internal::get_directory_from_sanitized_path(
            sanitized_path, directory)) {
      return result::invalid_path;
    }

    result res;

    if ((res = create_directory_recursive(directory)) != result::ok) {
      return res;
    }

    file_ptr file_pt = create_file(sanitized_path, res);

    if (!file_pt) {
      return res;
    }

    if (!file_pt->resize(file.second.size())) {
      return result::file_sizing_fail;
    }

    memory::memory_copy(
        file_pt->typed_data<char>(), file.second.data(), file.second.size());

    if (!file_pt->flush()) {
      return result::file_mapping_fail;
    }
  }

  return result::ok;
}

result mapping::initialize_files(std::initializer_list<
    const core::pair<containers::string_view, containers::string_view>>
        _files) {
  return initialize_files_internal(_files);
}

result mapping::initialize_files(containers::contiguous_range<
    const core::pair<containers::string_view, containers::string_view>>
        _files) {
  return initialize_files_internal(_files);
}

}  // namespace file_system
}  // namespace wn
