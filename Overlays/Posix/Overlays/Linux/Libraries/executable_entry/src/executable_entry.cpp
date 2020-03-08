// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "executable_entry/inc/executable_entry.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "WNCore/inc/types.h"
#include "WNUtilities/inc/WNCrashHandler.h"
#include "executable_data/inc/executable_data.h"
#include "executable_data/inc/host_specific_data.h"

extern int32_t wn_main(const wn::executable::executable_data* _data);

namespace wn {
namespace executable {

void wn_dummy() {}

}  // namespace executable
}  // namespace wn

int main(int _argc, char** _argv) {
  wn::utilities::initialize_crash_handler();

  const char* full_path;

  if (_argc > 0) {
    full_path = _argv[0];
  } else {
    full_path = "";
  }

  const char* executable = nullptr;
  size_t path_length = strlen(full_path);

  for (size_t i = path_length; i != 0; --i) {
    if (full_path[i - 1] == '/') {
      executable = full_path + i;

      break;
    }
  }

  if (!executable) {
    executable = full_path;
  }

  wn::executable::host_specific_data host_data{executable};
  wn::executable::executable_data executable_data{
      &host_data, full_path, _argc, _argv};

  ::srand(static_cast<unsigned int>(::time(NULL)));

  return wn_main(&executable_data);
}
