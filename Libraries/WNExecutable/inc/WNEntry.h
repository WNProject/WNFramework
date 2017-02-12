// Copyright (c) 2017, WNProject Authors. All rights reserved.
// found in the LICENSE.txt file.

#pragma once
#ifndef __WN_ENTRYPOINT_ENTRY_H__
#define __WN_ENTRYPOINT_ENTRY_H__

#include "WNCore/inc/WNBase.h"

void wn_dummy();

namespace wn {
namespace entry {

struct host_specific_data;

struct system_data WN_FINAL {
  host_specific_data* host_data;
  const char* program_name;
};

}  // namespace entry
}  // namespace wn

#endif  // __WN_ENTRYPOINT_ENTRY_H__