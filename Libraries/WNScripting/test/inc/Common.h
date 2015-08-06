// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_USE_PRECOMPILED_HEADER
#pragma once
#endif

#ifndef __WN_SCRIPTING_TESTS_COMMON_H__
#define __WN_SCRIPTING_TESTS_COMMON_H__

#include "WNCore/inc/WNBase.h"

#ifdef _WN_MSVC
#pragma warning(push)
#pragma warning(disable : 4275)
#endif

#ifdef _HAS_EXCEPTIONS
#undef _HAS_EXCEPTIONS
#endif

#define _HAS_EXCEPTIONS 0
#define GTEST_HAS_RTTI 0

#include <gtest/gtest.h>

#include <unordered_map>
#pragma message("TODO(awoloszyn): replace with custom hash_map")

#ifdef _WN_MSVC
#pragma warning(pop)
#endif

#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNScriptingEngine.h"
#include "WNScripting/inc/WNScriptingEngineFileManager.h"
#include <unordered_map>
#include <string>

namespace wn {
namespace scripting {

class test_file_buffer : public file_buffer {
 public:
  test_file_buffer(const char* _data_element, size_t _length)
      : m_data_element(_data_element), m_length(_length) {}
  const char* data() override { return m_data_element; }
  wn_size_t size() const override { return m_length; }

 private:
  const char* m_data_element;
  wn_size_t m_length;
};

class test_file_manager : public file_manager {
 public:
  struct file_pair {
    const char* file_name;
    const char* file_contents;
  };

  template <size_t N = 0>
  test_file_manager(memory::allocator* _allocator,
                    std::unordered_map<std::string, std::string>&& _files)
      : m_allocator(_allocator), m_files(std::move(_files)) {}

  void add_allocator(memory::allocator* _allocator) {
    m_allocator = _allocator;
  }

  memory::allocated_ptr<file_buffer> get_file(const char* _filename) override {
    return memory::make_allocated_ptr<test_file_buffer>(
        m_allocator, m_files[_filename].c_str(), m_files[_filename].size());
  }

 private:
  std::unordered_map<std::string, std::string> m_files;
  memory::allocator* m_allocator;
};
}
}
#endif  // __WN_SCRIPTING_TESTS_COMMON_H__
