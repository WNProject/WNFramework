// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_FILE_MANAGER_H__
#define __WN_SCRIPTING_ENGINE_FILE_MANAGER_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNErrors.h"

namespace wn {
namespace scripting {

enum class file_error {
#include "WNCore/inc/Internal/WNErrors.inc"
  file_not_found
};

class file_buffer {
 public:
   virtual ~file_buffer() {}
  virtual const char* data() = 0;
  virtual wn_size_t size() const = 0;
};

class file_manager {
 public:
  virtual memory::allocated_ptr<file_buffer> get_file(
      const char* _filename) = 0;
  virtual void write_file(const char* _filename,
                          const containers::string_view& data) = 0;
};

class file_backed_buffer : public file_buffer {
 public:
  file_backed_buffer() {}
  wn::scripting::file_error open_file(const char* _filename) {
    if (WNFileSystem::WNFile::ok !=
        _file.OpenFile(_filename, WNFileSystem::WNFile::eWNFMRead)) {
      return wn::scripting::file_error::file_not_found;
    }
    return wn::scripting::file_error::ok;
  }

  const char* data() override { return _file.GetDataBuffer(); }

  wn_size_t size() const override { return _file.GetFileSize(); }

 private:
  WNFileSystem::WNFile _file;
};

class file_based_manager : public file_manager {
 public:
  file_based_manager(memory::allocator* _allocator) : m_allocator(_allocator) {}

  memory::allocated_ptr<file_buffer> get_file(const char* _filename) override {
    memory::allocation_pair p =
        m_allocator->allocate_element<file_backed_buffer>();
    file_backed_buffer* buffer = static_cast<file_backed_buffer*>(p.m_location);

    memory::allocated_ptr<file_buffer> file =
        memory::make_allocated_ptr<file_backed_buffer>(m_allocator);
    memory::allocated_ptr<file_buffer> buff(
        buffer, memory::allocated_destroyer(
                    m_allocator,
                    &memory::allocated_destroyer::typed_deleter<file_buffer>));

    if (wn::scripting::file_error::ok != buffer->open_file(_filename)) {
      return wn_nullptr;
    }

    return std::move(buff);
  }

  void write_file(const char* _filename,
                  const containers::string_view& data) override {
    WNFileSystem::WNFile output_file;
    output_file.OpenFile(_filename, WNFileSystem::WNFile::eWNFMWrite |
                                        WNFileSystem::WNFile::eWNFMCreate |
                                        WNFileSystem::WNFile::eWNFMClobber);
    output_file.WriteData(data.data(), data.size());
    output_file.CommitFileBuffer();
  }

  memory::allocator* m_allocator;
};
}
}
#endif  //__WN_SCRIPTING_ENGINE_FILE_MANAGER_H__