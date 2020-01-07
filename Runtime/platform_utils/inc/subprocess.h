// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_PLATFORM_UTILS_SUBPROCESS_H__
#define __WN_RUNTIME_PLATFORM_UTILS_SUBPROCESS_H__

#include "WNContainers/inc/WNString.h"
#include "WNCore/inc/tuple.h"
#include "WNMemory/inc/allocator.h"

namespace wn {
namespace runtime {
namespace platform_utils {

enum class subprocess_error {
#include "WNCore/inc/internal/errors.inc"
  cannot_create_pipe,
  cannot_create_process,
  process_exited_uncleanly
};

struct subprocess_return {
  subprocess_return(subprocess_return&& _other) {
    std_out = core::move(_other.std_out);
    std_err = core::move(_other.std_err);
    err = _other.err;
    return_code = _other.return_code;
  }
  subprocess_return(containers::string&& _std_out,
      containers::string&& _std_err, subprocess_error _err,
      uint32_t _return_code)
    : std_out(core::move(_std_out)),
      std_err(core::move(_std_err)),
      err(_err),
      return_code(_return_code) {}

  containers::string std_out;
  containers::string std_err;
  subprocess_error err;
  uint32_t return_code;
};

subprocess_return call_subprocess(memory::allocator* _allocator,
    const containers::string_view application,
    const containers::contiguous_range<const containers::string_view> args);

}  // namespace platform_utils
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_PLATFORM_UTILS_SUBPROCESS_H__
