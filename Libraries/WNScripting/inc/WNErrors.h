// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_ERRORS_H__
#define __WN_SCRIPTING_ERRORS_H__

namespace wn {
namespace scripting {
enum class engine_error {
#include "WNCore/inc/Internal/WNErrors.inc"
  file_not_found
};
enum class parse_error {
#include "WNCore/inc/Internal/WNErrors.inc"
  file_not_found,
  parse_failed
};
}
}

#endif  //__WN_SCRIPTING_ERRORS_H__
