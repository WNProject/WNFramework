// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNLogging/inc/WNLog.h"
#include <fcntl.h>
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNLogging/inc/WNFileLogger.h"
#include "WNTesting/inc/WNTestHarness.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
#include "WNMultiTasking/inc/WNThread.h"
#endif

using namespace WNLogging;

TEST(WNLogInterface, SimpleTest) {
  WNLogging::WNConsoleLogger<> logger;
  WNLog mTestLog = WNLog(&logger);
  mTestLog.Log(eCritical, 0, "asdf", "asfd");
  mTestLog.Flush();
  mTestLog.Log(eCritical, 0, "asdf", "asfd");
  mTestLog.Flush();
}
