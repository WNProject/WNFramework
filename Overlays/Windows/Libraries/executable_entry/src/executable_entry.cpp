// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "executable_entry/inc/executable_entry.h"

#include "WNUtilities/inc/WNCrashHandler.h"
#include "core/inc/types.h"
#include "executable_data/inc/executable_data.h"
#include "executable_data/inc/host_specific_data.h"

#include <crtdbg.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

extern int32_t wn_main(const wn::executable::executable_data* _data);

namespace wn {
namespace executable {

void wn_dummy() {}

}  // namespace executable
}  // namespace wn

int main(int _argc, char** _argv) {
  wn::utilities::initialize_crash_handler();

  // disable all error dialog popups
  ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  ::_set_error_mode(_OUT_TO_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

  const char* full_path;

  if (_argc > 0) {
    full_path = _argv[0];
  } else {
    full_path = "";
  }

  const char* executable = nullptr;
  size_t path_length = strlen(full_path);

  for (size_t i = path_length; i != 0; --i) {
    if (full_path[i - 1] == '/' || full_path[i - 1] == '\\') {
      executable = full_path + i;

      break;
    }
  }

  if (!executable) {
    executable = full_path;
  }

  wn::executable::host_specific_data host_data{
      executable, ::GetModuleHandleW(NULL)};
  wn::executable::executable_data executable_data{
      &host_data, full_path, _argc, _argv};

  ::srand(static_cast<unsigned int>(::time(NULL)));

  return wn_main(&executable_data);
}
