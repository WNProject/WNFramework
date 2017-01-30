// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNLogging/inc/WNLog.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(WNLogValueTest, LogValueInt) {
  for (int i = 0; i < 10; ++i) {
    size_t buffSize = 25;
    char tempBuff[25];
    int t2 = rand();
    WNLogging::LogType(t2, tempBuff, buffSize);
    int t = 0;
    sscanf(tempBuff, "%d", &t);
    ASSERT_EQ(t, t2);
  }
}

TEST(WNLogValueTest, LogValueString) {
  for (int i = 0; i < 10; ++i) {
    size_t buffSize = 25;
    char tempBuff2[25];
    char tempBuff[25];
    char* tBuff2 = tempBuff2;
    int t2 = rand();
    sprintf(tempBuff2, "%d", t2);
    WNLogging::LogType(tBuff2, tempBuff, buffSize);
    ASSERT_EQ(strcmp(tempBuff, tempBuff2), 0);
  }
}
