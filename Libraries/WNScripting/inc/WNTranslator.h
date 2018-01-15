// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_TRANSLATOR_H__
#define __WN_SCRIPTING_TRANSLATOR_H__

#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace scripting {

// Base interface for file translation.
// Any implementor is responsible for overloading
// translate_file to perform the actual work.
class translator {
public:
  translator()
    : m_num_warnings(0), m_num_errors(0) {}
  virtual ~translator() = default;

  parse_error translate_file(const char* _file) {
    return translate_file_with_error(_file, false);
  }

  virtual parse_error translate_file_with_error(
      const char* file, bool _dump_ast_on_failure) = 0;
  size_t errors() const {
    return m_num_errors;
  }
  size_t warnings() const {
    return m_num_warnings;
  }

protected:
  size_t m_num_warnings;
  size_t m_num_errors;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_TRANSLATOR_H__
