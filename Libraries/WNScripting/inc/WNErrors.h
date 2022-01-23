// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_ERRORS_H__
#define __WN_SCRIPTING_ERRORS_H__

namespace wn {
namespace scripting {
enum class engine_error {
#include "core/inc/internal/errors.inc"
  file_not_found
};
enum class parse_error {
#include "core/inc/internal/errors.inc"
  file_not_found,
  parse_failed
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_ERRORS_H__
