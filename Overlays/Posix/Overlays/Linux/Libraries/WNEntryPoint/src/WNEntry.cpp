// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNTypes.h"
#include "WNUtilities/inc/WNCrashHandler.h"

extern int32_t wn_main(int32_t _argc, char* _argv[]);

void wn_dummy() {}

int main(int _argc, char** _argv) {
  wn::utilities::initialize_crash_handler();

  return (wn_main(_argc, _argv));
}