// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_USE_PRECOMPILED_HEADER
#pragma once
#endif

#ifndef __WN_SCRIPTING_TESTS_COMMON_H__
#define __WN_SCRIPTING_TESTS_COMMON_H__

#include "WNCore/inc/WNBase.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/WNEngineFileManager.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNTypeValidator.h"
#include "WNTesting/inc/WNTestHarness.h"

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

  test_file_manager(
      memory::allocator* _allocator,
      containers::hash_map<containers::string, containers::string>&& _files)
      : m_allocator(_allocator), m_files(std::move(_files)) {}

  test_file_manager(memory::allocator* _allocator)
      : m_allocator(_allocator), m_files(_allocator) {}

  void add_files(const containers::hash_map<containers::string,
                                            containers::string>& _files) {
    for (auto m : _files) {
      m_files[m.first] = m.second;
    }
  }

  void add_allocator(memory::allocator* _allocator) {
    m_allocator = _allocator;
  }

  memory::allocated_ptr<file_buffer> get_file(const char* _filename) override {
    return memory::make_allocated_ptr<test_file_buffer>(
        m_allocator, m_files[_filename].c_str(), m_files[_filename].size());
  }

  virtual void write_file(const char* _filename,
                          const containers::string_view& data) override {
    m_files[_filename] = data.to_string(m_allocator);
  }

 private:
  containers::hash_map<containers::string, containers::string> m_files;
  memory::allocator* m_allocator;
};

memory::allocated_ptr<scripting::script_file> test_parse_file(
    const char* _file, scripting::file_manager* _manager,
    memory::allocator* _allocator, WNLogging::WNLog* _log,
    wn_size_t* _num_warnings, wn_size_t* _num_errors) {

  scripting::type_validator validator(_allocator);
  memory::allocated_ptr<file_buffer> buff = _manager->get_file(_file);
  EXPECT_NE(wn_nullptr, buff);

  memory::allocated_ptr<scripting::script_file> ptr = scripting::parse_script(
      _allocator, &validator, _file, containers::string_view(buff->data(), buff->size()),
      _log, _num_warnings, _num_errors);

  return std::move(ptr);
}

memory::allocated_ptr<scripting::script_file> test_parse_file(
    const char* _file, scripting::file_manager* _manager,
    memory::allocator* _allocator, wn_size_t* _num_warnings,
    wn_size_t* _num_errors) {
  return test_parse_file(_file, _manager, _allocator,
                         WNLogging::get_null_logger(), _num_warnings,
                         _num_errors);
}

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_TESTS_COMMON_H__
