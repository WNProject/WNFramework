// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#pragma once

#include "WNContainers/inc/WNHashMap.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNFileSystem/src/WNUtilities.h"
#include "WNFileSystem/inc/object_info.h"
#include "WNMultiTasking/inc/mutex.h"
#include "WNMultiTasking/inc/spin_lock.h"

#ifndef __WN_MEMORY_BACKED_MAPPING_H__
#define __WN_MEMORY_BACKED_MAPPING_H__

namespace wn {
namespace file_system {

class memory_backed_mapping final : public mapping {
private:
  // This is where the actual data for a file is stored.
  class memory_backed_file_store : public memory::intrusive_ptr_base {
  public:
    memory_backed_file_store(memory::allocator* _allocator);

    memory_backed_file_store(
        memory::allocator* _allocator, const memory_backed_file_store& _other);

    memory_backed_file_store(const memory_backed_file_store& _other) = delete;

    containers::dynamic_array<file::value_type> m_data;
  };

  using memory_backed_file_store_ptr =
      memory::intrusive_ptr<memory_backed_file_store>;

  // This is the location that points to the data for the file.
  // It is not the data itself, because we need a place that will not
  // dissapear to be the authoritative view of the contents of the
  // "file". The file_store is expected to  be CoW, so something needs
  // to swap the correct "contents" in when necessary.
  class memory_backed_file_location : public memory::intrusive_ptr_base {
  public:
    memory_backed_file_location(
        memory::allocator* _allocator, memory_backed_file_store_ptr&& _data);
    void swap_data(memory::intrusive_ptr<memory_backed_file_store>& other);

    memory_backed_file_store_ptr m_data;
    multi_tasking::spin_lock m_lock;
  };

  using memory_backed_file_location_ptr =
      memory::intrusive_ptr<memory_backed_file_location>;

  // Implements the file interface. Has a CoW store, and a pointer
  // to the original location. When the file is written,
  // the copy is written, and when flushed, the original is
  // overwritten.
  class memory_backed_file : public file {
  public:
    memory_backed_file(memory::allocator* _allocator,
        memory_backed_file_location_ptr _location);

    virtual ~memory_backed_file() {
      close();
    }

    virtual pointer data();
    virtual const_pointer data() const;
    virtual object_info info() const;
    virtual bool is_open() const;

    // If 2 files try to flush back to the
    // filesystem at the same time, that is fine,
    // swap_data is internally synchronized, so at LEAST
    // one of the pieces of data will end up in the file.
    virtual bool flush();
    virtual bool resize(const size_type _size);
    virtual void close();

  private:
    void make_this_unique();

    memory::allocator* m_allocator;
    bool m_is_open;
    memory_backed_file_location_ptr m_location;
    memory_backed_file_store_ptr m_store;
  };

  using memory_backed_file_ptr = memory::intrusive_ptr<memory_backed_file>;
  // Directory structure, can hold other directories or files.
  class memory_backed_directory {
  public:
    memory_backed_directory(memory::allocator* _allocator);

    void clear();

    // Returns a pointer to a child directory if it exists,
    // otherwise returns nullptr.
    memory_backed_directory* get_child_directory(containers::string_view& name);

    // Returns a pointer to a child directory if it exists,
    // otherwise returns nullptr.
    template <typename string_view_iterator>
    memory_backed_directory* recursive_get_child_directory(
        string_view_iterator begin, string_view_iterator end);

    memory_backed_file_ptr get_file(containers::string_view& name);
    bool has_file(containers::string_view& name);
    void create_directory(containers::string_view& name);
    memory_backed_file_ptr create_file(containers::string_view& _name);
    void delete_child_file(const containers::string_view& _name);
    void delete_child_dir(const containers::string_view& _name);

  private:
    memory::allocator* m_allocator;
    containers::hash_map<containers::string,
        memory::unique_ptr<memory_backed_directory>>
        child_directories;
    containers::hash_map<containers::string, memory_backed_file_location_ptr>
        child_files;
  };

public:
  // The file interface.
  bool exists_file(const containers::string_view _path) const override;
  bool exists_directory(const containers::string_view _path) const override;
  file_ptr create_file(
      const containers::string_view _path, result& _result) override;
  result create_directory(const containers::string_view _path) override;
  file_ptr open_file(
      const containers::string_view _path, result& _result) override;
  result delete_directory(const containers::string_view _path) override;
  result delete_file(const containers::string_view _path) override;
  memory_backed_mapping(memory::allocator* _allocator);

private:
  // Returns a pointer to the directory that is represented by
  // _path. If a has_file is set then the last element in the
  // path is treated as a file, otherwise it is treated
  // as the deepest directory..
  memory_backed_directory* get_directory(
      bool has_file, const containers::string_view _path) const;

  // A map of directory names to virtual directories.
  // Each virtual directory is a map of file names to virtual files.
  memory_backed_directory m_root_dir;
  multi_tasking::mutex m_system_lock;
};

}  // namespace file_system
}  // namespace wn

#endif  // __WN_MEMORY_BACKED_MAPPING_H__
