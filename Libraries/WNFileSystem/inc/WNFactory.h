// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FACTORY_H__
#define __WN_FILE_SYSTEM_FACTORY_H__

#include "WNFileSystem/inc/WNMapping.h"

namespace wn {
namespace file_system {

enum class system_path { scratch };

class factory {
public:
  virtual mapping_ptr make_mapping(
      const system_path _system_path, memory::allocator* _allocator) const;
};

}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_FACTORY_H__
