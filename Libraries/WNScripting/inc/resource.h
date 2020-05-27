// Copyright(c) 2020, WNProject Authors.All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef _WN_SCRIPTING_INC_RESOURCE_H__
#define _WN_SCRIPTING_INC_RESOURCE_H__

#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"

namespace wn {
namespace scripting {
enum class convert_type { success, failed, use_default };
class resource {
public:
  virtual ~resource() {}
  explicit resource(containers::string _name) : m_name(_name) {}
  virtual bool convert_to_user_data(containers::string_view, void**) = 0;
  virtual containers::string_view get_file_extension() const {
    return "";
  }
  virtual convert_type convert_file(containers::string_view /*_file_name*/,
      containers::string* /*out_string*/) {
    return convert_type::failed;
  }
  virtual containers::string get_include_for_resource(
      containers::string_view /*_res*/) {
    return containers::string();
  }

  containers::string_view get_name() const {
    return m_name;
  }

private:
  containers::string m_name;
};

}  // namespace scripting
}  // namespace wn
#endif  // _WN_SCRIPTING_INC_RESOURCE_H__