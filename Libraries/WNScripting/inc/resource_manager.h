// Copyright(c) 2020, WNProject Authors.All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef _WN_SCRIPTING_INC_RESOURCE_H__
#define _WN_SCRIPTING_INC_RESOURCE_H__

#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNLogging/inc/WNLog.h"
#include "core/inc/optional.h"

namespace wn {
namespace file_system {
class mapping;
}
namespace scripting {
enum class convert_type { success, failed, use_default };
class resource_manager {
public:
  virtual ~resource_manager() {}
  explicit resource_manager(containers::string _name)
    : m_name(core::move(_name)) {}
  virtual bool convert_to_function(containers::string_view,
      containers::string_view, logging::log*, containers::string*,
      core::optional<uintptr_t>*) {
    return false;
  }
  virtual containers::string_view get_file_extension() const {
    return "";
  }

  bool must_be_instantiated() const {
    return m_must_be_instantiated;
  }
  bool can_be_instantiated() const {
    return m_can_be_instantiated;
  }

  virtual convert_type convert_file(logging::log* _log, file_system::mapping*,
      containers::string_view _file_name, containers::string* _out_data,
      containers::string* _out_string) {
    (void)_file_name;
    (void)_out_data;
    (void)_out_string;
    _log->log_error("Invalid use of default resource convert_file");
    return convert_type::failed;
  }
  virtual bool setup_resource(
      containers::string_view /*_res*/, containers::string* /*_out*/) {
    return true;
  }

  containers::string_view get_name() const {
    return m_name;
  }

private:
  containers::string m_name;

protected:
  bool m_must_be_instantiated = true;
  bool m_can_be_instantiated = true;
};

}  // namespace scripting
}  // namespace wn
#endif  // _WN_SCRIPTING_INC_RESOURCE_H__
