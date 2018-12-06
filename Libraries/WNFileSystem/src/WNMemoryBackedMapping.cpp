// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNMemoryBackedMapping.h"

namespace wn {
namespace file_system {

memory_backed_mapping::memory_backed_file_store::memory_backed_file_store(
    memory::allocator* _allocator)
  : memory::intrusive_ptr_base(_allocator), m_data(_allocator) {}

memory_backed_mapping::memory_backed_file_store::memory_backed_file_store(
    memory::allocator* _allocator, const memory_backed_file_store& _other)
  : memory::intrusive_ptr_base(_allocator), m_data(_allocator) {
  m_data.insert(m_data.begin(), _other.m_data.begin(), _other.m_data.end());
}

memory_backed_mapping::memory_backed_file_location::memory_backed_file_location(
    memory::allocator* _allocator, memory_backed_file_store_ptr&& _data)
  : memory::intrusive_ptr_base(_allocator), m_data(_data) {}

void memory_backed_mapping::memory_backed_file_location::swap_data(
    memory::intrusive_ptr<memory_backed_file_store>& other) {
  m_lock.lock();
  m_data.swap(other);
  m_lock.unlock();
}

memory_backed_mapping::memory_backed_file::memory_backed_file(
    memory::allocator* _allocator, memory_backed_file_location_ptr _location)
  : file(_allocator),
    m_allocator(_allocator),
    m_is_open(true),
    m_location(_location),
    m_store(_location->m_data) {}

file::pointer memory_backed_mapping::memory_backed_file::data() {
  make_this_unique();
  return m_store->m_data.empty() ? nullptr : m_store->m_data.data();
}

file::const_pointer memory_backed_mapping::memory_backed_file::data() const {
  return m_store->m_data.empty() ? nullptr : m_store->m_data.data();
}

object_info memory_backed_mapping::memory_backed_file::info() const {
  return object_info {
      object_type::file,                              // type
      static_cast<size_type>(m_store->m_data.size())  // size
  };
}

bool memory_backed_mapping::memory_backed_file::is_open() const {
  return m_is_open;
}
bool memory_backed_mapping::memory_backed_file::flush() {
  memory_backed_file_store_ptr temp_store = m_store;
  m_location->swap_data(temp_store);
  return true;
}

bool memory_backed_mapping::memory_backed_file::resize(const size_type _size) {
  make_this_unique();
  m_store->m_data.resize(_size);
  return true;
}

void memory_backed_mapping::memory_backed_file::close() {
  m_is_open = false;
  flush();
}
void memory_backed_mapping::memory_backed_file::make_this_unique() {
  // It is ok if this is not an atomic operation.
  // memory_backed_file is not internally synchronized.
  // This means that if any 2 people modify the SAME
  // memory_backed_file at the same time, undefined results
  // ensue. So m_store->reference_count == 1 IFF
  // this is the only person who has a reference.
  if (m_store->reference_count() == 1) {
    return;
  } else {
    // Someone else has (or had 2 lines ago) a reference,
    // make a copy of the data.
    m_store = memory::make_intrusive<memory_backed_file_store>(
        m_allocator, m_allocator, *m_store);
  }
}

memory_backed_mapping::memory_backed_directory::memory_backed_directory(
    memory::allocator* _allocator)
  : m_allocator(_allocator),
    child_directories(_allocator),
    child_files(_allocator) {}

void memory_backed_mapping::memory_backed_directory::clear() {
  child_directories.clear();
  child_files.clear();
}

memory_backed_mapping::memory_backed_directory*
memory_backed_mapping::memory_backed_directory::get_child_directory(
    containers::string_view& name) {
  auto child = child_directories.find(name.to_string(m_allocator));
  if (child != child_directories.end()) {
    return child->second.get();
  }
  return nullptr;
}

// Returns a pointer to a child directory if it exists,
// otherwise returns nullptr.
template <typename string_view_iterator>
memory_backed_mapping::memory_backed_directory*
memory_backed_mapping::memory_backed_directory::recursive_get_child_directory(
    string_view_iterator begin, string_view_iterator end) {
  if (begin == end)
    return this;
  auto child = child_directories.find(begin->to_string(m_allocator));
  if (child != child_directories.end()) {
    return child->second->recursive_get_child_directory(begin + 1, end);
  }
  return nullptr;
}

memory_backed_mapping::memory_backed_file_ptr
memory_backed_mapping::memory_backed_directory::get_file(
    containers::string_view& name) {
  auto child = child_files.find(name.to_string(m_allocator));
  if (child != child_files.end()) {
    return memory::make_intrusive<memory_backed_file>(
        m_allocator, m_allocator, child->second);
  }
  return nullptr;
}

bool memory_backed_mapping::memory_backed_directory::has_file(
    containers::string_view& name) {
  auto child = child_files.find(name.to_string(m_allocator));
  if (child != child_files.end()) {
    return true;
  }
  return false;
}

void memory_backed_mapping::memory_backed_directory::create_directory(
    containers::string_view& name) {
  if (get_child_directory(name)) {
    return;
  }
  child_directories.insert(core::pair<containers::string,
      memory::unique_ptr<memory_backed_directory>>(name.to_string(m_allocator),
      memory::make_unique<memory_backed_directory>(m_allocator, m_allocator)));
}

memory_backed_mapping::memory_backed_file_ptr
memory_backed_mapping::memory_backed_directory::create_file(
    containers::string_view& _name) {
  containers::string name = _name.to_string(m_allocator);
  auto child = child_files.find(name);
  if (child != child_files.end()) {
    return nullptr;
  }
  auto it = child_files.insert(
      core::pair<containers::string, memory_backed_file_location_ptr>(
          std::move(name), memory::make_intrusive<memory_backed_file_location>(
                               m_allocator, m_allocator,
                               memory::make_intrusive<memory_backed_file_store>(
                                   m_allocator, m_allocator))));

  return memory::make_intrusive<memory_backed_file>(
      m_allocator, m_allocator, it.first->second);
}

void memory_backed_mapping::memory_backed_directory::delete_child_file(
    const containers::string_view& _name) {
  auto it = child_files.find(_name.to_string(m_allocator));
  if (it != child_files.end()) {
    child_files.erase(it);
  }
}

void memory_backed_mapping::memory_backed_directory::delete_child_dir(
    const containers::string_view& _name) {
  auto it = child_directories.find(_name.to_string(m_allocator));
  if (it != child_directories.end()) {
    child_directories.erase(it);
  }
}

bool memory_backed_mapping::exists_file(
    const containers::string_view _path) const {
  containers::string path(m_allocator);
  containers::string_view file_name;
  if (!sanitize_and_validate_path(_path, path)) {
    return false;
  }

  memory_backed_directory* directory = get_directory(true, path);
  if (!directory) {
    return false;
  }

  internal::get_filename_from_sanitized_path(path, file_name);
  return directory->has_file(file_name);
}

bool memory_backed_mapping::exists_directory(
    const containers::string_view _path) const {
  return nullptr != get_directory(false, _path);
}

file_ptr memory_backed_mapping::create_file(
    const containers::string_view _path, result& _result) {
  multi_tasking::mutex_guard guard(m_system_lock);
  containers::string path(m_allocator);
  containers::string_view filename;
  if (!sanitize_and_validate_path(_path, path)) {
    return nullptr;
  }
  memory_backed_directory* dir = get_directory(true, path);
  if (!dir) {
    _result = result::invalid_path;
    return nullptr;
  }

  internal::get_filename_from_sanitized_path(path, filename);

  memory_backed_file_ptr pt = dir->create_file(filename);
  if (!pt) {
    _result = result::already_exists;
    return nullptr;
  }
  _result = result::ok;
  return pt;
}

result memory_backed_mapping::create_directory(
    const containers::string_view _path) {
  multi_tasking::mutex_guard guard(m_system_lock);
  containers::string path(m_allocator);
  containers::string_view file_name;
  if (!sanitize_and_validate_path(_path, path)) {
    return result::invalid_path;
  }
  containers::dynamic_array<containers::string_view> pieces(m_allocator);
  internal::split_sanitized_path(path, pieces);
  if (pieces.begin() == pieces.end()) {
    return result::already_exists;
  }

  memory_backed_directory* root_dir = m_root_dir.recursive_get_child_directory(
      pieces.begin(), pieces.end() - 1);
  root_dir->create_directory(pieces.back());
  return result::ok;
}

file_ptr memory_backed_mapping::open_file(
    const containers::string_view _path, result& _result) {
  multi_tasking::mutex_guard guard(m_system_lock);
  containers::string path(m_allocator);
  containers::string_view filename;
  if (!sanitize_and_validate_path(_path, path)) {
    return nullptr;
  }
  memory_backed_directory* dir = get_directory(true, path);
  if (!dir) {
    _result = result::invalid_path;
    return nullptr;
  }

  internal::get_filename_from_sanitized_path(path, filename);

  memory_backed_file_ptr pt = dir->get_file(filename);
  if (!pt) {
    _result = result::not_found;
    return nullptr;
  }
  _result = result::ok;
  return pt;
}

result memory_backed_mapping::delete_directory(
    const containers::string_view _path) {
  containers::string path(m_allocator);
  if (!sanitize_and_validate_path(_path, path)) {
    return result::invalid_path;
  }
  containers::dynamic_array<containers::string_view> pieces(m_allocator);
  internal::split_sanitized_path(path, pieces);
  if (pieces.begin() == pieces.end()) {
    m_root_dir.clear();
    return result::ok;
  }

  memory_backed_directory* dir = m_root_dir.recursive_get_child_directory(
      pieces.begin(), pieces.end() - 1);
  if (!dir) {
    return result::not_found;
  }
  dir->delete_child_dir(pieces.back());
  return result::ok;
}

result memory_backed_mapping::delete_file(const containers::string_view _path) {
  containers::string path(m_allocator);
  if (!sanitize_and_validate_path(_path, path)) {
    return result::invalid_path;
  }
  containers::dynamic_array<containers::string_view> pieces(m_allocator);
  internal::split_sanitized_path(path, pieces);
  if (pieces.begin() == pieces.end()) {
    m_root_dir.clear();
    return result::ok;
  }

  memory_backed_directory* dir = m_root_dir.recursive_get_child_directory(
      pieces.begin(), pieces.end() - 1);
  if (!dir) {
    return result::not_found;
  }
  dir->delete_child_file(pieces.back());
  return result::ok;
}

memory_backed_mapping::memory_backed_mapping(memory::allocator* _allocator)
  : mapping(_allocator, containers::string(_allocator), false),
    m_root_dir(_allocator) {}

memory_backed_mapping::memory_backed_directory*
memory_backed_mapping::get_directory(
    bool has_file, const containers::string_view _path) const {
  containers::string path(m_allocator);
  if (!sanitize_and_validate_path(_path, path)) {
    return nullptr;
  }
  containers::dynamic_array<containers::string_view> pieces(m_allocator);
  internal::split_sanitized_path(path, pieces);
  if (pieces.begin() == pieces.end()) {
    return const_cast<memory_backed_directory*>(&m_root_dir);
  }
  return const_cast<memory_backed_directory*>(&m_root_dir)
      ->recursive_get_child_directory(
          pieces.begin(), pieces.end() - (has_file ? 1 : 0));
}

}  // namespace file_system
}  // namespace wn
