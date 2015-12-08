// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include "WNScripting/inc/WNErrors.h"
#include "WNContainers/inc/WNStringView.h"

namespace wn {
namespace scripting {

class engine {
 public:
  typedef void (*void_func)();
  virtual ~engine() {}
  virtual parse_error parse_file(const char* file) = 0;
  virtual void_func get_function(containers::string_view _name) const = 0;

  wn_size_t errors() const { return m_num_errors; }
  wn_size_t warnings() const { return m_num_warnings; }

 protected:
  wn_size_t m_num_warnings;
  wn_size_t m_num_errors;
};

}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_ENGINE_H__