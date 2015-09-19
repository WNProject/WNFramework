// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_RESULT_H__
#define __WN_FILE_SYSTEM_RESULT_H__

namespace wn {
namespace file_system {

enum class result {
  ok,
  fail,
  invalid_path,
  not_found,
  already_exists,
  path_construction_fail,
  file_opening_fail,
  file_mapping_fail,
  file_size_retrieval_fail
};

}  // namespace file_system
}  // namespace wn

#endif  //__WN_FILE_SYSTEM_RESULT_H__
