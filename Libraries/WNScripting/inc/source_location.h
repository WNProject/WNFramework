// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SOURCE_LOCATION_H__
#define __WN_SCRIPTING_SOURCE_LOCATION_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
namespace scripting {
struct source_location {
  int64_t m_start_index;
  int64_t m_end_index;
  const uint8_t* m_line_start;
  size_t m_line_number;
  size_t m_char_number;
};

}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_SOURCE_LOCATION_H__
