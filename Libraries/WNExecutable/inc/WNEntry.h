// Copyright (c) 2017, WNProject Authors. All rights reserved.
// found in the LICENSE.txt file.

#pragma once
#ifndef __WN_ENTRYPOINT_ENTRY_H__
#define __WN_ENTRYPOINT_ENTRY_H__

#include "WNCore/inc/WNBase.h"

namespace wn {
namespace entry {

void wn_dummy();

#if defined _WN_ANDROID || defined _WN_LINUX

struct foo {
  foo() {
    wn_dummy();
  }
};

// This will force my_foo to be created, which will in turn
// force test_dummy to be called, which exists in WNTestHarness.cpp.
// This means that WNTestHarness.cpp will be examined, and
// wn_main will be found, and actually linked in.
// It is not needed on other platforms because they are creating
// an executable rather than a shared object library.
static foo __attribute__((used)) my_foo;
#endif  //_WN_ANDROID || _WN_LINUX

struct host_specific_data;

struct system_data WN_FINAL {
  host_specific_data* host_data;
  const char* program_name;
};

}  // namespace entry
}  // namespace wn

#endif  // __WN_ENTRYPOINT_ENTRY_H__