// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FACTORY_H__
#define __WN_FILE_SYSTEM_FACTORY_H__

#include "WNFileSystem/inc/WNMapping.h"

namespace wn {
namespace file_system {

enum class mapping_type { scratch, memory_backed };

class factory {
public:
  virtual mapping_ptr make_mapping(
      const mapping_type _mapping_type, memory::allocator* _allocator) const;
};

}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_FACTORY_H__
