// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_INFO_H__
#define __WN_FILE_SYSTEM_INFO_H__

#include "core/inc/types.h"

namespace wn {
namespace file_system {

enum class object_type { directory, file, symlink };

struct object_info final {
  using size_type = size_t;

  const object_type type;
  const size_type size;
};

}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_INFO_H__
