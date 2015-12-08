// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_AST_PASSES_H__
#define __WN_SCRIPTING_AST_PASSES_H__
#include "WNCore/inc/WNTypes.h"

namespace WNLogging {
class WNLog;
}

namespace wn {
namespace scripting {
class script_file;

bool run_type_association_pass(script_file* _file, WNLogging::WNLog* _log,
  wn_size_t* _num_warnings, wn_size_t* _num_errors);
bool run_id_association_pass(script_file* _file, WNLogging::WNLog* _log,
  wn_size_t* _num_warnings, wn_size_t* _num_errors);

}  // namespace scripting
}  // namesapce wn
#endif  //__WN_SCRIPTING_AST_PASSES_H__
